#include <string.h>
#include "ibe.h"
#include "ibe_lcl.h"
#include "aes.h"

int compute_hmac_key(unsigned int usr_id, unsigned char *usr_privkey, unsigned char *hmac_key)
{
    int ret = -1;
    /* 大端格式的ID */
    unsigned int be_id;
    element_t QID, d, g;

    element_init_G1(QID, pairing);
    element_init_G1(d, pairing);
    element_init_GT(g, pairing);

    /* QID = H1(ID) */
    be_id = to_be32(usr_id);
    hash1(QID, (unsigned char*)(&be_id), 4);

    /* d */
    element_from_bytes(d, usr_privkey);

    /* g = e(QID, d) */
    element_pairing(g, QID, d);

    /* H4(g) */
    hash4(hmac_key, g);

    ret = 0;
// end:
    element_clear(QID);
    element_clear(d);
    element_clear(g);
    return ret;
}

int compute_mac(unsigned int usr_id, unsigned char *msg, unsigned short msg_len, unsigned char *usr_privkey, unsigned char *mac)
{
    int ret = -1;
    /* hash4的输出 */
    unsigned char hmac_key[SM3_DIGEST_LENGTH];
    compute_hmac_key(usr_id, usr_privkey, hmac_key);

    /* mac = HMAC(m, key) */
    HMAC_SM3_once(mac, msg, msg_len, hmac_key, SM3_DIGEST_LENGTH);

    ret = 0;
// end:
    return ret;
}

int mac_gen(unsigned int usr_id, unsigned char *msg, unsigned short msg_len, unsigned char *usr_privkey, unsigned char *mac)
{
    return compute_mac(usr_id, msg, msg_len, usr_privkey, mac);
}

int mac_verify(unsigned int usr_id, unsigned char *msg, unsigned short msg_len, unsigned char *usr_privkey, unsigned char *mac)
{
    unsigned char mac1[SM3_DIGEST_LENGTH];

    compute_mac(usr_id, msg, msg_len, usr_privkey, mac1);
    if (memcmp(mac, mac1, IBE_MAC_LEN) == 0) /* mac = mac1 */
        return 0;
    else /* mac != mac1 */
        return -1;
}

int mac_encrypt(unsigned char *out, int *out_len, const unsigned char *in, int in_len, 
                unsigned int usr_id, unsigned char *usr_privkey)
{
    unsigned char hmac_key[32];
    AES_KEY key;

    if (out == NULL || out_len == NULL || in == NULL || in_len < 0)
        return -1;

    if (compute_hmac_key(usr_id, usr_privkey, hmac_key) == -1)
        return -1;

    AES_set_encrypt_key(hmac_key, 128, &key);
    AES_ctr_encrypt(out, out_len, in, in_len, &key);
    return 0;
}

int mac_decrypt(unsigned char *out, int *out_len, const unsigned char *in, int in_len, 
                unsigned int usr_id, unsigned char *usr_privkey)
{
    return mac_encrypt(out, out_len, in, in_len, usr_id, usr_privkey);
    // unsigned char hmac_key[32];
    // AES_KEY key;

    // if (out == NULL || out_len == NULL || in == NULL || in_len < 0)
    //     return -1;

    // if (compute_hmac_key(usr_id, usr_privkey, hmac_key) == -1)
    //     return -1;

    // AES_set_encrypt_key(hmac_key, 128, &key);
    // AES_ctr_encrypt(out, out_len, in, in_len, &key);

    // return 0;
}

