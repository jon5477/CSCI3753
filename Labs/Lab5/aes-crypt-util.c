/* aes-crypt.c
 * AES encryption demo program using OpenSSL EVP API via local aes-crypt library
 *
 * See aes-crypt.h and aes-crypt.c for more details
 *
 * By Andy Sayler (www.andysayler.com)
 * Created  04/17/12
 * Modified 04/18/12
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "aes-crypt.h"

int main(int argc, char **argv)
{
    
    /* Local vars */
    int action = 0;
    int ifarg;
    int ofarg;
    FILE* inFile = NULL;
    FILE* outFile = NULL;
    char* key_str = NULL;

    /* Check General Input */
    if(argc < 3){
	fprintf(stderr, "usage: %s %s\n", argv[0],
		"<type> <opt key phrase> <in path> <out path>");
	exit(EXIT_FAILURE);
    }

    /* Encrypt Case */
    if(!strcmp(argv[1], "-e")){
	/* Check Args */
	if(argc != 5){
	    fprintf(stderr, "usage: %s %s\n", argv[0],
		    "-e <key phrase> <in path> <out path>");
	    exit(EXIT_FAILURE);
	}
	/* Set Vars */
	key_str = argv[2];
	ifarg = 3;
	ofarg = 4;
	action = 1;
    }
    /* Decrypt Case */
    else if(!strcmp(argv[1], "-d")){
	/* Check Args */
	if(argc != 5){
	    fprintf(stderr, "usage: %s %s\n", argv[0],
		    "-d <key phrase> <in path> <out path>");
	    exit(EXIT_FAILURE);
	}
	/* Set Vars */
	key_str = argv[2];
	ifarg = 3;
	ofarg = 4;
	action = 0;
    }
    /* Pass-Through (Copy) Case */
    else if(!strcmp(argv[1], "-c")){
	/* Check Args */
	if(argc != 4){
	    fprintf(stderr, "usage: %s %s\n", argv[0],
		    "-c <in path> <out path>");
	    exit(EXIT_FAILURE);
	}
	/* Set Vars */
	key_str = NULL;
	ifarg = 2;
	ofarg = 3;
	action = -1;
    }
    /* Bad Case */
    else {
	fprintf(stderr, "Unkown action\n");
	exit(EXIT_FAILURE);
    }

    /* Open Files */
    inFile = fopen(argv[ifarg], "rb");
    if(!inFile){
	perror("infile fopen error");
	return EXIT_FAILURE;
    }
    outFile = fopen(argv[ofarg], "wb+");
    if(!outFile){
	perror("outfile fopen error");
	return EXIT_FAILURE;
    }

    /* Perform do_crpt action (encrypt, decrypt, copy) */
    if(!do_crypt(inFile, outFile, action, key_str)){
	fprintf(stderr, "do_crypt failed\n");
    }

    /* Cleanup */
    if(fclose(outFile)){
        perror("outFile fclose error\n");
    }
    if(fclose(inFile)){
	perror("inFile fclose error\n");
    }

    return EXIT_SUCCESS;
}
