/* C-ISO-OSI - Network layer - Functions - Franciuto */
// Library libraries
#include "constants.h" // Library constants
#include "level3_network.h"
#include "level2_datalink.h"

// Standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Function - livello3_send()
    Takes the value passed as input and sends it to layer 2 using its send function
    -- INPUT --
       -> Data to send to layer 2
    
    -- OUTPUT --
       -> Sends the data to layer 2
*/
char *livello3_send(const char *dati)
{
    printf("[L3] Inviato a livello 2: %s\n", dati);
    return livello2_send(dati);
}

/* Funzione - livello3_send()
    Function that adds the network header with hard coded IPs 
    -- INPUT --
       -> PDU from level2
    
    -- OUTPUT --
       -> Sends the final PDU to higher levels
*/
char* livello3_receive(const char* pdu) {
    // Get datas from level 2
    char* data_from_l2 = livello2_receive(pdu);
    
    // Check if no datas from l2
    if (data_from_l2 == NULL) {
        printf("[L3] Network - No data received from Layer 2\n");
        return NULL;
    }
    
    // Network header defintion (hard coded IPs only)
    const char* header = "[SRC=192.168.1.7] [DST=192.168.1.45] ";
    
    // Size calcs
    int header_len = strlen(header);
    int data_len = strlen(data_from_l2);
    int total_len = header_len + data_len + 1; // +1 for null byte terminator
    
    char* network_pdu = (char*)malloc(total_len);

    // Construct the final PDU
    strcpy(network_pdu, header);
    strcat(network_pdu, data_from_l2);
    
    // Debug print
    printf("[L3] Network - Sending data to upper layers: \"%s\"\n", network_pdu);
    
    // Free the data received from Layer 2
    free(data_from_l2);
    
    return network_pdu;
}
