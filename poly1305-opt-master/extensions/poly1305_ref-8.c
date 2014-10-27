/*
	poly1305 implementation using 8 bit * 8 bit = 16 bit multiplication and 32 bit addition

	based on the public domain reference version in supercop by djb
*/

#if !defined(POLY1305_ONEFILE)
#include <stddef.h>

typedef struct poly1305_key_t {
	unsigned char b[32];
} poly1305_key;

size_t poly1305_block_size_ref(void);
void poly1305_init_ext_ref(void *state, const poly1305_key *key, unsigned long long bytes_hint);
void poly1305_blocks_ref(void *state, const unsigned char *m, size_t bytes);
void poly1305_finish_ext_ref(void *state, const unsigned char *m, size_t remaining, unsigned char mac[16]);
void poly1305_auth_ref(unsigned char mac[16], const unsigned char *m, size_t bytes, const poly1305_key *key);
#endif

#if defined(_MSC_VER)
	#define POLY1305_REF_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
	#define POLY1305_REF_NOINLINE __attribute__((noinline))
#else
	#define POLY1305_REF_NOINLINE
#endif

#define poly1305_block_size 16

typedef struct poly1305_state_ref_t {
	unsigned char h[17];
	unsigned char r[17];
	unsigned char pad[17];
	unsigned char final;
} poly1305_state_ref_t;

size_t
poly1305_block_size_ref(void) {
	return poly1305_block_size;
}

void
poly1305_init_ext_ref(void *state, const poly1305_key *key, unsigned long long bytes_hint) {
	poly1305_state_ref_t *st = (poly1305_state_ref_t *)state;
	size_t i;

	/* bytes_hint not used */
	(void)bytes_hint;

	/* h = 0 */
	for (i = 0; i < 17; i++)
		st->h[i] = 0;

	/* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
	st->r[0] = key->b[0];
	st->r[1] = key->b[1];
	st->r[2] = key->b[2];
	st->r[3] = key->b[3] & 0x0f;
	st->r[4] = key->b[4] & 0xfc;
	st->r[5] = key->b[5];
	st->r[6] = key->b[6];
	st->r[7] = key->b[7] & 0x0f;
	st->r[8] = key->b[8] & 0xfc;
	st->r[9] = key->b[9];
	st->r[10] = key->b[10];
	st->r[11] = key->b[11] & 0x0f;
	st->r[12] = key->b[12] & 0xfc;
	st->r[13] = key->b[13];
	st->r[14] = key->b[14];
	st->r[15] = key->b[15] & 0x0f;
	st->r[16] = 0;

	/* save pad for later */
	for (i = 0; i < 16; i++)
		st->pad[i] = key->b[i+16];
	st->pad[16] = 0;

	st->final = 0;
}

static void
poly1305_add_ref(unsigned char h[17], const unsigned char c[17]) {
	unsigned short u;
	unsigned int i;
	for (u = 0, i = 0; i < 17; i++) {
		u += (unsigned short)h[i] + (unsigned short)c[i];
		h[i] = (unsigned char)u & 0xff;
		u >>= 8;
	}
}

static void
poly1305_squeeze_ref(unsigned char h[17], unsigned long hr[17]) {
	unsigned long u;
	unsigned int i;
	u = 0;
	for (i = 0; i < 16; i++) {
		u += hr[i];
		h[i] = (unsigned char)u & 0xff;
		u >>= 8;
	}
	u += hr[16];
	h[16] = (unsigned char)u & 0x03;
	u >>= 2;
	u += (u << 2); /* u *= 5; */
	for (i = 0; i < 16; i++) {
		u += h[i];
		h[i] = (unsigned char)u & 0xff;
		u >>= 8;
	}
	h[16] += (unsigned char)u;
}

static void
poly1305_freeze_ref(unsigned char h[17]) {
	static const unsigned char minusp[17] = {
		0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0xfc
	};
	unsigned char horig[17], negative;
	unsigned int i;

	/* compute h + -p */
	for (i = 0; i < 17; i++)
		horig[i] = h[i];
	poly1305_add_ref(h, minusp);

	/* select h if h < p, or h + -p if h >= p */
	negative = -(h[16] >> 7);
	for (i = 0; i < 17; i++)
		h[i] ^= negative & (horig[i] ^ h[i]);
}

void
poly1305_blocks_ref(void *state, const unsigned char *m, size_t bytes) {
	poly1305_state_ref_t *st = (poly1305_state_ref_t *)state;
	const unsigned char hibit = st->final ^ 1; /* 1 << 128 */

	while (bytes >= poly1305_block_size) {
		unsigned long hr[17], u;
		unsigned char c[17];
		unsigned int i, j;

		/* h += m */
		for (i = 0; i < 16; i++)
			c[i] = m[i];
		c[16] = hibit;
		poly1305_add_ref(st->h, c);

		/* h *= r */
		for (i = 0; i < 17; i++) {
			u = 0;
			for (j = 0; j <= i ; j++) {
				u += (unsigned short)st->h[j] * st->r[i - j];
			}
			for (j = i + 1; j < 17; j++) {
				unsigned long v = (unsigned short)st->h[j] * st->r[i + 17 - j];
				v = ((v << 8) + (v << 6)); /* v *= (5 << 6); */
				u += v;
			}
			hr[i] = u;
		}

		/* (partial) h %= p */
		poly1305_squeeze_ref(st->h, hr);

		m += poly1305_block_size;
		bytes -= poly1305_block_size;
	}
}

POLY1305_REF_NOINLINE void
poly1305_finish_ext_ref(void *state, const unsigned char *m, size_t remaining, unsigned char mac[16]) {
	poly1305_state_ref_t *st = (poly1305_state_ref_t *)state;
	size_t i;

	/* process the remaining block */
	if (remaining) {
		unsigned char final[poly1305_block_size] = {0};
		size_t i;
		for (i = 0; i < remaining; i++)
			final[i] = m[i];
		final[remaining] = 1;
		st->final = 1;
		poly1305_blocks_ref(st, final, poly1305_block_size);
	}

	/* fully reduce h */
	poly1305_freeze_ref(st->h);

	/* h = (h + pad) % (1 << 128) */
	poly1305_add_ref(st->h, st->pad);
	for (i = 0; i < 16; i++)
		mac[i] = st->h[i];

	/* zero out the state */
	for (i = 0; i < 17; i++)
		st->h[i] = 0;
	for (i = 0; i < 17; i++)
		st->r[i] = 0;
	for (i = 0; i < 17; i++)
		st->pad[i] = 0;
}

void
poly1305_auth_ref(unsigned char mac[16], const unsigned char *m, size_t bytes, const poly1305_key *key) {
	poly1305_state_ref_t st;
	size_t blocks;
	poly1305_init_ext_ref(&st, key, bytes);
	blocks = (bytes & ~(poly1305_block_size - 1));
	if (blocks) {
		poly1305_blocks_ref(&st, m, blocks);
		m += blocks;
		bytes -= blocks;
	}
	poly1305_finish_ext_ref(&st, m, bytes, mac);
}
