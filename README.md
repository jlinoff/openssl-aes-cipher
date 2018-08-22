# openssl-aes-cipher
C++ class that interfaces to OpenSSL AES ciphers and is interoperable
with openssl.

This is an update to the class named [Cipher](http://joelinoff.com/blog/?p=664) that i
released into the public domain from my own pages in 2013 to demonstrate how
to create a C++ class that interfaces (wraps) the OpenSSL library and allows one to encrypt and
decrypt files or strings.

It is interoperable with the `openssl` command line
tool which makes it a good introduction to using OpenSSL for ciphers. I think of it as the C++
version of the python [lock_files](https://github.com/jlinoff/lock_files) tool that i released
a few years ago.

Read the code and adapt it to your needs.

> Please note that the C++ is deliberately old for backward compatibility with old (pre-c++11) compilers.

I decided to move the project to github after updating it to work with the
opaque context objects that were introduced in openssl-1.1. It is my hope that
it will make it more widely accessible.

For more information about changes in openssl-1.1,
please see
[this page](https://wiki.openssl.org/index.php/OpenSSL_1.1.0_Changes#Compatibility_Layer).

I wrote it because I wish that I had found something like it when I was starting out with OpenSSL.

## Download and Build
Here are the steps necessary to download and build the tool locally.

To build, you will need c++, openssl development headers and regular
build tools.

```bash
$ https://github.com/jlinoff/openssl-aes-cipher.git
$ cd openssl-aes-cipher
$ make
```

When the make operation completes, the bin directory will contain two
programs: `ct.exe` and `text.exe`.

`Ct.exe` is a tool that uses the Cipher class to allow you to encrypt
and decrypt files and strings.

`Test.exe` is a the tool that is created for the unit tests.

You can use this as an example for incorporating openssl into your programs.

### Example Uses
Here are some simple examples that show how it works using the ct example
program.

#### Create the plaintext.
```bash
$ cat >plaintext <<EOF
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
eiusmod tempor incididunt ut labore et dolore magna aliqua. Convallis
aenean et tortor at risus viverra adipiscing at in. Duis at tellus at
urna condimentum mattis pellentesque id. Interdum varius sit amet
mattis vulputate enim nulla. Varius sit amet mattis vulputate enim. A
arcu cursus vitae congue mauris rhoncus aenean vel. Nibh mauris cursus
mattis molestie a iaculis at. Amet risus nullam eget felis. Molestie
nunc non blandit massa enim nec. Sit amet cursus sit amet
dictum. Scelerisque viverra mauris in aliquam sem fringilla ut morbi
tincidunt. Sit amet volutpat consequat mauris. Dolor sit amet
consectetur adipiscing elit ut. Velit sed ullamcorper morbi tincidunt
ornare massa eget egestas purus. Velit ut tortor pretium viverra
suspendisse. Vitae tempus quam pellentesque nec nam aliquam.
EOF
```

#### Example 1. Encrypt and decrypt using the ct example program
This example shows how encrypt and decrypt plaintext using the
example ct program.

```bash
$ bin/ct.exe -e -x 0102030405060708 -D md5 -p password -i plaintext -o encrypted
$ bin/ct.exe -d -x 0102030405060708 -D md5 -p password -i encrypted -o decrypted
$ diff plaintext decrypted
```

#### Example 2. Encrypt using ct and decrypt using openssl
This example show hows to encrypt using the ct program and decrypt
using openssl. It shows that the Cipher class is interoperable with
openssl.

```bash
$ bin/ct.exe -e -x 0102030405060708 -D md5 -p password -i plaintext -o encrypted
$ openssl aes-256-cbc -d -k password -salt -S 0102030405060708 -a -md md5 -in encrypted -out decrypted
$ diff plaintext decrypted
```

#### Example 3: Encrypt using openssl and decrypt using ct
This example show to encrypt using openssl and decrypt using
ct. It shows that the Cipher class is interoperable with openssl.
```bash
$ openssl aes-256-cbc -e -k password -salt -S 0102030405060708 -a -md md5 -in plaintext -out encrypted
$ bin/ct.exe -d -x 0102030405060708 -D md5 -p password -i encrypted -o decrypted
$ diff plaintext decrypted
```

## More Help
Type `./ct.exe -h` to get more information about how to use the tool.

## Using it in your code
The following example shows how to use the class encrypt/decrypt a file in your code.
For simple symmetric encryption/decryption it is much easier to use than the standard interface.

```c++
#include "cipher.h"
int main() {
  Cipher cipher;
   
  // encrypt a file.
  cipher.encrypt_file("plaintext", "encrypted", "password", "saltsalt");
   
  // decrypt a file.
  cipher.decrypt_file("encrypted", "decrypted", "password", "saltsalt");
  
  return 0;
}
```

The above example is not openssl compatible.
To make it ssl compatible you have to tweak a few things as shown in this example.

```c++
// openssl compatible
#include "cipher.h"

int main() {
  // explicitly specify the digest, md5 for 1.0.2 or earlier, sha256 for 1.1 or later.
  Cipher cipher("aes-256-cbc", "md5");
   
  // encrypt a file.
  cipher.encrypt_file("plaintext", "encrypted", "password", "saltsalt");
   
  // decrypt a file.
  cipher.decrypt_file("encrypted", "decrypted", "password", "saltsalt");

  return 0;
}
```
