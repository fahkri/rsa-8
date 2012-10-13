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

	/*  if n <= 0 */
	if (mpz_cmp_ui(n,0) <= 0)
	{
		return res;
	}

	/*  let's consider 1,2&3 as prime */
	if (mpz_cmp_ui(n,3) <= 0)
	{
		return !res;
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
				mpz_clears(q, s, n_1, n_3, x, a, l, NULL);
				gmp_randclear(rs);
				return res;
			}
		}
		if (mpz_cmp(x, n_1) == 0) continue;	

		/*  clear memory */
		mpz_clears(q, s, n_1, n_3, x, a, l, NULL);
		gmp_randclear(rs);
		return res;
	}
	res = true;

	/*  clear memory */
	mpz_clears(q, s, n_1, n_3, x, a, l, NULL);
	gmp_randclear(rs);
	return res;
}

/*
 * generate aleatoire key and 
 * write them in private.rsa & public.rsa
 */
int generate_keys ()
{
	printf ("generate keys \n");

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

	/*  n = pq */
	mpz_mul(n, p, q);

	//gmp_printf("n : %Zd ,p : %Zd, q : %Zd \n",n , p, q);

	/*  phi = (p-1)(q-1) */
	mpz_sub_ui(p, p, 1);
	mpz_sub_ui(q, q, 1);
	mpz_mul(phi, p, q);
	mpz_add_ui (p, p, 1);
	mpz_add_ui (q, q, 1);

	/*  we seek e : gcd(e, phi)= 1, with e alea < MAX_E + 3*/
	/*  init e, tmp */
	mpz_t e; mpz_t tmp;
	mpz_init (e); mpz_init (tmp);
	mpz_t max; mpz_init (max);
	mpz_set_ui(max, MAX_E);
	/*  init e */
	mpz_urandomm(e, rs, max);     	// random from 0 to MAX_E
	mpz_add_ui(e, e, 3);         	// random from 3 to MAX_E + 3

	mpz_gcd (tmp, e, phi); 		// tmp = gcd(e, phi)

	/*  while tmp != 1 */
	while (mpz_cmp_ui(tmp, 1) !=0)
	{
		mpz_urandomm(e, rs, max);     	// random from 0 to MAX_E
		mpz_add_ui(e, e, 3);         	// random from 3 to MAX_E + 3

		mpz_gcd (tmp, e, phi); 		// tmp = gcd(e, phi)
	}
	/*  clear rs  */
	gmp_randclear(rs);

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
	gmp_printf ("public key : %Zd %Zd \n", n, e);
	/*  print the private key in private.rsa */
	gmp_fprintf (priv, "%Zd", d);
	gmp_printf ("private key : %Zd \n", d);
	
	/*  close files */
	fclose(pub);
	fclose(priv);

	/*  clear memory */
	mpz_clears(q, p, n, e, d, phi, tmp, NULL);

	return EXIT_SUCCESS;
}

/*
 * cypher a message using the public key
 */
int chiffre ()
{
	printf ("cypher the message \n");

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

	//gmp_printf("Chiffre, n : %Zd ,e : %Zd, m : %Zd ,c : %Zd \n",n , e, m, c);

	gmp_printf("Chiffre, The plain text message is : %Zd \n", m);

	/*  clear memory */
	mpz_clears(c,n,e,m,NULL);

	return EXIT_SUCCESS;
}


/* x 
void  hash_whirlpool(mpz_t v,FILE* input){
	struct NESSIEstruct w;        
	u8 digest[DIGESTBYTES];
	int i;
	unsigned char c;
	NESSIEinit(&w);
	while(fscanf(input,"%c",&c) != EOF ){

		NESSIEadd(&c, 8, &w);
	}
	NESSIEfinalize(&w, digest);
	mpz_set_ui(v,0UL);
	for(i = 0;i<DIGESTBYTES;i++){
		mpz_mul_2exp (v,v, 8UL);
		mpz_add_ui(v,v,digest[i]);
	}
}
*/


/*
 * decypher the chiffre with the private key
 */
int dechiffre ()
{
	printf ("decypher the message \n");

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

	//gmp_printf("Dechiffre, n : %Zd d : %Zd, m : %Zd ,c : %Zd \n",n , d, m, c);

	gmp_printf("Dechiffre, The plain text message is : %Zd \n", m);

	/*  clear memory */
	mpz_clears(m, c, n,d, NULL);
	return EXIT_SUCCESS;
}

int signature ()
{
	printf("sign the message \n");

	/*  load the private key d to sign */
	FILE* priv = NULL;
	priv = fopen("private.rsa", "r");
	if (priv == NULL)
	{
		printf("Something goes wrong with teh file private.rsa you must have generate \
				rsa keys. \n");
		return EXIT_FAILURE;
	}
	mpz_t d; mpz_init (d);
	gmp_fscanf(priv, "%Zd", d); 
	fclose(priv);

	/*  load the public key n to sign */
	FILE* pub = NULL;
	pub = fopen("public.rsa", "r");
	if (pub == NULL)
	{
		printf("Something goes wrong with teh file public.rsa you must have generate \
				rsa keys. \n");
		return EXIT_FAILURE;
	}
	mpz_t n; mpz_init (n);
	gmp_fscanf(pub, "%Zd", n); 
	fclose(pub);

	/*  load the message to sign */
	FILE* mes = NULL;
	mes = fopen("message.rsa", "r");
	if (mes == NULL)
	{
		printf("Something goes wrong with teh file message.rsa you must write a file \
				message.rsa with the message to signe. \n");
		return EXIT_FAILURE;
	}
	char my_string [MAX_LENGTH] = "";
	fgets(my_string, MAX_LENGTH, mes);
	fclose(mes);

	/*  prepare result for the hash */
	unsigned char result[SHA_DIGEST_LENGTH];

	/*  use openssl SHA1 hash function */
	SHA1(my_string, strlen(my_string), result);


	/*  init h for the hash */
	mpz_t h;
	mpz_init (h);
	mpz_set_ui (h, 0);
	/*  load the hash on h */
	int i;
	for(i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		mpz_mul_2exp (h, h, 8UL);
		mpz_add_ui(h,h,result[i]);
	}
	// FIXME pas le bon mais l idee est là

	gmp_printf ("hash : %Zd \n", h);

	/*  sign the hashed message */
	mpz_t s;
	mpz_init (s);
	mpz_powm (s, h, d, n);

	//gmp_printf("h : %Zd ,s : %Zd \n", h, s);

	/*  print the signature in sign.rsa */
	FILE* sign = NULL;
	sign = fopen("sign.rsa", "w+");
	if (sign == NULL)
	{
		printf ("Something goes wrong with the file sign.rsa. \n");
		return EXIT_FAILURE;
	}
	/*  print the signature in sign.rsa */
	gmp_fprintf (sign, "%Zd", s);
	gmp_printf ("sign : %Zd", s);
	/*  close sign.rsa */
	fclose(sign);

	/*  clear memory */
	mpz_clears(n, d, h, s, NULL);
	return EXIT_SUCCESS;
}

bool verification ()
{
	printf("verify the signed message \n");

	bool res = false;

	/*  load the public key n,e to verify */
	FILE* pub = NULL;
	pub = fopen("public.rsa", "r");
	if (pub == NULL)
	{
		printf("Something goes wrong with teh file public.rsa you must have generate \
				rsa keys. \n");
		return EXIT_FAILURE;
	}
	mpz_t n; mpz_init (n);
	mpz_t e; mpz_init (e);
	gmp_fscanf(pub, "%Zd" "%Zd", n, e); 
	fclose(pub);

	/*  load the signature */
	FILE* sign = NULL;
	sign = fopen("sign.rsa", "r");
	if (sign == NULL)
	{
		printf ("Something goes wrong with the file sign.rsa, you must use sign first. \n");
		return EXIT_FAILURE;
	}
	mpz_t s; mpz_init (s);
	gmp_fscanf(sign, "%Zd", s); 
	/*  close sign.rsa */
	fclose(sign);

	/*  load the message to verify */
	FILE* mes = NULL;
	mes = fopen("message.rsa", "r");
	if (mes == NULL)
	{
		printf("Something goes wrong with teh file message.rsa you must write a file \
				message.rsa with the message to signe. \n");
		return EXIT_FAILURE;
	}
	char my_string [MAX_LENGTH] = "";
	fgets(my_string, MAX_LENGTH, mes);
	fclose(mes);

	/*  h_t = s^e mod n */
	mpz_t h_t; mpz_init (h_t);
	mpz_powm(h_t, s, e, n);

	// TODO h(m) = sha (m)
	// en attendant on admet h(m)=m
	mpz_t h;
	mpz_init (h);
	mpz_set_ui (h, 0);
	
	int i;
	unsigned char result[SHA_DIGEST_LENGTH];

	SHA1(my_string, strlen(my_string), result);

	for(i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		mpz_mul_2exp (h, h, 8UL);
		mpz_add_ui(h,h,result[i]);
	}
	// FIXME pas le bon mais l idee est là

	gmp_printf ("hash : %Zd \n", h);

	if (mpz_cmp (h_t, h) == 0)
	{
		res = true;
	}

	//gmp_printf("h : %Zd ,h_t : %Zd \n", h, h_t);

	/*  clear memory */
	mpz_clears (h_t, s, e, n, h, NULL);

	printf ("verif sign : %i \n", res);

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
	signature();
	verification ();


	return EXIT_SUCCESS;
}
