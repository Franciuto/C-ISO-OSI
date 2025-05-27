/* C-ISO-OSI - Session layer - Functions */
#include <constants.h>  // Library constants
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "level5_session.h"
#include "level4_transport.h"

int id_session_generator() {
    time_t now = time(NULL);
    int random = rand() % 1000;
    return ((now % 1000000) * 1000 + random);
}
int current_session_id;

char* livello5_send(const char* dati, const char* action) {
    printf("[Livello 5 - Sessione] SEND: Ricevuto payload: '%s', Azione: '%s'\n", dati, action);
    char header_buffer[128];                // inizialize header with the session level
    size_t header_len;
    int sess_id;  
    if (action == "INIT"){
        sess_id = id_session_generator();    // generate session id
        current_session_id = sess_id;           // update current session id
        snprintf(header_buffer, sizeof(header_buffer), "[SESS][INIT][ID = %d]", sess_id);}     // print and copy into the header_buffer
    else if(action == "CLOSE"){
        sess_id = current_session_id;
        snprintf(header_buffer, sizeof(header_buffer), "[SESS][CLOSE][ID = %d]", sess_id);     // print and copy into the header_buffer
        current_session_id = 0;}
    else {
        sess_id = current_session_id;
        snprintf(header_buffer, sizeof(header_buffer), "[SESS][NORMAL][ID = %d]", sess_id);
    }
    char pdu[PDU_SIZE];
    snprintf(pdu, sizeof(pdu), "%s%s", header_buffer, dati);
    return livello4_send(pdu);      
}

/*
char* livello5_receive(const char* pdu) {
    if (pdu == NULL) return NULL;                       // verify pdu is not empty

    if (strncmp(pdu, "[SESS]", 6) != 0) {               //verify the pdu validity
        printf("ERRORE! header SESS mancante!\n");
        return NULL;
    }
    char action[16];
    int id;
    if (sscanf(pdu, "[SESS][%15[^]]][ID = %d]", action, &id) != 2) {        // get action and id
        printf("ERRORE! Parsing fallito!\n");
        return NULL;
    }

    if (action == "INIT") {                                     // verify which action to execute
        current_session_id = id;
        printf("Sessione inizializzata con ID: %d\n", id);
    } else if (strcmp(action, "CLOSE") == 0) {
        if (id != current_session_id) {
            printf("ERRORE! ID CLOSE non valido!\n");
            return NULL;
        }
        printf("Sessione chiusa con ID: %d\n", id);
        current_session_id = 0;
    } else if (action == "NORMAL") {
        if (id != current_session_id) {
            printf("ERRORE! ID NORMAL non valido!\n");
            return NULL;
        }
    } else {
        printf("ERRORE! Action sconosciuta: %s\n", action);
        return NULL;
    }

    const char* payload_start = strchr(pdu, ']');
    if (payload_start) payload_start = strchr(payload_start + 1, ']');      // get the payload
    if (payload_start) payload_start = strchr(payload_start + 1, ']');
    if (!payload_start || !*(payload_start + 1)) {
        printf("ERRORE! Payload mancante!\n");
        return NULL;
    }

    return livello6_receive(++payload_start);     // go to livello6_receive
}
// da fare
*/
char* livello5_receive(const char* pdu) {
    if (pdu == NULL) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV ERROR: PDU è NULL.\n");
        return NULL;
    }
    
    printf("[Livello 5 - Sessione] RECV: Ricevuto PDU: '%s'\n", pdu);
    
    // Verifica che il PDU inizi con il header di sessione
    if (strncmp(pdu, "[SESS]", 6) != 0) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV ERROR: Header di sessione mancante.\n");
        return NULL;
    }
    
    // Trova l'azione e l'ID della sessione
    const char* action_start = strchr(pdu + 6, '[');
    const char* action_end = strchr(action_start, ']');
    
    if (!action_end || !action_start) {
        fprintf(stderr, "[Livello 5 - Sessione] RECV ERROR: Azione malformata.\n");
        return NULL;
    }
    
    // Estrai l'azione
    size_t action_len = action_end - action_start - 1;
    char action[32];
    strncpy(action, action_start + 1, action_len);
    action[action_len] = '\0';
    
    // Estrai l'ID della sessione
    int sess_id;
    sscanf(action_end + 1, "[ID = %d]", &sess_id);
    
    printf("[Livello 5 - Sessione] RECV: Azione '%s', ID sessione %d\n", action, sess_id);
    
    // Passa i dati al livello di trasporto
    const char* payload_start = action_end + 1;
    
    return livello4_receive(payload_start);
}
