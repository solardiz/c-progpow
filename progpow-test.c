/*
 * Test code for the plain C implementation of ProgPoW along with Ethash's DAG
 * computation and optional on-disk caching.  Expected output is:
 *
 * Digest = 11f19805c58ab46610ff9c719dcf0a5f18fa2f1605798eef770c47219274767d
 *
 * which matches upstream ProgPoW's test-vectors.md.
 *
 * Copyright (c) 2019 Solar Designer <solar at openwall.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 */

#include <stdio.h>

#include "libethash/ethash.h"
#include "progpow.h"

static void unhex(void *dst, const char *src)
{
	const char *p = src;
	unsigned char *q = dst;
	unsigned char v = 0;

	while (*p) {
		if (*p >= '0' && *p <= '9')
			v |= *p - '0';
		else if (*p >= 'a' && *p <= 'f')
			v |= *p - ('a' - 10);
		else
			break;
		if ((p++ - src) & 1)
			*q++ = v;
		v <<= 4;
	}
}

static void printhex(const void *src, size_t size)
{
	const unsigned char *p = src;

	while (size--)
		printf("%02x", *p++);
}

static int ethash_full_new_callback(unsigned int percent)
{
	fprintf(stderr, "Full DAG init %u%%\r", percent);
	return 0;
}

int main(void)
{
	uint64_t block_number = 30000;
	uint64_t nonce = 0x123456789abcdef0;
	hash32_t header;

	unhex(&header, "ffeeddccbbaa9988776655443322110000112233445566778899aabbccddeeff");

	ethash_light_t light = ethash_light_new(block_number);
	if (!light) {
		fprintf(stderr, "ethash_light_new() failed\n");
		return 1;
	}
	fprintf(stderr, "Light DAG init done\n");

	ethash_full_t full = ethash_full_new(light, ethash_full_new_callback);
	if (!full) {
		fprintf(stderr, "ethash_full_new() failed\n");
		return 1;
	}
	fprintf(stderr, "Full DAG init done\n");

	hash32_t digest = progPowHash(block_number / PROGPOW_PERIOD,
	    nonce, header, ethash_full_dag(full), ethash_full_dag_size(full));

	printf("Digest = ");
	printhex(&digest, sizeof(digest));
	putchar('\n');

	return 0;
}
