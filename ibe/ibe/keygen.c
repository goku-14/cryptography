#include "ibe.h"
#include "ibe_lcl.h"

int masterkey_gen(unsigned char *master_privkey, unsigned char *master_pubkey)
{
    int ret = -1;
    element_t s, Ppub;

    element_init_Zr(s, pairing);
    element_init_G1(Ppub, pairing);

    /* random master private key */
    element_random(s);
    // element_printf("%B\n", s);

    /* master public key */
    element_mul_zn(Ppub, P, s);
    // element_printf("%x",Ppub);

    /* master private key to byte array */
    element_to_bytes(master_privkey, s);

    /* public key to byte array */
    element_to_bytes(master_pubkey, Ppub);

    ret = 0;
// end:
    element_clear(s);
    element_clear(Ppub);
    return ret;
}

int userkey_gen(unsigned int usr_id, unsigned char *master_privkey, unsigned char *usr_privkey)
{
    int ret = -1;
    /* 大端格式的ID */
    unsigned int be_id;
    element_t QID, d, s;

    element_init_G1(QID, pairing);
    element_init_G1(d, pairing);
    element_init_Zr(s, pairing);

    /* QID = H1(ID) */
    be_id = to_be32(usr_id);
    hash1(QID, (unsigned char*)(&be_id), 4);

    /* master private key */
    element_from_bytes(s, master_privkey);

    /* d = s * QID */
    element_mul_zn(d, QID, s);

    /* user private key to byte array */
    element_to_bytes(usr_privkey, d);

    ret = 0;
// end:    
    element_clear(QID);
    element_clear(d);
    element_clear(s);
    return ret;
}