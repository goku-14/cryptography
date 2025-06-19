#pragma once
#include "pbc/pbc.h"

#ifdef  __cplusplus
extern "C" {
#endif

unsigned int to_be32(const unsigned int in);
void memzero(void *const p, const int len);
int pkcs5_pad(unsigned char *buf, unsigned int unpadded_buflen, unsigned int blocksize);
int pkcs5_unpad(unsigned int *unpadded_buflen_p, const unsigned char *buf, unsigned int blocksize);

// SM3算法消息摘要长度
#define SM3_DIGEST_LENGTH 32
// SM3算法消息分组长度
#define SM3_BLOCK_SIZE    64

typedef struct
{
    unsigned int digest[8];
    int nblocks;
    unsigned char block[64];
    int num;
}SM3_CTX;

#define ROTATELEFT(X,n) (((X)<<(n)) | ((X)>>(32-(n))))

#define P0(x) ((x) ^  ROTATELEFT((x),9)  ^ ROTATELEFT((x),17)) 
#define P1(x) ((x) ^  ROTATELEFT((x),15) ^ ROTATELEFT((x),23)) 

#define FF0(x,y,z) ((x) ^ (y) ^ (z)) 
#define FF1(x,y,z) (((x) & (y)) | ( (x) & (z)) | ( (y) & (z)))

#define GG0(x,y,z) ((x) ^ (y) ^ (z)) 
#define GG1(x,y,z) (((x) & (y)) | ( (~(x)) & (z)) )

int SM3_init(SM3_CTX *ctx);
int SM3_update(SM3_CTX *ctx, const unsigned char* data, int datalen);
int SM3_final(SM3_CTX *ctx, unsigned char *digest);
int SM3_once(const unsigned char *m, int mlen, unsigned char digest[SM3_DIGEST_LENGTH]);

/* {0, 1}* -> G1 */
void hash1(element_t r, unsigned char *m, int mlen);

/* {0, 1}* -> Zn */
void hash2(element_t r, unsigned char *m, int mlen);

/* G1 -> Zn */
void hash3(element_t r, element_t p);

/* GT -> {0, 1}*, r的长度是32字节 */
void hash4(unsigned char *r, element_t p);

#define MAC_KEY_LEN SM3_BLOCK_SIZE
#define HMAC_IPAD 0x36
#define HMAC_OPAD 0x5c

typedef struct HMAC_SM3_CTX
{
    SM3_CTX sm3_ctx;

    /* key */
    unsigned char key[MAC_KEY_LEN];
}HMAC_SM3_CTX;

int HMAC_SM3_init(HMAC_SM3_CTX *ctx, unsigned char *key, int key_len);
int HMAC_SM3_update(HMAC_SM3_CTX *ctx, unsigned char *in, int inlen);
int HMAC_SM3_final(HMAC_SM3_CTX *ctx, unsigned char *mac);
int HMAC_SM3_once(unsigned char *mac, unsigned char *msg, int msglen, unsigned char *key, int keylen);

#ifdef  __cplusplus
}
#endif