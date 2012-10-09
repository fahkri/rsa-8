/*
 * =====================================================================================
 *
 *       Filename:  rsa.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/21/2012 10:48:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Matthieu Keller (keller.mdpa@gmail.com) 
 *   Organization:  ENSTA-ParisTech
 *
 * =====================================================================================
 */
#include "rsa.h"

/*
 * Input: n > 2, an odd integer to be tested for primality;
 *        j, a parameter that determines the accuracy of the test
 * Output: composite if n is composite, otherwise probably prime
 */
bool miller_rabin_test(mpz_t n, int j)
{
	bool res = false;

	/*  if n is even */
	if (mpz_even_p(n) != 0)
	{
		printf("n is pair \n");
		return res;
	}

/* ******************************************************************************
 * write n − 1 as 2^s·d with d odd by factoring powers of 2 from n − 1
 * ******************************************************************************/

	/*  q = n-1 */
	mpz_t q;
	mpz_init(q);
	mpz_sub_ui(q, n, 1);

	/*  s = 0 */
	mpz_t s;
	mpz_init(s);

	/*  n - 1 = q = 2^s.d*/
	while (mpz_even_p(q))
	{
		/*  q = q/2 */
		mpz_divexact_ui(q,q,2);

		/*  s++ */
		mpz_add_ui(s, s, 1);
	}
	/*  n - 1 = 2^s.q */

	gmp_printf("n-1 = %Zd*2^%Zd \n", q, s);

/* ******************************************************************************
 * The loop of testinnt generate_keys ()g
 * ******************************************************************************/

	/*  init */

	/*  n_1 = n-1, n_3 = n-3 */
	mpz_t n_1; mpz_t n_3;
	mpz_init(n_1); mpz_init(n_3);
	mpz_sub_ui(n_1, n, 1);
	mpz_sub_ui(n_3, n, 3);

	/*  the number x for testing */
	mpz_t x;
	mpz_init (x);
	
	/*  the random number a  */
	mpz_t a;
	mpz_init(a);

	/*  the second loop indice l */
	mpz_t l;
	mpz_init (l);

	/*  the random generator */
	gmp_randstate_t rs;
	gmp_randinit_default(rs);

	/*  the loop */
	while (j-- > 0)
	{
		mpz_urandomm(a, rs, n_3);     	// random from 0 to n-4
		mpz_add_ui(a, a, 2);         	// random from 2 to n-2
		mpz_powm(x, a, q, n); 		  	// x = a^q mod n

		/* if x = 1 or x = n − 1 then do next LOOP  */
		if (mpz_cmp_ui(x, 1) == 0 || mpz_cmp(x, n_1) == 0)  continue;

	    mpz_set_ui(l, 0);  				// l = 0
		while (mpz_cmp(l, s) < 0)
		{
			if (mpz_cmp(x, n_1) == 0) break;
			mpz_powm_ui(x, x, 2, n); 	//  x = x^2 mod n
			mpz_add_ui(l, l, 1); 		// l++
			
			/*  if x = 1 then return composite */
			if (mpz_cmp_ui(x, 1) == 0) return res;
		}
		if (mpz_cmp(x, n_1) == 0) continue;	
		return res;
	}
	res = true;

	return res;
}

int generate_keys ()
{
	/*  init p, q, n, phi */
	mpz_t p; mpz_t q; mpz_t n; mpz_t phi;
	mpz_init (p); mpz_init (q); mpz_init(n); mpz_init (phi);
	
	// TODO generate p & q 100 bits each.
	mpz_set_ui (p, 3);
	mpz_set_ui (q, 13);
	
	mpz_mul(n, p, q); 				// n = pq

	/*  phi = (p-1)(q-1) */
	mpz_sub_ui(p, p, 1);
	mpz_sub_ui(q, q, 1);
	mpz_mul(phi, p, q);
	mpz_add_ui (p, p, 1);
	mpz_add_ui (q, q, 1);

	/*  we seek e : gcd(e, phi)= 1 */
	
	/*  init e, tmp */
	mpz_t e; mpz_t tmp;
	mpz_init (e); mpz_init (tmp);
	mpz_set_ui (e, 2);
	
	mpz_gcd (tmp, e, phi); 		// tmp = gcd(e, phi)
	/*  while tmp != 1 */
	while (mpz_cmp_ui(tmp, 1) !=0)
	{
		mpz_add_ui (e, e, 1); 		// e++
		mpz_gcd (tmp, e, phi); 		// tmp = gcd(e, phi)
	}

	/*  we seek d : e.d = 1 mod phi */
	mpz_t d;
	mpz_init (d);
	mpz_powm_ui (d, e, -1, phi);

	gmp_printf("n : %Zd ,e : %Zd, d : %Zd ,phi : %Zd \n",n , e, d, phi);

	// TODO write public.rsa (n,e) & private.rsa (d)

	return EXIT_SUCCESS;
}


int chiffre (mpz_t message, mpz_t n, mpz_t e)
{
	mpz_t c;
	mpz_init(c);

	mpz_powm(c, message, e, n);

	gmp_printf("n : %Zd ,e : %Zd, m : %Zd ,c : %Zd \n",n , e, message, c);

	return EXIT_SUCCESS;
}

int dechiffre (mpz_t chiffre, mpz_t d, mpz_t n)
{
	mpz_t m;
	mpz_init(m);

	mpz_powm(m, chiffre, d, n);

	gmp_printf("n : %Zd d : %Zd, m : %Zd ,c : %Zd \n",n , d, m, chiffre);

	return EXIT_SUCCESS;
}

int signature (mpz_t m, mpz_t d, mpz_t n)
{
	mpz_t h;
	mpz_init (h);

	// TODO h(m) = MD5 (m)
	
	mpz_t s;
	mpz_init (s);
		
	mpz_powm (s, h, d, n);

	return EXIT_SUCCESS;
}

bool verification (mpz_t m, mpz_t s, mpz_t e, mpz_t n)
{
	bool res = false;
	mpz_t tmp;
	mpz_init(tmp);

	mpz_powm(tmp, s, e, n);

	mpz_t h;
	mpz_init (h);

	// TODO h(m) = MD5 (m)
	 

	if (mpz_cmp (tmp, h))
	{
		res = true;
	}

	return res;
}

int main(int argc, const char *argv[])
{	
	/*  test miller rabin */
	//mpz_t n;
	//mpz_init_set_ui(n, 3967);
	//gmp_printf("n : %Zd \n", n);
	//printf("%i \n", miller_rabin_test(n,10));

	generate_keys();

	mpz_t m; mpz_t n; mpz_t e; mpz_t d;
	mpz_init(m); mpz_init(n); mpz_init(e); mpz_init(d);
	mpz_set_ui(m, 2); mpz_set_ui(n, 39), mpz_set_ui(e, 5); mpz_set_ui(d, 5);

	mpz_t c; mpz_init (c); mpz_set_ui(c, 32);
	chiffre (m, n, e);
	dechiffre(c, d, n);

	return EXIT_SUCCESS;
}
