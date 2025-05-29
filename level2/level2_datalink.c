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
#include "level2_datalink.h"

/* STANDARD HEADERS */
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
    
    // Additional validation could be added here (e.g., checksum verification)
    return 1; // Valid frame
}
// In file: C-ISO-OSI/level2/level2_datalink.c

// ... (other includes and functions like calculate_checksum, livello2_send)

char* livello2_receive(const char* pdu) {
    // The pdu parameter is ignored as it's coming from Layer 3
    // Instead, we get the data from Layer 1
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

    if (chk_marker == NULL || (chk_marker + 5 + 2) > (frame + strlen(frame)) || chk_marker[strlen(chk_marker)-1] != ']') {
        printf("[L2] Datalink ERROR - Checksum marker invalid or frame malformed. Frame: %s\n", frame);
        free(frame);
        return NULL;
    }

    // Extract data part (everything before "[CHK=")
    size_t data_len = chk_marker - frame;
    if (data_len >= PDU_SIZE) {
        printf("[L2] Datalink ERROR - Data part too long for buffer. Length: %zu, Frame: %s\n", data_len, frame);
        free(frame);
        return NULL;
    }
    strncpy(received_data, frame, data_len);
    received_data[data_len] = '\0';

    // Extract checksum string (the "XX" part)
    strncpy(received_checksum_str, chk_marker + 5, 2); // Skip "[CHK=" (5 chars)
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

    // IMPORTANT: The current implementation of livello3_receive expects the *full* frame 
    // (including L2 headers/trailers if it were to parse them).
    // If Layer 3 is meant to receive only the payload *without* the L2 checksum,
    // then 'strdup(received_data)' should be returned.
    // However, the existing L3 code doesn't seem to parse/strip L2 specific parts.
    // The original code returned 'frame', so we do the same if L3 is expected to handle it.
    // Given the project's simplified nature, Layer 3 might be expecting the data as it was before L2 checksum was added.
    // For now, let's return the data part that was checksummed. If L3 needs the [CHK=XX] part it's an issue.
    // The problem description's output shows L3 failing *after* L2 discards, implying L2 *should* forward valid data.
    // What L3 receives from L2 should be what L2 received from L1, with the L2 framing (checksum) removed.
    
    // Return the validated data (payload of L2 frame) to Layer 3
    char* data_for_l3 = strdup(received_data);
    free(frame); // Free the original frame received from L1
    return data_for_l3;
}

/*
char* livello2_receive(const char* pdu) {
    // The pdu parameter is ignored as it's coming from Layer 3
    // Instead, we get the data from Layer 1
    char* frame = livello1_receive();
    
    // Check if frame is NULL (no more frames in FIFO)
    if (frame == NULL) {
        printf("[L2] Datalink - No more frames available from Physical layer\n");
        return NULL;
    }
    
    // Validate the frame
    char received_checksum[3];
    char received_data[PDU_SIZE];
    unsigned char calc_checksum;
    sscanf(frame, "%[^[][CHK=%2s]", received_data, received_checksum);
    calc_checksum = calculate_checksum(received_data);
    
    char hex_checksum[3];
    snprintf(hex_checksum, 3, "%02X", calc_checksum);

    if (strcmp(hex_checksum, received_checksum) != 0) {
        printf("[L2] Datalink ERROR - Checksum error! Frame discarded. Received checksum: %s, Calculated checksum: %s\n", received_checksum, hex_checksum);
        free(frame);
        return NULL;
    }
    
    printf("[L2] Datalink - Received valid frame: %s\n", frame);
    
    // Return the frame to Layer 3
    // Note: The memory will be freed by the upper layer
    return frame;
}
*/
// Function to send a frame to Layer 1
char* livello2_send(const char* dati) {
    if (dati == NULL) {
        printf("[L2] Datalink ERROR - Cannot send NULL data\n");
        return NULL;
    }
    
    printf("[L2] Datalink - Sending frame: %s\n", dati);
    
    // In a real implementation, we would add a checksum here
    // For this simulation, we'll just pass the data to Layer 1
    unsigned char checksum = calculate_checksum(dati);
    char frame_with_checksum[PDU_SIZE];
    snprintf(frame_with_checksum, sizeof(frame_with_checksum), "%s[CHK=%02X]", dati, checksum);
    livello1_send(frame_with_checksum);
    
    return strdup(dati); // Return a copy of the sent data
}
