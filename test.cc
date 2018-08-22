// ================================================================
// Description: Cipher test program.
// Copyright:   Copyright (c) 2012 by Joe Linoff
// Version:     1.3.0
// Author:      Joe Linoff
// ================================================================
#include "cipher.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib> // exit, atoi
#include <cstdio>
using namespace std;

// ================================================================
// MACROS
// ================================================================
#define DBG_PRE __FILE__ << ":" << setfill('0') << setw(4) << __LINE__ << ": "
#define PKV(v) vdump(__FILE__,__LINE__,#v,v)

namespace
{
  // ================================================================
  // DEBUG mode only.
  // Formated dump of a general type.
  // ================================================================
  template<typename T> void vdump(const string& fn,
                                  uint ln,
                                  const string& prefix,
                                  const T& d)
  {
    cout << fn << ":" << ln << ": " << prefix << "\t" << d << endl;
  }
  // ================================================================
  // DEBUG mode only.
  // Explicit template instantiation of the above for string
  // types so that I can report the length.
  // ================================================================
  template<> void vdump<string>(const string& fn,
                      uint ln,
                      const string& prefix,
                      const string& d)
  {
    cout << fn << ":" << ln << ": " 
         << prefix << "\t" 
         << left << setw(64) << d 
         << " (" << d.size() << ")"
         << endl;
  }
}

// ================================================================
// test_cipher1 - encrypt a buffer and then decrypt it.
// ================================================================
void test_cipher1(pair<int,int>& st,int v)
{
  if (v) {
    cout << DBG_PRE << "Cipher Test 1" << endl;
  }
  string pass = "Tally Ho!";
  string salt = "12345678"; // must be 8 characters
    
  if (v) {
    PKV(pass);
    PKV(salt);
  }
    
  string plaintext = 
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id\n"
    "rhoncus lacus. Proin sit amet mi quam. Donec congue lorem vel tellus\n"
    ;

  if (v) {
    PKV(plaintext);
  }

  Cipher c;
  if (v>1) {
    c.debug();
  }
    
  string ciphertext = c.encrypt(plaintext,pass,salt);
  if (v) {
    PKV(ciphertext);
  }
    
  string decode = c.decrypt(ciphertext,pass,salt);
  if (v) {
    PKV(decode);
  }
    
  st.first += 1;
  cout << DBG_PRE << "cipher_test1:\t";
  if (decode==plaintext) {
    cout << "passed";
  }
  else {
    cout << "failed";
    st.second += 1;
  }
  cout << endl;
}
// ================================================================
// test_cipher2 - encrypt a file.
// ================================================================
void test_cipher2(pair<int,int>& st,int v)
{
  if (v) {
    cout << DBG_PRE << "Cipher Test 2" << endl;
  }
  string pass  = "Tally Ho!";
  string salt  = "12345678"; // must be 8 characters
    
  if (v) {
    PKV(pass);
    PKV(salt);
  }

  string ifn = "test_cipher2.txt";
  string ofn = "test_cipher2.dat";
  string plaintext = 
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id\n"
    "rhoncus lacus. Proin sit amet mi quam. Donec congue lorem vel tellus\n"
    ;

  // Create the ciphertext.
  Cipher c1;
  string ciphertext = c1.encrypt(plaintext,pass,salt);
  ciphertext += "\n";

  // Create the plaintext file.
  ofstream ofs(ifn.c_str());
  ofs << plaintext;
  ofs.close();

  // Create the cipher object and encrypt.
  Cipher c;
  if (v>1) {
    c.debug();
  }
  c.encrypt_file(ifn,ofn,pass,salt);

  // Read the encrypted file and check it.
  ifstream ifs(ofn.c_str());
  string str((istreambuf_iterator<char>(ifs)),
	     istreambuf_iterator<char>());
  ifs.close();

  st.first += 1;
  cout << DBG_PRE << "cipher_test2:\t";
  if (ciphertext == str) {
    cout << "passed";
    remove(ifn.c_str());
    remove(ofn.c_str());
  }
  else {
    cout << "failed";
    st.second += 1;
    cout << endl;
    cout << "1: " << ciphertext;
    cout << "2: " << str;
  }
  cout << endl;
}

// ================================================================
// test_cipher3 - decrypt a file.
// ================================================================
void test_cipher3(pair<int,int>& st,int v)
{
  if (v) {
    cout << DBG_PRE << "Cipher Test 3" << endl;
  }
  string pass  = "password";
  string salt  = "ABCDEFGH"; // must be 8 characters
    
  if (v) {
    PKV(pass);
    PKV(salt);
  }

  string ifn = "test_cipher3.dat";
  string ofn = "test_cipher3.txt";
  string plaintext = 
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id\n"
    "rhoncus lacus. Proin sit amet mi quam. Donec congue lorem vel tellus\n"
    ;

  // Create the ciphertext.
  Cipher c1;
  string ciphertext = c1.encrypt(plaintext,pass,salt);
  ciphertext += "\n";

  // Create the encrypted file.
  if (v) {
    cout << DBG_PRE << "creating " << ifn << endl;
  }
  ofstream ofs(ifn.c_str());
  ofs << ciphertext;
  ofs.close();

  // Create the Cipher object and decrypt it.
  Cipher c;
  if (v>1) {
    c.debug();
  }
  c.decrypt_file(ifn,ofn,pass,salt);

  // Read the decrypted file and check it.
  ifstream ifs(ofn.c_str());
  string str((istreambuf_iterator<char>(ifs)),
	     istreambuf_iterator<char>());
  ifs.close();
    
  st.first += 1;
  cout << DBG_PRE << "cipher_test3:\t";
  if (plaintext == str) {
    cout << "passed";
    remove(ifn.c_str());
    remove(ofn.c_str());
  }
  else {
    cout << "failed";
    st.second += 1;
  }
  cout << endl;
}

// ================================================================
// test_cipher4 - problem described by Mihai Todor
// ================================================================
void test_cipher4(pair<int,int>& st,int v)
{
  if (v) {
    cout << DBG_PRE << "Cipher Test 3" << endl;
  }
  string pass  = "password";
  string salt  = "ABCDEFGH"; // must be 8 characters
    
  if (v) {
    PKV(pass);
    PKV(salt);
  }

  Cipher cipher;
  string plain = "test";
  string enc = cipher.encrypt(plain, "1234", "12345678");
  string dec = "failed!";
  try {
    dec = cipher.decrypt(enc, "1234");
  }
  catch (exception& e) {
  }
  if (v) {
    PKV(plain);
    PKV(enc);
    PKV(dec);
  }
  
  st.first += 1;
  cout << DBG_PRE << "cipher_test4:\t";
  if (plain != dec) {
    cout << "failed  mismatch: '" << plain << "' != '" << dec << "'" << endl;
    st.second += 1;
  }
  else {
    cout << "passed" << endl;
  }
 
}

// ================================================================
// test
// ================================================================
pair<int,int> test(int v)
{
  pair<int,int> st(0,0);
  try {
    test_cipher1(st,v);
    test_cipher2(st,v);
    test_cipher3(st,v);
    test_cipher4(st,v);
  }
  catch (exception& e) {
    cout << "ERROR: " << e.what() << endl;
    st.second += 1;
    if (st.second>st.first) {
      st.first = st.second; // total is always >= num failed
    }
  }

  int total  = st.first;
  int failed = st.second;
  int passed = st.first-st.second;
  PKV(total);
  PKV(passed);
  PKV(failed);
  return st;
}

// ================================================================
// MAIN
// ================================================================
int main(int argc,char** argv)
{
  int v=0;
  for(int i=1;i<argc;++i) {
    string opt = argv[i];
    if (opt=="-v") {
      ++v;
    }
    else {
      cout << "ERROR: unrecognized option " << opt << endl;
      exit(1);
    }
  }
  test(v);
  return 0;
}
