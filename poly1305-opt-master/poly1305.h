/* $OpenBSD: poly1305.h,v 1.3 2014/07/25 14:04:51 jsing Exp $ */
/*
 * Copyright (c) 2014 Joel Sing <jsing@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef HEADER_POLY1305_H
#define HEADER_POLY1305_H

#include <openssl/opensslconf.h>

#if defined(OPENSSL_NO_POLY1305)
#error Poly1305 is disabled.
#endif

#include <stddef.h>

#ifdef  __cplusplus
extern "C" {
#endif
    
    typedef struct poly1305_context {
        size_t aligner;
        unsigned char opaque[136];
    } poly1305_context;
    
    typedef struct poly1305_context poly1305_state;
    
    void poly1305_init(poly1305_context *ctx, const unsigned char key[32]);
    void poly1305_update(poly1305_context *ctx, const unsigned char *in,
                                size_t len);
    void poly1305_finish(poly1305_context *ctx, unsigned char mac[16]);
    
#ifdef  __cplusplus
}
#endif

#endif /* HEADER_POLY1305_H */

//#ifndef POLY1305_H
//#define POLY1305_H
//
//#include <stddef.h>
//
//typedef unsigned char poly1305_context[512];
//
//typedef struct poly1305_key_t {
//	unsigned char b[32];
//} poly1305_key;
//
//int poly1305_detect(void);
//int poly1305_power_on_self_test(void);
//
//void poly1305_init(poly1305_context *ctx, const poly1305_key *key);
//void poly1305_init_ext(poly1305_context *ctx, const poly1305_key *key, unsigned long long bytes_hint);
//void poly1305_update(poly1305_context *ctx, const unsigned char *m, size_t bytes);
//void poly1305_finish(poly1305_context *ctx, unsigned char mac[16]);
//
//void poly1305_auth(unsigned char mac[16], const unsigned char *m, size_t bytes, const poly1305_key *key);
//
//
//#endif /* POLY1305_H */
//
