/*
   BLAKE2 reference source code package - reference C implementations

   Copyright 2012, Samuel Neves <sneves@dei.uc.pt>.  You may use this under the
   terms of the CC0, the OpenSSL Licence, or the Apache Public License 2.0, at
   your option.  The terms of these licenses can be found at:

   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
   - OpenSSL license   : https://www.openssl.org/source/license.html
   - Apache 2.0        : http://www.apache.org/licenses/LICENSE-2.0

   More information about the BLAKE2 hash function can be found at
   https://blake2.net.
*/
#ifndef PPR_BLAKE2S_H
#define PPR_BLAKE2S_H

#include <ppr_machdep.h>

enum ppr_blake2s_constant {
	BLAKE2S_BLOCKBYTES    = 64,
	BLAKE2S_OUTBYTES      = 32,
	BLAKE2S_KEYBYTES      = 32,
	BLAKE2S_SALTBYTES     = 8,
	BLAKE2S_PERSONALBYTES = 8
};

typedef struct ppr_blake2s_state__ {
	ppr_uint32_t h[8];
	ppr_uint32_t t[2];
	ppr_uint32_t f[2];
	ppr_uint8_t  buf[BLAKE2S_BLOCKBYTES];
	ppr_size_t   buflen;
	ppr_size_t   outlen;
	ppr_uint8_t  last_node;
} ppr_blake2s_state;

#ifdef PPR_HAS_PACK
#pragma pack(1)
#endif
struct ppr_blake2s_param__ {
	ppr_uint8_t  digest_length; /* 1 */
	ppr_uint8_t  key_length;    /* 2 */
	ppr_uint8_t  fanout;	    /* 3 */
	ppr_uint8_t  depth;	    /* 4 */
	ppr_uint32_t leaf_length;   /* 8 */
	ppr_uint32_t node_offset;   /* 12 */
	ppr_uint16_t xof_length;    /* 14 */
	ppr_uint8_t  node_depth;    /* 15 */
	ppr_uint8_t  inner_length;  /* 16 */
	/* ppr_uint8_t  reserved[0]; */
	ppr_uint8_t salt[BLAKE2S_SALTBYTES];	     /* 24 */
	ppr_uint8_t personal[BLAKE2S_PERSONALBYTES]; /* 32 */
};
#ifdef PPR_HAS_PACK
#pragma pack()
#endif

typedef struct ppr_blake2s_param__ ppr_blake2s_param;

#if 0
  /* Padded structs result in a compile-time error */
  enum {
    BLAKE2_DUMMY_1 = 1/(int)(sizeof(ppr_blake2s_param) == BLAKE2S_OUTBYTES)
  };
#endif

/* Streaming API */
int ppr_blake2s_init(ppr_blake2s_state* S, ppr_size_t outlen);
int ppr_blake2s_init_key(ppr_blake2s_state* S, ppr_size_t outlen, const void* key, ppr_size_t keylen);
int ppr_blake2s_init_param(ppr_blake2s_state* S, const ppr_blake2s_param* P);
int ppr_blake2s_update(ppr_blake2s_state* S, const void* in, ppr_size_t inlen);
int ppr_blake2s_final(ppr_blake2s_state* S, void* out, ppr_size_t outlen);

/* Simple API */
int ppr_blake2s(void* out, ppr_size_t outlen, const void* in, ppr_size_t inlen, const void* key, ppr_size_t keylen);

#endif
