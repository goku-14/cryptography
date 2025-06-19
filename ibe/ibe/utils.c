#include "ibe_lcl.h"

#define ROTL32(a, n) (((a) << n) | ((a) >> (32-n)))
#define ROTR32(a, n) (((a) >> n) | ((a) << (32-n)))
#define BSWAP4(x)  ((ROTL32(x, 8) & 0x00ff00ff) | (ROTR32(x, 8) & 0xff00ff00))

#define ORDER_BIG_ENDIAN     0
#define ORDER_LITTLE_ENDIAN  1

#ifdef _MSC_VER
  static union { unsigned int x; unsigned char c; }endian_test = { 1 };
  #define ENDIANESS  (endian_test.c)
#else
  #define ENDIANESS  (((union { unsigned int x; unsigned char c; }){1}).c)
#endif

/* 转换成大端 */
unsigned int to_be32(const unsigned int in)
{
    int ret = in;
    if (ENDIANESS == ORDER_LITTLE_ENDIAN)
        ret = BSWAP4(in);

    return ret;
}

void memzero(void *const p, const int len)
{
    volatile unsigned char *volatile p_ =
        (volatile unsigned char *volatile) p;
    int i = 0;

    while (i < len) 
        p_[i++] = 0;
}

/* constant time PKCS5 padding */
int pkcs5_pad(unsigned char *buf, unsigned int unpadded_buflen, unsigned int blocksize)
{
    unsigned char           *tail;
    unsigned int            i;
    unsigned int            xpadlen;
    volatile unsigned char  mask;
    unsigned char           pad;

    if (blocksize <= 0U || blocksize >= 256U || unpadded_buflen >= blocksize)
        return -1;

    xpadlen = blocksize - unpadded_buflen;
    pad = (unsigned char)xpadlen;
    tail = &buf[blocksize - 1U];

    mask = 0U;
    for (i = 0U; i < blocksize; i++) {
        mask |= (unsigned char) (((i ^ xpadlen) - 1U)
           >> ((sizeof(unsigned int) - 1) * 8));
        *(tail - i) = ((*(tail - i)) & mask) | (pad & (~mask));
    }
    return 0;
}

/* constant time PKCS5 unpadding */
int pkcs5_unpad(unsigned int *unpadded_buflen_p, const unsigned char *buf, unsigned int blocksize)
{
    const unsigned char       *tail;
    unsigned char              mask = 0U;
    unsigned char              c;
    unsigned char              invalid = 0U;
    unsigned int               i;
    unsigned char              pad;

    if (blocksize <= 0U || blocksize >= 256U)
        return -1;

    tail = &buf[blocksize - 1U];
    pad = *tail;

    for (i = 0U; i < blocksize; i++) {
        c = *(tail - i);
        mask |= (unsigned char) (((i ^ (unsigned int) pad) - 1U) >> ((sizeof(unsigned int) - 1) * 8));
        invalid |= (c ^ pad) & (~mask);
    }
    *unpadded_buflen_p = blocksize - (unsigned int) pad;

    return (int)((invalid == 0U) - 1);
}