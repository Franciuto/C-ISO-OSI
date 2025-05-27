/* C-ISO-OSI - Session layer - Functions */
#include <constants.h>  // Library constants
#include "level5_session.h"
#include "level4_transport.h"

int id_session_generator() {
    time_t now = time(NULL);
    int random = rand() % 1000;
    return ((now % 1000000) * 1000 + random);
}
int current_session_id;

char* livello5_send(const char* dati const char* action) {
    printf("[Livello 5 - Sessione] SEND: Ricevuto payload: '%s', Azione: '%s'\n", dati, action);
    char header_buffer[128];                // inizialize header with the session level
    size_t header_len;
    int sess_id;  
    current_session_id = sess_id;           // update current session id
    if (action == "INIT"){
        sess_id = generate_session_id();    // generate session id
        snprintf(header_buffer, sizeof(header_buffer), "[SESS][INIT][ID = %d]", &sess_id);}     // print and copy into the header_buffer
    else if(action == "CLOSE"){
        snprintf(header_buffer, sizeof(header_buffer), "[SESS][CLOSE][ID = %d]", &sess_id);     // print and copy into the header_buffer
        sess_id = 0;}
    else
       snprintf(header_buffer, sizeof(header_buffer), "[SESS][NORMAL][ID= %d]", &sess_id);      // print and copy into the header_buffer
    char* segment = livello4_send(sess_id);
    return segment;
}

/*
char* livello5_receive(const char* pdu) {
    if(pdu == NULL) return NULL;
    char data[PDU_SIZE];
    if(strncmp(pdu, "[SESS]", 6) != 0) {                //verify the pdu validity
        printf("ERRORE! header SESS mancante!");
        return NULL;}
    const char* action_start = strchr(pdu + 6, '[');
    const char* action_end = strchr(action_start, ']');
    if (!action_end || !action_start){
        printf("ERRORE! action mancante o malformato!");
        return NULL;}
    
    size_t action_len = action_end - action_start - 1;  //get action len
    if (action_len >= 15){
        printf("ERRORE! Action non riconosciuta perche' troppo lunga!");
        return NULL;}
    const char* id_start = strstr(action_end, "[ID =");
    if (!id_start){
        printf("ERRORE! ID non presente!");         // get ID
        return NULL;}
    
    if (strcmp(action, "INIT") == 0) {
        current_session_id = id;                                // control which action esecute
        printf("Sessione inizializzata con ID: %d\n", id);
    }
    else if (strcmp(action, "CLOSE") == 0) {
        if (id != current_session_id) {                         // control ID validity
            printf("ERRORE! ID CLOSE non valido!\n");
            return NULL;
        }
        printf("Sessione chiusa con ID: %d\n", id);
        current_session_id = 0;
    }
    else if (strcmp(action, "NORMAL") == 0) {
        if (id != current_session_id) {
            printf("ERRORE! ID NORMAL non valido!\n");
            return NULL;
        }
    }
    else {
        printf("ERRORE! Action sconosciuta: %s\n", action);
        return NULL;
    }
    const char* payload_start = strchr(id_start, ']');          //get payload
    if (!payload_start) {                                       // verify payload validity
        printf("ERRORE! Payload mancante!\n");
        return NULL;
    }
    return livello6_receive(++payload_start);                   //go to level6_receive
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
