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

static uint32_t bswap(uint32_t a)
{
	a = (a << 16) | (a >> 16);
	return ((a & 0x00ff00ff) << 8) | ((a >> 8) & 0x00ff00ff);
}

static void unhex(hash32_t *dst, const char *src)
{
	const char *p = src;
	uint32_t *q = dst->uint32s;
	uint32_t v = 0;

	while (*p && q <= &dst->uint32s[7]) {
		if (*p >= '0' && *p <= '9')
			v |= *p - '0';
		else if (*p >= 'a' && *p <= 'f')
			v |= *p - ('a' - 10);
		else
			break;
		if (!((++p - src) & 7))
			*q++ = bswap(v);
		v <<= 4;
	}
}

static void printhex(const hash32_t *src)
{
	const uint32_t *p = src->uint32s;

	while (p <= &src->uint32s[7])
		printf("%08x", bswap(*p++));
}

static int ethash_full_new_callback(unsigned int percent)
{
	fprintf(stderr, "Full DAG init %u%%\r", percent);
	return 0;
}

int main(void)
{
#if 1
	uint64_t block_number = 30000;
	uint64_t nonce = 0x123456789abcdef0;
	hash32_t header;
	unhex(&header, "ffeeddccbbaa9988776655443322110000112233445566778899aabbccddeeff");
#else
	uint64_t block_number = 10000000;
	uint64_t nonce = 0x005e30899481055e;
	hash32_t header;
	unhex(&header, "efda178de857b2b1703d8d5403bd0f848e19cff5c50ba5c0d6210ddb16250ec3");
#endif

	static progPowStats_t stats; /* assumed zero-initialized */
	progPowStats = &stats;

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
	printhex(&digest);
	putchar('\n');

	unsigned int i;
	printf("Merge %lu total ", stats.merge_total);
	for (i = 0; i < 4; i++)
		printf("%c%lu", i ? ' ' : '(', stats.merge[i]);
	printf(")\nMath  %lu total ", stats.math_total);
	for (i = 0; i < 11; i++)
		printf("%c%lu", i ? ' ' : '(', stats.math[i]);
	puts(")");

	return 0;
}
