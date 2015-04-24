/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  Minor modifications and note by Andy Sayler (2012) <www.andysayler.com>

  Source: fuse-2.8.7.tar.gz examples directory
  http://sourceforge.net/projects/fuse/files/fuse-2.X/

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags` fusexmp.c -o fusexmp `pkg-config fuse --libs`

  Note: This implementation is largely stateless and does not maintain
        open file handels between open and release calls (fi->fh).
        Instead, files are opened and closed as necessary inside read(), write(),
        etc calls. As such, the functions that rely on maintaining file handles are
        not implmented (fgetattr(), etc). Those seeking a more efficient and
        more complete implementation may wish to add fi->fh support to minimize
        open() and close() calls and support fh dependent functions.

*/

#define FUSE_USE_VERSION 28
#define HAVE_SETXATTR

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include "aes-crypt.h"
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#define ENC_PARAMS ((EncryptionParams*) fuse_get_context()->private_data)

typedef struct {
	char* key;
	char* dir;
} EncryptionParams;

char* get_path(const char *path) {
	int len = strlen(path) + strlen(ENC_PARAMS->dir) + 1;
	char* ret = malloc(sizeof(char) * len);
	strcpy(ret, ENC_PARAMS->dir);
	strcat(ret, path);
	return ret;
}

static int xmp_getattr(const char *fpath, struct stat *stbuf) {
	char *path = get_path(fpath);
	int res;
	res = lstat(path, stbuf);
	if (res == -1) {
		return -errno;
	}
	return 0;
}

static int xmp_access(const char *fpath, int mask) {
	char *path = get_path(fpath);
	int res;
	res = access(path, mask);
	if (res == -1) {
		return -errno;
	}
	
	return 0;
}

static int xmp_readlink(const char *fpath, char *buf, size_t size) {
	char *path = get_path(fpath);
	int res;
	res = readlink(path, buf, size - 1);
	if (res == -1) {
		return -errno;
	}
	buf[res] = '\0';
	return 0;
}

static int xmp_readdir(const char *fpath, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	char *path = get_path(fpath);
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mknod(const char *fpath, mode_t mode, dev_t rdev) {
	char *path = get_path(fpath);
	int res;

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *fpath, mode_t mode) {
	char *path = get_path(fpath);
	int res;

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *fpath) {
	char *path = get_path(fpath);
	int res;

	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *fpath) {
	char *path = get_path(fpath);
	int res;

	res = rmdir(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to) {
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to) {
	int res;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to) {
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *fpath, mode_t mode) {
	char *path = get_path(fpath);
	int res;

	res = chmod(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *fpath, uid_t uid, gid_t gid) {
	char *path = get_path(fpath);
	int res;

	res = lchown(path, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *fpath, off_t size) {
	char *path = get_path(fpath);
	int res;

	res = truncate(path, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *fpath, const struct timespec ts[2]) {
	char *path = get_path(fpath);
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(path, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *fpath, struct fuse_file_info *fi) {
	char *path = get_path(fpath);
	int res;

	res = open(path, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

int is_encrypted(const char *path) {
	int ret;
	char xattr_val[5];
	getxattr(path, "user.enc", xattr_val, sizeof(char)*5);
	ret = (strcmp(xattr_val, "true") == 0);
	return ret;
}

/*static int file_size(FILE *file) {
    struct stat st;
    if (fstat(fileno(file), &st) == 0) {
        return st.st_size;
	}
    return -1;
}*/

static int xmp_read(const char *fpath, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	int fd;
	int res;
	(void) fi;
	FILE *file, *tempfile;
	const char *file_path = get_path(fpath);
	file = fopen(file_path, "r");
	tempfile = tmpfile();
	if (file == NULL || tempfile == NULL) {
		fprintf(stderr, "Failed to open file or create temp file.\n");
		return -errno;
	}
	// Check if file is encrypted
	fd = is_encrypted(file_path) ? 0 : -1;
	if (do_crypt(file, tempfile, fd, ENC_PARAMS->key) == 0) {
		return -errno;
	}
	fflush(tempfile);
	fseek(tempfile, offset, SEEK_SET);
	res = fread(buf, 1, size, tempfile);
	if (res == -1) {
		res = -errno;
	}
	fclose(tempfile);
	fclose(file);
	return res;
}

static int xmp_write(const char *fpath, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	int fd;
	int res;
	(void) fi;
	FILE *file, *tempfile;
	int tempfile_desc;
	const char *file_path = get_path(fpath);
	file = fopen(file_path, "r+");
	tempfile = tmpfile();
	tempfile_desc = fileno(tempfile);
	if (file == NULL || tempfile == NULL) {
		fprintf(stderr, "Failed to open file or create temp file.\n");
		return -errno;
	}
	if (xmp_access(fpath, R_OK) == 0 && size > 0) {
		fd = is_encrypted(file_path) ? 0 : -1;
		if (do_crypt(file, tempfile, fd, ENC_PARAMS->key) == 0) {
			return --errno;
		}
		rewind(file);
		rewind(tempfile);
	}
	res = pwrite(tempfile_desc, buf, size, offset);
	if (res == -1) {
		res = -errno;
	}
	fd = is_encrypted(file_path) ? 0 : -1;
	if (do_crypt(tempfile, file, fd, ENC_PARAMS->key) == 0) {
		return -errno;
	}
	fclose(tempfile);
	fclose(file);
	return res;
}

static int xmp_statfs(const char *fpath, struct statvfs *stbuf) {
	char *path = get_path(fpath);
	int res;
	res = statvfs(path, stbuf);
	if (res == -1) {
		return -errno;
	}
	return 0;
}

int add_encrypted_attr(const char *path) {
	int ret;
	int setxattr_ret;
	setxattr_ret = setxattr(path, "user.enc", "true", (sizeof(char)*5), 0);
	ret = setxattr_ret == 0;
	return ret;
}

static int xmp_create(const char* fpath, mode_t mode, struct fuse_file_info* fi) {
	char *path = get_path(fpath);
	(void) fi;
    int res;
    res = creat(path, mode);
    if (res == -1) {
		return -errno;
	}
    close(res);
    if (!add_encrypted_attr(path)) {
		return -errno;
	}
    return 0;
}

static int xmp_release(const char *fpath, struct fuse_file_info *fi) {
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */
	(void) fpath;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *fpath, int isdatasync, struct fuse_file_info *fi) {
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) fpath;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *fpath, const char *name, const char *value, size_t size, int flags) {
	char *path = get_path(fpath);
	int res = lsetxattr(path, name, value, size, flags);
	if (res == -1) {
		return -errno;
	}
	return 0;
}

static int xmp_getxattr(const char *fpath, const char *name, char *value, size_t size) {
	char *path = get_path(fpath);
	int res = lgetxattr(path, name, value, size);
	if (res == -1) {
		return -errno;
	}
	return res;
}

static int xmp_listxattr(const char *fpath, char *list, size_t size) {
	char *path = get_path(fpath);
	int res = llistxattr(path, list, size);
	if (res == -1) {
		return -errno;
	}
	return res;
}

static int xmp_removexattr(const char *fpath, const char *name) {
	char *path = get_path(fpath);
	int res = lremovexattr(path, name);
	if (res == -1) {
		return -errno;
	}
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create     = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
#ifdef HAVE_SETXATTR
	.setxattr	= xmp_setxattr,
	.getxattr	= xmp_getxattr,
	.listxattr	= xmp_listxattr,
	.removexattr	= xmp_removexattr,
#endif
};

int main(int argc, char *argv[]) {
	// ./pa5-encfs <Key Phrase> <Mirror Directory> <Mount Point>.
	umask(0);
	if (argc < 4) {
		// not enough args
		fprintf(stderr, "Not enough arguments.\n");
		fprintf(stderr, "Usage: pa5-encfs <Key Phrase> <Mirror Directory> <Mount Point>\n");
		return 1;
	}
	EncryptionParams* enc_params = NULL;
	enc_params = malloc(sizeof(EncryptionParams));
	if (enc_params == NULL) {
		fprintf(stderr, "Memory allocation error.\n");
		return 1;
	}
	// 0 (1:key) (2:mirror) (3:mount)
	enc_params->key = strdup(argv[1]);
	enc_params->dir = strdup(argv[2]);
	// Hack
	// leave arg0 unchanged
	argv[1] = argv[3];
	argv[2] = NULL;
	argv[3] = NULL;
	return fuse_main(argc - 2, argv, &xmp_oper, enc_params);
}
