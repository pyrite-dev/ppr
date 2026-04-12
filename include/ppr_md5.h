/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */

#ifndef __PPR_MD5_H__
#define __PPR_MD5_H__

#include <ppr_machdep.h>

typedef struct ppr_md5_context ppr_md5_context_t;

struct ppr_md5_context {
	ppr_uint32_t  i[2];	  /* number of _bits_ handled mod 2^64 */
	ppr_uint32_t  buf[4];	  /* scratch buffer */
	unsigned char in[64];	  /* input buffer */
	unsigned char digest[16]; /* actual digest after MD5Final call */
};

void ppr_md5_init(ppr_md5_context_t* mdContext);
void ppr_md5_update(ppr_md5_context_t* mdContext, const void* inBuf, unsigned int inLen);
void ppr_md5_final(ppr_md5_context_t* mdContext);

#endif
