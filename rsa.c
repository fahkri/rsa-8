/*
 * =====================================================================================
 *
 *       Filename:  rsa.c
 *
 *    Description:  Cryptologie TP2 - Simple implementation of RSA
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
		//printf("n is pair \n");
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

	//gmp_printf("n-1 = %Zd*2^%Zd \n", q, s);

	/* ******************************************************************************
	 * The loop of testing
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
	/* include time for a real alea */
	time_t t;
	time(&t);
	gmp_randseed_ui(rs, t);

	/*  the loop */
	while (j-- > 0)
	{
		mpz_urandomm(a, rs, n_3);     	// random from 0 to n-4
		mpz_add_ui(a, a, 2);         	// random from 2 to n-2
		mpz_powm(x, a, q, n); 		  	// x = a^q mod n

		//gmp_printf("a : %Zd \n",a);

		/* if x = 1 or x = n − 1 then do next LOOP  */
		if (mpz_cmp_ui(x, 1) == 0 || mpz_cmp(x, n_1) == 0)  continue;

		mpz_set_ui(l, 0);  				// l = 0
		while (mpz_cmp(l, s) < 0)
		{
			if (mpz_cmp(x, n_1) == 0) break;
			mpz_powm_ui(x, x, 2, n); 	//  x = x^2 mod n
			mpz_add_ui(l, l, 1); 		// l++

			/*  if x = 1 then return composite */
			if (mpz_cmp_ui(x, 1) == 0) 
			{
				/*  clear memory */
				mpz_clear(q);
				mpz_clear(s);
				mpz_clear(n_1);
				mpz_clear(n_3);
				mpz_clear(x);
				mpz_clear(a);
				mpz_clear(l);
				gmp_randclear(rs);
				return res;
			}
		}
		if (mpz_cmp(x, n_1) == 0) continue;	

		/*  clear memory */
		mpz_clear(q);
		mpz_clear(s);
		mpz_clear(n_1);
		mpz_clear(n_3);
		mpz_clear(x);
		mpz_clear(a);
		mpz_clear(l);
		gmp_randclear(rs);
		return res;
	}
	res = true;

	/*  clear memory */
	mpz_clear(q);
	mpz_clear(s);
	mpz_clear(n_1);
	mpz_clear(n_3);
	mpz_clear(x);
	mpz_clear(a);
	mpz_clear(l);
	gmp_randclear(rs);
	return res;
}

/*
 * generate aleatoire key and 
 * write them in private.rsa & public.rsa
 */
int generate_keys ()
{
	/*  init p, q, n, phi */
	mpz_t p; mpz_t q; mpz_t n; mpz_t phi;
	mpz_init (p); mpz_init (q); mpz_init(n); mpz_init (phi);

	/* generate aleatoire p & q 100 bits each. */
	/*  the random generator */
	gmp_randstate_t rs;
	gmp_randinit_default(rs);
	/* include time for a real alea */
	time_t t;
	time(&t);
	gmp_randseed_ui(rs, t);

	/*  generate p prime */
	bool cont = false;
	while (!cont)
	{
		mpz_urandomb (p, rs, P_Q_LENGHT);
		cont = miller_rabin_test(p, MAX_DECOMPOSE);
	}

	/* generate q prime  */
	cont = false;
	while (!cont)
	{
		mpz_urandomb (q, rs, P_Q_LENGHT);
		cont = miller_rabin_test(q, MAX_DECOMPOSE);
	}
	gmp_randclear(rs);

	mpz_mul(n, p, q); 				// n = pq

	//gmp_printf("n : %Zd ,p : %Zd, q : %Zd \n",n , p, q);

	/*  phi = (p-1)(q-1) */
	mpz_sub_ui(p, p, 1);
	mpz_sub_ui(q, q, 1);
	mpz_mul(phi, p, q);
	mpz_add_ui (p, p, 1);
	mpz_add_ui (q, q, 1);

	/*  we seek e : gcd(e, phi)= 1 */
	/*  init e, tmp */
	// TODO e must be alea generate
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

	/* d = e^-1 mod phi */
	mpz_invert (d, e, phi);

	//gmp_printf("should be 1 : %Zd \n", tmp);

	//gmp_printf("n : %Zd ,e : %Zd, d : %Zd ,phi : %Zd \n",n , e, d, phi);

	/*  write public.rsa (n,e) & private .rsa (d) */
	FILE* pub = NULL;
	pub = fopen ("public.rsa", "w+");
	FILE* priv = NULL;
	priv = fopen("private.rsa", "w+");

	/* if something goes wrong with the files */
	if (pub == NULL || priv == NULL)
	{
		printf ("Something goes wrong with the file public.rsa and private.rsa \n");
		return EXIT_FAILURE;
	}
	/*  print the public key in public.rsa */
	gmp_fprintf (pub, "%Zd %Zd", n, e);
	/*  print the private key in private.rsa */
	gmp_fprintf (priv, "%Zd", d);
	
	/*  close files */
	fclose(pub);
	fclose(priv);

	/*  clear memory */
	mpz_clear(p);
	mpz_clear(q);
	mpz_clear(n);
	mpz_clear(e);
	mpz_clear(d);
	mpz_clear(phi);
	mpz_clear(tmp);

	return EXIT_SUCCESS;
}

/*
 * cypher a message using the public key
 */
int chiffre ()
{
	/*  load the file message.rsa */
	FILE* mes = NULL;
	mes = fopen("message.rsa", "r"); 
	if (mes == NULL)
	{
		printf("Something goes wrong with teh file message.rsa you must write a file \
				message.rsa with the message to cypher. \n");
		return EXIT_FAILURE;
	}
	/*  init message */
	mpz_t m; mpz_init(m);
	/*  load message from message.rsa */
	gmp_fscanf(mes,"%Zd", m);
	/*  close mes.rsa */
	fclose(mes);


	/*  load the public key from public.rsa */
	FILE* pub = NULL;
	pub = fopen("public.rsa", "r"); 
	if (pub == NULL)
	{
		printf ("Something goes wrong with the file public.rsa you must use \
				generate_keys first. \n");
		return EXIT_FAILURE;
	}
	/*  init public key */
	mpz_t n; mpz_t e;
	mpz_init(n); mpz_init(e);
	/*  load public key from public.rsa */
	gmp_fscanf(pub, "%Zd %Zd", n, e);
	/*  close public.rsa */
	fclose(pub);
		
	/*  init cypher message */
	mpz_t c;
	mpz_init(c);

	/* cyper the message using modular exponentiation */
	mpz_powm(c, m, e, n);

	FILE* cypher = NULL;
	cypher = fopen("cypher.rsa", "w+");
	if (cypher == NULL)
	{
		printf ("Something goes wrong with the file cypher.rsa");
		return EXIT_FAILURE;
	}
	/*  print the cypher message in cypher.rsa */
	gmp_fprintf (cypher, "%Zd", c);
	/*  close cypher.rsa */
	fclose(cypher);

	gmp_printf("Chiffre, n : %Zd ,e : %Zd, m : %Zd ,c : %Zd \n",n , e, m, c);

	gmp_printf("The plain text message is : %Zd \n", m);

	/*  clear memory */
	mpz_clear(c);
	mpz_clear(n);
	mpz_clear(e);
	mpz_clear(m);

	return EXIT_SUCCESS;
}

/*
 * decypher the chiffre with the private key
 */
int dechiffre ()
{
	/*  load the cypher message from file */
	FILE* cypher = NULL;
	cypher = fopen("cypher.rsa", "r");
	if (cypher == NULL)
	{
		printf ("Something goes wrong with the file cypher.rsa you must cypher a message before \
				decypher it. \n");
		return EXIT_FAILURE;
	}
	/*  init the cypher message */
	mpz_t c; mpz_init(c);
	/*  load the cypher message from cypher.rsa */
	gmp_fscanf(cypher, "%Zd", c);
	fclose(cypher);

	/*  load the public key from file */
	FILE* pub = NULL;
	pub = fopen("public.rsa", "r");
	if (pub == NULL)
	{
		printf ("Something goes wrong with the file public.rsa you must have cypher a message and \
				the public key corresponding. \n ");
		return EXIT_FAILURE;
	}
	/*  init the public key, only n is needed */
	mpz_t n; mpz_init(n);
	/*  load n from public.rsa */
	gmp_fscanf(pub, "%Zd", n);
	fclose(pub);

	/*  load the private key from file */
	FILE* priv = NULL;
	priv = fopen("private.rsa", "r");
	if (priv == NULL)
	{
		printf ("Something goes wrong with the file private.rsa you must have cypher \
				a message and the private key corresponding. \n ");
		return EXIT_FAILURE;
	}
	/*  init the private key */
	mpz_t d; mpz_init (d);
	/*  load e from private.rsa */
	gmp_fscanf(priv, "%Zd", d);
	fclose(priv);

	/*  init the plain text message */
	mpz_t m;
	mpz_init(m);

	/*  decypher */
	mpz_powm(m, c, d, n);

	gmp_printf("Dechiffre, n : %Zd d : %Zd, m : %Zd ,c : %Zd \n",n , d, m, c);

	gmp_printf("The plain text message is : %Zd \n", m);

	/*  clear memory */
	mpz_clear (m);
	mpz_clear (c);
	mpz_clear (n);
	mpz_clear (d);
	return EXIT_SUCCESS;
}

int signature ()
{
	mpz_t h;
	mpz_init (h);

	// TODO h(m) = MD5 (m)

	mpz_t s;
	mpz_init (s);

//	mpz_powm (s, h, d, n);

//	gmp_printf("n : %Zd d : %Zd, h : %Zd ,s : %Zd \n",n , d, h, s);

	return EXIT_SUCCESS;
}

bool verification ()
{
	bool res = false;
	mpz_t tmp;
	mpz_init(tmp);

//	mpz_powm(tmp, s, e, n);

	mpz_t h;
	mpz_init (h);

	// TODO h(m) = MD5 (m)


	//	if (mpz_cmp (tmp, h))
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

	/*  test chiffre & dechiffre */
	chiffre ();
	dechiffre();


	return EXIT_SUCCESS;
}
