


# define AES_MAXNR 14
# define AES_BLOCK_SIZE 16

struct aes_key_st {
    unsigned int rd_key[4 * (AES_MAXNR + 1)];
    int rounds;
};
typedef struct aes_key_st AES_KEY;

int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);

int AES_ecb_encrypt(unsigned char *out, int *out_len, const unsigned char *in, int in_len, const AES_KEY *key);
int AES_ecb_decrypt(unsigned char *out, int *out_len, const unsigned char *in, int in_len, const AES_KEY *key);
int AES_ctr_encrypt(unsigned char *out, int *out_len, const unsigned char *in, int in_len, const AES_KEY *key);
