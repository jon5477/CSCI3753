/* aes-crypt.c
 * Extended attribute manipulation demo program
 *
 * See aes-crypt.h and aes-crypt.c for more details
 *
 * By Andy Sayler (www.andysayler.com)
 * Created  04/17/12
 * Modified 04/18/12
 *
 * Note: The most common issues here are neglecting the necessary namespace
 *       prefixes on attribute names, or using a file system that does not
 *	 support extended attributes. For namespace info, see
 *       http://www.freedesktop.org/wiki/CommonExtendedAttributes.
 *       For info on enabling xattr on EXT file systems, see
 *	 http://wiki.kaspersandberg.com/doku.php?id=howtos:xattr#getting_ea_s_enabled
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/xattr.h>
#include <linux/xattr.h>
#include <sys/types.h>

#define CMDLS "-l"
#define CMDSET "-s"
#define CMDGET "-g"
#define CMDREM "-r"
#define USAGE_GENERAL "<command> <opt args> <path>"
#define USAGE_LIST CMDLS " <path>"
#define USAGE_SET CMDSET " <Attr Name> <Attr Value> <path>"
#define USAGE_GET CMDGET " <Attr Name> <path>"
#define USAGE_REM CMDREM " <Attr Name> <path>"

#ifdef linux
/* Linux is missing ENOATTR error, using ENODATA instead */
#define ENOATTR ENODATA
#endif

/* Add namespace defintion for older kernels. Normally included in linux/xattr.h */
#ifndef XATTR_USER_PREFIX
#define XATTR_USER_PREFIX "user."
#define XATTR_USER_PREFIX_LEN (sizeof (XATTR_USER_PREFIX) - 1)
#endif

static void printUsageGeneral(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_GENERAL);    
}

static void printUsageList(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_LIST);    
}

static void printUsageSet(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_SET);    
}

static void printUsageGet(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_GET);    
}

static void printUsageRem(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_REM);    
}

int main(int argc, char* argv[]){

    /* Local vars */
    char* lst = NULL;
    char* chr = NULL;
    char* start = NULL;
    char* tmpstr = NULL;
    char* tmpval = NULL;
    ssize_t cnt = 0;
    ssize_t lstsize = 0;
    ssize_t valsize = 0;

    /* Check base input */
    if(argc < 2){
	printUsageGeneral(argv[0]);
	exit(EXIT_FAILURE);
    }
    
    /* Parse Command */
    if(!strcmp(argv[1], CMDLS)){
	/* List Case */
	/* Check proper input */
	if(argc != 3){
	    printUsageList(argv[0]);
	    exit(EXIT_FAILURE);
	}
	/* Call xattr list to get size */
	lstsize = listxattr(argv[2], NULL, 0);
	if(lstsize < 0){
	    perror("listxattr error");
	    fprintf(stderr, "path  = %s\n", argv[2]);
	    exit(EXIT_FAILURE);
	}
	/* Malloc space for list items */
	lst = malloc(sizeof(*lst)*lstsize);
	if(!lst){
	    perror("malloc of 'lst' error");
	    exit(EXIT_FAILURE);
	}
	/* Call xattr list to get data */
	lstsize = listxattr(argv[2], lst, lstsize);
	if(lstsize < 0 || !lst){
	    perror("listxattr error");
	    fprintf(stderr, "path  = %s\n", argv[2]);
	    exit(EXIT_FAILURE);
	}
	/* Tokenize null seperated array on \0 */
	chr = start = lst;
	cnt = 0;
	while(cnt < lstsize){
	    /* Print attribute names, one per line */
	    fprintf(stdout, "%s\n", start);
	    while(*chr != '\0'){
		chr++;
		cnt++;
	    }
	    start = ++chr;
	    cnt++;
	}
    }
    else if(!strcmp(argv[1], CMDSET)){
	/* Set Case */
	/* Check proper input */
	if(argc != 5){
	    printUsageSet(argv[0]);
	    exit(EXIT_FAILURE);
	}
	/* Append necessary 'user.' namespace prefix to beginning of name string */
	tmpstr = malloc(strlen(argv[2]) + XATTR_USER_PREFIX_LEN + 1);
	if(!tmpstr){
	    perror("malloc of 'tmpstr' error");
	    exit(EXIT_FAILURE);
	}
	strcpy(tmpstr, XATTR_USER_PREFIX);
	strcat(tmpstr, argv[2]);
	/* Set attribute */
	if(setxattr(argv[4], tmpstr, argv[3], strlen(argv[3]), 0)){
	    perror("setxattr error");
	    fprintf(stderr, "path  = %s\n", argv[4]);
	    fprintf(stderr, "name  = %s\n", tmpstr);
	    fprintf(stderr, "value = %s\n", argv[3]);
	    fprintf(stderr, "size  = %zd\n", strlen(argv[3]));
	    exit(EXIT_FAILURE);
	}
	/* Cleanup */
	free(tmpstr);
    }
    else if(!strcmp(argv[1], CMDGET)){
	/* Get Case */
	/* Check proper input */
	if(argc != 4){
	    printUsageGet(argv[0]);
	    exit(EXIT_FAILURE);
	}
	/* Append necessary 'user.' namespace prefix to beginning of name string */
	tmpstr = malloc(strlen(argv[2]) + XATTR_USER_PREFIX_LEN + 1);
	if(!tmpstr){
	    perror("malloc of 'tmpstr' error");
	    exit(EXIT_FAILURE);
	}
	strcpy(tmpstr, XATTR_USER_PREFIX);
	strcat(tmpstr, argv[2]);
	/* Get attribute value size */
	valsize = getxattr(argv[3], tmpstr, NULL, 0);
	if(valsize < 0){
	    if(errno == ENOATTR){
		fprintf(stdout, "No %s attribute set on %s\n", tmpstr, argv[3]);
		free(tmpstr);
		return EXIT_SUCCESS;
	    }
	    else{
		perror("getxattr error");
		fprintf(stderr, "path  = %s\n", argv[3]);
		fprintf(stderr, "name  = %s\n", tmpstr);
		fprintf(stderr, "value = %s\n", "NULL");
		fprintf(stderr, "size  = %zd\n", valsize);
		exit(EXIT_FAILURE);
	    }
	}
	/* Malloc Value Space */
	tmpval = malloc(sizeof(*tmpval)*(valsize+1));
	if(!tmpval){
	    perror("malloc of 'tmpval' error");
	    exit(EXIT_FAILURE);
	}
	/* Get attribute value */
	valsize = getxattr(argv[3], tmpstr, tmpval, valsize);
	if(valsize < 0){
	    if(errno == ENOATTR){
		fprintf(stdout, "No %s attribute set on %s\n", tmpstr, argv[3]);
		free(tmpval);
		free(tmpstr);
		return EXIT_SUCCESS;
	    }
	    else{
		perror("getxattr error");
		fprintf(stderr, "path  = %s\n", argv[3]);
		fprintf(stderr, "name  = %s\n", tmpstr);
		fprintf(stderr, "value = %s\n", tmpval);
		fprintf(stderr, "size  = %zd\n", valsize);
		exit(EXIT_FAILURE);
	    }
	}
	
	/* Print Value */
	tmpval[valsize] = '\0';
	fprintf(stdout, "%s = %s\n", tmpstr, tmpval);
	
	/* Clean Up */
	free(tmpval);
	free(tmpstr);
    }
    else if(!strcmp(argv[1], CMDREM)){
	/* Remove Case */
	/* Check proper input */
	if(argc != 4){
	    printUsageRem(argv[0]);
	    exit(EXIT_FAILURE);
	}
	/* Append necessary 'user.' namespace prefix to beginning of name string */
	tmpstr = malloc(strlen(argv[2]) + XATTR_USER_PREFIX_LEN + 1);
	if(!tmpstr){
	    perror("malloc of 'tmpstr' error");
	    exit(EXIT_FAILURE);
	}
	strcpy(tmpstr, XATTR_USER_PREFIX);
	strcat(tmpstr, argv[2]);
	/* Remove Attr */
	if(removexattr(argv[3], tmpstr)){
	    if(errno == ENOATTR){
		fprintf(stdout, "No %s attribute set on %s\n", tmpstr, argv[3]);
		free(tmpstr);
		return EXIT_SUCCESS;
	    }
	    else{
		perror("removexattr error");
		fprintf(stderr, "path  = %s\n", argv[3]);
		fprintf(stderr, "name  = %s\n", tmpstr);
		exit(EXIT_FAILURE);
	    }
	}
	/* Cleanup */
	free(tmpstr);
    }
    else{
	/* Bad Case */
	fprintf(stderr, "Unrecognized option\n");
	exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
