#include "portable-jane.h"
#include <x86intrin.h>
typedef __m128i xmmi;
typedef __m256i ymmi;

typedef uint8_t poly1305_state[324];

enum poly1305_state_flags_t {
	poly1305_started = 1,
	poly1305_r4 = 2,
	poly1305_final_shift8 = 4,
	poly1305_final_shift16 = 8,
	poly1305_final_shift24 = 16,
	poly1305_final_shift32 = 32,
	poly1305_finalize = 64
};

typedef struct poly1305_element_t {
	uint32_t r;
	uint32_t temp;
} poly1305_element;

typedef struct poly1305_state_internal_t {
	union {
		uint32_t h[5];
		ymmi H[5];           /* 160 bytes  */
	};
	poly1305_element R[20];  /* 160 bytes   */
	uint32_t flags;          /*   4 bytes  */
} poly1305_state_internal;   /* 324 bytes total */

/* copy 0-63 bytes */
static void INLINE
poly1305_block_copy(uint8_t *dst, const uint8_t *src, size_t bytes) {
	size_t offset = src - dst;
	if (bytes & 32) { _mm256_store_si256((ymmi *)dst, _mm256_loadu_si256((ymmi *)(dst + offset))); dst += 32; }
	if (bytes & 16) { _mm_store_si128((xmmi *)dst, _mm_loadu_si128((xmmi *)(dst + offset))); dst += 16; }
	if (bytes &  8) { *(uint64_t *)dst = *(uint64_t *)(dst + offset); dst += 8; }
	if (bytes &  4) { *(uint32_t *)dst = *(uint32_t *)(dst + offset); dst += 4; }
	if (bytes &  2) { *(uint16_t *)dst = *(uint16_t *)(dst + offset); dst += 2; }
	if (bytes &  1) { *( uint8_t *)dst = *( uint8_t *)(dst + offset);           }
}

NOINLINE void
poly1305_init_ext(poly1305_state_internal *st, const unsigned char key[32], size_t bytes) {
	poly1305_element *e;
	uint32_t t0,t1,t2,t3;
	uint32_t r0,r1,r2,r3,r4;
	uint32_t rt0,rt1,rt2,rt3,rt4;
	uint32_t st1,st2,st3,st4;
	uint32_t b;
	uint64_t t[5];
	size_t i;

	if (!bytes) bytes = ~(size_t)0;

	/* H = 0 */
	st->H[0] = _mm256_setzero_si256();
	st->H[1] = _mm256_setzero_si256();
	st->H[2] = _mm256_setzero_si256();
	st->H[3] = _mm256_setzero_si256();
	st->H[4] = _mm256_setzero_si256();

	/* clamp key */
	t0 = U8TO32_LE(key+0);
	t1 = U8TO32_LE(key+4);
	t2 = U8TO32_LE(key+8);
	t3 = U8TO32_LE(key+12);
	r0 = t0 & 0x3ffffff; t0 >>= 26; t0 |= t1 << 6;
	r1 = t0 & 0x3ffff03; t1 >>= 20; t1 |= t2 << 12;
	r2 = t1 & 0x3ffc0ff; t2 >>= 14; t2 |= t3 << 18;
	r3 = t2 & 0x3f03fff; t3 >>= 8;
	r4 = t3 & 0x00fffff;

	/* r^1 */
	e = &st->R[0];
	e[0].temp = r0;
	e[1].temp = r1;
	e[2].temp = r2;
	e[3].temp = r3;
	e[4].temp = r4;

	/* r^2 */
	if (bytes > 16) {
		st1 = r1 * 5;
		st2 = r2 * 5;
		st3 = r3 * 5;
		st4 = r4 * 5;

		t[0]  = mul32x32_64(r0,r0  ) + mul32x32_64(r2*2,st3) + mul32x32_64(r4*2,st1);
		t[1]  = mul32x32_64(r0,r1*2) + mul32x32_64(r4*2,st2) + mul32x32_64(r3  ,st3);
		t[2]  = mul32x32_64(r1,r1  ) + mul32x32_64(r0*2, r2) + mul32x32_64(r4*2,st3);
		t[3]  = mul32x32_64(r0,r3*2) + mul32x32_64(r1*2, r2) + mul32x32_64(r4  ,st4);
		t[4]  = mul32x32_64(r2,r2  ) + mul32x32_64(r0*2, r4) + mul32x32_64(r3*2, r1);

						rt0 = (uint32_t)t[0] & 0x3ffffff; b = (uint32_t)(t[0] >> 26);
		t[1] += b;      rt1 = (uint32_t)t[1] & 0x3ffffff; b = (uint32_t)(t[1] >> 26);
		t[2] += b;      rt2 = (uint32_t)t[2] & 0x3ffffff; b = (uint32_t)(t[2] >> 26);
		t[3] += b;      rt3 = (uint32_t)t[3] & 0x3ffffff; b = (uint32_t)(t[3] >> 26);
		t[4] += b;      rt4 = (uint32_t)t[4] & 0x3ffffff; b = (uint32_t)(t[4] >> 26);
		rt0 += b * 5;   b = (rt0 >> 26); rt0 &= 0x3ffffff;
		rt1 += b;

		e[5].temp = rt0;
		e[6].temp = rt1;
		e[7].temp = rt2;
		e[8].temp = rt3;
		e[9].temp = rt4;
	}

	/* r^3 */
	if (bytes > 32) {
		st1 = rt1 * 5;
		st2 = rt2 * 5;
		st3 = rt3 * 5;
		st4 = rt4 * 5;

		t[0]  = mul32x32_64(r0,rt0) + mul32x32_64(r1,st4) + mul32x32_64(r2,st3) + mul32x32_64(r3,st2) + mul32x32_64(r4,st1);
		t[1]  = mul32x32_64(r0,rt1) + mul32x32_64(r1,rt0) + mul32x32_64(r2,st4) + mul32x32_64(r3,st3) + mul32x32_64(r4,st2);
		t[2]  = mul32x32_64(r0,rt2) + mul32x32_64(r1,rt1) + mul32x32_64(r2,rt0) + mul32x32_64(r3,st4) + mul32x32_64(r4,st3);
		t[3]  = mul32x32_64(r0,rt3) + mul32x32_64(r1,rt2) + mul32x32_64(r2,rt1) + mul32x32_64(r3,rt0) + mul32x32_64(r4,st4);
		t[4]  = mul32x32_64(r0,rt4) + mul32x32_64(r1,rt3) + mul32x32_64(r2,rt2) + mul32x32_64(r3,rt1) + mul32x32_64(r4,rt0);

						r0 = (uint32_t)t[0] & 0x3ffffff; b = (uint32_t)(t[0] >> 26);
		t[1] += b;      r1 = (uint32_t)t[1] & 0x3ffffff; b = (uint32_t)(t[1] >> 26);
		t[2] += b;      r2 = (uint32_t)t[2] & 0x3ffffff; b = (uint32_t)(t[2] >> 26);
		t[3] += b;      r3 = (uint32_t)t[3] & 0x3ffffff; b = (uint32_t)(t[3] >> 26);
		t[4] += b;      r4 = (uint32_t)t[4] & 0x3ffffff; b = (uint32_t)(t[4] >> 26);
		r0 += b * 5;   b = (r0 >> 26); r0 &= 0x3ffffff;
		r1 += b;

		e[10].temp = r0;
		e[11].temp = r1;
		e[12].temp = r2;
		e[13].temp = r3;
		e[14].temp = r4;
	}

	/* pad */
	e[15].temp = U8TO32_LE(key + 16);
	e[16].temp = U8TO32_LE(key + 20);
	e[17].temp = U8TO32_LE(key + 24);
	e[18].temp = U8TO32_LE(key + 28);

	/* r^4 */
	if (bytes > 48) {
		st1 = rt1 * 5;
		st2 = rt2 * 5;
		st3 = rt3 * 5;
		st4 = rt4 * 5;

		t[0]  = mul32x32_64(rt0,rt0  ) + mul32x32_64(rt2*2,st3) + mul32x32_64(rt4*2,st1);
		t[1]  = mul32x32_64(rt0,rt1*2) + mul32x32_64(rt4*2,st2) + mul32x32_64(rt3  ,st3);
		t[2]  = mul32x32_64(rt1,rt1  ) + mul32x32_64(rt0*2,rt2) + mul32x32_64(rt4*2,st3);
		t[3]  = mul32x32_64(rt0,rt3*2) + mul32x32_64(rt1*2,rt2) + mul32x32_64(rt4  ,st4);
		t[4]  = mul32x32_64(rt2,rt2  ) + mul32x32_64(rt0*2,rt4) + mul32x32_64(rt3*2,rt1);

						rt0 = (uint32_t)t[0] & 0x3ffffff; b = (uint32_t)(t[0] >> 26);
		t[1] += b;      rt1 = (uint32_t)t[1] & 0x3ffffff; b = (uint32_t)(t[1] >> 26);
		t[2] += b;      rt2 = (uint32_t)t[2] & 0x3ffffff; b = (uint32_t)(t[2] >> 26);
		t[3] += b;      rt3 = (uint32_t)t[3] & 0x3ffffff; b = (uint32_t)(t[3] >> 26);
		t[4] += b;      rt4 = (uint32_t)t[4] & 0x3ffffff; b = (uint32_t)(t[4] >> 26);
		rt0 += b * 5;   b = (rt0 >> 26); rt0 &= 0x3ffffff;
		rt1 += b;

		e[ 0].r = e[ 1].r = e[ 2].r = e[ 3].r = rt0;
		e[ 4].r = e[ 5].r = e[ 6].r = e[ 7].r = rt1;
		e[ 8].r = e[ 9].r = e[10].r = e[11].r = rt2;
		e[12].r = e[13].r = e[14].r = e[15].r = rt3;
		e[16].r = e[17].r = e[18].r = e[19].r = rt4;
	}

	st->flags = 0;
}

NOINLINE void
poly1305_blocks(poly1305_state_internal *st, const uint8_t *m, size_t bytes) {
	const ymmi MMASK = _mm256_broadcastq_epi64(_mm_cvtsi32_si128((1 << 26) - 1));
	const ymmi FIVE = _mm256_broadcastq_epi64(_mm_cvtsi32_si128(5));
	ymmi ALIGN(64) HIBIT = _mm256_broadcastq_epi64(_mm_cvtsi32_si128(1 << 24));

	ymmi H0,H1,H2,H3,H4;
	ymmi T0,T1,T2,T3,T4,T5,T6,T7,T8;
	ymmi M0,M1,M2,M3,M4;
	ymmi M5,M6,M7,M8,M9;
	ymmi C1,C2;
	ymmi R40,R41,R42,R43,R44,S41,S42,S43,S44;

	if (st->flags & (poly1305_final_shift8|poly1305_final_shift16|poly1305_final_shift24|poly1305_final_shift32)) {
		T0 = _mm256_srli_si256(HIBIT, 8);
		if (st->flags & poly1305_final_shift8)  T0 = _mm256_permute4x64_epi64(T0, _MM_SHUFFLE(3,0,0,0));
		if (st->flags & poly1305_final_shift16) T0 = _mm256_permute4x64_epi64(T0, _MM_SHUFFLE(3,3,0,0));
		if (st->flags & poly1305_final_shift24) T0 = _mm256_permute4x64_epi64(T0, _MM_SHUFFLE(3,3,3,0));
		if (st->flags & poly1305_final_shift32) T0 = _mm256_setzero_si256();
		HIBIT = T0;
	}

	if (!(st->flags & poly1305_started)) {
		/* H = [Mx,My] */
		T7 = _mm256_loadu_si256((ymmi *)(m + 0));
		T8 = _mm256_loadu_si256((ymmi *)(m + 32));
		T5 = _mm256_unpacklo_epi64(T7, T8);
		T6 = _mm256_unpackhi_epi64(T7, T8);
		T5 = _mm256_permute4x64_epi64(T5, _MM_SHUFFLE(3,1,2,0));
		T6 = _mm256_permute4x64_epi64(T6, _MM_SHUFFLE(3,1,2,0));
		H0 = _mm256_and_si256(MMASK, T5);
		H1 = _mm256_and_si256(MMASK, _mm256_srli_epi64(T5, 26));
		T5 = _mm256_or_si256(_mm256_srli_epi64(T5, 52), _mm256_slli_epi64(T6, 12));
		H2 = _mm256_and_si256(MMASK, T5);
		H3 = _mm256_and_si256(MMASK, _mm256_srli_epi64(T5, 26));
		H4 = _mm256_srli_epi64(T6, 40); 
		H4 = _mm256_or_si256(H4, HIBIT);
		m += 64;
		bytes -= 64;
		st->flags |= poly1305_started;
	} else {
		H0 = st->H[0];
		H1 = st->H[1];
		H2 = st->H[2];
		H3 = st->H[3];
		H4 = st->H[4];
	}

	if (bytes >= 64) {
		R40 = _mm256_load_si256((ymmi *)&st->R[0]);
		R41 = _mm256_load_si256((ymmi *)&st->R[4]);
		R42 = _mm256_load_si256((ymmi *)&st->R[8]);
		R43 = _mm256_load_si256((ymmi *)&st->R[12]);
		R44 = _mm256_load_si256((ymmi *)&st->R[16]);
		S41 = _mm256_mul_epu32(R41, FIVE);
		S42 = _mm256_mul_epu32(R42, FIVE);
		S43 = _mm256_mul_epu32(R43, FIVE);
		S44 = _mm256_mul_epu32(R44, FIVE);

		do {
			ymmi v01,v02,v03,v04;
			ymmi v11,v12,v13,v14;
			ymmi v21,v22,v23,v24;
			ymmi v31,v32,v33,v34;
			ymmi v41,v42,v43,v44;
			ymmi T14,T15;

			/* H *= [r^4,r^4] */
			T15 = S42;
			T0  = H4; T0  = _mm256_mul_epu32(T0, S41);
			v01 = H3; v01 = _mm256_mul_epu32(v01, T15);
			T14 = S43;
			T1  = H4; T1  = _mm256_mul_epu32(T1 , T15);
			v11 = H3; v11 = _mm256_mul_epu32(v11, T14);
			T2  = H4; T2  = _mm256_mul_epu32(T2 , T14); T0 = _mm256_add_epi64(T0, v01);
			T15 = S44;
			v02 = H2; v02 = _mm256_mul_epu32(v02, T14);
			T3  = H4; T3  = _mm256_mul_epu32(T3 , T15); T1 = _mm256_add_epi64(T1, v11);
			v03 = H1; v03 = _mm256_mul_epu32(v03, T15);
			v12 = H2; v12 = _mm256_mul_epu32(v12, T15); T0 = _mm256_add_epi64(T0, v02);
			T14 = R40;
			v21 = H3; v21 = _mm256_mul_epu32(v21, T15);
			v31 = H3; v31 = _mm256_mul_epu32(v31, T14); T0 = _mm256_add_epi64(T0, v03);
			T4  = H4; T4  = _mm256_mul_epu32(T4 , T14); T1 = _mm256_add_epi64(T1, v12);
			v04 = H0; v04 = _mm256_mul_epu32(v04, T14); T2 = _mm256_add_epi64(T2, v21);
			v13 = H1; v13 = _mm256_mul_epu32(v13, T14); T3 = _mm256_add_epi64(T3, v31);
			T15 = R41;
			v22 = H2; v22 = _mm256_mul_epu32(v22, T14);
			v32 = H2; v32 = _mm256_mul_epu32(v32, T15); T0 = _mm256_add_epi64(T0, v04);
			v41 = H3; v41 = _mm256_mul_epu32(v41, T15); T1 = _mm256_add_epi64(T1, v13);
			v14 = H0; v14 = _mm256_mul_epu32(v14, T15); T2 = _mm256_add_epi64(T2, v22);
			T14 = R42;
			v23 = H1; v23 = _mm256_mul_epu32(v23, T15); T3 = _mm256_add_epi64(T3, v32);
			v33 = H1; v33 = _mm256_mul_epu32(v33, T14); T4 = _mm256_add_epi64(T4, v41);
			v42 = H2; v42 = _mm256_mul_epu32(v42, T14); T1 = _mm256_add_epi64(T1, v14);
			T15 = R43;
			v24 = H0; v24 = _mm256_mul_epu32(v24, T14); T2 = _mm256_add_epi64(T2, v23);
			v34 = H0; v34 = _mm256_mul_epu32(v34, T15); T3 = _mm256_add_epi64(T3, v33);
			v43 = H1; v43 = _mm256_mul_epu32(v43, T15); T4 = _mm256_add_epi64(T4, v42);
			v44 = H0; v44 = _mm256_mul_epu32(v44, R44); T2 = _mm256_add_epi64(T2, v24);
			                                            T3 = _mm256_add_epi64(T3, v34);
			                                            T4 = _mm256_add_epi64(T4, v43);
			                                            T4 = _mm256_add_epi64(T4, v44);

			/* H += [Mx,My] */
			T5 = _mm256_loadu_si256((ymmi *)(m + 0));
			T6 = _mm256_loadu_si256((ymmi *)(m + 32));
			T7 = _mm256_permute2x128_si256(T5, T6, 0x20);
			T8 = _mm256_permute2x128_si256(T5, T6, 0x31);
			T5 = _mm256_unpacklo_epi32(T7, T8);
			T6 = _mm256_unpackhi_epi32(T7, T8);
			M0 = _mm256_unpacklo_epi32(T5, _mm256_setzero_si256());
			M1 = _mm256_unpackhi_epi32(T5, _mm256_setzero_si256());
			M2 = _mm256_unpacklo_epi32(T6, _mm256_setzero_si256());
			M3 = _mm256_unpackhi_epi32(T6, _mm256_setzero_si256());
			M1 = _mm256_slli_epi64(M1, 6);
			M2 = _mm256_slli_epi64(M2, 12);
			M3 = _mm256_slli_epi64(M3, 18);
			T0 = _mm256_add_epi64(T0, M0);
			T1 = _mm256_add_epi64(T1, M1);
			T2 = _mm256_add_epi64(T2, M2);
			T3 = _mm256_add_epi64(T3, M3);
			T4 = _mm256_add_epi64(T4, HIBIT);
			m += 64;

			/* reduce */
			C1 = _mm256_srli_epi64(T0, 26); C2 = _mm256_srli_epi64(T3, 26); T0 = _mm256_and_si256(T0, MMASK); T3 = _mm256_and_si256(T3, MMASK); T1 = _mm256_add_epi64(T1, C1); T4 = _mm256_add_epi64(T4, C2); 
			C1 = _mm256_srli_epi64(T1, 26); C2 = _mm256_srli_epi64(T4, 26); T1 = _mm256_and_si256(T1, MMASK); T4 = _mm256_and_si256(T4, MMASK); T2 = _mm256_add_epi64(T2, C1); T0 = _mm256_add_epi64(T0, _mm256_mul_epu32(C2, FIVE)); 
			C1 = _mm256_srli_epi64(T2, 26); C2 = _mm256_srli_epi64(T0, 26); T2 = _mm256_and_si256(T2, MMASK); T0 = _mm256_and_si256(T0, MMASK); T3 = _mm256_add_epi64(T3, C1); T1 = _mm256_add_epi64(T1, C2);
			C1 = _mm256_srli_epi64(T3, 26);                                 T3 = _mm256_and_si256(T3, MMASK);                                   T4 = _mm256_add_epi64(T4, C1);
			
			/* H = (H*[r^4,r^4] + [Mx,My]) */
			H0 = T0;
			H1 = T1;
			H2 = T2;
			H3 = T3;
			H4 = T4;

			bytes -= 64;
		} while (bytes >= 64);
	}

	if (!(st->flags & poly1305_finalize)) {
		st->H[0] = H0;
		st->H[1] = H1;
		st->H[2] = H2;
		st->H[3] = H3;
		st->H[4] = H4;
	} else {
		uint32_t t0,t1,t2,t3,t4;
		uint32_t g0,g1,g2,g3,g4,c,nc;
		uint32_t f0,f1,f2,f3;

		/* H = H[0]+H[1] */
		T0 = H0;
		T1 = H1;
		T2 = H2;
		T3 = H3;
		T4 = H4;
		T0 = _mm256_add_epi64(T0, _mm256_permute4x64_epi64(T0, 0xf5));
		T1 = _mm256_add_epi64(T1, _mm256_permute4x64_epi64(T1, 0xf5));
		T2 = _mm256_add_epi64(T2, _mm256_permute4x64_epi64(T2, 0xf5));
		T3 = _mm256_add_epi64(T3, _mm256_permute4x64_epi64(T3, 0xf5));
		T4 = _mm256_add_epi64(T4, _mm256_permute4x64_epi64(T4, 0xf5));
		T0 = _mm256_add_epi64(T0, _mm256_permute4x64_epi64(T0, 0xaa));
		T1 = _mm256_add_epi64(T1, _mm256_permute4x64_epi64(T1, 0xaa));
		T2 = _mm256_add_epi64(T2, _mm256_permute4x64_epi64(T2, 0xaa));
		T3 = _mm256_add_epi64(T3, _mm256_permute4x64_epi64(T3, 0xaa));
		T4 = _mm256_add_epi64(T4, _mm256_permute4x64_epi64(T4, 0xaa));
		t0 = _mm_cvtsi128_si32(_mm256_castsi256_si128(T0))    ; c = (t0 >> 26); t0 &= 0x3ffffff;
		t1 = _mm_cvtsi128_si32(_mm256_castsi256_si128(T1)) + c; c = (t1 >> 26); t1 &= 0x3ffffff;
		t2 = _mm_cvtsi128_si32(_mm256_castsi256_si128(T2)) + c; c = (t2 >> 26); t2 &= 0x3ffffff;
		t3 = _mm_cvtsi128_si32(_mm256_castsi256_si128(T3)) + c; c = (t3 >> 26); t3 &= 0x3ffffff;
		t4 = _mm_cvtsi128_si32(_mm256_castsi256_si128(T4)) + c; c = (t4 >> 26); t4 &= 0x3ffffff;
		t0 = t0 + (c * 5)             ; c = (t0 >> 26); t0 &= 0x3ffffff;
		t1 = t1 + c                   ; c = (t1 >> 26); t1 &= 0x3ffffff;
		t2 = t2 + c                   ; c = (t2 >> 26); t2 &= 0x3ffffff;
		t3 = t3 + c                   ; c = (t3 >> 26); t3 &= 0x3ffffff;
		t4 = t4 + c                   ; c = (t4 >> 26); t4 &= 0x3ffffff;
		t0 = t0 + (c * 5)             ; c = (t0 >> 26); t0 &= 0x3ffffff;
		t1 = t1 + c                   ;

		g0 = t0 + 5; c = g0 >> 26; g0 &= 0x3ffffff;
		g1 = t1 + c; c = g1 >> 26; g1 &= 0x3ffffff;
		g2 = t2 + c; c = g2 >> 26; g2 &= 0x3ffffff;
		g3 = t3 + c; c = g3 >> 26; g3 &= 0x3ffffff;
		g4 = t4 + c - (1 << 26);

		c = (g4 >> 31) - 1;
		nc = ~c;
		t0 = (t0 & nc) | (g0 & c);
		t1 = (t1 & nc) | (g1 & c);
		t2 = (t2 & nc) | (g2 & c);
		t3 = (t3 & nc) | (g3 & c);
		t4 = (t4 & nc) | (g4 & c);

		st->h[0] = t0;
		st->h[1] = t1;
		st->h[2] = t2;
		st->h[3] = t3;
		st->h[4] = t4;
	}
}

NOINLINE void
poly1305_finish_ext(poly1305_state_internal *st, const uint8_t *m, size_t leftover, unsigned char mac[16]) {
	uint32_t h0,h1,h2,h3,h4;
	uint32_t f0,f1,f2,f3,c;
	unsigned char ALIGN(64) final[64];

	if (leftover) {
		_mm256_store_si256((ymmi *)(final + 0), _mm256_setzero_si256());
		_mm256_store_si256((ymmi *)(final + 32), _mm256_setzero_si256());
		poly1305_block_copy(final, m, leftover);
		if ((leftover % 16) != 0) final[leftover] = 1;
		if (leftover >= 48) st->flags |= poly1305_final_shift8;
		else if (leftover >= 32) st->flags |= poly1305_final_shift16;
		else if (leftover >= 16) st->flags |= poly1305_final_shift24;
		else st->flags |= poly1305_final_shift32;
		if ((st->flags & poly1305_started) && (leftover <= 32)) {
			size_t in = 10, out = 3, count = 1, i;
			if (leftover <= 16) {
				out = 2;
				count = 2;
			}
			for (i = 0; i < count; i++) {
				st->R[out+ 0].r = st->R[in+0].temp;
				st->R[out+ 4].r = st->R[in+1].temp;
				st->R[out+ 8].r = st->R[in+2].temp;
				st->R[out+12].r = st->R[in+3].temp;
				st->R[out+16].r = st->R[in+4].temp;
				out += 1;
				in -= 5;
			}
		}
		poly1305_blocks(st, final, 64);
	}

	if (st->flags & poly1305_started) {
		size_t in = 10, out = 0, count = 3, i;
		if (!leftover || (leftover > 48)) {
			out = 1;
		} else if (leftover > 32) {
		} else if (leftover > 16) {
			in = 5;
			count = 2;
		} else {
			in = 0;
			count = 1;
		}
		for (i = 0; i < count; i++) {
			st->R[out+ 0].r = st->R[in+0].temp;
			st->R[out+ 4].r = st->R[in+1].temp;
			st->R[out+ 8].r = st->R[in+2].temp;
			st->R[out+12].r = st->R[in+3].temp;
			st->R[out+16].r = st->R[in+4].temp;
			out += 1;
			in -= 5;
		}
		while (out < 4) {
			st->R[out+ 0].r = 1;
			st->R[out+ 4].r = 0;
			st->R[out+ 8].r = 0;
			st->R[out+12].r = 0;
			st->R[out+16].r = 0;
			out += 1;
		}
		st->flags |= (poly1305_finalize|poly1305_final_shift32);
		_mm256_store_si256((ymmi *)(final + 0), _mm256_setzero_si256());
		_mm256_store_si256((ymmi *)(final + 32), _mm256_setzero_si256());
		poly1305_blocks(st, final, 64);
	}

	/* pad */
	h0 = st->h[0];
	h1 = st->h[1];
	h2 = st->h[2];
	h3 = st->h[3];
	h4 = st->h[4];

	f0 = ((h0      ) | (h1 << 26));
	f1 = ((h1 >>  6) | (h2 << 20));
	f2 = ((h2 >> 12) | (h3 << 14));
	f3 = ((h3 >> 18) | (h4 <<  8));

	__asm__ __volatile__(
		"addl %4, %0;\n"
		"adcl %5, %1;\n"
		"adcl %6, %2;\n"
		"adcl %7, %3;\n"
		: "+r"(f0), "+r"(f1), "+r"(f2), "+r"(f3)
		: "rm"(st->R[15].temp), "rm"(st->R[16].temp), "rm"(st->R[17].temp), "rm"(st->R[18].temp)
		: "flags", "cc"
	);

	_mm256_storeu_si256((ymmi *)st + 0, _mm256_setzero_si256());
	_mm256_storeu_si256((ymmi *)st + 1, _mm256_setzero_si256());
	_mm256_storeu_si256((ymmi *)st + 2, _mm256_setzero_si256());
	_mm256_storeu_si256((ymmi *)st + 3, _mm256_setzero_si256());
	_mm256_storeu_si256((ymmi *)st + 4, _mm256_setzero_si256());
	_mm256_storeu_si256((ymmi *)st + 5, _mm256_setzero_si256());
	_mm256_storeu_si256((ymmi *)st + 6, _mm256_setzero_si256());
	_mm256_storeu_si256((ymmi *)st + 7, _mm256_setzero_si256());

	U32TO8_LE(mac +  0, f0);
	U32TO8_LE(mac +  4, f1);
	U32TO8_LE(mac +  8, f2);
	U32TO8_LE(mac + 12, f3);
}

void
poly1305_auth(unsigned char out[16], const unsigned char *m, size_t inlen, const unsigned char key[32]) {
	poly1305_state_internal ALIGN(64) st;
	poly1305_init_ext(&st, key, inlen);
	if (inlen & ~63) {
		size_t bytes = inlen & ~63;
		poly1305_blocks(&st, m, bytes);
		m += bytes;
		inlen -= bytes;
	}
	poly1305_finish_ext(&st, m, inlen, out);
}
