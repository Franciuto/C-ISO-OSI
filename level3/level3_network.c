/* C-ISO-OSI - Network Layer - Fontanesi Francesco
 *
 * Handles the routing of data packets across networks.
 *   - Adding source and destination IP addresses to the PDU
 *   - Establishing logical connections between network nodes
 *
 * In this implementation, the layer receives data from upper layers,
 * adds network headers, and passes it down
 */

// LIBRARY HEADERS
#include "constants.h" // Library constants
#include "level3_network.h"
#include "level2_datalink.h"

// STANDARD HEADERS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *livello3_send(const char *dati)
{
    // Network header defintion (hard coded IPs only)
    const char *header = "[SRC=192.168.1.7] [DST=192.168.1.45] ";
    
    // len calcs
    int header_len = strlen(header);
    int data_len = strlen(dati);
    int total_len = header_len + data_len + 1; // +1 for '\0'
    
    char *network_pdu = (char *)malloc(total_len);
    
    // Construct the final PDU
    strcpy(network_pdu, header);
    strcat(network_pdu, dati);
    
    // Free the data received from Layer 2
    printf(" │    [3] Inviato a livello 2: %s\n", network_pdu);

    return livello2_send(network_pdu);
}


#include <ctype.h>  // for isdigit

// Helper function to validate IP addresses using mask and shift
int validate_ip(const char *ip) {
    unsigned int ip_val = 0;
    int octet = 0;
    int octet_count = 0;
    while (*ip) {
        if (*ip >= '0' && *ip <= '9') {
            octet = octet * 10 + (*ip - '0');
            if (octet > 255)
                return 0;
        } else if (*ip == '.') {
            ip_val = (ip_val << 8) | octet;
            octet = 0;
            octet_count++;
            if (octet_count > 3)
                return 0;
        } else {
            return 0;
        }
        ip++;
    }
    if (octet_count != 3)
        return 0;
    ip_val = (ip_val << 8) | octet;
    return 1;
}

char *livello3_receive()
{
    // Get data from level 2
    char *data_from_l2 = livello2_receive();

    // Check if no data received from Layer 2
    if (data_from_l2 == NULL){
        printf("[L3] Network - No data received from Layer 2\n");
        return NULL;
    }

    // Expected header format: "[SRC=xxx.xxx.xxx.xxx] [DST=xxx.xxx.xxx.xxx] "
    // Find end of the source header: first closing bracket ']'
    char *endSrc = strchr(data_from_l2, ']');
    if (endSrc == NULL){
        printf("[L3] Network - Error: Missing source header\n");
        free(data_from_l2);
        return NULL;
    }

    // Extract source IP: skip "[SRC=" (5 characters) then copy until ']'
    int srcLen = endSrc - (data_from_l2 + 5);
    if (srcLen <= 0 || srcLen >= 16) {
        printf("[L3] Network - Error: Invalid source IP length\n");
        free(data_from_l2);
        return NULL;
    }
    char srcIP[16];
    strncpy(srcIP, data_from_l2 + 5, srcLen);
    srcIP[srcLen] = '\0';

    // Find the destination header: it should start with "[DST=" after the source header
    char *dstStart = strstr(endSrc, "[DST=");
    if (dstStart == NULL){
        printf("[L3] Network - Error: Missing destination header\n");
        free(data_from_l2);
        return NULL;
    }

    // Find the closing bracket ']' for destination header
    char *endDst = strchr(dstStart, ']');
    if (endDst == NULL){
        printf("[L3] Network - Error: Malformed destination header\n");
        free(data_from_l2);
        return NULL;
    }

    // Extract destination IP: skip "[DST=" (5 characters) then copy until ']'
    int dstLen = endDst - (dstStart + 5);
    if (dstLen <= 0 || dstLen >= 16) {
        printf("[L3] Network - Error: Invalid destination IP length\n");
        free(data_from_l2);
        return NULL;
    }
    char dstIP[16];
    strncpy(dstIP, dstStart + 5, dstLen);
    dstIP[dstLen] = '\0';

    // Validate IP addresses using mask and shift
    if (!validate_ip(srcIP)) {
        printf("[L3] Network - Error: Invalid source IP format: %s\n", srcIP);
        free(data_from_l2);
        return NULL;
    }
    if (!validate_ip(dstIP)) {
        printf("[L3] Network - Error: Invalid destination IP format: %s\n", dstIP);
        free(data_from_l2);
        return NULL;
    }

    // Locate the start of the payload: it should be after the destination header and a following space.
    char *pduStart = endDst + 1;
    if (*pduStart == ' ')
        pduStart++;

    // Duplicate the payload into new memory to return
    char *network_pdu = strdup(pduStart);
    if (network_pdu == NULL) {
        printf("[L3] Network - Error: Memory allocation failure\n");
        free(data_from_l2);
        return NULL;
    }

    printf("    │    [3] Network - Valid header. Passing data to upper layers: \"%s\"\n", network_pdu);

    // Free the data received from Layer 2
    free(data_from_l2);

    return network_pdu;
}
