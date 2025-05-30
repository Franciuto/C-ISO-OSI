/* C-ISO-OSI - Data Link Layer - Functions - Francesco Fontanesi

    - Appends a control code (checksum) at the end of the payload for error detection.
    - On receiving a frame, verifies the checksum (CRC).
    - If the checksum is correct, forwards the payload to the upper layer.
    - If errors are detected, discards the frame without forwarding.

    Ensures reliable data transfer across the physical link by detecting and handling transmission errors.
*/

/* LIBRARY HEADERS */
#include "constants.h"  // Library constants
#include "level1_fisic.h"

//calculate checksum
unsigned char calculate_checksum(const char* data) {
    unsigned int sum = 0;
    for (int i = 0; data[i]; ++i) {
        sum += (unsigned char)data[i];
    }
    return sum % 256;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Simple frame validation function
static int validate_frame(const char* frame) {
    // For this simulation, we'll just check if the frame is not NULL and not empty
    if (frame == NULL || frame[0] == '\0') {
        return 0; // Invalid frame
    }
    return 1; // Valid frame
}
// In file: C-ISO-OSI/level2/level2_datalink.c

char* livello2_receive(const char* pdu) {

    char* frame = livello1_receive();

    // Check if frame is NULL (no more frames in FIFO)
    if (frame == NULL) {
        printf("[L2] Datalink - No more frames available from Physical layer\n");
        return NULL;
    }

    char received_checksum_str[3]; // For "XX"
    char received_data[PDU_SIZE];
    unsigned char calculated_checksum_val;
    
    // Find the checksum marker "[CHK="
    char* chk_marker = strstr(frame, "[CHK=");

    // Extract data part (everything before "[CHK=")
    size_t data_len = chk_marker - frame;
    if (data_len >= PDU_SIZE) {
        printf("[L2] Datalink ERROR - Data part too long for buffer. Length: %zu, Frame: %s\n", data_len, frame);
        free(frame);
        return NULL;
    }
    strncpy(received_data, frame, data_len);
    received_data[data_len] = '\0';

    // Extract checksum string 
    strncpy(received_checksum_str, chk_marker + 5, 2); // Skip "[CHK=" --> 5 characters
    received_checksum_str[2] = '\0';

    // Calculate checksum for the extracted data
    calculated_checksum_val = calculate_checksum(received_data);

    // Convert calculated checksum to a hex string for comparison
    char hex_calculated_checksum[3];
    snprintf(hex_calculated_checksum, sizeof(hex_calculated_checksum), "%02X", calculated_checksum_val);

    // Compare checksums
    if (strcmp(hex_calculated_checksum, received_checksum_str) != 0) {
        printf("[L2] Datalink ERROR - Checksum error! Frame discarded. Received checksum: %s, Calculated checksum: %s, Data: \"%.*s...\"\n", 
               received_checksum_str, hex_calculated_checksum, (int)(data_len > 30 ? 30 : data_len) , received_data);
        free(frame);
        return NULL;
    }

    printf("[L2] Datalink - Received valid frame, checksum OK. Data: \"%.*s...\"\n", (int)(data_len > 30 ? 30 : data_len), received_data);

    // Return the validated data (payload of L2 frame) to Layer 3
    char* data_for_l3 = strdup(received_data);
    free(frame); // Free the original frame received from L1
    return data_for_l3;
}
    
// Function to send a frame to Layer 1
char* livello2_send(const char* dati) {
    if (dati == NULL) {
        printf("[L2] Datalink ERROR - Cannot send NULL data\n");
        return NULL;
    }
    printf("[L2] Datalink - Sending frame: %s\n", dati);

    // For this simulation, we'll just pass the data to Layer 1
    unsigned char checksum = calculate_checksum(dati);
    char frame_with_checksum[PDU_SIZE];
    snprintf(frame_with_checksum, sizeof(frame_with_checksum), "%s[CHK=%02X]", dati, checksum);
    livello1_send(frame_with_checksum);
    
    return strdup(dati); // Return a copy of the sent data
}
