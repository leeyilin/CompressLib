/**
 * MD5.H - header file for MD5C.C
 * @author: RSA Data Security, Inc.
 * @package SSLib
 * @version 1.0.0
 * @Date  : 1991-xx-xx
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */


/*

    how to get MD5-digest for your data?
    very simple:
        just call one function -- MD5Digest() --, done!

        MD5Digest()     [you give me your data-buffer and it's length,
                         and i give the digest back to you,
                         which is allways array of 16 bytes(alloced by yourself!)]

*/


#ifndef _SS_MD5_H_
#define _SS_MD5_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 encapsulate MD5Init(),MD5Update() and MD5Final(),
   you can call this function instead!   by xfwang.
 */
void MD5Digest(unsigned char *sIn, unsigned int nInLen, unsigned char sOut[16]);
void ComputeMD5(const unsigned char *sIn, unsigned int nInLen, char sOut[33]);

#ifdef __cplusplus
}
#endif

#endif
