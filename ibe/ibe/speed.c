#include <time.h>
#include <string.h>
#include "ibe.h"
#include "speed.h"

#define MSG_LEN 233

/* default : 3.4 GHz, 根据实际情况调整 */
double cpu_speed = 3.4 * 1000;

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

void sign_speed()
{
    int N = 4567;
    double t;
    unsigned int usr_id;
    unsigned char master_privkey[IBE_MASTER_PRIVKEY_LEN];
    unsigned char master_pubkey[IBE_MASTER_PUBKEY_LEN];
    unsigned char usr_privkey[IBE_USR_PRIVKEY_LEN];
    unsigned char msg[MSG_LEN];
    unsigned char sig[IBE_SIG_LEN];

    masterkey_gen(master_privkey, master_pubkey);

    usr_id = 777777777;
    userkey_gen(usr_id, master_privkey, usr_privkey);

    random_string(msg, MSG_LEN);

    TIMER_VARS;
    TIMER_START();

    for (int i = 0; i < N; i++)
        digital_sign(msg, MSG_LEN, usr_privkey, sig);

    TIMER_STOP();
    t = USEC(TICKS());
    printf("average cycles per op : %d \n", (int)(TICKS()/N));
    // tt = t / 1000000;
    printf("签名 : %d  op/s\n\n", (int)((double)N*1000000/t));

    TIMER_START();

    for (int i = 0; i < N; i++)
        digital_verify(sig, msg, MSG_LEN, usr_id, master_pubkey);

    TIMER_STOP();
    t = USEC(TICKS());
    printf("average cycles per op : %d \n", (int)(TICKS()/N));
    // tt = t / 1000000;
    printf("验签 : %d  op/s\n\n", (int)((double)N*1000000/t));
}

void mac_speed()
{
    int N = 23456;
    double t;
    unsigned char master_privkey[IBE_MASTER_PRIVKEY_LEN];
    unsigned char master_pubkey[IBE_MASTER_PUBKEY_LEN];

    unsigned int usr_id_a;
    unsigned char usr_privkey_a[IBE_USR_PRIVKEY_LEN];

    unsigned int usr_id_b;
    unsigned char usr_privkey_b[IBE_USR_PRIVKEY_LEN];

    unsigned char msg[MSG_LEN];
    unsigned char mac[IBE_MAC_LEN];

    masterkey_gen(master_privkey, master_pubkey);

    usr_id_a = 7777777;
    userkey_gen(usr_id_a, master_privkey, usr_privkey_a);

    usr_id_b = 666666;
    userkey_gen(usr_id_b, master_privkey, usr_privkey_b);

    random_string(msg, MSG_LEN);

    TIMER_VARS;
    TIMER_START();

    for (int i = 0; i < N; i++)
        mac_gen(usr_id_b, msg, MSG_LEN, usr_privkey_a, mac);

    TIMER_STOP();
    t = USEC(TICKS());
    printf("average cycles per op : %d \n", (int)(TICKS()/N));
    // tt = t / 1000000;
    printf("mac_gen : %d  op/s\n\n", (int)((double)N*1000000/t));

    TIMER_START();

    for (int i = 0; i < N; i++)
        mac_verify(usr_id_a, msg, MSG_LEN, usr_privkey_b, mac);

    TIMER_STOP();
    t = USEC(TICKS());
    printf("average cycles per op : %d \n", (int)(TICKS()/N));
    // tt = t / 1000000;
    printf("mac_verify : %d  op/s\n\n", (int)((double)N*1000000/t));
}

void mac_enc_speed()
{
    int N = 23456;
    double t;
    unsigned char master_privkey[IBE_MASTER_PRIVKEY_LEN];
    unsigned char master_pubkey[IBE_MASTER_PUBKEY_LEN];

    unsigned int usr_id_a;
    unsigned char usr_privkey_a[IBE_USR_PRIVKEY_LEN];

    unsigned int usr_id_b;
    unsigned char usr_privkey_b[IBE_USR_PRIVKEY_LEN];

    unsigned char msg[MSG_LEN];
    unsigned char cipher[MSG_LEN + 16];
    int cipher_len;
    unsigned char decipher[MSG_LEN];
    int decipher_len;

    masterkey_gen(master_privkey, master_pubkey);

    usr_id_a = 7777777;
    userkey_gen(usr_id_a, master_privkey, usr_privkey_a);

    usr_id_b = 666666;
    userkey_gen(usr_id_b, master_privkey, usr_privkey_b);

    random_string(msg, MSG_LEN);

    TIMER_VARS;
    TIMER_START();

    for (int i = 0; i < N; i++)
        mac_encrypt(cipher, &cipher_len, msg, MSG_LEN, usr_id_b, usr_privkey_a);

    TIMER_STOP();
    t = USEC(TICKS());
    printf("average cycles per op : %d \n", (int)(TICKS()/N));
    // tt = t / 1000000;
    printf("mac_encrypt : %d  op/s\n\n", (int)((double)N*1000000/t));

    TIMER_START();

    for (int i = 0; i < N; i++)
        mac_decrypt(decipher, &decipher_len, cipher, cipher_len, usr_id_a, usr_privkey_b);

    TIMER_STOP();
    t = USEC(TICKS());
    printf("average cycles per op : %d \n", (int)(TICKS()/N));
    // tt = t / 1000000;
    printf("mac_decrypt : %d  op/s\n\n", (int)((double)N*1000000/t));
}

int main(int argc, char **argv)
{
    ibe_init();
    sign_speed();
    mac_speed();
    mac_enc_speed();
    return 0;
}