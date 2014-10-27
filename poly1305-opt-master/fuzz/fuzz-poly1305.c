#if defined(_WIN32)
	#include <windows.h>
	#include <wincrypt.h>
	typedef unsigned char uint8_t;
	typedef unsigned int uint32_t;
	typedef unsigned __int64 uint64_t;
#else
	#include <stdint.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern void poly1305_auth_ref (uint8_t mac[16], const uint8_t *m, size_t len, const uint8_t key[32]);
extern void poly1305_auth_x86 (uint8_t mac[16], const uint8_t *m, size_t len, const uint8_t key[32]);
extern void poly1305_auth_sse2(uint8_t mac[16], const uint8_t *m, size_t len, const uint8_t key[32]);
extern void poly1305_auth_avx (uint8_t mac[16], const uint8_t *m, size_t len, const uint8_t key[32]);
extern void poly1305_auth_avx2(uint8_t mac[16], const uint8_t *m, size_t len, const uint8_t key[32]);

static void
print_diff(const char *desc, const uint8_t *a, const uint8_t *b, size_t len) {
	size_t p = 0;
	uint8_t diff;
	printf("%s diff:\n", desc);
	while (len--) {
		diff = *a++ ^ *b++;
		if (!diff)
			printf("____,");
		else
			printf("0x%02x,", diff);
		if ((++p & 15) == 0)
			printf("\n");
	}
	printf("\n\n");
}

static void
print_bytes(const char *desc, const uint8_t *bytes, size_t len) {
	size_t p = 0;
	printf("%s:\n", desc);
	while (len--) {
		printf("0x%02x,", *bytes++);
		if ((++p & 15) == 0)
			printf("\n");
	}
	printf("\n\n");
}


/* chacha20/12 prng */
void
prng(uint8_t *out, size_t bytes) {
	static uint32_t state[16];
	static int init = 0;
	uint32_t x[16], t;
	size_t i;

	if (!init) {
	#if defined(_WIN32)
		HCRYPTPROV csp;
		if (!CryptAcquireContext(&csp, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
			printf("CryptAcquireContext failed\n");
			exit(1);
		}
		if (!CryptGenRandom(csp, (DWORD)sizeof(state), (BYTE*)state)) {
			printf("CryptGenRandom failed\n");
			exit(1);
		}
		CryptReleaseContext(csp, 0);
	#else
		FILE *f = NULL;
		f = fopen("/dev/urandom", "rb");
		if (!f) {
			printf("failed to open /dev/urandom\n");
			exit(1);
		}
		if (fread(state, sizeof(state), 1, f) != 1) {
			printf("read error on /dev/urandom\n");
			exit(1);
		}
	#endif
		init = 1;
	}

	while (bytes) {
		for (i = 0; i < 16; i++) x[i] = state[i];

		#define rotl32(x,k) ((x << k) | (x >> (32 - k)))
		#define quarter(a,b,c,d) \
			x[a] += x[b]; t = x[d]^x[a]; x[d] = rotl32(t,16); \
			x[c] += x[d]; t = x[b]^x[c]; x[b] = rotl32(t,12); \
			x[a] += x[b]; t = x[d]^x[a]; x[d] = rotl32(t, 8); \
			x[c] += x[d]; t = x[b]^x[c]; x[b] = rotl32(t, 7);

		for (i = 0; i < 12; i += 2) {
			quarter( 0, 4, 8,12)
			quarter( 1, 5, 9,13)
			quarter( 2, 6,10,14)
			quarter( 3, 7,11,15)
			quarter( 0, 5,10,15)
			quarter( 1, 6,11,12)
			quarter( 2, 7, 8,13)
			quarter( 3, 4, 9,14)
		};

		if (bytes <= 64) {
			memcpy(out, x, bytes);
			bytes = 0;
		} else {
			memcpy(out, x, 64);
			bytes -= 64;
			out += 64;
		}

		/* don't need a nonce, so last 4 words are the counter. 2^136 bytes can be generated */
		if (!++state[12]) if (!++state[13]) if (!++state[14]) ++state[15];
	}
}



int main() {
	const size_t keymax = 1024;
	const size_t msgmax = 32768;
	static uint8_t key[1024][32];
	static uint8_t msg[32768];
	uint8_t mac[32][16];
	uint8_t *kp, *mp;
	size_t ki, mi, maci, i;
	uint64_t ctr;

	printf("fuzzing: ");
	printf(" ref");
#if defined(POLY1305_X86)
	printf(" x86");
#endif
#if defined(POLY1305_SSE2)
	printf(" sse2");
#endif
#if defined(POLY1305_AVX)
	printf(" avx");
#endif
#if defined(POLY1305_AVX2)
	printf(" avx2");
#endif
	printf("\n\n");

	for (ctr = 0, ki = keymax, mi = msgmax;;ctr++) {
		size_t mlen;

		if (ki == keymax) {
			prng((uint8_t *)key, sizeof(key));
			ki = 0;
		}
		kp = key[ki];
		ki += 1;

		if ((mi + 2) >= msgmax) {
			prng((uint8_t *)msg, sizeof(msg));
			mi = 0;
		}
		mlen = (msg[mi] | ((size_t)msg[mi + 1] << 8)) % 1024;
		mi += sizeof(size_t); /* keep it word aligned */
		if ((mi + mlen) >= msgmax) {
			prng((uint8_t *)msg, sizeof(msg));
			mi = 0;
		}
		mp = &msg[mi];
		mi += (mlen + (sizeof(size_t) - 1)) & ~(sizeof(size_t) - 1); /* keep it word aligned */

		maci = 0;
		poly1305_auth_ref(mac[maci++], mp, mlen, kp);
		#if defined(POLY1305_X86)
			poly1305_auth_x86(mac[maci++], mp, mlen, kp);
		#endif
		#if defined(POLY1305_SSE2)
			poly1305_auth_sse2(mac[maci++], mp, mlen, kp);
		#endif
		#if defined(POLY1305_AVX)
			poly1305_auth_avx(mac[maci++], mp, mlen, kp);
		#endif
		#if defined(POLY1305_AVX2)
			poly1305_auth_avx2(mac[maci++], mp, mlen, kp);
		#endif

		for (i = 1; i < maci; i++) {
			if (memcmp(mac[0], mac[i], 16) != 0) {
				size_t maci2 = 1;
				printf("\n\n");
				print_bytes("key", kp, 32);
				print_bytes("msg", mp, mlen);
				print_bytes("ref", mac[0], 16);
				#if defined(POLY1305_X86)
					print_diff("x86", mac[0], mac[maci2++], 16);
				#endif
				#if defined(POLY1305_SSE2)
					print_diff("sse2", mac[0], mac[maci2++], 16);
				#endif
				#if defined(POLY1305_AVX)
					print_diff("avx", mac[0], mac[maci2++], 16);
				#endif
				#if defined(POLY1305_AVX2)
					print_diff("avx2", mac[0], mac[maci2++], 16);
				#endif
				exit(1);
			}
		}

		if (ctr && (ctr % 0x8000 == 0)) {
			printf(".");
			if ((ctr % 0x100000) == 0) {
				printf(" [");
				for (i = 0; i < 8; i++)
					printf("%02x", (uint8_t)(ctr >> ((7 - i) * 8)));
				printf("]\n");
			}
		}
	}
}

