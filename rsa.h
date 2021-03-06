/*
 * =====================================================================================
 *
 *       Filename:  rsa.h
 *
 *    Description:  Header for rsa.h
 *
 *        Version:  1.1
 *        Created:  10/08/2012 10:57:21 
 *       Revision:  10/14/2012 17:38:00
 *       Compiler:  gcc
 *
 *         Author:  Matthieu Keller <keller.mdpa@gmail.com>
 *   Organization:  ENSTA-ParisTech
 *
 * =====================================================================================
 */

#ifndef _RSA_H_
#define _RSA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <gmp.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_DECOMPOSE 100
#define P_Q_LENGHT 102
#define MAX_E 200
#define MAX_LENGTH 500

bool miller_rabin_test(mpz_t n, int j);
int generate_keys ();
int cypher ();
int decipher ();
int sign ();
int verify ();

#endif

