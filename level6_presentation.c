#include "level6_presentation.h"
#include <string.h>
#include <stdio.h>
#include "level5_session.h"

char tmpbuf[1024] = {0};

char* rot13(const char* input) {
    char* output = strdup(input);
    for (int i = 0; output[i] != '\0'; i++) {
        char c = output[i];

        if (c >= 'A' && c <= 'Z') {
            output[i] = ((c - 'A' + 13) % 26) + 'A';
        } else if (c >= 'a' && c <= 'z') {
            output[i] = ((c - 'a' + 13) % 26) + 'a';
        }
    }
    return output;
}

char* livello6_send(const char* dati) {
    char *res = rot13(dati);
    strcpy(tmpbuf, res);
    printf("[L6] Sessione - Messaggio da inviare: %s\n", res);
    return res;
}

char* livello6_receive(const char* pdu) {
    printf("[L6] Sessione - Messaggio da ricevere: %s\n", tmpbuf);
    return rot13(tmpbuf);
}
