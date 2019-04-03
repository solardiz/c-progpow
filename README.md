# Plain C implementation of ProgPoW

This is a plain C implementation of ProgPoW.  The progpow.c file is created by
merging and revising pieces of (pseudo)code from different files in upstream
ProgPoW (README.md, libethash-cl/CLMiner_kernel.cl, libprogpow/ProgPow.h,
libethash-cuda/cuda_helper.h) and go-ethereum/consensus/ethash/progpow.go
(the latter for clz() and popcount() only, which are trivial).

The uses of C++ references have been removed, but some structs are still
passed and returned by value (allowed in C, just uncommon and inefficient).

The files under libethash are from https://github.com/ethereum/ethash and are
used here to initialize the DAG for ProgPoW, which is the same as Ethash's.

## Testing

On a Unix-like system, invoke `make` to build the progpow-test program.  When
run, it currently computes and caches on disk Ethash's ~1 GB DAG for block 30k,
then computes ProgPoW using a set of nonce and header values found in upstream
ProgPoW's test-vectors.md.  The expected output is:

```
Light DAG init done
Full DAG init done
ProgPoW version 0.9.2
Block   30000
Digest  11f19805c58ab46610ff9c719dcf0a5f18fa2f1605798eef770c47219274767d
Merge   36864 total (8192 12288 5120 11264)
Math    20480 total (2048 4096 2048 1024 2048 1024 4096 1024 1024 2048 0)
```

where the digest matches upstream ProgPoW's test-vectors.md.

To compute the proposed ProgPoW version 0.9.3 instead, uncomment
`-DPROGPOW_VERSION=93` in `Makefile`.  The expected output becomes:

```
Light DAG init done
Full DAG init done
ProgPoW version 0.9.3
Block   30000
Digest  6018c151b0f9895ebe44a4ca6ce2829e5ba6ae1a68a4ccd05a67ac01219655c1
Merge   33792 total (8192 7168 10240 8192)
Math    18432 total (0 2048 0 5120 5120 0 0 0 4096 0 2048)
```

## Known issues

This implementation of ProgPoW is effectively untested on big-endian because
Ethash's included implementation of Keccak assumes little-endian (and more):
https://github.com/coruus/keccak-tiny/issues/7

The included implementation of Ethash DAG initialization contains currently
disabled code that would use SSE4.1 intrinsics for some speedup.  That code
is currently broken (at least) because the same implementation misaligns the
data buffer (shifting it from the properly aligned mmap()'ed region by the
8-byte magic value, thereby never achieving the needed 16-byte alignment).

## License

Both Ethereum's ethash and upstream ProgPoW were under GPLv3, which is how we
get infected by that license.  Some source files written from scratch (the
test program and the Makefile) are also available under relaxed terms specified
in those files.

## Authors, maintainer, and contact information

This plain C implementation has been put together from the above sources and is
maintained by:

Solar Designer \<solar at openwall.com\>

Ethash and ProgPoW are by their many corresponding authors.  Comments in the
Ethash source files under libethash specify several of their authors, but there
are probably more as not every committer updated those comments.

The snippets of ProgPoW (pseudo)code included in progpow.c are apparently by
ifdefelse, Radix Pi, and maybe other ProgPoW contributors.  Ideally, proper
copyright statements need to be identified and added, but upstream ProgPoW
currently lacks those as well.

## Related work

Other implementations of ProgPoW exist in:

- https://github.com/ifdefelse/ProgPOW (C++ with mandatory CUDA or OpenCL)
- https://github.com/ifdefelse/go-ethereum (Go)
- https://github.com/chfast/ethash (C++)

Those are larger source trees that are not as focused on ProgPoW itself/alone
as this one is.
