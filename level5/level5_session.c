/* C-ISO-OSI - Session layer - Functions */
#include "constants.h"  // Library constants
#include "level5_session.h"
#include "level4_transport.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* livello5_send(const char* dati, const char* action) {
    printf("[Livello 5 - Sessione] SEND: Ricevuto payload: '%s', Azione: '%s'\n", dati, action);
    char header_buffer[128];                // inizializza l'header del livello di sessione
    static int sess_id = -1; // session ID statico per coerenza tra chiamate
    if (strcmp(action, "INIT") == 0){
        sess_id = rand() % 10000 + 1; // genera ID dinamico casuale tra 1 e 10000
        snprintf(header_buffer, sizeof(header_buffer), "[SESS] [INIT] [ID = %d]", sess_id);     // print and copy into the header_buffer
    }
    else if(strcmp(action, "CLOSE") == 0){
        snprintf(header_buffer, sizeof(header_buffer), "[SESS] [CLOSE] [ID = %d]", sess_id);
        sess_id = -1; // resetta ID dopo CLOSE
        sess_id = 0;
    }
    else {
        snprintf(header_buffer, sizeof(header_buffer), "[SESS] [NORMAL] [ID= %d]", sess_id);
    }
    size_t total_len = strlen(header_buffer) + strlen(dati) + 1;
    char* pack = malloc(total_len);
    snprintf(pack, total_len, "%s%s", header_buffer, dati);

    return livello4_send(pack);
}

/*
char* livello5_receive(const char* pdu) {
    if(pdu == NULL) return NULL;
    char data[PDU_SIZE];
    if(strncmp(pdu, "[SESS]", 6) != 0) {
        printf("ERRORE! header SESS mancante!");
        return NULL;}
    const char* action_start = strchr(pdu + 6, '[');
    const char* action_end = strchr(action_start, ']');
    if (!action_end || !action_start){
        printf("ERRORE! action mancante o malformato!");
        return NULL;}
    


    return 0;
}
// da fare
*/
char* livello5_receive(const char* pdu) {
    if (pdu == NULL) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV ERROR: PDU è NULL.\n");
        return NULL;
    }
    
    printf("[Livello 5 - Sessione] RECV: Ricevuto PDU: '%s'\n", pdu);
    
    // Check if this is a transport layer fragment or a complete message
    if (strncmp(pdu, "[TRANS]", 7) == 0) {
        // This is a transport layer fragment - it should be handled by the transport layer
        fprintf(stderr, "[Livello 5 - Sessione] RECV: Ricevuto un frammento di trasporto, passando al livello 4.\n");
        return NULL; // Return NULL to indicate we can't process this fragment yet
    }
    
    // Verifica che il PDU inizi con il header di sessione
    if (strncmp(pdu, "[SESS]", 6) != 0) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV ERROR: Header di sessione mancante.\n");
        return NULL;
    }
    
    // Trova l'azione e l'ID della sessione
    const char* action_start = strchr(pdu + 6, '[');
    const char* action_end = NULL;
    
    if (action_start) {
        action_end = strchr(action_start, ']');
    }
    
    if (!action_end || !action_start) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV ERROR: Azione malformata.\n");
        return NULL;
    }
    
    // Estrai l'azione
    size_t action_len = action_end - action_start - 1;
    char action[32];
    strncpy(action, action_start + 1, action_len);
    action[action_len] = '\0';
    
    // Scan forward for the ID section
    const char* id_part = action_end + 1;
    while (*id_part == ' ') id_part++; // Skip spaces
    
    // Check if we found the ID section
    if (strncmp(id_part, "[ID", 3) != 0) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV ERROR: ID section not found after action.\n");
        return NULL;
    }
    
    // Extract the session ID
    static int sess_id = -1; // session ID statico per coerenza tra chiamate
    int scanned = sscanf(id_part, "[ID= %d]", &sess_id);
    if (scanned != 1) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV WARNING: Could not parse session ID.\n");
    } else {
        printf("[Livello 5 - Sessione] RECV: Azione '%s', ID sessione %d\n", action, sess_id);
    static int current_session_id = -1;
    if (strcmp(action, "INIT") == 0) {
        current_session_id = sess_id;
    } else if (current_session_id != sess_id) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV ERROR: ID sessione incoerente (atteso: %d, ricevuto: %d)\n", current_session_id, sess_id);
        return NULL;
    } else if (strcmp(action, "CLOSE") == 0) {
        current_session_id = -1;
    }
    }
    
    // Find the end of the session header (closing bracket of ID section)
    const char* id_end = strchr(id_part, ']');
    if (!id_end) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV ERROR: ID section malformed.\n");
        return NULL;
    }
    
    // Extract the actual payload after all session headers
    const char* payload_start = id_end + 1;
    
    // Skip any leading whitespace in the payload
    while (*payload_start == ' ') payload_start++;
    
    // Find presentation layer header in the payload
    const char* pres_header = strstr(payload_start, "[PRES]");
    if (pres_header) {
        // If found, return just the presentation PDU
        printf("[Livello 5 - Sessione] RECV: Found presentation header in payload\n");
        printf("[Livello 5 - Sessione] RECV: Extracted clean payload: '%s'\n", pres_header);
        return strdup(pres_header);
    } else {
        // Otherwise return the whole payload (might contain other headers)
        printf("[Livello 5 - Sessione] RECV: No presentation header found, returning full payload\n");
        printf("[Livello 5 - Sessione] RECV: Extracted payload: '%s'\n", payload_start);
        return strdup(payload_start);
    }
}
