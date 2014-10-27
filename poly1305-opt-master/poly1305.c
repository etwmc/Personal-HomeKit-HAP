#include <string.h>
#include "poly1305.h"

/* $OpenBSD: poly1305-donna.c,v 1.3 2014/06/12 15:49:30 deraadt Exp $ */
/*
 * Public Domain poly1305 from Andrew Moon
 * Based on poly1305-donna.c, poly1305-donna-32.h and poly1305-donna.h from:
 *   https://github.com/floodyberry/poly1305-donna
 */

#include <stddef.h>

/*
 * poly1305 implementation using 32 bit * 32 bit = 64 bit multiplication
 * and 64 bit addition.
 */

#define poly1305_block_size 16

/* 17 + sizeof(size_t) + 14*sizeof(unsigned long) */
typedef struct poly1305_state_internal_t {
    unsigned long r[5];
    unsigned long h[5];
    unsigned long pad[4];
    size_t leftover;
    unsigned char buffer[poly1305_block_size];
    unsigned char final;
} poly1305_state_internal_t;

/* interpret four 8 bit unsigned integers as a 32 bit unsigned integer in little endian */
static unsigned long
U8TO32(const unsigned char *p)
{
    return (((unsigned long)(p[0] & 0xff)) |
            ((unsigned long)(p[1] & 0xff) <<  8) |
            ((unsigned long)(p[2] & 0xff) << 16) |
            ((unsigned long)(p[3] & 0xff) << 24));
}

/* store a 32 bit unsigned integer as four 8 bit unsigned integers in little endian */
static void
U32TO8(unsigned char *p, unsigned long v)
{
    p[0] = (v) & 0xff;
    p[1] = (v >>  8) & 0xff;
    p[2] = (v >> 16) & 0xff;
    p[3] = (v >> 24) & 0xff;
}

void poly1305_init(poly1305_context *ctx, const unsigned char key[32])
{
    poly1305_state_internal_t *st = (poly1305_state_internal_t *)ctx;
    
    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    st->r[0] = (U8TO32(&key[0])) & 0x3ffffff;
    st->r[1] = (U8TO32(&key[3]) >> 2) & 0x3ffff03;
    st->r[2] = (U8TO32(&key[6]) >> 4) & 0x3ffc0ff;
    st->r[3] = (U8TO32(&key[9]) >> 6) & 0x3f03fff;
    st->r[4] = (U8TO32(&key[12]) >> 8) & 0x00fffff;
    
    /* h = 0 */
    st->h[0] = 0;
    st->h[1] = 0;
    st->h[2] = 0;
    st->h[3] = 0;
    st->h[4] = 0;
    
    /* save pad for later */
    st->pad[0] = U8TO32(&key[16]);
    st->pad[1] = U8TO32(&key[20]);
    st->pad[2] = U8TO32(&key[24]);
    st->pad[3] = U8TO32(&key[28]);
    
    st->leftover = 0;
    st->final = 0;
}

void poly1305_blocks(poly1305_state_internal_t *st, const unsigned char *m, size_t bytes)
{
    const unsigned long hibit = (st->final) ? 0 : (1 << 24); /* 1 << 128 */
    unsigned long r0, r1, r2, r3, r4;
    unsigned long s1, s2, s3, s4;
    unsigned long h0, h1, h2, h3, h4;
    unsigned long long d0, d1, d2, d3, d4;
    unsigned long c;
    
    r0 = st->r[0];
    r1 = st->r[1];
    r2 = st->r[2];
    r3 = st->r[3];
    r4 = st->r[4];
    
    s1 = r1 * 5;
    s2 = r2 * 5;
    s3 = r3 * 5;
    s4 = r4 * 5;
    
    h0 = st->h[0];
    h1 = st->h[1];
    h2 = st->h[2];
    h3 = st->h[3];
    h4 = st->h[4];
    
    while (bytes >= poly1305_block_size) {
        /* h += m[i] */
        h0 += (U8TO32(m + 0)) & 0x3ffffff;
        h1 += (U8TO32(m + 3) >> 2) & 0x3ffffff;
        h2 += (U8TO32(m + 6) >> 4) & 0x3ffffff;
        h3 += (U8TO32(m + 9) >> 6) & 0x3ffffff;
        h4 += (U8TO32(m + 12) >> 8) | hibit;
        
        /* h *= r */
        d0 = ((unsigned long long)h0 * r0) +
        ((unsigned long long)h1 * s4) +
        ((unsigned long long)h2 * s3) +
        ((unsigned long long)h3 * s2) +
        ((unsigned long long)h4 * s1);
        d1 = ((unsigned long long)h0 * r1) +
        ((unsigned long long)h1 * r0) +
        ((unsigned long long)h2 * s4) +
        ((unsigned long long)h3 * s3) +
        ((unsigned long long)h4 * s2);
        d2 = ((unsigned long long)h0 * r2) +
        ((unsigned long long)h1 * r1) +
        ((unsigned long long)h2 * r0) +
        ((unsigned long long)h3 * s4) +
        ((unsigned long long)h4 * s3);
        d3 = ((unsigned long long)h0 * r3) +
        ((unsigned long long)h1 * r2) +
        ((unsigned long long)h2 * r1) +
        ((unsigned long long)h3 * r0) +
        ((unsigned long long)h4 * s4);
        d4 = ((unsigned long long)h0 * r4) +
        ((unsigned long long)h1 * r3) +
        ((unsigned long long)h2 * r2) +
        ((unsigned long long)h3 * r1) +
        ((unsigned long long)h4 * r0);
        
        /* (partial) h %= p */
        c = (unsigned long)(d0 >> 26);
        h0 = (unsigned long)d0 & 0x3ffffff;
        d1 += c;
        c = (unsigned long)(d1 >> 26);
        h1 = (unsigned long)d1 & 0x3ffffff;
        d2 += c;
        c = (unsigned long)(d2 >> 26);
        h2 = (unsigned long)d2 & 0x3ffffff;
        d3 += c;
        c = (unsigned long)(d3 >> 26);
        h3 = (unsigned long)d3 & 0x3ffffff;
        d4 += c;
        c = (unsigned long)(d4 >> 26);
        h4 = (unsigned long)d4 & 0x3ffffff;
        h0 += c * 5;
        c = (h0 >> 26);
        h0 = h0 & 0x3ffffff;
        h1 += c;
        
        m += poly1305_block_size;
        bytes -= poly1305_block_size;
    }
    
    st->h[0] = h0;
    st->h[1] = h1;
    st->h[2] = h2;
    st->h[3] = h3;
    st->h[4] = h4;
}

void poly1305_update(poly1305_context *ctx, const unsigned char *m, size_t bytes)
{
    poly1305_state_internal_t *st = (poly1305_state_internal_t *)ctx;
    size_t i;
    
    /* handle leftover */
    if (st->leftover) {
        size_t want = (poly1305_block_size - st->leftover);
        if (want > bytes)
            want = bytes;
        for (i = 0; i < want; i++)
            st->buffer[st->leftover + i] = m[i];
        bytes -= want;
        m += want;
        st->leftover += want;
        if (st->leftover < poly1305_block_size)
            return;
        poly1305_blocks(st, st->buffer, poly1305_block_size);
        st->leftover = 0;
    }
    
    /* process full blocks */
    if (bytes >= poly1305_block_size) {
        size_t want = (bytes & ~(poly1305_block_size - 1));
        poly1305_blocks(st, m, want);
        m += want;
        bytes -= want;
    }
    
    /* store leftover */
    if (bytes) {
        for (i = 0; i < bytes; i++)
            st->buffer[st->leftover + i] = m[i];
        st->leftover += bytes;
    }
}

void poly1305_finish(poly1305_context *ctx, unsigned char mac[16])
{
    poly1305_state_internal_t *st = (poly1305_state_internal_t *)ctx;
    unsigned long h0, h1, h2, h3, h4, c;
    unsigned long g0, g1, g2, g3, g4;
    unsigned long long f;
    unsigned long mask;
    
    /* process the remaining block */
    if (st->leftover) {
        size_t i = st->leftover;
        st->buffer[i++] = 1;
        for (; i < poly1305_block_size; i++)
            st->buffer[i] = 0;
        st->final = 1;
        poly1305_blocks(st, st->buffer, poly1305_block_size);
    }
    
    /* fully carry h */
    h0 = st->h[0];
    h1 = st->h[1];
    h2 = st->h[2];
    h3 = st->h[3];
    h4 = st->h[4];
    
    c = h1 >> 26;
    h1 = h1 & 0x3ffffff;
    h2 += c;
    c = h2 >> 26;
    h2 = h2 & 0x3ffffff;
    h3 += c;
    c = h3 >> 26;
    h3 = h3 & 0x3ffffff;
    h4 += c;
    c = h4 >> 26;
    h4 = h4 & 0x3ffffff;
    h0 += c * 5;
    c = h0 >> 26;
    h0 = h0 & 0x3ffffff;
    h1 += c;
    
    /* compute h + -p */
    g0 = h0 + 5;
    c = g0 >> 26;
    g0 &= 0x3ffffff;
    g1 = h1 + c;
    c = g1 >> 26;
    g1 &= 0x3ffffff;
    g2 = h2 + c;
    c = g2 >> 26;
    g2 &= 0x3ffffff;
    g3 = h3 + c;
    c = g3 >> 26;
    g3 &= 0x3ffffff;
    g4 = h4 + c - (1 << 26);
    
    /* select h if h < p, or h + -p if h >= p */
    mask = (g4 >> ((sizeof(unsigned long) * 8) - 1)) - 1;
    g0 &= mask;
    g1 &= mask;
    g2 &= mask;
    g3 &= mask;
    g4 &= mask;
    mask = ~mask;
    h0 = (h0 & mask) | g0;
    h1 = (h1 & mask) | g1;
    h2 = (h2 & mask) | g2;
    h3 = (h3 & mask) | g3;
    h4 = (h4 & mask) | g4;
    
    /* h = h % (2^128) */
    h0 = ((h0) | (h1 << 26)) & 0xffffffff;
    h1 = ((h1 >>  6) | (h2 << 20)) & 0xffffffff;
    h2 = ((h2 >> 12) | (h3 << 14)) & 0xffffffff;
    h3 = ((h3 >> 18) | (h4 <<  8)) & 0xffffffff;
    
    /* mac = (h + pad) % (2^128) */
    f = (unsigned long long)h0 + st->pad[0];
    h0 = (unsigned long)f;
    f = (unsigned long long)h1 + st->pad[1] + (f >> 32);
    h1 = (unsigned long)f;
    f = (unsigned long long)h2 + st->pad[2] + (f >> 32);
    h2 = (unsigned long)f;
    f = (unsigned long long)h3 + st->pad[3] + (f >> 32);
    h3 = (unsigned long)f;
    
    U32TO8(mac +  0, h0);
    U32TO8(mac +  4, h1);
    U32TO8(mac +  8, h2);
    U32TO8(mac + 12, h3);
    
    /* zero out the state */
    st->h[0] = 0;
    st->h[1] = 0;
    st->h[2] = 0;
    st->h[3] = 0;
    st->h[4] = 0;
    st->r[0] = 0;
    st->r[1] = 0;
    st->r[2] = 0;
    st->r[3] = 0;
    st->r[4] = 0;
    st->pad[0] = 0;
    st->pad[1] = 0;
    st->pad[2] = 0;
    st->pad[3] = 0;
}

//typedef struct poly1305_extension_t {
//	size_t (*block_size)(void);
//	void (*init_ext)(void *ctx, const poly1305_key *key, unsigned long long bytes_hint);
//	void (*blocks)(void *ctx, const unsigned char *m, size_t bytes);
//	void (*finish_ext)(void *ctx, const unsigned char *m, size_t remaining, unsigned char mac[16]);
//	void (*auth)(unsigned char mac[16], const unsigned char *m, size_t bytes, const poly1305_key *key);
//} poly1305_extension;
//
//
///* declare an extension */
//#define DECLARE_POLY1305_EXTENSION(ext)                                                                             \
//	size_t poly1305_block_size_##ext(void);                                                                         \
//	void poly1305_init_ext_##ext(void *ctx, const poly1305_key *key, unsigned long long bytes_hint);                \
//	void poly1305_blocks_##ext(void *ctx, const unsigned char *m, size_t bytes);                                    \
//	void poly1305_finish_ext_##ext(void *ctx, const unsigned char *m, size_t remaining, unsigned char mac[16]);     \
//	void poly1305_auth_##ext(unsigned char mac[16], const unsigned char *m, size_t bytes, const poly1305_key *key); \
//	static const poly1305_extension poly1305_##ext = { \
//		poly1305_block_size_##ext,                     \
//		poly1305_init_ext_##ext,                       \
//		poly1305_blocks_##ext,                         \
//		poly1305_finish_ext_##ext,                     \
//		poly1305_auth_##ext                            \
//	};
//
//#if !defined(POLY1305_IMPL)
//
//#include "poly1305_config.inc"
//
///* reference implementation */
//#define POLY1305_ONEFILE
//
//DECLARE_POLY1305_EXTENSION(ref)
//
//#if defined(POLY1305_EXT_REF_32)
//	#include "extensions/poly1305_ref-32.c"
//#elif defined(POLY1305_EXT_REF_8)
//	#include "extensions/poly1305_ref-8.c"
//#else
//	#error Unable to find a suitable reference implementation! Did you run 'sh configure.sh'?
//#endif
//
///* detect available extensions */
//#define IS_X86_64 (defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(_M_X64))
//
//#if defined(POLY1305_EXT_AVX2)
//DECLARE_POLY1305_EXTENSION(avx2)
//#endif
//
//#if defined(POLY1305_EXT_AVX)
//DECLARE_POLY1305_EXTENSION(avx)
//#endif
//
//#if defined(POLY1305_EXT_SSE2) && !(IS_X86_64)
//DECLARE_POLY1305_EXTENSION(sse2)
//#endif
//
//#if defined(POLY1305_EXT_X86)
//DECLARE_POLY1305_EXTENSION(x86)
//#endif
//
///* best available primitives */
//static const poly1305_extension *poly1305_best_ext = &poly1305_ref;
//#else
//
//#define DECLARE_SPECIFIC_EXTENSION2(ext) \
//	DECLARE_POLY1305_EXTENSION(ext)     \
//	static const poly1305_extension *poly1305_best_ext = &poly1305_##ext;
//
//#define DECLARE_SPECIFIC_EXTENSION(ext) DECLARE_SPECIFIC_EXTENSION2(ext) 
//
//
//DECLARE_SPECIFIC_EXTENSION(POLY1305_IMPL)
//
//#endif
//
///* poly1305 implentation wrapped around provided primitives */
//typedef struct poly1305_context_internal_t {
//	unsigned char state[328]; /* largest state required (avx2) */
//	unsigned char buffer[64];
//	const poly1305_extension *ext;
//	unsigned char leftover, block_size;
//} poly1305_context_internal;
//
//static poly1305_context_internal *
//poly1305_get_context_internal(poly1305_context *ctx) {
//	unsigned char *c = (unsigned char *)ctx;
//	c += 63;
//	c -= (size_t)c & 63;
//	return (poly1305_context_internal *)c;
//}
//
//void
//poly1305_init(poly1305_context *ctx, const poly1305_key *key) {
//	poly1305_context_internal *ctxi = poly1305_get_context_internal(ctx);
//	ctxi->ext = poly1305_best_ext;
//	ctxi->ext->init_ext(ctxi, key, 0);
//	ctxi->leftover = 0;
//	ctxi->block_size = (unsigned char)ctxi->ext->block_size();
//}
//
//void
//poly1305_init_ext(poly1305_context *ctx, const poly1305_key *key, unsigned long long bytes_hint) {
//	poly1305_context_internal *ctxi = poly1305_get_context_internal(ctx);
//	ctxi->ext = poly1305_best_ext;
//	ctxi->ext->init_ext(ctxi, key, bytes_hint);
//	ctxi->leftover = 0;
//	ctxi->block_size = (unsigned char)ctxi->ext->block_size();
//}
//
//void
//poly1305_update(poly1305_context *ctx, const unsigned char *m, size_t bytes) {
//	poly1305_context_internal *ctxi = poly1305_get_context_internal(ctx);
//
//	/* handle leftover */
//	if (ctxi->leftover) {
//		size_t want = (ctxi->block_size - ctxi->leftover);
//		if (want > bytes)
//			want = bytes;
//		memcpy(ctxi->buffer + ctxi->leftover, m, want);
//		bytes -= want;
//		m += want;
//		ctxi->leftover += want;
//		if (ctxi->leftover < ctxi->block_size)
//			return;
//		ctxi->ext->blocks(ctxi, ctxi->buffer, ctxi->block_size);
//		ctxi->leftover = 0;
//	}
//
//	/* process full blocks */
//	if (bytes >= ctxi->block_size) {
//		size_t want = (bytes & ~(ctxi->block_size - 1));
//		ctxi->ext->blocks(ctxi, m, want);
//		m += want;
//		bytes -= want;
//	}
//
//	/* store leftover */
//	if (bytes) {
//		memcpy(ctxi->buffer + ctxi->leftover, m, bytes);
//		ctxi->leftover += bytes;
//	}
//}
//
//void
//poly1305_finish(poly1305_context *ctx, unsigned char mac[16]) {
//	poly1305_context_internal *ctxi = poly1305_get_context_internal(ctx);
//	ctxi->ext->finish_ext(ctxi, ctxi->buffer, ctxi->leftover, mac);
//}
//
//void
//poly1305_auth(unsigned char mac[16], const unsigned char *m, size_t bytes, const poly1305_key *key) {
//	poly1305_best_ext->auth(mac, m, bytes, key);
//}
//
//
///* test a few basic operations */
//int
//poly1305_power_on_self_test(void) {
//	/* example from nacl */
//	static const poly1305_key nacl_key = {{
//		0xee,0xa6,0xa7,0x25,0x1c,0x1e,0x72,0x91,
//		0x6d,0x11,0xc2,0xcb,0x21,0x4d,0x3c,0x25,
//		0x25,0x39,0x12,0x1d,0x8e,0x23,0x4e,0x65,
//		0x2d,0x65,0x1f,0xa4,0xc8,0xcf,0xf8,0x80,
//	}};
//
//	static const unsigned char nacl_msg[131] = {
//		0x8e,0x99,0x3b,0x9f,0x48,0x68,0x12,0x73,
//		0xc2,0x96,0x50,0xba,0x32,0xfc,0x76,0xce,
//		0x48,0x33,0x2e,0xa7,0x16,0x4d,0x96,0xa4,
//		0x47,0x6f,0xb8,0xc5,0x31,0xa1,0x18,0x6a,
//		0xc0,0xdf,0xc1,0x7c,0x98,0xdc,0xe8,0x7b,
//		0x4d,0xa7,0xf0,0x11,0xec,0x48,0xc9,0x72,
//		0x71,0xd2,0xc2,0x0f,0x9b,0x92,0x8f,0xe2,
//		0x27,0x0d,0x6f,0xb8,0x63,0xd5,0x17,0x38,
//		0xb4,0x8e,0xee,0xe3,0x14,0xa7,0xcc,0x8a,
//		0xb9,0x32,0x16,0x45,0x48,0xe5,0x26,0xae,
//		0x90,0x22,0x43,0x68,0x51,0x7a,0xcf,0xea,
//		0xbd,0x6b,0xb3,0x73,0x2b,0xc0,0xe9,0xda,
//		0x99,0x83,0x2b,0x61,0xca,0x01,0xb6,0xde,
//		0x56,0x24,0x4a,0x9e,0x88,0xd5,0xf9,0xb3,
//		0x79,0x73,0xf6,0x22,0xa4,0x3d,0x14,0xa6,
//		0x59,0x9b,0x1f,0x65,0x4c,0xb4,0x5a,0x74,
//		0xe3,0x55,0xa5
//	};
//
//	static const unsigned char nacl_mac[16] = {
//		0xf3,0xff,0xc7,0x70,0x3f,0x94,0x00,0xe5,
//		0x2a,0x7d,0xfb,0x4b,0x3d,0x33,0x05,0xd9
//	};
//
//	/* generates a final value of (2^130 - 2) == 3 */
//	static const poly1305_key wrap_key = {{
//		0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//	}};
//
//	static const unsigned char wrap_msg[16] = {
//		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
//		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
//	};
//
//	static const unsigned char wrap_mac[16] = {
//		0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//	};
//
//	/*
//		mac of the macs of messages of length 0 to 256, where the key and messages
//		have all their values set to the length
//	*/
//	static const poly1305_key total_key = {{
//		0x01,0x02,0x03,0x04,0x05,0x06,0x07,
//		0xff,0xfe,0xfd,0xfc,0xfb,0xfa,0xf9,
//		0xff,0xff,0xff,0xff,0xff,0xff,0xff,
//		0xff,0xff,0xff,0xff,0xff,0xff,0xff
//	}};
//
//	static const unsigned char total_mac[16] = {
//		0x64,0xaf,0xe2,0xe8,0xd6,0xad,0x7b,0xbd,
//		0xd2,0x87,0xf9,0x7c,0x44,0x62,0x3d,0x39
//	};
//
//	poly1305_context ctx;
//	poly1305_context total_ctx;
//	poly1305_key all_key;
//	unsigned char all_msg[256];
//	unsigned char mac[16];
//	size_t i, j;
//	int result = 1;
//
//	memset(mac, 0, sizeof(mac));
//	poly1305_auth(mac, nacl_msg, sizeof(nacl_msg), &nacl_key);
//	result &= (memcmp(nacl_mac, mac, sizeof(nacl_mac)) == 0);
//
//	/* SSE2/AVX have a 32 byte block size, but also support 64 byte blocks, so
//	   make sure everything still works varying between them */
//	memset(mac, 0, sizeof(mac));
//	poly1305_init(&ctx, &nacl_key);
//	poly1305_update(&ctx, nacl_msg +   0, 32);
//	poly1305_update(&ctx, nacl_msg +  32, 64);
//	poly1305_update(&ctx, nacl_msg +  96, 16);
//	poly1305_update(&ctx, nacl_msg + 112,  8);
//	poly1305_update(&ctx, nacl_msg + 120,  4);
//	poly1305_update(&ctx, nacl_msg + 124,  2);
//	poly1305_update(&ctx, nacl_msg + 126,  1);
//	poly1305_update(&ctx, nacl_msg + 127,  1);
//	poly1305_update(&ctx, nacl_msg + 128,  1);
//	poly1305_update(&ctx, nacl_msg + 129,  1);
//	poly1305_update(&ctx, nacl_msg + 130,  1);
//	poly1305_finish(&ctx, mac);
//	result &= (memcmp(nacl_mac, mac, sizeof(nacl_mac)) == 0);
//
//	memset(mac, 0, sizeof(mac));
//	poly1305_auth(mac, wrap_msg, sizeof(wrap_msg), &wrap_key);
//	result &= (memcmp(wrap_mac, mac, sizeof(nacl_mac)) == 0);
//
//	poly1305_init(&total_ctx, &total_key);
//	for (i = 0; i < 256; i++) {
//		/* set key and message to 'i,i,i..' */
//		for (j = 0; j < sizeof(all_key); j++)
//			all_key.b[j] = i;
//		for (j = 0; j < i; j++)
//			all_msg[j] = i;
//		poly1305_auth(mac, all_msg, i, &all_key);
//		poly1305_update(&total_ctx, mac, 16);
//	}
//	poly1305_finish(&total_ctx, mac);
//	result &= (memcmp(total_mac, mac, sizeof(total_mac)) == 0);
//
//	return result;
//}
//
//#if !defined(POLY1305_IMPL)
///* extension selection & testing */
//size_t poly1305_cpuid(void);
//
///* detect the best available implementation */
//int
//poly1305_detect(void) {
//	int result = 1;
//	size_t flags = poly1305_cpuid();
//
//	poly1305_best_ext = &poly1305_ref; result &= poly1305_power_on_self_test();
//
//#if defined(POLY1305_EXT_X86)
//	#define cpuid_mmx    (1 <<  0)
//	#define cpuid_sse    (1 <<  1)
//	#define cpuid_sse2   (1 <<  2)
//	#define cpuid_sse3   (1 <<  3)
//	#define cpuid_ssse3  (1 <<  4)
//	#define cpuid_sse4_1 (1 <<  5)
//	#define cpuid_sse4_2 (1 <<  6)
//	#define cpuid_avx    (1 <<  7)
//	#define cpuid_xop    (1 <<  8)
//	#define cpuid_avx2   (1 <<  9)
//
//	poly1305_best_ext = &poly1305_x86; result &= poly1305_power_on_self_test();
//
//#if defined(POLY1305_EXT_SSE2) && !(IS_X86_64)
//	if (flags & cpuid_sse2) { poly1305_best_ext = &poly1305_sse2; result &= poly1305_power_on_self_test(); }
//#endif
//
//#if defined(POLY1305_EXT_AVX)
//	if (flags & cpuid_avx) { poly1305_best_ext = &poly1305_avx; result &= poly1305_power_on_self_test(); }
//#endif
//
//#if defined(POLY1305_EXT_AVX2)
//	if (flags & cpuid_avx2) { poly1305_best_ext = &poly1305_avx2; result &= poly1305_power_on_self_test(); }
//#endif
//#endif /* POLY1305_X86 */
//
//	return result;
//}
//#else
//int
//poly1305_detect(void) {
//	return poly1305_power_on_self_test();
//}
//#endif /* POLY1305_IMPL */
//
