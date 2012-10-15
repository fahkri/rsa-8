This program is a simple implementation of the RSA algorithm.

Dependencies of this program :
	this program depend on the GMP libraray. (aptitude install libgmp-dev)
	this programme depend on the OpenSSL library. (aptitude install libssl-dev)

To use this program
	make
	(chmod +x rsa)
	./rsa (will list you all option available)

use of rsa : 
	rsa generate : generate the rsa keys 
	rsa cypher   : cypher the file message.rsa 
	rsa decipher : decipher the 	file cypher.rsa 
	rsa sign     : sign the file message.rsa 
	rsa verify   : verify the file sign.rsa according to message.rsa

The file message.rsa must be a number without any space (1234567890 for instance) for encryption (and decryption by extension) (the encryption of a classic text message is a future feature).
However a classic text message can be signed and verified.

The SHA1 hash function as knwon issues do not use this code for a real application there is no real security 
Use SHA2 (SHA3 soon) instead.

The Miller-Rabin algorithm was inspired from :
	http://rosettacode.org/wiki/Miller-Rabin_primality_test

Inside the code (see commentary for more informations) :
	we generate rsa keys (public.rsa & private.rsa) using Miller-Rabin algorithm for probably prime number.
	we cypher the rsa message (message.rsa) using rsa cypher algorithm.
	we decipher the rsa cypher message (cypher.rsa) using rsa decipher algorithm.
	we sign the rsa message using rsa sign algorithm.
	we verify the signed message (sign.rsa) using rsa sign algorithm.

