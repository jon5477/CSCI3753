/* aes-crypt.c
 * High level function interface for performing AES encryption on FILE pointers
 * Uses OpenSSL libcrypto EVP API
 *
 * By Andy Sayler (www.andysayler.com)
 * Created  04/17/12
 * Modified 04/18/12
 *
 * Derived from OpenSSL.org EVP_Encrypt_* Manpage Examples
 * http://www.openssl.org/docs/crypto/EVP_EncryptInit.html#EXAMPLES
 *
 * With additional information from Saju Pillai's OpenSSL AES Example
 * http://saju.net.in/blog/?p=36
 * http://saju.net.in/code/misc/openssl_aes.c.txt
 *
 */

#include "aes-crypt.h"

#define BLOCKSIZE 1024
#define FAILURE 0
#define SUCCESS 1

extern int do_crypt(FILE* in, FILE* out, int action, char* key_str){
    /* Local Vars */

    /* Buffers */
    unsigned char inbuf[BLOCKSIZE];
    int inlen;
    /* Allow enough space in output buffer for additional cipher block */
    unsigned char outbuf[BLOCKSIZE + EVP_MAX_BLOCK_LENGTH];
    int outlen;
    int writelen;

    /* OpenSSL libcrypto vars */
    EVP_CIPHER_CTX ctx;
    unsigned char key[32];
    unsigned char iv[32];
    int nrounds = 5;
    
    /* tmp vars */
    int i;

    /* Setup Encryption Key and Cipher Engine if in cipher mode */
    if(action >= 0){
	if(!key_str){
	    /* Error */
	    fprintf(stderr, "Key_str must not be NULL\n");
	    return 0;
	}
	/* Build Key from String */
	i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), NULL,
			   (unsigned char*)key_str, strlen(key_str), nrounds, key, iv);
	if (i != 32) {
	    /* Error */
	    fprintf(stderr, "Key size is %d bits - should be 256 bits\n", i*8);
	    return 0;
	}
	/* Init Engine */
	EVP_CIPHER_CTX_init(&ctx);
	EVP_CipherInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv, action);
    }    

    /* Loop through Input File*/
    for(;;){
	/* Read Block */
	inlen = fread(inbuf, sizeof(*inbuf), BLOCKSIZE, in);
	if(inlen <= 0){
	    /* EOF -> Break Loop */
	    break;
	}
	
	/* If in cipher mode, perform cipher transform on block */
	if(action >= 0){
	    if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
		{
		    /* Error */
		    EVP_CIPHER_CTX_cleanup(&ctx);
		    return 0;
		}
	}
	/* If in pass-through mode. copy block as is */
	else{
	    memcpy(outbuf, inbuf, inlen);
	    outlen = inlen;
	}

	/* Write Block */
	writelen = fwrite(outbuf, sizeof(*outbuf), outlen, out);
	if(writelen != outlen){
	    /* Error */
	    perror("fwrite error");
	    EVP_CIPHER_CTX_cleanup(&ctx);
	    return 0;
	}
    }
    
    /* If in cipher mode, handle necessary padding */
    if(action >= 0){
	/* Handle remaining cipher block + padding */
	if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
	    {
		/* Error */
		EVP_CIPHER_CTX_cleanup(&ctx);
		return 0;
	    }
	/* Write remainign cipher block + padding*/
	fwrite(outbuf, sizeof(*inbuf), outlen, out);
	EVP_CIPHER_CTX_cleanup(&ctx);
    }
    
    /* Success */
    return 1;
}
