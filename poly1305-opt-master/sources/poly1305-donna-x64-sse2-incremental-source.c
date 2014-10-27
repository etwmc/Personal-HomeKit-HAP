#include "portable-jane.h"
#include <x86intrin.h>
typedef __m128i xmmi;

typedef struct poly1305_element_t {
	uint32_t r;
	uint32_t temp;
} poly1305_element;

enum poly1305_state_flags_t {
	poly1305_started = 1,
	poly1305_final_shift8 = 4,
	poly1305_final_shift16 = 8
};

typedef struct poly1305_state_internal_t {
	union {
		uint64_t h[3];
		xmmi H[5];           /*  80 bytes  */
	};
	poly1305_element R[20];  /* 160 bytes  */
	uint64_t flags;          /*   8 bytes  */
} poly1305_state_internal;   /* 248 bytes total */

typedef uint8_t poly1305_state[308];

/* copy 0-31 bytes */
static void INLINE
poly1305_block_copy31(uint8_t *dst, const uint8_t *src, size_t bytes) {
	size_t offset = src - dst;
	if (bytes & 16) { _mm_store_si128((xmmi *)dst, _mm_loadu_si128((xmmi *)(dst + offset))); dst += 16; }
	if (bytes &  8) { *(uint64_t *)dst = *(uint64_t *)(dst + offset); dst += 8; }
	if (bytes &  4) { *(uint32_t *)dst = *(uint32_t *)(dst + offset); dst += 4; }
	if (bytes &  2) { *(uint16_t *)dst = *(uint16_t *)(dst + offset); dst += 2; }
	if (bytes &  1) { *( uint8_t *)dst = *( uint8_t *)(dst + offset);           }
}

NOINLINE void
poly1305_init_ext(poly1305_state_internal *st, const unsigned char key[32], size_t bytes) {
	poly1305_element *e;
	uint128_t d[3],m0;
	uint64_t r0,r1,r2;
	uint32_t rp0,rp1,rp2,rp3,rp4;
	uint64_t rt0,rt1,rt2,st2,c;
	uint64_t t0,t1;
	size_t i;

	if (!bytes) bytes = ~(size_t)0;

	/* H = 0 */
	st->H[0] = _mm_setzero_si128();
	st->H[1] = _mm_setzero_si128();
	st->H[2] = _mm_setzero_si128();
	st->H[3] = _mm_setzero_si128();
	st->H[4] = _mm_setzero_si128();

	/* clamp key */
	t0 = U8TO64_LE(key + 0);
	t1 = U8TO64_LE(key + 8);
	r0 = t0 & 0xffc0fffffff; t0 >>= 44; t0 |= t1 << 20;
	r1 = t0 & 0xfffffc0ffff; t1 >>= 24;
	r2 = t1 & 0x00ffffffc0f;

	/* r^1 */
	e = &st->R[0];
	e[0].temp = (uint32_t)( r0                      ) & 0x3ffffff;
	e[1].temp = (uint32_t)(( r0 >> 26) | ( r1 << 18)) & 0x3ffffff;
	e[2].temp = (uint32_t)(( r1 >>  8)              ) & 0x3ffffff;
	e[3].temp = (uint32_t)(( r1 >> 34) | ( r2 << 10)) & 0x3ffffff;
	e[4].temp = (uint32_t)(( r2 >> 16)              );

	/* save pad */
	e[5].temp = U8TO32_LE(key + 16);
	e[6].temp = U8TO32_LE(key + 20);
	e[7].temp = U8TO32_LE(key + 24);
	e[8].temp = U8TO32_LE(key + 28);

	rt0 = r0;
	rt1 = r1;
	rt2 = r2;

	/* r^2, r^4 */
	if (bytes > 16) {
		st2 = rt2 * (5 << 2);
		mul64x64_128(d[0], rt0, rt0); mul64x64_128(m0, rt1 * 2, st2);  add128(d[0], m0);
		mul64x64_128(d[1], rt2, st2); mul64x64_128(m0, rt0 * 2, rt1);  add128(d[1], m0);
		mul64x64_128(d[2], rt1, rt1); mul64x64_128(m0, rt2 * 2, rt0);  add128(d[2], m0);
							rt0 = lo128(d[0]) & 0xfffffffffff; c = shr128(d[0], 44);
		add128_64(d[1], c); rt1 = lo128(d[1]) & 0xfffffffffff; c = shr128(d[1], 44);
		add128_64(d[2], c); rt2 = lo128(d[2]) & 0x3ffffffffff; c = shr128(d[2], 42);
		rt0 += c * 5; c = (rt0 >> 44); rt0 = rt0 & 0xfffffffffff;
		rt1 += c    ; c = (rt1 >> 44); rt1 = rt1 & 0xfffffffffff;
		rt2 += c    ; /* even if rt2 overflows, it will still fit in rp4 safely, and is safe to multiply with */

		rp0 = (uint32_t)( rt0                     ) & 0x3ffffff;
		rp1 = (uint32_t)((rt0 >> 26) | (rt1 << 18)) & 0x3ffffff;
		rp2 = (uint32_t)((rt1 >> 8)               ) & 0x3ffffff;
		rp3 = (uint32_t)((rt1 >> 34) | (rt2 << 10)) & 0x3ffffff;
		rp4 = (uint32_t)((rt2 >> 16)              );

		e = &st->R[0];
		e[0].r = e[1].r = rp0;
		e[2].r = e[3].r = rp1;
		e[4].r = e[5].r = rp2;
		e[6].r = e[7].r = rp3;
		e[8].r = e[9].r = rp4;
	}

	if (bytes > 64) {
		st2 = rt2 * (5 << 2);
		mul64x64_128(d[0], rt0, rt0); mul64x64_128(m0, rt1 * 2, st2);  add128(d[0], m0);
		mul64x64_128(d[1], rt2, st2); mul64x64_128(m0, rt0 * 2, rt1);  add128(d[1], m0);
		mul64x64_128(d[2], rt1, rt1); mul64x64_128(m0, rt2 * 2, rt0);  add128(d[2], m0);
							rt0 = lo128(d[0]) & 0xfffffffffff; c = shr128(d[0], 44);
		add128_64(d[1], c); rt1 = lo128(d[1]) & 0xfffffffffff; c = shr128(d[1], 44);
		add128_64(d[2], c); rt2 = lo128(d[2]) & 0x3ffffffffff; c = shr128(d[2], 42);
		rt0 += c * 5; c = (rt0 >> 44); rt0 = rt0 & 0xfffffffffff;
		rt1 += c    ; c = (rt1 >> 44); rt1 = rt1 & 0xfffffffffff;
		rt2 += c    ; /* even if rt2 overflows, it will still fit in rp4 safely, and is safe to multiply with */

		rp0 = (uint32_t)( rt0                     ) & 0x3ffffff;
		rp1 = (uint32_t)((rt0 >> 26) | (rt1 << 18)) & 0x3ffffff;
		rp2 = (uint32_t)((rt1 >> 8)               ) & 0x3ffffff;
		rp3 = (uint32_t)((rt1 >> 34) | (rt2 << 10)) & 0x3ffffff;
		rp4 = (uint32_t)((rt2 >> 16)              );

		e = &st->R[10];
		e[0].r = e[1].r = rp0;
		e[2].r = e[3].r = rp1;
		e[4].r = e[5].r = rp2;
		e[6].r = e[7].r = rp3;
		e[8].r = e[9].r = rp4;
	}

	st->flags = 0;
}

NOINLINE void
poly1305_blocks(poly1305_state_internal *st, const uint8_t *m, size_t bytes) {
	xmmi ALIGN(64) HIBIT = _mm_shuffle_epi32(_mm_cvtsi32_si128(1 << 24), _MM_SHUFFLE(1,0,1,0));
	const xmmi MMASK = _mm_shuffle_epi32(_mm_cvtsi32_si128((1 << 26) - 1), _MM_SHUFFLE(1,0,1,0));
	const xmmi FIVE = _mm_shuffle_epi32(_mm_cvtsi32_si128(5), _MM_SHUFFLE(1,0,1,0));

	xmmi H0,H1,H2,H3,H4;
	xmmi T0,T1,T2,T3,T4,T5,T6,T7,T8;
	xmmi M0,M1,M2,M3,M4;
	xmmi M5,M6,M7,M8,M9;
	xmmi C1,C2;
	xmmi R20,R21,R22,R23,R24,S21,S22,S23,S24;
	xmmi R40,R41,R42,R43,R44,S41,S42,S43,S44;

	if (st->flags & poly1305_final_shift8) HIBIT = _mm_srli_si128(HIBIT, 8);
	if (st->flags & poly1305_final_shift16) HIBIT = _mm_setzero_si128();

	if (!(st->flags & poly1305_started)) {
		/* H = [Mx,My] */
		T5 = _mm_unpacklo_epi64(_mm_loadl_epi64((xmmi *)(m + 0)), _mm_loadl_epi64((xmmi *)(m + 16)));
		T6 = _mm_unpacklo_epi64(_mm_loadl_epi64((xmmi *)(m + 8)), _mm_loadl_epi64((xmmi *)(m + 24)));
		H0 = _mm_and_si128(MMASK, T5);
		H1 = _mm_and_si128(MMASK, _mm_srli_epi64(T5, 26));
		T5 = _mm_or_si128(_mm_srli_epi64(T5, 52), _mm_slli_epi64(T6, 12));
		H2 = _mm_and_si128(MMASK, T5);
		H3 = _mm_and_si128(MMASK, _mm_srli_epi64(T5, 26));
		H4 = _mm_srli_epi64(T6, 40); 
		H4 = _mm_or_si128(H4, HIBIT);
		m += 32;
		bytes -= 32;
		st->flags |= poly1305_started;
	} else {
		H0 = st->H[0];
		H1 = st->H[1];
		H2 = st->H[2];
		H3 = st->H[3];
		H4 = st->H[4];
	}

	R20 = _mm_load_si128((xmmi *)&st->R[0]);
	R21 = _mm_load_si128((xmmi *)&st->R[2]);
	R22 = _mm_load_si128((xmmi *)&st->R[4]);
	R23 = _mm_load_si128((xmmi *)&st->R[6]);
	R24 = _mm_load_si128((xmmi *)&st->R[8]);
	S21 = _mm_mul_epu32(R21, FIVE);
	S22 = _mm_mul_epu32(R22, FIVE);
	S23 = _mm_mul_epu32(R23, FIVE);
	S24 = _mm_mul_epu32(R24, FIVE);

	if (bytes >= 64) {
		R40 = _mm_load_si128((xmmi *)&st->R[10]);
		R41 = _mm_load_si128((xmmi *)&st->R[12]);
		R42 = _mm_load_si128((xmmi *)&st->R[14]);
		R43 = _mm_load_si128((xmmi *)&st->R[16]);
		R44 = _mm_load_si128((xmmi *)&st->R[18]);
		S41 = _mm_mul_epu32(R41, FIVE);
		S42 = _mm_mul_epu32(R42, FIVE);
		S43 = _mm_mul_epu32(R43, FIVE);
		S44 = _mm_mul_epu32(R44, FIVE);

		while (bytes >= 64) {
			xmmi v00,v01,v02,v03,v04;
			xmmi v10,v11,v12,v13,v14;
			xmmi v20,v21,v22,v23,v24;
			xmmi v30,v31,v32,v33,v34;
			xmmi v40,v41,v42,v43,v44;
			xmmi T14,T15;

			/* H *= [r^4,r^4] */
			T15 = S42;
			T0  = H4; T0  = _mm_mul_epu32(T0, S41);
			v01 = H3; v01 = _mm_mul_epu32(v01, T15);
			T14 = S43;
			T1  = H4; T1  = _mm_mul_epu32(T1 , T15);
			v11 = H3; v11 = _mm_mul_epu32(v11, T14);
			T2  = H4; T2  = _mm_mul_epu32(T2 , T14); T0 = _mm_add_epi64(T0, v01);
			T15 = S44;
			v02 = H2; v02 = _mm_mul_epu32(v02, T14);
			T3  = H4; T3  = _mm_mul_epu32(T3 , T15); T1 = _mm_add_epi64(T1, v11);
			v03 = H1; v03 = _mm_mul_epu32(v03, T15);
			v12 = H2; v12 = _mm_mul_epu32(v12, T15); T0 = _mm_add_epi64(T0, v02);
			T14 = R40;
			v21 = H3; v21 = _mm_mul_epu32(v21, T15);
			v31 = H3; v31 = _mm_mul_epu32(v31, T14); T0 = _mm_add_epi64(T0, v03);
			T4  = H4; T4  = _mm_mul_epu32(T4 , T14); T1 = _mm_add_epi64(T1, v12);
			T5 = _mm_unpacklo_epi64(_mm_loadl_epi64((xmmi *)(m + 0)), _mm_loadl_epi64((xmmi *)(m + 16)));
			v04 = H0; v04 = _mm_mul_epu32(v04, T14); T2 = _mm_add_epi64(T2, v21);
			v13 = H1; v13 = _mm_mul_epu32(v13, T14); T3 = _mm_add_epi64(T3, v31);
			T15 = R41;
			v22 = H2; v22 = _mm_mul_epu32(v22, T14);
			v32 = H2; v32 = _mm_mul_epu32(v32, T15); T0 = _mm_add_epi64(T0, v04);
			v41 = H3; v41 = _mm_mul_epu32(v41, T15); T1 = _mm_add_epi64(T1, v13);
			T6 = _mm_unpacklo_epi64(_mm_loadl_epi64((xmmi *)(m + 8)), _mm_loadl_epi64((xmmi *)(m + 24)));
			v14 = H0; v14 = _mm_mul_epu32(v14, T15); T2 = _mm_add_epi64(T2, v22);
			T14 = R42;
			v23 = H1; v23 = _mm_mul_epu32(v23, T15); T3 = _mm_add_epi64(T3, v32);
			v33 = H1; v33 = _mm_mul_epu32(v33, T14); T4 = _mm_add_epi64(T4, v41);
			v42 = H2; v42 = _mm_mul_epu32(v42, T14); T1 = _mm_add_epi64(T1, v14);
			M0 = _mm_and_si128(MMASK, T5);
			M1 = _mm_and_si128(MMASK, _mm_srli_epi64(T5, 26));
			T15 = R43;
			v24 = H0; v24 = _mm_mul_epu32(v24, T14); T2 = _mm_add_epi64(T2, v23);
			v34 = H0; v34 = _mm_mul_epu32(v34, T15); T3 = _mm_add_epi64(T3, v33);
			v43 = H1; v43 = _mm_mul_epu32(v43, T15); T4 = _mm_add_epi64(T4, v42);
			T5 = _mm_or_si128(_mm_srli_epi64(T5, 52), _mm_slli_epi64(T6, 12));
			M3 = _mm_and_si128(MMASK, _mm_srli_epi64(T6, 14));
			v44 = H0; v44 = _mm_mul_epu32(v44, R44); T2 = _mm_add_epi64(T2, v24);
													 T3 = _mm_add_epi64(T3, v34);
													 T4 = _mm_add_epi64(T4, v43);
													 T4 = _mm_add_epi64(T4, v44);

			/* H += [Mx,My]*[r^2,r^2] */
			M2 = _mm_and_si128(MMASK, T5);
			M4 = _mm_or_si128(_mm_srli_epi64(T6, 40), HIBIT);

			/* H += [Mx,My] */
			T5 = _mm_loadu_si128((xmmi *)(m + 32));
			T6 = _mm_loadu_si128((xmmi *)(m + 48));
			T7 = _mm_unpacklo_epi32(T5, T6);
			T8 = _mm_unpackhi_epi32(T5, T6);
			M5 = _mm_unpacklo_epi32(T7, _mm_setzero_si128());
			M6 = _mm_unpackhi_epi32(T7, _mm_setzero_si128());
			M7 = _mm_unpacklo_epi32(T8, _mm_setzero_si128());
			M8 = _mm_unpackhi_epi32(T8, _mm_setzero_si128());
			M6 = _mm_slli_epi64(M6, 6);
			M7 = _mm_slli_epi64(M7, 12);
			M8 = _mm_slli_epi64(M8, 18);
			T0 = _mm_add_epi64(T0, M5);
			T1 = _mm_add_epi64(T1, M6);
			T2 = _mm_add_epi64(T2, M7);
			T3 = _mm_add_epi64(T3, M8);
			T4 = _mm_add_epi64(T4, HIBIT);

			T15 = S22;
			v00 = M4; v00 = _mm_mul_epu32(v00, S21);
			v01 = M3; v01 = _mm_mul_epu32(v01, T15);
			T14 = S23;
			v10 = M4; v10 = _mm_mul_epu32(v10, T15);
			v11 = M3; v11 = _mm_mul_epu32(v11, T14); T0 = _mm_add_epi64(T0, v00);
			v20 = M4; v20 = _mm_mul_epu32(v20, T14); T0 = _mm_add_epi64(T0, v01);
			T15 = S24;
			v02 = M2; v02 = _mm_mul_epu32(v02, T14); T1 = _mm_add_epi64(T1, v10);
			v30 = M4; v30 = _mm_mul_epu32(v30, T15); T1 = _mm_add_epi64(T1, v11);
			v03 = M1; v03 = _mm_mul_epu32(v03, T15); T2 = _mm_add_epi64(T2, v20);
			v12 = M2; v12 = _mm_mul_epu32(v12, T15); T0 = _mm_add_epi64(T0, v02);
			T14 = R20;
			v21 = M3; v21 = _mm_mul_epu32(v21, T15); T3 = _mm_add_epi64(T3, v30);
			v31 = M3; v31 = _mm_mul_epu32(v31, T14); T0 = _mm_add_epi64(T0, v03);
			v40 = M4; v40 = _mm_mul_epu32(v40, T14); T1 = _mm_add_epi64(T1, v12);
			v04 = M0; v04 = _mm_mul_epu32(v04, T14); T2 = _mm_add_epi64(T2, v21);
			v13 = M1; v13 = _mm_mul_epu32(v13, T14); T3 = _mm_add_epi64(T3, v31);
			T15 = R21;
			v22 = M2; v22 = _mm_mul_epu32(v22, T14); T4 = _mm_add_epi64(T4, v40);
			v32 = M2; v32 = _mm_mul_epu32(v32, T15); T0 = _mm_add_epi64(T0, v04);
			v41 = M3; v41 = _mm_mul_epu32(v41, T15); T1 = _mm_add_epi64(T1, v13);
			v14 = M0; v14 = _mm_mul_epu32(v14, T15); T2 = _mm_add_epi64(T2, v22);
			T14 = R22;
			v23 = M1; v23 = _mm_mul_epu32(v23, T15); T3 = _mm_add_epi64(T3, v32);
			v33 = M1; v33 = _mm_mul_epu32(v33, T14); T4 = _mm_add_epi64(T4, v41);
			v42 = M2; v42 = _mm_mul_epu32(v42, T14); T1 = _mm_add_epi64(T1, v14);
			T15 = R23;
			v24 = M0; v24 = _mm_mul_epu32(v24, T14); T2 = _mm_add_epi64(T2, v23);
			v34 = M0; v34 = _mm_mul_epu32(v34, T15); T3 = _mm_add_epi64(T3, v33);
			v43 = M1; v43 = _mm_mul_epu32(v43, T15); T4 = _mm_add_epi64(T4, v42);
			v44 = M0; v44 = _mm_mul_epu32(v44, R24); T2 = _mm_add_epi64(T2, v24);
													 T3 = _mm_add_epi64(T3, v34);
													 T4 = _mm_add_epi64(T4, v43);
													 T4 = _mm_add_epi64(T4, v44);

			/* reduce */
			C1 = _mm_srli_epi64(T0, 26); C2 = _mm_srli_epi64(T3, 26); T0 = _mm_and_si128(T0, MMASK); T3 = _mm_and_si128(T3, MMASK); T1 = _mm_add_epi64(T1, C1); T4 = _mm_add_epi64(T4, C2); 
			C1 = _mm_srli_epi64(T1, 26); C2 = _mm_srli_epi64(T4, 26); T1 = _mm_and_si128(T1, MMASK); T4 = _mm_and_si128(T4, MMASK); T2 = _mm_add_epi64(T2, C1); T0 = _mm_add_epi64(T0, _mm_mul_epu32(C2, FIVE)); 
			C1 = _mm_srli_epi64(T2, 26); C2 = _mm_srli_epi64(T0, 26); T2 = _mm_and_si128(T2, MMASK); T0 = _mm_and_si128(T0, MMASK); T3 = _mm_add_epi64(T3, C1); T1 = _mm_add_epi64(T1, C2);
			C1 = _mm_srli_epi64(T3, 26);                              T3 = _mm_and_si128(T3, MMASK);                                T4 = _mm_add_epi64(T4, C1);
		
			/* H = (H*[r^4,r^4] + [Mx,My]*[r^2,r^2] + [Mx,My]) */
			H0 = T0;
			H1 = T1;
			H2 = T2;
			H3 = T3;
			H4 = T4;

			m += 64;
			bytes -= 64;
		}
	}


	if (bytes >= 32) {
		xmmi v01,v02,v03,v04;
		xmmi v11,v12,v13,v14;
		xmmi v21,v22,v23,v24;
		xmmi v31,v32,v33,v34;
		xmmi v41,v42,v43,v44;
		xmmi T14,T15;

		/* H *= [r^2,r^2] */
		T15 = S22;
		T0  = H4; T0  = _mm_mul_epu32(T0, S21);
		v01 = H3; v01 = _mm_mul_epu32(v01, T15);
		T14 = S23;
		T1  = H4; T1  = _mm_mul_epu32(T1 , T15);
		v11 = H3; v11 = _mm_mul_epu32(v11, T14);
		T2  = H4; T2  = _mm_mul_epu32(T2 , T14); T0 = _mm_add_epi64(T0, v01);
		T15 = S24;
		v02 = H2; v02 = _mm_mul_epu32(v02, T14);
		T3  = H4; T3  = _mm_mul_epu32(T3 , T15); T1 = _mm_add_epi64(T1, v11);
		v03 = H1; v03 = _mm_mul_epu32(v03, T15);
		v12 = H2; v12 = _mm_mul_epu32(v12, T15); T0 = _mm_add_epi64(T0, v02);
		T14 = R20;
		v21 = H3; v21 = _mm_mul_epu32(v21, T15);
		v31 = H3; v31 = _mm_mul_epu32(v31, T14); T0 = _mm_add_epi64(T0, v03);
		T4  = H4; T4  = _mm_mul_epu32(T4 , T14); T1 = _mm_add_epi64(T1, v12);
		v04 = H0; v04 = _mm_mul_epu32(v04, T14); T2 = _mm_add_epi64(T2, v21);
		v13 = H1; v13 = _mm_mul_epu32(v13, T14); T3 = _mm_add_epi64(T3, v31);
		T15 = R21;
		v22 = H2; v22 = _mm_mul_epu32(v22, T14);
		v32 = H2; v32 = _mm_mul_epu32(v32, T15); T0 = _mm_add_epi64(T0, v04);
		v41 = H3; v41 = _mm_mul_epu32(v41, T15); T1 = _mm_add_epi64(T1, v13);
		v14 = H0; v14 = _mm_mul_epu32(v14, T15); T2 = _mm_add_epi64(T2, v22);
		T14 = R22;
		v23 = H1; v23 = _mm_mul_epu32(v23, T15); T3 = _mm_add_epi64(T3, v32);
		v33 = H1; v33 = _mm_mul_epu32(v33, T14); T4 = _mm_add_epi64(T4, v41);
		v42 = H2; v42 = _mm_mul_epu32(v42, T14); T1 = _mm_add_epi64(T1, v14);
		T15 = R23;
		v24 = H0; v24 = _mm_mul_epu32(v24, T14); T2 = _mm_add_epi64(T2, v23);
		v34 = H0; v34 = _mm_mul_epu32(v34, T15); T3 = _mm_add_epi64(T3, v33);
		v43 = H1; v43 = _mm_mul_epu32(v43, T15); T4 = _mm_add_epi64(T4, v42);
		v44 = H0; v44 = _mm_mul_epu32(v44, R24); T2 = _mm_add_epi64(T2, v24);
		                                         T3 = _mm_add_epi64(T3, v34);
		                                         T4 = _mm_add_epi64(T4, v43);
		                                         T4 = _mm_add_epi64(T4, v44);
		
		/* H += [Mx,My] */
		if (m) {
			T5 = _mm_loadu_si128((xmmi *)(m + 0));
			T6 = _mm_loadu_si128((xmmi *)(m + 16));
			T7 = _mm_unpacklo_epi32(T5, T6);
			T8 = _mm_unpackhi_epi32(T5, T6);
			M0 = _mm_unpacklo_epi32(T7, _mm_setzero_si128());
			M1 = _mm_unpackhi_epi32(T7, _mm_setzero_si128());
			M2 = _mm_unpacklo_epi32(T8, _mm_setzero_si128());
			M3 = _mm_unpackhi_epi32(T8, _mm_setzero_si128());
			M1 = _mm_slli_epi64(M1, 6);
			M2 = _mm_slli_epi64(M2, 12);
			M3 = _mm_slli_epi64(M3, 18);
			T0 = _mm_add_epi64(T0, M0);
			T1 = _mm_add_epi64(T1, M1);
			T2 = _mm_add_epi64(T2, M2);
			T3 = _mm_add_epi64(T3, M3);
			T4 = _mm_add_epi64(T4, HIBIT);
		}

		/* reduce */
		C1 = _mm_srli_epi64(T0, 26); C2 = _mm_srli_epi64(T3, 26); T0 = _mm_and_si128(T0, MMASK); T3 = _mm_and_si128(T3, MMASK); T1 = _mm_add_epi64(T1, C1); T4 = _mm_add_epi64(T4, C2); 
		C1 = _mm_srli_epi64(T1, 26); C2 = _mm_srli_epi64(T4, 26); T1 = _mm_and_si128(T1, MMASK); T4 = _mm_and_si128(T4, MMASK); T2 = _mm_add_epi64(T2, C1); T0 = _mm_add_epi64(T0, _mm_mul_epu32(C2, FIVE)); 
		C1 = _mm_srli_epi64(T2, 26); C2 = _mm_srli_epi64(T0, 26); T2 = _mm_and_si128(T2, MMASK); T0 = _mm_and_si128(T0, MMASK); T3 = _mm_add_epi64(T3, C1); T1 = _mm_add_epi64(T1, C2);
		C1 = _mm_srli_epi64(T3, 26);                              T3 = _mm_and_si128(T3, MMASK);                                T4 = _mm_add_epi64(T4, C1);
		
		/* H = (H*[r^2,r^2] + [Mx,My]) */
		H0 = T0;
		H1 = T1;
		H2 = T2;
		H3 = T3;
		H4 = T4;
	}

	if (m) {
		st->H[0] = H0;
		st->H[1] = H1;
		st->H[2] = H2;
		st->H[3] = H3;
		st->H[4] = H4;
	} else {
		uint32_t t0,t1,t2,t3,t4,b;
		uint64_t h0,h1,h2,g0,g1,g2,c,nc;

		/* H = H[0]+H[1] */
		T0 = H0;
		T1 = H1;
		T2 = H2;
		T3 = H3;
		T4 = H4;

		T0 = _mm_add_epi64(T0, _mm_srli_si128(T0, 8));
		T1 = _mm_add_epi64(T1, _mm_srli_si128(T1, 8));
		T2 = _mm_add_epi64(T2, _mm_srli_si128(T2, 8));
		T3 = _mm_add_epi64(T3, _mm_srli_si128(T3, 8));
		T4 = _mm_add_epi64(T4, _mm_srli_si128(T4, 8));

		t0 = _mm_cvtsi128_si32(T0)    ; b = (t0 >> 26); t0 &= 0x3ffffff;
		t1 = _mm_cvtsi128_si32(T1) + b; b = (t1 >> 26); t1 &= 0x3ffffff;
		t2 = _mm_cvtsi128_si32(T2) + b; b = (t2 >> 26); t2 &= 0x3ffffff;
		t3 = _mm_cvtsi128_si32(T3) + b; b = (t3 >> 26); t3 &= 0x3ffffff;
		t4 = _mm_cvtsi128_si32(T4) + b; 

		/* everything except t4 is in range, so this is all safe */
		h0 =  (((uint64_t)t0      ) | ((uint64_t)t1 << 26)                       ) & 0xfffffffffffull;
		h1 =  (((uint64_t)t1 >> 18) | ((uint64_t)t2 <<  8) | ((uint64_t)t3 << 34)) & 0xfffffffffffull;
		h2 =  (((uint64_t)t3 >> 10) | ((uint64_t)t4 << 16)                       );

		             c = (h2 >> 42); h2 &= 0x3ffffffffff;
		h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
		h1 += c;	 c = (h1 >> 44); h1 &= 0xfffffffffff;
		h2 += c;     c = (h2 >> 42); h2 &= 0x3ffffffffff;
		h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
		h1 += c;

		g0 = h0 + 5; c = (g0 >> 44); g0 &= 0xfffffffffff;
		g1 = h1 + c; c = (g1 >> 44); g1 &= 0xfffffffffff;
		g2 = h2 + c - ((uint64_t)1 << 42);

		c = (g2 >> 63) - 1;
		nc = ~c;
		h0 = (h0 & nc) | (g0 & c);
		h1 = (h1 & nc) | (g1 & c);
		h2 = (h2 & nc) | (g2 & c);

		st->h[0] = h0;
		st->h[1] = h1;
		st->h[2] = h2;
	}
}

NOINLINE void
poly1305_finish_ext(poly1305_state_internal *st, const uint8_t *m, size_t leftover, unsigned char mac[16]) {
	uint64_t h0,h1,h2;
	uint64_t t0,t1,c;

	if (leftover) {
		unsigned char ALIGN(16) final[32] = {0};
		poly1305_block_copy31(final, m, leftover);
		if (leftover != 16) final[leftover] = 1;
		st->flags |= (leftover >= 16) ? poly1305_final_shift8 : poly1305_final_shift16;
		poly1305_blocks(st, final, 32);
	}

	if (st->flags & poly1305_started) {
		/* finalize, H *= [r^2,r], or H *= [r,1] */
		if (!leftover || (leftover > 16)) {
			st->R[1].r = st->R[0].temp;
			st->R[3].r = st->R[1].temp;
			st->R[5].r = st->R[2].temp;
			st->R[7].r = st->R[3].temp;
			st->R[9].r = st->R[4].temp;
		} else {
			st->R[0].r = st->R[0].temp; st->R[1].r = 1;
			st->R[2].r = st->R[1].temp; st->R[3].r = 0;
			st->R[4].r = st->R[2].temp; st->R[5].r = 0;
			st->R[6].r = st->R[3].temp; st->R[7].r = 0;
			st->R[8].r = st->R[4].temp; st->R[9].r = 0;
		}

		poly1305_blocks(st, NULL, 32);
	}

	h0 = st->h[0];
	h1 = st->h[1];
	h2 = st->h[2];

	/* pad */
	h0 = ((h0      ) | (h1 << 44));
	h1 = ((h1 >> 20) | (h2 << 24));

	t0 = ((uint64_t)st->R[6].temp << 32) | (uint64_t)st->R[5].temp;
	t1 = ((uint64_t)st->R[8].temp << 32) | (uint64_t)st->R[7].temp;

	__asm__ __volatile__(
		"addq %2, %0 ;\n"
		"adcq %3, %1 ;\n"
		: "+r"(h0), "+r"(h1)
		: "r"(t0), "r"(t1)
		: "flags", "cc"
	);

	_mm_store_si128((xmmi *)st + 0, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 1, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 2, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 3, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 4, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 5, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 6, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 7, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 8, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 9, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 10, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 11, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 12, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 13, _mm_setzero_si128());
	_mm_store_si128((xmmi *)st + 14, _mm_setzero_si128());

	U64TO8_LE(mac + 0, h0);
	U64TO8_LE(mac + 8, h1);
}



void
poly1305_auth(unsigned char out[16], const unsigned char *m, size_t inlen, const unsigned char key[32]) {
	poly1305_state_internal ALIGN(64) st;
	size_t blocks;
	poly1305_init_ext(&st, key, inlen);
	blocks = inlen & ~31;
	if (blocks) {
		poly1305_blocks(&st, m, blocks);
		m += blocks;
		inlen -= blocks;
	}
	poly1305_finish_ext(&st, m, inlen, out);
}

