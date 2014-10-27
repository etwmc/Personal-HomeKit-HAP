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
		uint32_t h[5];
		xmmi H[5];           /*  80 bytes  */
	};
	poly1305_element R[20];  /* 160 bytes  */
	uint32_t flags;          /*   4 bytes  */
} poly1305_state_internal;   /* 244 bytes total */

typedef unsigned char poly1305_state[308];

/* copy 0-31 bytes */
static void
poly1305_block_copy31(uint8_t *dst, const uint8_t *src, size_t bytes) {
	size_t offset = 0;
	if (bytes & 16) { _mm_store_si128((xmmi *)(dst + offset + 0), _mm_loadu_si128((xmmi *)(src + offset + 0))); offset += 16; }
	if (bytes &  8) { *(uint64_t *)(dst + offset) = *(uint64_t *)(src + offset); offset += 8; }
	if (bytes &  4) { *(uint32_t *)(dst + offset) = *(uint32_t *)(src + offset); offset += 4; }
	if (bytes &  2) { *(uint16_t *)(dst + offset) = *(uint16_t *)(src + offset); offset += 2; }
	if (bytes &  1) { *( uint8_t *)(dst + offset) = *( uint8_t *)(src + offset);              }
}

NOINLINE void
poly1305_init_ext(poly1305_state_internal *st, const unsigned char key[32], uint64_t bytes_hint) {
	poly1305_element *e;
	uint32_t t0,t1,t2,t3;
	uint32_t r0,r1,r2,r3,r4;
	uint32_t s1,s2,s3,s4;
	uint32_t b;
	uint64_t t[5];
	size_t i, bytes;

	bytes = ((bytes_hint > 0xffffffffull) || (bytes_hint == 0)) ? ~(size_t)0 : (uint32_t)bytes_hint;

	/* H = 0 */
	st->H[0] = _mm_setzero_si128();
	st->H[1] = _mm_setzero_si128();
	st->H[2] = _mm_setzero_si128();
	st->H[3] = _mm_setzero_si128();
	st->H[4] = _mm_setzero_si128();

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

	/* save pad */
	e[5].temp = U8TO32_LE(key + 16);
	e[6].temp = U8TO32_LE(key + 20);
	e[7].temp = U8TO32_LE(key + 24);
	e[8].temp = U8TO32_LE(key + 28);

	/* r^2, r^4 */
	for (i = 0; i < 2; i++) {
		if (i == 0) {
			if (bytes < 16)
				break;
		} else {
			if (bytes < 64)
				break;
		}

		s1 = r1 * 5;
		s2 = r2 * 5;
		s3 = r3 * 5;
		s4 = r4 * 5;

		t[0]  = mul32x32_64(r0,r0  ) + mul32x32_64(r2*2,s3) + mul32x32_64(r4*2,s1);
		t[1]  = mul32x32_64(r0,r1*2) + mul32x32_64(r4*2,s2) + mul32x32_64(r3  ,s3);
		t[2]  = mul32x32_64(r1,r1  ) + mul32x32_64(r0*2,r2) + mul32x32_64(r4*2,s3);
		t[3]  = mul32x32_64(r0,r3*2) + mul32x32_64(r1*2,r2) + mul32x32_64(r4  ,s4);
		t[4]  = mul32x32_64(r2,r2  ) + mul32x32_64(r0*2,r4) + mul32x32_64(r3*2,r1);

						r0 = (uint32_t)t[0] & 0x3ffffff; b = (uint32_t)(t[0] >> 26);
		t[1] += b;      r1 = (uint32_t)t[1] & 0x3ffffff; b = (uint32_t)(t[1] >> 26);
		t[2] += b;      r2 = (uint32_t)t[2] & 0x3ffffff; b = (uint32_t)(t[2] >> 26);
		t[3] += b;      r3 = (uint32_t)t[3] & 0x3ffffff; b = (uint32_t)(t[3] >> 26);
		t[4] += b;      r4 = (uint32_t)t[4] & 0x3ffffff; b = (uint32_t)(t[4] >> 26);
		r0 += b * 5;   b = (r0 >> 26); r0 &= 0x3ffffff;
		r1 += b;

		e = &st->R[i * 10];
		e[0].r = e[1].r = r0;
		e[2].r = e[3].r = r1;
		e[4].r = e[5].r = r2;
		e[6].r = e[7].r = r3;
		e[8].r = e[9].r = r4;
	}

	st->flags = 0;
}

NOINLINE void
poly1305_blocks(poly1305_state_internal *st, const uint8_t *m, size_t bytes) {
	xmmi ALIGN(64) HIBIT = _mm_shuffle_epi32(_mm_cvtsi32_si128(1 << 24), _MM_SHUFFLE(1,0,1,0));;
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
			v04 = H0; v04 = _mm_mul_epu32(v04, T14); T2 = _mm_add_epi64(T2, v21);
			v13 = H1; v13 = _mm_mul_epu32(v13, T14); T3 = _mm_add_epi64(T3, v31);
			T15 = R41;
			v22 = H2; v22 = _mm_mul_epu32(v22, T14);
			v32 = H2; v32 = _mm_mul_epu32(v32, T15); T0 = _mm_add_epi64(T0, v04);
			v41 = H3; v41 = _mm_mul_epu32(v41, T15); T1 = _mm_add_epi64(T1, v13);
			v14 = H0; v14 = _mm_mul_epu32(v14, T15); T2 = _mm_add_epi64(T2, v22);
			T14 = R42;
			v23 = H1; v23 = _mm_mul_epu32(v23, T15); T3 = _mm_add_epi64(T3, v32);
			v33 = H1; v33 = _mm_mul_epu32(v33, T14); T4 = _mm_add_epi64(T4, v41);
			v42 = H2; v42 = _mm_mul_epu32(v42, T14); T1 = _mm_add_epi64(T1, v14);
			T15 = R43;
			v24 = H0; v24 = _mm_mul_epu32(v24, T14); T2 = _mm_add_epi64(T2, v23);
			v34 = H0; v34 = _mm_mul_epu32(v34, T15); T3 = _mm_add_epi64(T3, v33);
			v43 = H1; v43 = _mm_mul_epu32(v43, T15); T4 = _mm_add_epi64(T4, v42);
			v44 = H0; v44 = _mm_mul_epu32(v44, R44); T2 = _mm_add_epi64(T2, v24);
													 T3 = _mm_add_epi64(T3, v34);
													 T4 = _mm_add_epi64(T4, v43);
													 T4 = _mm_add_epi64(T4, v44);

			/* H += [Mx,My]*[r^2,r^2] */
			T5 = _mm_unpacklo_epi64(_mm_loadl_epi64((xmmi *)(m + 0)), _mm_loadl_epi64((xmmi *)(m + 16)));
			T6 = _mm_unpacklo_epi64(_mm_loadl_epi64((xmmi *)(m + 8)), _mm_loadl_epi64((xmmi *)(m + 24)));
			M0 = _mm_and_si128(MMASK, T5);
			M1 = _mm_and_si128(MMASK, _mm_srli_epi64(T5, 26));
			T5 = _mm_or_si128(_mm_srli_epi64(T5, 52), _mm_slli_epi64(T6, 12));
			M2 = _mm_and_si128(MMASK, T5);
			M3 = _mm_and_si128(MMASK, _mm_srli_epi64(T6, 14));
			M4 = _mm_or_si128(_mm_srli_epi64(T6, 40), HIBIT);

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
		uint32_t t0,t1,t2,t3,t4;
		uint32_t g0,g1,g2,g3,g4,c,nc;
		uint32_t f0,f1,f2,f3;

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

		t0 = _mm_cvtsi128_si32(T0)    ; c = (t0 >> 26); t0 &= 0x3ffffff;
		t1 = _mm_cvtsi128_si32(T1) + c; c = (t1 >> 26); t1 &= 0x3ffffff;
		t2 = _mm_cvtsi128_si32(T2) + c; c = (t2 >> 26); t2 &= 0x3ffffff;
		t3 = _mm_cvtsi128_si32(T3) + c; c = (t3 >> 26); t3 &= 0x3ffffff;
		t4 = _mm_cvtsi128_si32(T4) + c; c = (t4 >> 26); t4 &= 0x3ffffff;
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
	unsigned char ALIGN(64) final[32] = {0};

	if (leftover) {
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
		: "rm"(st->R[5].temp), "rm"(st->R[6].temp), "rm"(st->R[7].temp), "rm"(st->R[8].temp)
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

	U32TO8_LE(mac +  0, f0);
	U32TO8_LE(mac +  4, f1);
	U32TO8_LE(mac +  8, f2);
	U32TO8_LE(mac + 12, f3);
}

void
poly1305_auth(unsigned char out[16], const unsigned char *m, size_t inlen, const unsigned char key[32]) {
	poly1305_state_internal ALIGN(64) st;
	poly1305_init_ext(&st, key, inlen);
	if (inlen & ~31) {
		size_t bytes = inlen & ~31;
		poly1305_blocks(&st, m, bytes);
		m += bytes;
		inlen -= bytes;
	}
	poly1305_finish_ext(&st, m, inlen, out);
}

