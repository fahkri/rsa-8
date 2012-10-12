rsa : rsa.c rsa.h
	gcc -g -Wall -lgmp -lssl rsa.c -o rsa

