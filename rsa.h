/*
 * =====================================================================================
 *
 *       Filename:  rsa.h
 *
 *    Description:  
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
#include <gmp.h>
#include <openssl/md5.h>

#define MAX_DECOMPOSE 100

bool miller_rabin_test(mpz_t n, int j);
int generate_keys ();

#endif

