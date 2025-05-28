/* C-ISO-OSI - Network layer - Functions */
#include "constants.h" // Library constants
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "level3_network.h"
#include "level2_datalink.h"

char *livello3_send(const char *dati)
{
    printf("[L3] Inviato a livello 2: %s\n", dati);
    return livello2_send(dati);
}

char* livello3_receive(const char* pdu) {
    // Get data from Layer 2
    char* data_from_l2 = livello2_receive(pdu);
    
    // Check if we received valid data from Layer 2
    if (data_from_l2 == NULL) {
        printf("[L3] Network - No data received from Layer 2\n");
        return NULL;
    }
    
    // Network header
    const char* header = "[SRC=192.168.1.7] [DST=192.168.1.45] ";
    
    // Allocate buffer for header + data
    size_t header_len = strlen(header);
    size_t data_len = strlen(data_from_l2);
    size_t total_len = header_len + data_len + 1; // +1 for null terminator
    
    char* network_pdu = (char*)malloc(total_len);
    if (network_pdu == NULL) {
        printf("[L3] Network ERROR - Failed to allocate memory for PDU\n");
        free(data_from_l2); // Clean up
        return NULL;
    }
    
    // Construct the network PDU with header and data
    strcpy(network_pdu, header);
    strcat(network_pdu, data_from_l2);
    
    // Debug print
    printf("[L3] Network - Sending data to upper layers: \"%s\"\n", network_pdu);
    
    // Free the data received from Layer 2
    free(data_from_l2);
    
    return network_pdu;
}
