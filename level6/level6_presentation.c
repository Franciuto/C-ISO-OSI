/* C-ISO-OSI - Presentation layer - Functions

    - Responsible for data translation and encryption (ROT13 only).
    - Converts data from the application layer into a common format for transmission.
    - Implements ROT13 encoding/decoding scheme.
    - Ensures that data sent from the application layer of one system can be read by the application layer of another.

    Takes data from the session layer, applies ROT13 cryptography, and passes it to the next layer.
*/

/* LIBRARY HEADERS */
#include "constants.h"  // Library constants
#include "level6_presentation.h"
#include "level5_session.h"

/* STANDARD HEADERS */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* rot13_encrypt(const char* input) {
    if (input == NULL) return NULL;
    char* output = strdup(input);
    if (output == NULL) {
        perror("Failed to allocate memory in rot13_encrypt");
        return NULL;
    }
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
    // ROT13 is its own inverse
    return rot13_encrypt(input);
}

char* livello6_send(const char* dati, const char* enc_type) {
    printf("[6] Presentation - Datas from L7: %s\n", dati);

    // Validate encryption type
    if (strcmp(enc_type, "ROT13") != 0) {
        fprintf(stderr, "[6] Presentation ERROR: Unsupported encoding type '%s'. Only ROT13 is supported.\n", enc_type);
        return NULL;
    }

    char* dati_enc = rot13_encrypt(dati);
    const char* header_l6_str = "[PRES][ENC=ROT13]";

    size_t header_len = strlen(header_l6_str);
    size_t enc_len = dati_enc ? strlen(dati_enc) : 0;
    
    // Check for encryption failure
    if (dati_enc == NULL) {
        fprintf(stderr, "[6] Presentation ERROR: ROT13 encryption failed\n");
        return NULL;
    }

    char* pdu_l6 = (char*)malloc(header_len + enc_len + 1);
    if (!pdu_l6) {
        fprintf(stderr, "[6] Presentation ERROR: Memory allocation failed\n");
        free(dati_enc);
        return NULL;
    }

    strcpy(pdu_l6, header_l6_str);
    strcat(pdu_l6, dati_enc);
    
    printf("[6] Presentation - PDU L6 created: \"%s\"\n", pdu_l6);
    free(dati_enc); 

    char* risultato_da_l5 = livello5_send(pdu_l6, "NORMAL"); 
    free(pdu_l6);
    return risultato_da_l5;
}

char* livello6_receive(const char* sdu_from_l5) {
    printf("[6] Presentation RECV - Starting processing of SDU from L5\n");
    printf("[6] Presentation RECV - Input SDU: \"%s\"\n", sdu_from_l5);

    if (sdu_from_l5 == NULL) {
        printf("[6] Presentation RECV ERROR: Received NULL SDU\n");
        return NULL;
    }

    const char* pres_header_tag = "[PRES][ENC=ROT13]";
    const char* payload_ptr = NULL;
    char* decoded_sdu = NULL;

    if (strncmp(sdu_from_l5, pres_header_tag, strlen(pres_header_tag)) == 0) {
        payload_ptr = sdu_from_l5 + strlen(pres_header_tag);
        printf("[6] Presentation RECV - Found ROT13 header. Encoded payload: \"%s\"\n", payload_ptr);
        
        decoded_sdu = rot13_decrypt(payload_ptr); 
        if (!decoded_sdu) {
            fprintf(stderr, "[6] Presentation RECV ERROR: ROT13 decoding failed\n");
            return NULL; 
        }
        printf("[6] Presentation RECV - ROT13 decoded to: \"%s\"\n", decoded_sdu);
    } else {
        fprintf(stderr, "[6] Presentation RECV ERROR: Missing or invalid ROT13 header\n");
        return NULL; 
    }
    
    return decoded_sdu;
}