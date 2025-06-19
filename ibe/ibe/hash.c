#include <string.h>
#include "ibe.h"
#include "ibe_lcl.h"

static void sm3_compress(unsigned int digest[8], const unsigned char block[SM3_BLOCK_SIZE])
{
    int j;
    unsigned int W[68], W1[64];
    const unsigned int *pblock = (const unsigned int *)block;

    unsigned int A = digest[0];
    unsigned int B = digest[1];
    unsigned int C = digest[2];
    unsigned int D = digest[3];
    unsigned int E = digest[4];
    unsigned int F = digest[5];
    unsigned int G = digest[6];
    unsigned int H = digest[7];
    unsigned int SS1, SS2, TT1, TT2, T[64];

    for (j = 0; j < 16; j++)
        W[j] = to_be32(pblock[j]);

    for (j = 16; j < 68; j++)
        W[j] = P1(W[j - 16] ^ W[j - 9] ^ ROTATELEFT(W[j - 3], 15)) ^ ROTATELEFT(W[j - 13], 7) ^ W[j - 6];

    for (j = 0; j < 64; j++)
        W1[j] = W[j] ^ W[j + 4];

    for (j = 0; j < 16; j++) {

        T[j] = 0x79CC4519;
        SS1 = ROTATELEFT((ROTATELEFT(A, 12) + E + ROTATELEFT(T[j], j)), 7);
        SS2 = SS1 ^ ROTATELEFT(A, 12);
        TT1 = FF0(A, B, C) + D + SS2 + W1[j];
        TT2 = GG0(E, F, G) + H + SS1 + W[j];
        D = C;
        C = ROTATELEFT(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTATELEFT(F, 19);
        F = E;
        E = P0(TT2);
    }

    for (j = 16; j < 64; j++) {

        T[j] = 0x7A879D8A;
        SS1 = ROTATELEFT((ROTATELEFT(A, 12) + E + ROTATELEFT(T[j], j)), 7);
        SS2 = SS1 ^ ROTATELEFT(A, 12);
        TT1 = FF1(A, B, C) + D + SS2 + W1[j];
        TT2 = GG1(E, F, G) + H + SS1 + W[j];
        D = C;
        C = ROTATELEFT(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTATELEFT(F, 19);
        F = E;
        E = P0(TT2);
    }

    digest[0] ^= A;
    digest[1] ^= B;
    digest[2] ^= C;
    digest[3] ^= D;
    digest[4] ^= E;
    digest[5] ^= F;
    digest[6] ^= G;
    digest[7] ^= H;
}

int SM3_init(SM3_CTX *ctx)
{
    ctx->digest[0] = 0x7380166F;
    ctx->digest[1] = 0x4914B2B9;
    ctx->digest[2] = 0x172442D7;
    ctx->digest[3] = 0xDA8A0600;
    ctx->digest[4] = 0xA96F30BC;
    ctx->digest[5] = 0x163138AA;
    ctx->digest[6] = 0xE38DEE4D;
    ctx->digest[7] = 0xB0FB0E4E;

    ctx->nblocks = 0;
    ctx->num = 0;
    return 0;
}

int SM3_update(SM3_CTX *ctx, const unsigned char *data, int datalen)
{
    if (ctx->num) {
        unsigned int left = SM3_BLOCK_SIZE - ctx->num;
        if (datalen < left) {
            memcpy(ctx->block + ctx->num, data, datalen);
            ctx->num += datalen;
            return 0;
        }
        else {
            memcpy(ctx->block + ctx->num, data, left);
            sm3_compress(ctx->digest, ctx->block);
            ctx->nblocks++;
            data += left;
            datalen -= left;
        }
    }
    while (datalen >= SM3_BLOCK_SIZE) {
        sm3_compress(ctx->digest, data);
        ctx->nblocks++;
        data += SM3_BLOCK_SIZE;
        datalen -= SM3_BLOCK_SIZE;
    }
    ctx->num = datalen;
    if (datalen)
        memcpy(ctx->block, data, datalen);

    return 0;
}

int SM3_final(SM3_CTX *ctx, unsigned char *digest)
{
    int i;
    unsigned int *pdigest = (unsigned int *)digest;
    unsigned int *count = (unsigned int *)(ctx->block + SM3_BLOCK_SIZE - 8);

    ctx->block[ctx->num] = 0x80;

    if (ctx->num + 9 <= SM3_BLOCK_SIZE)
        memset(ctx->block + ctx->num + 1, 0, SM3_BLOCK_SIZE - ctx->num - 9);
    else {
        memset(ctx->block + ctx->num + 1, 0, SM3_BLOCK_SIZE - ctx->num - 1);
        sm3_compress(ctx->digest, ctx->block);
        memset(ctx->block, 0, SM3_BLOCK_SIZE - 8);
    }

    count[0] = to_be32((ctx->nblocks) >> 23);
    count[1] = to_be32((ctx->nblocks << 9) + (ctx->num << 3));

    sm3_compress(ctx->digest, ctx->block);
    for (i = 0; i < 8; i++)
        pdigest[i] = to_be32(ctx->digest[i]);

    memzero(ctx, sizeof(*ctx));
    return 0;
}

int SM3_once(const unsigned char *m, int mlen, unsigned char digest[SM3_DIGEST_LENGTH])
{
    SM3_CTX ctx;
    SM3_init(&ctx);
    SM3_update(&ctx, m, mlen);
    SM3_final(&ctx, digest);
    return 0;
}

/* {0, 1}* -> G1 */
void hash1(element_t r, unsigned char *m, int mlen)
{
    unsigned char h[SM3_DIGEST_LENGTH];
    SM3_once(m, mlen, h);
    element_from_hash(r, h, SM3_DIGEST_LENGTH);
}

/* {0, 1}* -> Zn */
void hash2(element_t r, unsigned char *m, int mlen)
{
    unsigned char h[SM3_DIGEST_LENGTH];
    
    SM3_once(m, mlen, h);
    element_from_hash(r, h, SM3_DIGEST_LENGTH);
}

/* G1 -> Zn */
void hash3(element_t r, element_t p)
{
    int mlen;
    unsigned char h[SM3_DIGEST_LENGTH];
    /* 64B 足够放下 p 了, p 大概只有16字节 */
    unsigned char m[64];

    mlen = element_length_in_bytes(p);
    element_to_bytes(m, p);
    SM3_once(m, mlen, h);
    element_from_hash(r, h, SM3_DIGEST_LENGTH);
}

/* GT -> {0, 1}*, 这里就把r的长度固定成32字节了 */
void hash4(unsigned char *r, element_t p)
{
    int mlen;
    /* 64B 足够放下 p 了, p 很短的 */
    unsigned char m[64];

    mlen = element_length_in_bytes(p);
    element_to_bytes(m, p);

    SM3_once(m, mlen, r);
}


int HMAC_SM3_init(HMAC_SM3_CTX *ctx, unsigned char *key, int key_len)
{
    unsigned char key_ipad[SM3_BLOCK_SIZE];

    if (key == NULL || key_len < 0 || ctx == NULL)
        return -1;

    /* set key */
    memset(ctx->key, 0, SM3_BLOCK_SIZE);
    if (key_len > SM3_BLOCK_SIZE)
        SM3_once(key, key_len, ctx->key);
    else
        memcpy(ctx->key, key, key_len);

    for (int i = 0; i < SM3_BLOCK_SIZE; i++)
        key_ipad[i] = ctx->key[i] ^ HMAC_IPAD;

    SM3_init(&ctx->sm3_ctx);
    SM3_update(&ctx->sm3_ctx, key_ipad, SM3_BLOCK_SIZE);

    memzero(key_ipad, SM3_BLOCK_SIZE);
    return 0;
}

int HMAC_SM3_update(HMAC_SM3_CTX *ctx, unsigned char *in, int inlen)
{
    return SM3_update(&ctx->sm3_ctx, in, inlen);
}

int HMAC_SM3_final(HMAC_SM3_CTX *ctx, unsigned char *mac)
{
    unsigned char key_opad[SM3_BLOCK_SIZE];
    unsigned char digest[SM3_DIGEST_LENGTH];

    for (int i = 0; i < SM3_BLOCK_SIZE; i++)
        key_opad[i] = ctx->key[i] ^ HMAC_OPAD;

    SM3_final(&ctx->sm3_ctx, digest);
    SM3_init(&ctx->sm3_ctx);
    SM3_update(&ctx->sm3_ctx, key_opad, SM3_BLOCK_SIZE);
    SM3_update(&ctx->sm3_ctx, digest, SM3_DIGEST_LENGTH);
    SM3_final(&ctx->sm3_ctx, digest);

    for (int i = 0; i < SM3_DIGEST_LENGTH / 2; i++)
        mac[i] = digest[i] ^ digest[i + SM3_DIGEST_LENGTH / 2];

    memzero(ctx, sizeof(*ctx));
    return 0;
}

int HMAC_SM3_once(unsigned char *mac, unsigned char *msg, int msglen, unsigned char *key, int keylen)
{
    HMAC_SM3_CTX hmac_ctx;
    HMAC_SM3_init(&hmac_ctx, key, keylen);
    HMAC_SM3_update(&hmac_ctx, msg, msglen);
    HMAC_SM3_final(&hmac_ctx, mac);
    return 0;
}