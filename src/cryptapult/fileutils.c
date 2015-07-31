#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <sodium.h>

long file_readwhole(char* filename, unsigned char **buf) {
	unsigned char *content;
	long fsize;
	FILE *f = fopen(filename, "rb");
	if(f == NULL) {
		fprintf(stderr, "Failure reading '%s' (non existent file?)\n",
				filename);
		return -1;
	}
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	content = sodium_allocarray(fsize, sizeof(char));
	if(content == NULL) {
		fprintf(stderr, "Failure reading '%s' (not enough memory?)\n",
				filename);
		return -2;
	}
	size_t rbytes = fread(content, sizeof(char), fsize, f);
	if(rbytes != (size_t) fsize) {
		fprintf(stderr, "Failure reading '%s' (%ld bytes read instead of %ld)\n",
				filename, rbytes, fsize);
		return -3;
	}
	fclose(f);

	sodium_mprotect_readonly(content);
	*buf = content;
	return fsize;
}

