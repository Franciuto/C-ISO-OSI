/* C-ISO-OSI - Presentation layer - Functions

    - Responsible for data translation, encryption, and compression.
    - Converts data from the application layer into a common format for transmission.
    - Implements encoding/decoding schemes (e.g., ASCII, EBCDIC, encryption, compression).
    - Ensures that data sent from the application layer of one system can be read by the application layer of another.

    Takes data from the session layer, applies the selected encoding/decoding (e.g., ROT13), and passes it to the next layer.
*/

/* LIBRARY HEADERS */
#include <constants.h>  // Library constants
#include "level6_presentation.h"
#include "level5_session.h"

/* STANDARD HEADERS */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char tmpbuf[PDU_SIZE] = {0};

char* rot13_encrypt(const char* input) {
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
char* rot13_decrypt(const char* input){
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
    printf("[6] Presentation - Datas from L7: %s\n", dati);

    char* dati_enc = rot13_encrypt(dati);

    const char header_l6[] = "[PRES][ENC=ROT13]";
    size_t header_len = strlen(header_l6);
    size_t enc_len = strlen(dati_enc);
    
    char* pdu_l6 = (char*)malloc(header_len + enc_len + 1);

    strcpy(pdu_l6, header_l6);
    strcat(pdu_l6, dati_enc);

    printf("[6] Presentation - PDU L6 created: %s...\n", pdu_l6);

    free(dati_enc);

    char* risultato_da_l5 = livello5_send(pdu_l6, "NORMAL");
    
    free(pdu_l6);
    
    return risultato_da_l5;
}


char* livello6_receive(const char* pdu_l7) {
    
    printf("[6] Presentation RECV - Received PDU: %s...\n", pdu_l7);

    char* pdu_l6 = livello5_receive(pdu_l7);
    
    // Check if pdu_l6 is NULL before proceeding
    if (pdu_l6 == NULL) {
        printf("[6] Presentation RECV ERROR: Received NULL from Session Layer\n");
        return NULL;
    }
    
    // Now it's safe to use pdu_l6 after NULL check
    printf("[6] Presentation RECV - PDU to process (SDU da L5): %s...\n", pdu_l6);

    const char header[] = "[PRES][ENC=ROT13]";
    size_t header_len = strlen(header);

    if (strncmp(pdu_l6, header, header_len) == 0) {
        const char* payload_enc = pdu_l6 + header_len;
        printf("[6] Presentation RECV - Payload decrypted: %s...\n", payload_enc);

        char* dati_enc = rot13_decrypt(payload_enc);
        
        free(pdu_l6);

        return dati_enc; 
    } else {
        printf("[6] Presentation RECV ERROR: Header L6 '[PRES][ENC=ROT13]' not found. PDU received from L5: \"%s...\"\n", pdu_l6);
        free(pdu_l6);
        return NULL;
    }
}
