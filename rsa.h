/*
 * =====================================================================================
 *
 *       Filename:  rsa.h
 *
 *    Description:  Header for rsa.h
 *
 *        Version:  1.0
 *        Created:  10/08/2012 10:57:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
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
#include <openssl/md5.h>

#define MAX_DECOMPOSE 100
#define P_Q_LENGHT 102
#define MAX_E 200

bool miller_rabin_test(mpz_t n, int j);
int generate_keys ();
int chiffre ();
int dechiffre ();
int signature ();
bool verification ();

#endif

