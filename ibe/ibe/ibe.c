#include "ibe.h"

pairing_t pairing;
/* G1 generator */
char *P_str = "[9541583739409553, 23964324730191654]";
element_t P;
int initialized = 0;

static char *param = 
"type a\n"
"q 27021598569529331\n"
"h 12\n"
"r 2251799880794111\n"
"exp2 51\n"
"exp1 26\n"
"sign1 1\n"
"sign0 -1\n";

int ibe_init()
{
    if (initialized == 0) {
        if (pairing_init_set_str(pairing, param) != 0) /* failure */
            goto end;

        element_init_G1(P, pairing);
        if (element_set_str(P, P_str, 10) == 0) /* failure */
            goto end;
    }

    initialized = 1;
    return 0;
end:
    pairing_clear(pairing);
    // element_clear(P);
    return -1;
}

