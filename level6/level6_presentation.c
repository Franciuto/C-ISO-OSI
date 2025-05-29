/* C-ISO-OSI - Presentation layer - Functions

    - Responsible for data translation, encryption, and compression.
    - Converts data from the application layer into a common format for transmission.
    - Implements encoding/decoding schemes (e.g., ASCII, EBCDIC, encryption, compression).
    - Ensures that data sent from the application layer of one system can be read by the application layer of another.

    Takes data from the session layer, applies the selected cryptography (ROT13 in this case), and passes it to the next layer.
*/

/* LIBRARY HEADERS */
#include "constants.h"  // Library constants
#include "level6_presentation.h"
#include "level5_session.h"

/* STANDARD HEADERS */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Arry to contain all the function used during base64 encode/decode process
const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Debug print function (can be shared or defined per layer)

static void debug_print_pdu(const char* prefix, const char* pdu) {
    if (pdu == NULL) {
        printf("%s: (null)\n", prefix);
        return;
    }
    size_t len = strlen(pdu);
    // Adjust length for debug output if needed
    const int debug_len = 60; 
    if (len > debug_len) {
        char buf[debug_len + 4]; // for "..." and null terminator
        strncpy(buf, pdu, debug_len);
        strcpy(buf + debug_len, "...");
        printf("%s: \"%s\"\n", prefix, buf);
    } else {
        printf("%s: \"%s\"\n", prefix, pdu);
    }
}


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

char* base64_encode(const char* input) {
    if (input == NULL) return NULL;
    int len = strlen(input);
    // Adjusted allocation size: 4 * ceil(len / 3) + 1
    // (len + 2) / 3 ensures ceiling division for groups of 3.
    char* output = malloc((size_t)(4 * ((len + 2) / 3) + 1));
    if (output == NULL) {
        perror("Failed to allocate memory in base64_encode");
        return NULL;
    }

    int i = 0, j = 0, idx = 0;
    unsigned char buf3[3], buf4[4];

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

    if(i) { // Handle padding
        for(j = i; j < 3; j++) buf3[j] = '\0';
        buf4[0] = (buf3[0] & 0xfc) >> 2;
        buf4[1] = ((buf3[0] & 0x03) << 4) + ((buf3[1] & 0xf0) >> 4);
        buf4[2] = ((buf3[1] & 0x0f) << 2) + ((buf3[2] & 0xc0) >> 6);
        // buf4[3] = buf3[2] & 0x3f; // Not used for padding assignment

        for(j = 0; j < i + 1; j++) output[idx++] = base64_chars[buf4[j]];
        while(i++ < 3) output[idx++] = '='; // Append padding characters
    }
    output[idx] = '\0';
    return output;
}

char* base64_decode(const char* input) {
    if (input == NULL) return NULL;
    int len = strlen(input);
    if (len % 4 != 0) { // Base64 string length must be a multiple of 4
        // fprintf(stderr, "Invalid Base64 input length.\n");
        return NULL; 
    }

    // Estimate decoded length: 3 * (len / 4), minus padding
    // A more precise calculation would count non-padding chars.
    char* output = malloc((size_t)(len * 3 / 4 + 1));
    if (output == NULL) {
        perror("Failed to allocate memory in base64_decode");
        return NULL;
    }

    int i = 0, j = 0, idx = 0, in_ = 0;
    unsigned char buf4[4], buf3[3];
    int b64_val[256]; // Lookup table for base64 char values
    for(int k=0; k<256; k++) b64_val[k] = -1;
    for(int k=0; k<64; k++) b64_val[(int)base64_chars[k]] = k;
    b64_val['='] = -2; // Padding

    while(len-- && input[in_] != '\0' && input[in_] != '=') {
        if (b64_val[(int)input[in_]] == -1) { // Invalid char
            free(output); return NULL;
        }
        buf4[i++] = input[in_++];
        if (i == 4) {
            for (i = 0; i < 4; i++) buf4[i] = b64_val[(int)buf4[i]];

            buf3[0] = (buf4[0] << 2) + ((buf4[1] & 0x30) >> 4);
            buf3[1] = ((buf4[1] & 0x0f) << 4) + ((buf4[2] & 0x3c) >> 2);
            buf3[2] = ((buf4[2] & 0x03) << 6) + buf4[3];

            for (i = 0; i < 3; i++) output[idx++] = buf3[i];
            i = 0;
        }
    }

    if (i) { // Handle remaining characters and padding
        for (j = 0; j < i; j++) buf4[j] = b64_val[(int)buf4[j]]; // Convert already read chars
        
        // For padding, check '=' characters
        if (input[in_] == '=') {
            buf4[i++] = -2; // Use padding marker
            if (input[in_+1] == '=') buf4[i++] = -2;
        }
        
        for (j = i; j < 4; j++) buf4[j] = 0; // Fill rest with 0 if not already padding marker

        buf3[0] = (buf4[0] << 2) + ((buf4[1] & 0x30) >> 4);
        if (buf4[1] != -2) { // Check if there's enough data for buf3[1]
             buf3[1] = ((buf4[1] & 0x0f) << 4) + ((buf4[2] & 0x3c) >> 2);
        }


        output[idx++] = buf3[0];
        if (buf4[2] != -2) output[idx++] = buf3[1]; // Only add if not double padded
    }
    output[idx] = '\0';
    return output;
}


char* livello6_send(const char* dati, const char* enc_type) {
    printf("[6] Presentation - Datas from L7: %s\n", dati);

    char* dati_enc = NULL;
    const char* header_l6_str = NULL; 

    if (strcmp(enc_type, "BASE64") == 0) {
        dati_enc = base64_encode(dati);
        header_l6_str = "[PRES][ENC=BASE64]";
    } else { // Default to ROT13
        dati_enc = rot13_encrypt(dati);
        header_l6_str = "[PRES][ENC=ROT13]";
    }

    size_t header_len = strlen(header_l6_str);
    size_t enc_len = strlen(dati_enc);
    char* pdu_l6 = (char*)malloc(header_len + enc_len + 1);

    if (!pdu_l6) {
        fprintf(stderr, "[6] Presentation ERROR: Memory allocation failed\n");
        free(dati_enc);
        return NULL;
    }

    strcpy(pdu_l6, header_l6_str);
    strcat(pdu_l6, dati_enc);
    
    // Using the debug_print_pdu for consistent log formatting
    printf("[6] Presentation - PDU L6 created"); // Removed extra colon from previous suggestion.
    debug_print_pdu("", pdu_l6);


    free(dati_enc); 

    char* risultato_da_l5 = livello5_send(pdu_l6, "NORMAL"); 

    free(pdu_l6);

    return risultato_da_l5;
}

char* livello6_receive(const char* sdu_from_l5) { // Parameter renamed for clarity
    printf("[6] Presentation RECV - Starting processing of SDU from L5\n");
    debug_print_pdu("[6] Presentation RECV - Input SDU", sdu_from_l5);

    if (sdu_from_l5 == NULL) {
        printf("[6] Presentation RECV ERROR: Received NULL SDU\n");
        return NULL;
    }

    const char* pres_header_rot13_tag = "[PRES][ENC=ROT13]";
    const char* pres_header_base64_tag = "[PRES][ENC=BASE64]";
    
    const char* payload_ptr = NULL;
    char* decoded_sdu = NULL;

    if (strncmp(sdu_from_l5, pres_header_rot13_tag, strlen(pres_header_rot13_tag)) == 0) {
        payload_ptr = sdu_from_l5 + strlen(pres_header_rot13_tag);
        // No need to skip whitespace here if strcat in send doesn't add it.
        
        printf("[6] Presentation RECV - Found ROT13 header. Encoded payload: \"%s\"\n", payload_ptr);
        decoded_sdu = rot13_decrypt(payload_ptr); 
        if (!decoded_sdu) {
            // This case should ideally not happen if rot13_decrypt always allocates or payload_ptr is valid
            fprintf(stderr, "[6] Presentation RECV ERROR: ROT13 decoding failed (NULL returned) for payload: \"%s\"\n", payload_ptr);
            return NULL; 
        }
        printf("[6] Presentation RECV - ROT13 decoded \"%s\" to \"%s\"\n", payload_ptr, decoded_sdu);

    } else if (strncmp(sdu_from_l5, pres_header_base64_tag, strlen(pres_header_base64_tag)) == 0) {
        payload_ptr = sdu_from_l5 + strlen(pres_header_base64_tag);

        printf("[6] Presentation RECV - Found BASE64 header. Encoded payload: \"%s\"\n", payload_ptr);
        decoded_sdu = base64_decode(payload_ptr);
        if (!decoded_sdu) {
            fprintf(stderr, "[6] Presentation RECV ERROR: BASE64 decoding failed for payload: \"%s\"\n", payload_ptr);
            return NULL;
        }
        printf("[6] Presentation RECV - BASE64 decoded payload: \"%s\" to \"%s\"\n", payload_ptr, decoded_sdu); 
    } else {
        fprintf(stderr, "[6] Presentation RECV ERROR: Presentation header not found or unknown encoding in SDU: ");
        debug_print_pdu("", sdu_from_l5);
        return NULL; 
    }
    
    return decoded_sdu;
}