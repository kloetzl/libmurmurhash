//-----------------------------------------------------------------------------
// libmurmurhash was written by Fabian Klötzl, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "murmurhash.h"

__attribute__((noreturn)) void usage(int exit_code);

int main(int argc, char *argv[])
{
	const char *file_name = NULL;
	int opt = 0;
	while ((opt = getopt(argc, argv, "h")) != -1) {
		if (opt == 'h') {
			usage(EXIT_SUCCESS);
		}
		if (opt == '?') {
			usage(EXIT_FAILURE);
		}
	}

	argc -= optind, argv += optind;

	if (argc == 0) {
		usage(EXIT_FAILURE);
	}

	file_name = argv[0];

	int fd = open(file_name, O_RDONLY);
	if (fd == -1) {
		err(errno, "%s: failed to open file", file_name);
	}

	struct stat sb;
	if (fstat(fd, &sb) == -1) {
		err(errno, "%s: failed to get stats", file_name);
	}

	size_t length = sb.st_size;
	void *data = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
	if (data == (void *)-1) {
		err(errno, "%s: failed to map file", file_name);
	}

	uint32_t hash;
	MurmurHash3_x86_32(data, length, 0, &hash);
	printf("%" PRIx32 "", hash);

	uint32_t hash2[4];
	MurmurHash3_x86_128(data, length, 0, &hash2);
	printf("  ");
	for (int i = 0; i < 4; i++) {
		printf("%" PRIx32 "", hash2[i]);
	}

	uint64_t hash3[2];
	MurmurHash3_x64_128(data, length, 0, &hash3);
	printf("  ");
	for (int i = 0; i < 2; i++) {
		printf("%" PRIx64 "", hash3[i]);
	}

	printf("\n");

	munmap(data, length);
	close(fd);
	return 0;
}

void usage(int exit_code)
{
	static const char str[] = {
		"mmh FILE\n"
		"Compute the murmurhash of a file.\n\n"
		"  -h      Print help\n" //
	};

	fprintf(exit_code == EXIT_SUCCESS ? stdout : stderr, str);
	exit(exit_code);
}
