// ================================================================
// Description: Cipher manipulation program.
// Copyright:   Copyright (c) 2012 by Joe Linoff
// Version:     1.3.0
// Author:      Joe Linoff
//
// LICENSE
//   The cipher package is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
//   The cipher package is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   General Public License for more details. You should have received
//   a copy of the GNU General Public License along with the change
//   tool; if not, write to the Free Software Foundation, Inc., 59
//   Temple Place, Suite 330, Boston, MA 02111-1307 USA.
// ================================================================
#include "cipher.h"
#include <cstdarg>
#include <queue>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib> // exit, atoi
using namespace std;

typedef unsigned int uint;

// ================================================================
// Print the help.
// ================================================================
void help()
{
  cout <<
    "NAME\n"
    "\tct - Cipher tool that used to encrypt or decrypt files.\n"
    "\n"
    "SYNOPSIS\n"
    "\tct [OPTIONS]\n"
    "\n"
    "DESCRIPTION\n"
    "\tEncrypt or decrypt a file.\n"
    "\n"
    "OPTIONS\n"
    "\t-b, --debug\t\tTurn on internal debugging.\n"
    "\n"
    "\t-c NUM, --count NUM\n"
    "\t\t\tCount of number of init rounds.\n"
    "\n"
    "\t-C CIPHER, --cipher CIPHER\n"
    "\t\t\tThe name of the cipher to use (ex. aes-256-cbc).\n"
    "\n"
    "\t-d, --decrypt\tDecrypt.\n"
    "\n"
    "\t-D DIGEST, --digest DIGEST\n"
    "\t\t\tThe name of the digest to use (ex. sha256).\n"
    "\n"
    "\t-e, --encrypt\tEncrypt.\n"
    "\n"
    "\t-h\t\tThis help message.\n"
    "\n"
    "\t-i FILE, --in FILE\n"
    "\t\t\tThe input file.\n"
    "\t\t\tDefault is stdin.\n"
    "\n"
    "\t-n, --no-salt-prefix\n"
    "\t\t\tDo not embed the salt prefix.\n"
    "\t\t\tThe result will not be compatible with openssl.\n"
    "\n"
    "\t-o FILE, --out FILE\n"
    "\t\t\tThe output file.\n"
    "\t\t\tDefault is stdout.\n"
    "\n"
    "\t-p PASS, --pass PASS\n"
    "\t\t\tPassphrase.\n"
    "\n"
    "\t-s SALT, --salt SALT\n"
    "\t\t\tSalt as a string.\n"
    "\n"
    "\t-x HEX_SALT, --hex-salt HEX_SALT\n"
    "\t\t\tSalt as hex digits (16).\n"
    "\t\t\tEach character is 2 hex digits.\n"
    "\n"
    "\t-v, --verbose\tIncrease the level of verbosity.\n"
    "\n"
    "\t-V, --version\tPrint the version number and exit.\n"
    "\n"
    "EXAMPLES\n"
    "\t% # Help\n"
    "\t% ./ct.exe -h\n"
    "\n"
    "\t% # Encrypt stdin to stdout\n"
    "\t% echo 'Lorem ipsum dolor sit amet' | ./ct.exe -e\n"
    "\tU2FsdGVkX19nxmlzUf9K7K0Z40ZlovVWSfndp4VHLKSl9j5FXuvHi7dz08nVDsrV\n"
    "\n"
    "\t% # Decrypt stdin to stdout\n"
    "\t% echo U2FsdGVkX19nxmlzUf9K7K0Z40ZlovVWSfndp4VHLKSl9j5FXuvHi7dz08nVDsrV | ./ct.exe -d\n"
    "\tLorem ipsum dolor sit amet\n"
    "\n"
    "\t% # Encrypt a file\n"
    "\t% # It is okay to reference the same file.\n"
    "\t\% ./ct.exe -e -p 'Tally Ho!' -i foo.txt -o foo.txt\n"
    "\n"
    "\t% # Decrypt a file\n"
    "\t% # It is okay to reference the same file.\n"
    "\t\% ./ct.exe -d -p 'Tally Ho!' -i foo.txt -o foo.txt\n"
    "\n"
    "\t% # Encrypt/decrypt using pipes. Use -n to strip out the salt prefix.\n"
    "\t% echo 'Lorem ipsum dolor sit amet' |\\\n"
    "\t\t./ct.exe -e -n -s 12345678 -p foobar |\\\n"
    "\t\t./ct.exe -d -n -s 12345678 -p foobar\n"
    "\tLorem ipsum dolor sit amet\n"
    "\n"
    "\t% # Encrypt with ct, decrypt with openssl.\n"
    "\t% ct.exe -x 0102030405060708 -D md5 -p password -i in.txt -o m.out\n"
    "\t% openssl aes-256-cbc -d -k password -salt -S 0102030405060708 -a -md md5 -in m.out -out test.txt\n"
    "\t% diff in.txt test.txt\n"
    "\n"
    "AUTHOR\n"
    "\tJoe Linoff\n"
    "\n"
    << endl;
  exit(0);
}

// ================================================================
// Verbose output
// ================================================================
#define PKV(n) vdump(#n, n)
template<typename T> void vdump(const string& k, const T& v)
{
  cout << setw(16) << left << k << " : " << v << endl;
}

template<> void vdump<string>(const string& k, const string& v)
{
  string qv = "'" + v + "'";
  cout << setw(16) << left << k << " : "
       << setw(32) << left << qv << " (" << setw(3) << right << v.size() << ")"
       << left << endl;
}

// ================================================================
// CHK_ARG
// ================================================================
#define CHK_ARG                                                         \
  if (++i >= argc) {                                                    \
    cerr << "ERROR:" << __LINE__ << " missing argument for " << opt << endl; \
    exit(1);                                                            \
  }

// ================================================================
// atoh
// ================================================================
uint atoh(char x)
{
  if (x>='0' && x<='9') {
    return x-'0';
  }
  if (x>='a' && x<='f') {
    return 10+(x-'a');
  }
  if (x>='A' && x<='F') {
    return 10+(x-'A');
  }
  throw runtime_error("atoi(): invalid hex digit encountered");
  return 0;
}

// ================================================================
// Arguments match.
// ================================================================
bool match(std::string opt, ...)
{
  va_list args;
  va_start(args, opt);
  const char* arg = va_arg(args, const char*);
  while (arg and arg[0]) {
    if (opt == arg) {
      va_end(args);
      return true;
    }
    arg = va_arg(args, const char*);
  }
  va_end(args);
  return false;
}

// ================================================================
// MAIN
// ================================================================
int main(int argc,char** argv)
{
  string ifn;
  string ofn;
  string pass;
  string salt;
  string cipher=CIPHER_DEFAULT_CIPHER;
  string digest=CIPHER_DEFAULT_DIGEST;
  uint   count=CIPHER_DEFAULT_COUNT;
  uint   v=0;
  bool   debug = false;
  bool   encrypt = true;
  bool   embed = true;

  queue<string> cache;
  int i = 1;
  string opt;
  while (i < argc or cache.size()) {
    bool increment = false;
    if (cache.size()) {
      opt = cache.front();
      cache.pop();
    }
    else {
      opt = argv[i];
      increment = true;
    }

    // Create the new (pseudo) option strings.
    if (opt.size() > 2 and opt[1] != '-') {
      for(uint j=1; j<opt.size(); j++) {
        string xopt = "-";
        xopt += opt[j];
        cache.push(xopt);
      }
      i++;
      continue;
    }

    // Process the options.
    // Allow both long and short form specifications.
    if (match(opt, "-h", "--help", 0)) { help(); }
    else if (match(opt, "-b", "--debug", 0)) { debug = true; }
    else if (match(opt, "-c", "--count", 0)) { CHK_ARG count = atoi(argv[i]);}
    else if (match(opt, "-C", "--cipher", 0)) { CHK_ARG cipher = argv[i];}
    else if (match(opt, "-d", "--decrypt", 0)) { encrypt = false; }
    else if (match(opt, "-D", "--digest", 0)) { CHK_ARG digest = argv[i];}
    else if (match(opt, "-e", "--encrypt", 0)) { encrypt = true; }
    else if (match(opt, "-i", "--in", 0)) { CHK_ARG ifn = argv[i];}
    else if (match(opt, "-n", "--no-salt-prefix", 0)) { embed = false; }
    else if (match(opt, "-o", "--out", 0)) { CHK_ARG ofn = argv[i]; }
    else if (match(opt, "-p", "--pass", 0)) { CHK_ARG pass = argv[i]; }
    else if (match(opt, "-s", "--salt", 0)) { CHK_ARG salt = argv[i]; }
    else if (match(opt, "-v", "--verbose", 0)) { ++v; }
    else if (match(opt, "-V", "--version", 0)) {
      cout << "Cipher version: " << Cipher::get_version() << endl;
      cout << "SSL version: " << Cipher::get_ssl_version() << endl;
      exit(0);
    }
    else if (match(opt, "-x", "--hex-salt", 0)) {
      // Special handling for hex specification
      // of the salt.
      // Allow the user specify each character as
      // as a two digit hex number.
      // For "0" would be specified as "60".
      CHK_ARG
      string x = argv[i];
      if (x.size()!=16) {
	cerr << "ERROR:" << __LINE__ << " value must be 16 hex digits for " << opt << endl;
	exit(1);
      }

      salt = "";
      if (v) {
	cout << setw(16) << left << "xsalt" << " : ";
      }

      for(const char* p=x.c_str(); *p; p+=2) {
	uint h =(atoh(*p)*16) + atoh(*(p+1));
	if (v) {
	  cout << hex << setw(2) << h << dec;
	}
	salt += char(h);
      }
      if (v) {
	cout << endl;
      }
    }
    else {
      cout << "ERROR: unrecognized option " << opt <<endl;
      exit(1);
    }

    if (increment) {
      i++;
    }
  }

  // Print out some useful information.
  if (v) {
    PKV(ifn);
    PKV(ofn);
    PKV(pass);
    PKV(salt);
    PKV(cipher);
    PKV(digest);
    PKV(count);
    PKV(debug);
  }

  try {
    // Collect the input data.
    string in;
    if (ifn.empty()) {
      string x((istreambuf_iterator<char>(cin)),
	       istreambuf_iterator<char>());
      in = x;
    }
    else {
      ifstream ifs(ifn.c_str());
      if (!ifs) {
	string msg = "cannot read file: "+ifn;
	throw runtime_error(msg);
      }
      string x((istreambuf_iterator<char>(ifs)),
	       istreambuf_iterator<char>());
      ifs.close();
      in = x;
    }
    if (v) {
      cout << "input: (" << in.size() << ")" << endl;
      cout << "<begin>" << endl;
      cout << in << endl;
      cout << "<endl>" << endl;
    }
    if (in.empty()) {
      throw runtime_error("zero length input is not supported");
    }

    Cipher mgr(cipher,digest,count,embed);
    mgr.debug(debug);
    string out;
    if (encrypt) {
      out = mgr.encrypt(in,pass,salt);
    }
    else {
      out = mgr.decrypt(in,pass,salt);
    }

    // Write the output data.
    if (ofn.empty()) {
      cout << out;
      if (encrypt) {
	cout << endl;
      }
    }
    else {
      ofstream ofs(ofn.c_str());
      if (!ofs) {
	string msg = "cannot write file: "+ofn;
	throw runtime_error(msg);
      }
      ofs << out;
      if (encrypt) {
	ofs << endl;
      }
      ofs.close();
    }
  }
  catch (exception& e) {
    cerr << "ERROR: " << e.what() << endl;
    return 1;
  }
  return 0;
}
