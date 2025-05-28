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
#include "level4_transport.h"

/* STANDARD HEADERS */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char tmpbuf[PDU_SIZE] = {0};
const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char* base64_encode(const char* input);
char* base64_decode(const char* input);

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

char* base64_encode(const char* input) {
    int len = strlen(input), i = 0, j = 0;
    unsigned char buf3[3], buf4[4];
    char* output = malloc((4 * ((len + 2) / 3) + 1));
    int idx = 0;

    while(len--) {
        buf3[i++] = *(input++);
        if(i == 3) {
            buf4[0] = (buf3[0] & 0xfc) >> 2;
            buf4[1] = ((buf3[0] & 0x03) << 4) + ((buf3[1] & 0xf0) >> 4);
            buf4[2] = ((buf3[1] & 0x0f) << 2) + ((buf3[2] & 0xc0) >> 6);
            buf4[3] = buf3[2] & 0x3f;

            for(i = 0; i < 4; i++) output[idx++] = base64_chars[buf4[i]];
            i = 0;
        }
    }

    if(i) {
        for(j = i; j < 3; j++) buf3[j] = '\0';

        buf4[0] = (buf3[0] & 0xfc) >> 2;
        buf4[1] = ((buf3[0] & 0x03) << 4) + ((buf3[1] & 0xf0) >> 4);
        buf4[2] = ((buf3[1] & 0x0f) << 2) + ((buf3[2] & 0xc0) >> 6);
        buf4[3] = buf3[2] & 0x3f;

        for(j = 0; j < i + 1; j++) output[idx++] = base64_chars[buf4[j]];
        while(i++ < 3) output[idx++] = '=';
    }
    output[idx] = '\0';
    return output;
}

char* base64_decode(const char* input) {
    int len = strlen(input), i = 0, j, in = 0;
    unsigned char buf4[4], buf3[3];
    char* output = malloc(len * 3 / 4 + 1);
    int idx = 0;

    while(len-- && ( input[in] != '=')) {
        buf4[i++] = input[in++];
        if(i ==4) {
            for(i = 0; i <4; i++) buf4[i] = strchr(base64_chars, buf4[i]) - base64_chars;

            buf3[0] = ( buf4[0] << 2 ) + ((buf4[1] & 0x30) >> 4);
            buf3[1] = ((buf4[1] & 0xf) << 4) + ((buf4[2] & 0x3c) >> 2);
            buf3[2] = ((buf4[2] & 0x3) << 6) + buf4[3];

            for(i = 0; i < 3; i++) output[idx++] = buf3[i];
            i = 0;
        }
    }
    if(i) {
        for(j = i; j <4; j++) buf4[j] = 0;
        for(j = 0; j <4; j++) buf4[j] = strchr(base64_chars, buf4[j]) - base64_chars;
        buf3[0] = ( buf4[0] << 2 ) + ((buf4[1] & 0x30) >> 4);
        buf3[1] = ((buf4[1] & 0xf) << 4) + ((buf4[2] & 0x3c) >> 2);
        for(j = 0; j < i - 1; j++) output[idx++] = buf3[j];
    }
    output[idx] = '\0';
    return output;
}

char* livello6_send(const char* dati, const char* enc_type) {
    printf("[6] Presentation - Datas from L7: %s\n", dati);

    char* dati_enc = NULL;
    const char* header_l6 = NULL;

    if (strcmp(enc_type, "BASE64") == 0) {
        dati_enc = base64_encode(dati);
        header_l6 = "[PRES][ENC=BASE64]";
    } else {
        dati_enc = rot13_encrypt(dati);
        header_l6 = "[PRES][ENC=ROT13]";
    }

    if (!dati_enc || !header_l6) {
        fprintf(stderr, "[6] Presentation ERROR: Encoding failed\n");
        return NULL;
    }

    size_t header_len = strlen(header_l6);
    size_t enc_len = strlen(dati_enc);
    char* pdu_l6 = (char*)malloc(header_len + enc_len + 1);

    if (!pdu_l6) {
        fprintf(stderr, "[6] Presentation ERROR: Memory allocation failed\n");
        free(dati_enc);
        return NULL;
    }

    strcpy(pdu_l6, header_l6);
    strcat(pdu_l6, dati_enc);

    printf("[6] Presentation - PDU L6 created: %s...\n", pdu_l6);

    free(dati_enc);

    char* risultato_da_l5 = livello5_send(pdu_l6, "NORMAL");

    free(pdu_l6);

    return risultato_da_l5;
}


// Check if the PDU contains a transport header
static int is_transport_fragment(const char* pdu) {
    if (pdu == NULL) return 0;
    
    // Check for direct transport header at start
    if (strncmp(pdu, "[TRANS]", 7) == 0) {
        return 1;
    }
    
    // Check for transport header after network headers
    if (strstr(pdu, "[TRANS]") != NULL) {
        return 1;
    }
    
    return 0;
}

// Print the PDU content for debugging, limited to a reasonable length
static void debug_print_pdu(const char* prefix, const char* pdu) {
    if (pdu == NULL) {
        printf("%s: (null)\n", prefix);
        return;
    }
    
    size_t len = strlen(pdu);
    if (len > 40) {
        char buf[44];
        strncpy(buf, pdu, 40);
        buf[40] = '.';
        buf[41] = '.';
        buf[42] = '.';
        buf[43] = '\0';
        printf("%s: \"%s\"\n", prefix, buf);
    } else {
        printf("%s: \"%s\"\n", prefix, pdu);
    }
}

char* livello6_receive(const char* pdu_l7) {
    printf("[6] Presentation RECV - Starting processing of PDU\n");
    debug_print_pdu("[6] Presentation RECV - Input PDU", pdu_l7);
    
    // Check for NULL input
    if (pdu_l7 == NULL) {
        printf("[6] Presentation RECV ERROR: Received NULL PDU\n");
        return NULL;
    }
    
    // Handle transport fragments first
    if (is_transport_fragment(pdu_l7)) {
        printf("[6] Presentation RECV - Detected transport fragment, forwarding to transport layer\n");
        return livello4_receive(pdu_l7);
    }
    
    // Process PDU from session layer
    printf("[6] Presentation RECV - Processing session layer PDU\n");
    char* session_pdu = livello5_receive(pdu_l7);
    if (!session_pdu) {
        printf("[6] Presentation RECV ERROR: NULL from Session layer\n");
        return NULL;
    }
    
    printf("[6] Presentation RECV - Got PDU from Session: %s\n", session_pdu);
    
    // Find presentation header
    const char* header = "[PRES][ENC=ROT13]";
    const char* payload = strstr(session_pdu, header);
    if (!payload) {
        printf("[6] Presentation RECV ERROR: Missing [PRES] header\n");
        free(session_pdu);
        return NULL;
    }
    
    // Skip header and whitespace
    payload += strlen(header);
    while (*payload && (*payload == ' ' || *payload == '\n' || *payload == '\t')) 
        payload++;
    
    printf("[6] Presentation RECV - Found encoded payload: %s\n", payload);
    
    // ROT13 decode
    char* decoded = NULL;
    if (strstr(session_pdu, "[ENC=BASE64]")) {
        decoded = base64_decode(payload);
        if (!decoded) {
            printf("[6] Presentation RECV ERROR: Failed to decode BASE64\n");
            free(session_pdu);
            return NULL;
        }
    } else {
        decoded = rot13_decrypt(payload);
        if (!decoded) {
            printf("[6] Presentation RECV ERROR: Failed to decode ROT13\n");
            free(session_pdu);
            return NULL;
        }
    }
    if (!decoded) {
        printf("[6] Presentation RECV ERROR: Failed to decode ROT13\n");
        free(session_pdu);
        return NULL;
    }
    
    printf("[6] Presentation RECV - ROT13 decoded '%s' back to '%s'\n", 
           payload, decoded);
    
    // Clean up
    free(session_pdu);
    
    return decoded;
}
