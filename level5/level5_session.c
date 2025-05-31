/* C-ISO-OSI - Session layer - Functions */
#include "constants.h"  // Library constants
#include "level5_session.h"
#include "level4_transport.h"
#include "level6_presentation.h" // For livello6_receive
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* livello5_send(const char* dati, const char* action) {
    printf("[5] - Sessione SEND: Ricevuto payload: '%s', Azione: '%s'\n", dati, action);
    char header_buffer[128];                // inizialize header and session level
    static int sess_id = -1; // static session ID for coerence
    if (strcmp(action, "INIT") == 0){
        sess_id = rand() % 10000 + 1; // generate dinamic ID between 1 and 10000
        
        snprintf(header_buffer, sizeof(header_buffer), "[SESS][INIT][ID=%d]", sess_id);     // print and copy into the header_buffer
    }
    else if(strcmp(action, "CLOSE") == 0){
        snprintf(header_buffer, sizeof(header_buffer), "[SESS][CLOSE][ID=%d]", sess_id);
        sess_id = -1; // ID reset after CLOSE
        sess_id = 0;
    }
    else {
        snprintf(header_buffer, sizeof(header_buffer), "[SESS][NORMAL][ID=%d]", sess_id);
    }
    size_t total_len = strlen(header_buffer) + strlen(dati) + 1;
    char* pack = malloc(total_len);
    snprintf(pack, total_len, "%s%s", header_buffer, dati);
    livello4_send(pack);
    return pack;
}

char* livello5_receive() {
    char *pdu = livello4_receive(); // Receive PDU from transport layer
    // Trova l'azione e l'ID della sessione
    if (pdu == NULL) {
        fprintf(stderr, "[5] - Sessione RECV INFO: Livello trasporto non ha restituito un SDU completo (potrebbe essere in attesa di altri frammenti o FIFO vuota).\n");
        return NULL; // Propagate NULL or handle error appropriately
    }

    const char* action_start = strchr(pdu + 6, '[');
    const char* action_end = NULL;
    
    if (action_start) {
        action_end = strchr(action_start, ']');
    }
    
    if (!action_end || !action_start) {
        fprintf(stderr, "[5] - Sessione RECV ERROR: Azione malformata.\n");
        return NULL;
    }
    
    // axtract action
    size_t action_len = action_end - action_start - 1;
    char action[32];
    strncpy(action, action_start + 1, action_len);
    action[action_len] = '\0';
    
    // Scan forward for the ID section
    const char* id_part = action_end + 1;
    while (*id_part == ' ') id_part++; // Skip spaces
    
    // Check if we found the ID section
    if (strncmp(id_part, "[ID", 3) != 0) {
        fprintf(stderr, "[5] - Sessione RECV ERROR: ID section not found after action.\n");
        return NULL;
    }
    
    // Extract the session ID
    static int sess_id = -1; // static session ID for coerence between calls
    sscanf(id_part, "[ID=%d]", &sess_id);
    
    printf("[5] - Sessione RECV: Azione '%s', ID sessione %d\n\n\n", action, sess_id);
    static int current_session_id = 0;
    if (strcmp(action, "INIT") == 0) {
        current_session_id = sess_id;
        return livello5_receive();
     } else if (current_session_id != sess_id) {
        fprintf(stderr, "[5] - Sessione RECV ERROR: ID sessione incoerente (atteso: %d, ricevuto: %d)\n", current_session_id, sess_id);
        return NULL;
    } else if (strcmp(action, "CLOSE") == 0) {
        current_session_id = -1;
    }
    
    // find the end of ID section
    const char* id_end = strchr(id_part, ']');
    if (!id_end) {
        fprintf(stderr, "[5] - Sessione RECV ERROR: ID section malformed.\n");
        return NULL;
    }
    
    // Extract the actual payload after all session headers
    const char* payload_start = id_end + 1;
    
    // Skip any leading whitespace in the payload
    while (*payload_start == ' ') payload_start++;
    
    // Find presentation layer header in the payload
    char* pres_header = strstr(payload_start, "[PRES]");
       
    return pres_header; // Return the decoded data from L6
}