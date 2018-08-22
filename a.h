/*! \mainpage Cipher Class
 * \section intro 1. Introduction
 * The Cipher object allows you to encrypt and decrypt files or
 * strings using the OpenSSL cipher algorithms. You can use it to
 * programatically read encrypted files that were created using the
 * openssl command line tool and vice-versa. It is a good introduction
 * to using OpenSSL for ciphers.
 *
 * Click 
 * @link http://www.openssl.org/docs/ here @endlink
 * to get more information about the OpenSSL interface. 
 * I found the web site to be a bit challenging to navigate.  To get
 * information about specific functions in the crypto section you have
 * to avoid the HTML page and get a directory listing. Here is where I
 * typically start out:
 * @link http://www.openssl.org/docs/crypto @endlink.
 *
 * \section read 2. Read an Encrypted File
 * The first use-case that we will consider is reading an encrypted
 * file that was created with openssl. 
 *
 * \subsection create1 2.1 Create the Encrypted File using openssl
 * First create the encrypted file using openssl using the 
 * passphrase: "Tally Ho!".
 * @verbatim
% cat >x <<EOF
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id
rhoncus lacus. Proin sit amet mi quam. Donec congue lorem vel tellus
mattis vestibulum. Nam at felis at libero vulputate rhoncus. Etiam ut
orci enim, vitae dapibus nulla. Nulla in odio diam, vitae porta
arcu. Sed mollis orci at nulla rhoncus mattis. Vivamus in nibh non
neque vulputate vulputate a sit amet diam. Nam nisi purus, iaculis
vitae laoreet non, aliquam ultrices lacus. Cum sociis natoque
penatibus et magnis dis parturient montes, nascetur ridiculus
mus. Vestibulum mattis tincidunt ante sit amet dignissim. Aenean nec
mauris quis neque fermentum facilisis. Suspendisse lorem justo,
tristique sit amet bibendum at, egestas in tellus. Quisque ultricies,
lectus eu eleifend lacinia, mi sapien tempus ante, sit amet tincidunt
nisl dui adipiscing justo.
EOF
% openssl enc -e -a -md sha256 -aes-256-cbc -salt -pass pass:"Tally Ho!" -in x -out x.txt
salt=3B0C2E749C927202
key=852C6BA3C079D32DA777159AA2C95B6112F5D0418DC2D59FD9904ECE1CDB6B47
iv =7FE7BB9FC8CDB753DA2FD866D7A1BCD3
@endverbatim
 *
 * Here is the content of the encrypted file. Note that it is in ASCII
 * format because we used the -a (base64) option.
 * @verbatim
U2FsdGVkX187DC50nJJyAquXOL3b9QJBeFLlw3k/WszBkD6+qfWqZn/FpAX1MHvi
Ui6NZURvEb+KzHpp710bZVx2VNpQAnFPScfZjeMc0lu9ySB67dODmT5GxLw1Cncm
EeyqXO/PVDw8dd7N6DYw8PT94X9toAwfGfMqeI3CmSFrHcQSXpRRVs1ZLyG0sVeL
64KZ4vfpjVyOFhdlWZWtfbal9SFH46ZksAElp74Uejl/PDBXA7IRv4SyRm+TDlCq
7vicZAj4wJDUVKX1p3VZ247LZ6KG4JcAlXJpHc8UJRHOswhOV2wEv7YS4H+jOnDz
pnn59CP1MLpfmCWU+QfVz8ARkI+oeaEpCh0M8W/zcrcuWrZkxoKFoPaMIRrLLXU0
y8XJoddTlAM/e+N5p7dflzFjAIDiocZD7isVovI+SSlwzPhiz5bAazU1jwc3RdCX
Wi/aSzOv4WVawb/QcpzoQ81d6NhimkjFcPKtlY7plA+flPzewc6qe96jURRJoB27
8ACzIKwbRnN1choDLI6q0NitXeNaPUemrs1U0WXx1uA59IwqqmqLyebFSON63s7u
2RR5aboYiNO85qX7dd3Q9YtIUO3mnlxyzw0CI1yphCRFMA5hFW20B12WGAjRlOXY
w/iLW7Fe10ibpsGrkFsKb7NTFOyByunUVbObZddlzvWWiEr4RwsP+8aKrUSXQmrh
FQ6R66NVSFGC9FTcnC47Ab2f/3X5ynNBpzMcOyoAGib7YVowCYCeNgMmb89nKVER
nrcu7axs6QSCiR7rxzQDCj1schuUQfDCiVo3yk/bECG4XL3bV3u9StOeSxHaiHsi
wtqPTI2SyHXGUd88kTFwRQyjFebHMLpi1yNp0Jwm0sg3bjhWSBfBusOA5Du93iWs
Z8bz5psdyyXNAwZkmSIcGYnPKQj5F3LoDK9UVOB3PKRZuM2D1wDqJ0U8TQlzG4uP
Tz78PWmPYZajsyqvbynWcCCvMiYCFEUV88UFmnmzfSja84tniqBPDl4hV+dmPT8P
IN8AgE11euq9CKXbOtfdlgYDQ8Zcb3V8s4sd7+uEvqp/djZuI0bsQ8lOQC/ckpqM
CXGqxU4EZvZp4Ow8GLaN7q2IJi1VxiJqN7K6OY+W8hI=
   @endverbatim
 *
 * \subsection read1 2.2 Read the Encrypted File in C++
 * Now you can read it using the Crypt object as follows:
 * @code
 * // This example shows how to read the encrypted file
 * // that was created in the previous subsection.
 * #include "cipher.h"
 * #include <string>
 * #include <fstream>
 * #include <sstream>
 * #include <iostream>
 * #include <stdexcept>
 * using namespace std;
 *
 * void decrypt(const string& license_file)
 * {
 *   try {
 *      // Read in the encrypted data.
 *      string ifn="x.txt";
 *      ifstream ifs(ifn.c_str());
 *      if (!ifs) {
 *         string msg = "Can't read file: "+ifn;
 *         throw runtime_error(msg);
 *      }
 *      string ciphertext((istreambuf_iterator<char>(ifs)),
 *                         istreambuf_iterator<char>());
 *      ifs.close();
 *
 *      // Decrypt the ciphertext and print it.
 *      Cipher mgr;
 *      string plaintext = mgr.decrypt(ciphertext,"Tally Ho!");
 *      cout << plaintext;
 *   }
 *   catch (exception& e) {
 *     cerr << "ERROR: decrypt failed: " << e.what() << endl;
 *   }
 * }
 * @endcode
 *
 * You can do the same thing by running the ct.exe program that is
 * provided with the package.
 * @verbatim % ./ct.exe -e -p 'Tally Ho!' -i x.txt -v @endverbatim
 *
 * Or you can use the openssl program.
 * @verbatim % openssl enc -d -p -a -md sha256 -aes-256-cbc -pass pass:'Tally Ho!' -in x.txt @endverbatim
 *
 * \section write 3. Write an Encrypted File
 * This use-case demonstrates to create an encrypted file using the
 * Cipher object and then read it using openssl.
 *
 * \subsection create3 3.2 Create the Encrypted File using C++
 * Here is how you create the encrypted file using C++.
 * @code
 * // This example shows how to create the encrypted file.
 * // Note that I am not using C++11 raw strings because
 * // they are not yet available for most compilers.
 * #include "cipher.h"
 * #include <string>
 * #include <fstream>
 * #include <sstream>
 * #include <iostream>
 * #include <stdexcept>
 * using namespace std;
 *
 * void encrypt(const string& license_file)
 * {
 *   try {
 *      // Create the plaintext.
 *      string plaintext = 
 *         "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id\n"
 *         "rhoncus lacus. Proin sit amet mi quam. Donec congue lorem vel tellus\n"
 *         ;
 *
 *      // Encrypt it.
 *      Cipher mgr;
 *      string ciphertext = mgr.decrypt(plaintext,"Tally Ho!");
 *
 *      // Write out the encrypted data.
 *      // It is ASCII.
 *      string ofn = "y.txt";
 *      ofstream ofs(ifn.c_str());
 *      if (!ofs) {
 *         string msg = "Can't write file: "+ofn;
 *         throw runtime_error(msg);
 *      }
 *      ofs << ciphertext;
 *      ofs.close();
 *   }
 *   catch (exception& e) {
 *     cerr << "ERROR: encrypt failed: " << e.what() << endl;
 *   }
 * }
 * @endcode
 *
 *
 * \subsection read3 3.3 Read the Encrypted File using openssl
 * Here is how you read the encrypted file using openssl.
 * @verbatim openssl enc -d -a -p -md sha256 -aes-256-cbc -pass pass:'Tally Ho!' -in y.txt @endverbatim
 *
 * You can do the same thing using the ct.exe program that is provided
 * with the package.
 * @verbatim ./ct.exe -d -p 'Tally Ho!' -i y.txt @endverbatim
 *
 * \section license LICENSE
 * The cipher package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *     
 * The cipher package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details. You should have received
 * a copy of the GNU General Public License along with the change
 * tool; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * \section copy_sec Copyright Notice
 * Copyright (c) 2012 by Joe Linoff
 *
 * @version 1.3.0
 * @author Joe Linoff
 */

