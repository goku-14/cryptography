#include "ibe.h"
#include "ibe_lcl.h"

int digital_sign(unsigned char *msg, unsigned short msg_len, unsigned char *usr_privkey, unsigned char *ds)
{
    int ret = -1;
    element_t k, d, R, S, tmp1, tmp2;

    element_init_Zr(k, pairing);
    element_init_G1(d, pairing);
    element_init_G1(R, pairing);
    element_init_G1(S, pairing);
    element_init_Zr(tmp1, pairing);
    element_init_G1(tmp2, pairing);

    /* random */
    element_random(k);

    /* R = k * P */
    element_mul_zn(R, P, k);

    /* H2(M) */
    hash2(tmp1, msg, msg_len);

    /* H2(M) * P */
    element_mul_zn(S, P, tmp1);

    /* d */
    element_from_bytes(d, usr_privkey);

    /* H3(R) */
    hash3(tmp1, R);

    /* H3(R) * d */
    element_mul_zn(tmp2, d, tmp1);

    /* (H2(M) * P + H3(R) * d) */
    element_add(S, S, tmp2);

    /* k^-1 */
    element_invert(k, k);

    /* S = k^-1 * (H2(M) * P + H3(R) * d) */
    element_mul_zn(S, S, k);

    /* R -> byte array */
    element_to_bytes_compressed(ds, R);
    /* S -> byte array */
    element_to_bytes_compressed(ds + (IBE_SIG_LEN / 2), S);

    ret = 0;
// end:
    element_clear(k);
    element_clear(d);
    element_clear(R);
    element_clear(S);
    element_clear(tmp1);
    element_clear(tmp2);
    return ret;
}

int digital_verify(unsigned char *ds, unsigned char *msg, unsigned short msg_len, unsigned int usr_id, unsigned char *master_pubkey)
{
    int ret = -1;
    /* 大端格式的ID */
    unsigned int be_id;
    element_t R, S, QID, Ppub, g, g1, g2, tmp;

    element_init_G1(R, pairing);
    element_init_G1(S, pairing);
    element_init_G1(QID, pairing);
    element_init_G1(Ppub, pairing);
    element_init_GT(g, pairing);
    element_init_GT(g1, pairing);
    element_init_GT(g2, pairing);
    element_init_Zr(tmp, pairing);

    element_from_bytes_compressed(R, ds);
    element_from_bytes_compressed(S, ds + (IBE_SIG_LEN / 2));

    /* Ppub */
    element_from_bytes(Ppub, master_pubkey);

    /* g = e(R, S) */
    element_pairing(g, R, S);

    /* QID = H1(ID) */
    be_id = to_be32(usr_id);
    hash1(QID, (unsigned char*)(&be_id), 4);

    /* e(P, P) */
    element_pairing(g1, P, P);

    /* H2(M) */
    hash2(tmp, msg, msg_len);

    /* e(P, P) ^ H2(M) */
    element_pow_zn(g1, g1, tmp);

    /* e(Ppub, QID) */
    element_pairing(g2, Ppub, QID);

    /* H3(R) */
    hash3(tmp, R);

    /* e(Ppub, QID) ^ H3(R) */
    element_pow_zn(g2, g2, tmp);

    /* (e(P, P) ^ H2(M)) * (e(Ppub, QID) ^ H3(R)) */
    element_mul(g1, g1, g2);

    /* g ?= g1 */
    if (element_cmp(g, g1) != 0) /* not equal */
        goto end;

    ret = 0;
end:
    element_clear(R);
    element_clear(S);
    element_clear(QID);
    element_clear(Ppub);
    element_clear(g);
    element_clear(g1);
    element_clear(g2);
    element_clear(tmp);
    return ret;
}