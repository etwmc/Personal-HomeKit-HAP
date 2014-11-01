#include "../Configuration.h"

typedef SHACTX ed25519_hash_context;

static void
ed25519_hash_init(ed25519_hash_context *ctx) {
	SHAInit(ctx);
}

static void
ed25519_hash_update(ed25519_hash_context *ctx, const uint8_t *in, size_t inlen) {
	SHAUpdate(ctx, in, inlen);
}

static void
ed25519_hash_final(ed25519_hash_context *ctx, uint8_t *hash) {
	SHAFinal(hash, ctx);
}

static void
ed25519_hash(uint8_t *hash, const uint8_t *in, size_t inlen) {
	SHA512(in, inlen, hash);
}
