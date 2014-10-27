# ABOUT #

These are specially optimized primitives for [Poly1305](http://cr.yp.to/mac.html), 
a "secret-key message-authentication code suitable for a wide variety of applications". 

A sample Poly1305 implementation utilizing the primitives which provides one-pass and
incremental support with CPU dispatching is included.

All assembler is PIC safe.

All pointers should _probably_ be word aligned. I haven't implemented alignment checking on messages yet, 
I don't know if using misaligned pointers is common or not.

# Usage #

See: [http://nacl.cace-project.eu/onetimeauth.html](http://nacl.cace-project.eu/onetimeauth.html), in specific, 
slightly plagiarized:

The poly1305_auth function, viewed as a function of the message for a uniform random key, is 
designed to meet the standard notion of unforgeability after a single message. After the sender 
authenticates one message, an attacker cannot find authenticators for any other messages.

The sender **MUST NOT** use poly1305_auth to authenticate more than one message under the same key.
Authenticators for two messages under the same key should be expected to reveal enough information 
to allow forgeries of authenticators on other messages.

# Compiling #

    sh configure.sh (--compiler [*gcc,clang,icc,..])
    [gcc,clang,icc,..] poly1305.c poly1305_extensions.S -O3 -o poly1305.o (-fPIC)

## Configuring by Hand ##

configure.sh creates poly1305_config.inc, which indicates what the compiler supports. Available options are

* `#define POLY1305_EXT_REF_8`, support for 8x8=16 bit multiplications and 32 bit additions
* `#define POLY1305_EXT_REF_32`, support for 32x32=64 bit multiplications and 64 bit additions
* `#define POLY1305_EXT_X86`, support for 32 bit x86 instructions
* `#define POLY1305_EXT_X86_64`, support for 64 bit x86 instructions
* `#define POLY1305_EXT_SSE2`, support for SSE2
* `#define POLY1305_EXT_AVX`, support for AVX
* `#define POLY1305_EXT_AVX2`, support for AVX2

# Calling #

There are two ways to use the code, through the sample implmentation in [poly1305.c](poly1305.c) or
directly with the platform specific versions.

## Sample Implementation ##

`int poly1305_detect(void);`

Before using the sample implementation, call `poly1305_detect` to determine the best implementation for the current
CPU. `poly1305_detect` additionally calls `poly1305_power_on_self_test` for each implementation to verify they are
working properly.

`poly1305_detect` returns `1` if everything is working, or `0` if there is a failure.

The sample implementation provides two ways to calculate authenticators.

### 1. Single Call version ###

`void poly1305_auth(unsigned char mac[16], const unsigned char *m, size_t bytes, const poly1305_key *key);`

where `mac` is the buffer which receives the 16 byte authenticator,

`m` is a pointer to the message to be processed,

`bytes` is the number of bytes in the message, and

`key` is the poly1305 key that is **only used for this message and is discarded immediately after**.

### 2. Incremental version ###

`poly1305_context` is declared in [poly1305.h](poly1305.h) and is an opaque structure large enough to support 
every underlying platform specific implementation. It has no alignment requirements, but **must not** be copied
as it is aligned internally and a different base address will result in a different alignment.

`void poly1305_init(poly1305_context *ctx, const poly1305_key *key);`
`void poly1305_init_ext(poly1305_context *ctx, const poly1305_key *key, unsigned long long bytes_hint);`

where

`key` is the poly1305 key that is **only used for this message and is discarded immediately after**,

and, when using `poly1305_init_ext`, `bytes_hint` is the total length of the message that will be processed. This allows
the underlying implementation to skip some pre-calculations if the message will not be long enough to warrant them.


`void poly1305_update(poly1305_context *ctx, const unsigned char *m, size_t bytes);`

where `m` is a pointer to the message fragment to be processed, and

`bytes` is the length of the message fragment


`void poly1305_finish(poly1305_context *ctx, unsigned char mac[16]);`

where `mac` is the buffer which receives the 16 byte authenticator. After calling finish, the underlying
implementation will zero out `ctx`.

## Platform Specific Implementations ##

The platform specific implementations provide a single call function, and a set of functions which are used to build
an incremental Poly1305. Each implementation suffixes all functions, e.g. poly1305_auth_ref, poly1305_auth_x86, 
poly1305_avx, etc.

### 1. Single Call version ###

`void poly1305_auth_xxx(unsigned char mac[16], const unsigned char *m, size_t bytes, const poly1305_key *key);`

where `mac` is the buffer which receives the 16 byte authenticator,

`m` is a pointer to the message to be processed,

`bytes` is the number of bytes in the message, and

`key` is the poly1305 key that is **only used for this message and is discarded immediately after**.

### 2. Incremental functions ###

The platform specific incremental functions take a void pointer to a context, which varies in size and alignment
requirements:

* 8 bit reference: 52 bytes, no alignment requirement
* 32 bit reference: (14 * sizeof(unsigned long)) + 1 byte, sizeof(unsigned long) byte alignment requirement
* SSE2: 240 bytes + sizeof(size_t), 16 byte alignment
* AVX: 240 bytes + sizeof(size_t), 16 byte alignment
* AVX2: 320 bytes + sizeof(size_t), 32 byte alignment

The cover-all choice for x86 is a 328 byte buffer with at least 32 byte alignment.

`size_t poly1305_block_size_xxx(void);`

returns the block size of the underlying implementation

`void poly1305_init_ext_xxx(void *ctx, const poly1305_key *key, unsigned long long bytes_hint);`

where `key` is the poly1305 key that is **only used for this message and is discarded immediately after**,

and, `bytes_hint` is the total length of the message that will be processed. Implementations that don't use
precomputations are free to ignore this.

`void poly1305_blocks_xxx(void *ctx, const unsigned char *m, size_t bytes)`

where `m` is a pointer to the message fragment to be processed, and

`bytes` is the number of bytes in the message fragment. `bytes` **must** be a multiple of the block size, it 
is not possible to feed arbitrary length fragments in

`void poly1305_finish_ext_xxx(void *ctx, const unsigned char *m, size_t remaining, unsigned char mac[16]);`

where `m` is a pointer to the remaining bytes of the message,

`remaining` is the number of bytes in `m`. Note that `remaining` can be 0, and must be less than the block size of the 
underlying implementation, and

`mac` is the buffer which receives the 16 byte authenticator.

Before returning, poly1305_finish_ext will zero out the context.

# Examples #

## Creating an authenticator, single call: 

    poly1305_key key = {{...}};
    const uint8_t msg[100] = {...};
    uint8_t mac[16];
    
    poly1305_auth(mac, msg, 100, &key);

## Creating an authenticator, incrementally: 

    poly1305_key key = {{...}};
    const uint8_t msg[100] = {...};
    poly1305_context ctx;
    uint8_t mac[16];
    size_t i;

    poly1305_init(&ctx, &key);

    /* update one byte at a time, extremely inefficient */
    for (i = 0; i < 100; i++)
        poly1305_update(&ctx, msg, 1);

    poly1305_finish(&ctx, mac);

# Performance #

Timings are in cycles (rdtsc). Raw cycles are reported for 1 byte to give an idea for very short message overhead, and
cycles/byte for 64 and above.

Results sorted by long message performance.

Ref32, and especially Ref8, have fairly poor performance, but as both are provided for portability on un-optimized
platforms this is not an issue.

[bench-x86.sh](bench-x86.sh) is provided to easily test implementations. It uses gcc, but any gcc compatible compiler
can be used.

## [E5200](http://ark.intel.com/products/37212/) ##

<table>
<thead><tr><th>Impl.</th><th>1 byte</th><th>64 bytes</th><th>576 bytes</th><th>4096 bytes</th></tr></thead>
<tbody>
<tr> <td>x86-64    </td><td>  262</td><td>   5.66</td><td>   2.06</td><td>   1.54</td></tr>
<tr> <td>SSE2-32   </td><td>  300</td><td>   7.81</td><td>   2.43</td><td>   1.81</td></tr>
<tr> <td>x86-32    </td><td>  287</td><td>   7.23</td><td>   3.75</td><td>   3.36</td></tr>
<tr> <td>Ref32-64  </td><td>  275</td><td>   7.42</td><td>   4.95</td><td>   4.62</td></tr>
<tr> <td>Ref32-32  </td><td>  412</td><td>  14.45</td><td>  11.22</td><td>  10.88</td></tr>
<tr> <td>Ref8-32   </td><td> 2250</td><td> 124.02</td><td> 116.73</td><td> 116.89</td></tr>
<tr> <td>Ref8-64   </td><td> 2662</td><td> 149.81</td><td> 145.07</td><td> 144.48</td></tr>
</tbody>
</table>

## [i7-4770K](http://ark.intel.com/products/75123) ##

Timings are with Turbo Boost and Hyperthreading, so accuracy is not concrete. 

<table>
<thead><tr><th>Impl.</th><th>1 byte</th><th>64 bytes</th><th>576 bytes</th><th>4096 bytes</th></tr></thead>
<tbody>
<tr> <td>AVX2-64   </td><td>  194</td><td>   3.73</td><td>   0.98</td><td>   0.63</td></tr>
<tr> <td>AVX2-32   </td><td>  218</td><td>   5.88</td><td>   1.25</td><td>   0.73</td></tr>
<tr> <td>AVX-64    </td><td>  173</td><td>   3.27</td><td>   1.34</td><td>   1.08</td></tr>
<tr> <td>AVX-32    </td><td>  194</td><td>   5.03</td><td>   1.48</td><td>   1.08</td></tr>
<tr> <td>x86-64    </td><td>  176</td><td>   3.69</td><td>   1.41</td><td>   1.20</td></tr>
<tr> <td>SSE2-32   </td><td>  194</td><td>   5.03</td><td>   1.54</td><td>   1.14</td></tr>
<tr> <td>x86-32    </td><td>  206</td><td>   4.80</td><td>   2.56</td><td>   2.34</td></tr>
<tr> <td>Ref32-64  </td><td>  143</td><td>   3.69</td><td>   2.62</td><td>   2.55</td></tr>
<tr> <td>Ref32-32  </td><td>  286</td><td>   9.73</td><td>   8.01</td><td>   7.84</td></tr>
<tr> <td>Ref8-32   </td><td> 1025</td><td>  54.69</td><td>  51.85</td><td>  51.56</td></tr>
<tr> <td>Ref8-64   </td><td> 1165</td><td>  64.19</td><td>  61.54</td><td>  61.22</td></tr>
</tbody>
</table>

## AMD FX-8120 ##

Timings are with Turbo on, so accuracy is not concrete.

<table>
<thead><tr><th>Impl.</th><th>1 byte</th><th>64 bytes</th><th>576 bytes</th><th>4096 bytes</th></tr></thead>
<tbody>
<tr> <td>AVX-64    </td><td>  268</td><td>   5.08</td><td>   1.33</td><td>   0.85</td></tr>
<tr> <td>x86-64    </td><td>  266</td><td>   5.05</td><td>   1.43</td><td>   0.96</td></tr>
<tr> <td>AVX-32    </td><td>  306</td><td>   7.31</td><td>   1.86</td><td>   1.25</td></tr>
<tr> <td>SSE2-32   </td><td>  301</td><td>   7.31</td><td>   1.90</td><td>   1.28</td></tr>
<tr> <td>x86-32    </td><td>  351</td><td>   8.39</td><td>   3.61</td><td>   3.03</td></tr>
<tr> <td>Ref32-64  </td><td>  291</td><td>   9.09</td><td>   6.77</td><td>   6.60</td></tr>
<tr> <td>Ref32-32  </td><td>  412</td><td>  13.75</td><td>  11.34</td><td>  10.68</td></tr>
<tr> <td>Ref8-32   </td><td> 2060</td><td> 117.20</td><td> 113.40</td><td> 113.01</td></tr>
<tr> <td>Ref8-64   </td><td> 2310</td><td> 131.92</td><td> 128.22</td><td> 127.75</td></tr>
</tbody>
</table>

# Testing #

Fuzzing against the reference implementations is available. See [fuzz/README](fuzz/README.md).

`poly1305_power_on_self_test` is run for every implementation available when `poly1305_detect` is called. There
are unfortunately no official test vectors available, so it tests against the test vector in NaCl, a message/key
that cause the internal hash to come out larger than the prime modulus, and messages of length 0 to 256. 

# Sources #

[sources/](sources/) contains the base source files that were used to to put everything together.

* Ref8 is based on [poly1305/ref](sources/crypto_onetimeauth_poly1305_ref_auth.c) by djb
* Ref32 is mine
* x86 is hand altered from [poly1305/x86](sources/crypto_onetimeauth_poly1305_x86_auth.s) by djb
* SSE2 and AVX are based on SSE2 ([32bit](sources/poly1305-donna-x86-sse2-incremental-source.c) / [64bit](sources/poly1305-donna-x64-sse2-incremental-source.c)) by me
* AVX2 is based on AVX2 ([32bit](sources/poly1305-donna-x86-avx2-incremental-source.c) / [64bit](sources/poly1305-donna-x64-avx2-incremental-source.c)) by me

# LICENSE #

Public Domain, or MIT