#include <time.h>
#include <string.h>
#include "ibe.h"
#include "speed.h"

#define MSG_LEN 16

/* 0->'0', 1->'1', ... , 15->'F' */
static const uint8_t ascii_table[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
};

int u8_to_hex(const uint8_t *in, int in_len, uint8_t *out)
{
    if (in == NULL || out == NULL)
        return -1;

    for (int i = 0; i < in_len; i++) {
        out[0] = ascii_table[in[i] >> 4];
        out[1] = ascii_table[in[i] & 0xf];
        out += 2;
    }
    return 0;
}

void random_string(unsigned char *s, int len)
{
    static int c = 0;
    srand((unsigned)time(NULL) + c);
    while (len--)
        s[len] = rand() % 256;

    c++;
}

int random_number()
{
    static int c = 0;
    srand((unsigned)time(NULL) + c);
    c++;
    return rand();
}

void print_hex(const char *name, unsigned char *s, int slen)
{
    for(int i = 0; i < strlen(name); i++)
        printf("%c", name[i]);
    unsigned char *hex = (unsigned char*)malloc(2*slen);
    u8_to_hex(s, slen, hex);
    for(int i = 0; i < 2*slen; i++)
        printf("%c", hex[i]);
    printf("\n");
    free(hex);
}

int sign_test()
{
    int N = 1111;
    int ret = -1;
    unsigned int usr_id;
    unsigned char master_privkey[IBE_MASTER_PRIVKEY_LEN];
    unsigned char master_pubkey[IBE_MASTER_PUBKEY_LEN];
    unsigned char usr_privkey[IBE_USR_PRIVKEY_LEN];
    unsigned char msg[MSG_LEN];
    unsigned char sig[IBE_SIG_LEN];

    for (int i = 0; i < N; i++) {
        /* generate master key */
        if (masterkey_gen(master_privkey, master_pubkey) == -1) {
            printf("masterkey_gen failed\n");
            goto end;
        }

        usr_id = random_number();
        /* generate user key */
        if (userkey_gen(usr_id, master_privkey, usr_privkey) == -1) {
            printf("userkey_gen failed\n");
            goto end;
        }

        random_string(msg, MSG_LEN);
        /* 使用用户的私钥计算消息msg的签名 */
        if (digital_sign(msg, MSG_LEN, usr_privkey, sig) == -1) {
            printf("digital_sign failed\n");
            goto end;
        }

        /* 使用用户的ID和主公钥验证消息msg的签名 */
        if (digital_verify(sig, msg, MSG_LEN, usr_id, master_pubkey) == -1) {
            printf("digital_verify failed\n");
            goto end;
        }
    }

    printf("signature test PASS\n");
    ret = 0;
end:
    return ret;
}

int mac_test()
{
    int N = 2222;
    int ret = -1;
    unsigned char master_privkey[IBE_MASTER_PRIVKEY_LEN];
    unsigned char master_pubkey[IBE_MASTER_PUBKEY_LEN];

    unsigned int usr_id_a;
    unsigned char usr_privkey_a[IBE_USR_PRIVKEY_LEN];

    unsigned int usr_id_b;
    unsigned char usr_privkey_b[IBE_USR_PRIVKEY_LEN];

    unsigned char msg[MSG_LEN];
    unsigned char mac[IBE_MAC_LEN];

    for (int i = 0; i < N; i++) {
        /* generate master key */
        if (masterkey_gen(master_privkey, master_pubkey) == -1) {
            printf("masterkey_gen failed\n");
            goto end;
        }

        usr_id_a = random_number();
        /* 生成用户a的私钥 */
        if (userkey_gen(usr_id_a, master_privkey, usr_privkey_a) == -1) {
            printf("userkey_gen failed\n");
            goto end;
        }

        usr_id_b = random_number();
        /* 生成用户b的私钥 */
        if (userkey_gen(usr_id_b, master_privkey, usr_privkey_b) == -1) {
            printf("userkey_gen failed\n");
            goto end;
        }

        random_string(msg, MSG_LEN);
        /* 用户a计算消息msg的认证码mac，只有用户b能验证mac */
        if (mac_gen(usr_id_b, msg, MSG_LEN, usr_privkey_a, mac) == -1) {
            printf("mac_gen failed\n");
            goto end;
        }

        /* 用户b使用私钥验证mac */
        if (mac_verify(usr_id_a, msg, MSG_LEN, usr_privkey_b, mac) == -1) {
            printf("mac_verify failed\n");
            goto end;
        }
    }

    printf("mac test PASS\n");
    ret = 0;
end:
    return ret;
}

/* mac加解密测试 */
int mac_enc_test()
{
    int N = 2222;
    int ret = -1;
    unsigned char master_privkey[IBE_MASTER_PRIVKEY_LEN];
    unsigned char master_pubkey[IBE_MASTER_PUBKEY_LEN];

    unsigned int usr_id_a;
    unsigned char usr_privkey_a[IBE_USR_PRIVKEY_LEN];

    unsigned int usr_id_b;
    unsigned char usr_privkey_b[IBE_USR_PRIVKEY_LEN];

    /* 消息明文 */
    unsigned char msg[MSG_LEN];
    /* 消息密文 */
    unsigned char cipher[MSG_LEN + 16];
    int cipher_len;
    /* 解密出的消息 */
    unsigned char decipher[MSG_LEN];
    int decipher_len;

    for (int i = 0; i < N; i++) {
        /* generate master key */
        if (masterkey_gen(master_privkey, master_pubkey) == -1) {
            printf("masterkey_gen failed\n");
            goto end;
        }

        usr_id_a = random_number();
        /* 生成用户a的私钥 */
        if (userkey_gen(usr_id_a, master_privkey, usr_privkey_a) == -1) {
            printf("userkey_gen failed\n");
            goto end;
        }

        usr_id_b = random_number();
        /* 生成用户b的私钥 */
        if (userkey_gen(usr_id_b, master_privkey, usr_privkey_b) == -1) {
            printf("userkey_gen failed\n");
            goto end;
        }

        /* 随机消息 */
        random_string(msg, MSG_LEN);
        /* 用户a计算对称加密密钥，加密消息 */
        if (mac_encrypt(cipher, &cipher_len, msg, MSG_LEN,
                        usr_id_b, usr_privkey_a) == -1) {
            printf("mac_encrypt failed\n");
            goto end;
        }

        /* 用户b计算对称加密密钥,解密消息 */
        if (mac_decrypt(decipher, &decipher_len, cipher, cipher_len,
                        usr_id_a, usr_privkey_b) == -1) {
            printf("mac_decrypt failed\n");
            goto end;
        }

        if (MSG_LEN != decipher_len || memcmp(msg, decipher, MSG_LEN) != 0) {
            printf("failed \n");
            goto end;
        }
    }

    printf("mac enc test PASS\n");
    ret = 0;
end:
    return ret;
}

int main(int argc, char **argv)
{
    ibe_init();
    sign_test();
    mac_test();
    mac_enc_test();
    return 0;
}
