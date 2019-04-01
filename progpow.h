#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// blocks before changing the random program
#define PROGPOW_PERIOD          50

/* hash32_t is from libethash-cl/CLMiner_kernel.cl */
typedef struct
{
    uint32_t uint32s[32 / sizeof(uint32_t)];
} hash32_t;

hash32_t progPowHash(
    const uint64_t prog_seed, // value is (block_number/PROGPOW_PERIOD)
    const uint64_t nonce,
    const hash32_t header,
    const uint32_t *dag, // gigabyte DAG located in framebuffer - the first portion gets cached
    const uint64_t dag_bytes);

typedef struct
{
    unsigned long merge_total;
    unsigned long merge[4];
    unsigned long math_total;
    unsigned long math[11];
} progPowStats_t;

extern progPowStats_t *progPowStats;

#ifdef __cplusplus
}
#endif
