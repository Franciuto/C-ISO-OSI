#include "constants.h"
#include "level4_transport.h"
#include "level3_network.h"
#include "level5_session.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MTU_PAYLOAD 20
#define MAX_FRAGMENTS 16
#define MAX_MESSAGES 8

static int transport_pdu_id_counter = 1;

// Arrays for fragment reassembly
static int msg_ids[MAX_MESSAGES] = {-1};  // Message IDs
static int msg_total_fragments[MAX_MESSAGES] = {0};  // Total fragments per message
static int msg_fragments_received[MAX_MESSAGES] = {0};  // Fragments received per message
static int msg_in_use[MAX_MESSAGES] = {0};  // Whether slot is in use
static char* msg_fragments[MAX_MESSAGES][MAX_FRAGMENTS] = {{NULL}};  // Fragment payloads

// Flag to track if arrays are initialized
static int reassembly_initialized = 0;

// Function prototypes for internal functions
void init_reassembly_arrays(void);
void free_message(int index);
int find_message_by_id(int id);
int find_empty_slot(void);
char* reassemble_message(int msg_index);
const char* find_transport_header(const char* pdu);

// Initialize the transport layer
void livello4_init() {
    printf("[4] Trasporto - Initializing transport layer...\n");
    
    // Reset PDU ID counter
    transport_pdu_id_counter = 1;
    
    // Initialize the reassembly arrays
    init_reassembly_arrays();
    
    printf("[4] Trasporto - Transport layer initialized successfully\n");
}

char* livello4_send(const char* sdu_from_l5) {
    if (sdu_from_l5 == NULL) {
        fprintf(stderr, "[4] Trasporto SEND ERRORE: SDU da L5 è NULL. Invio PDU vuota a L3.\n");
        char pdu_buffer[100];
        int temp_id = transport_pdu_id_counter++;
        snprintf(pdu_buffer, sizeof(pdu_buffer), "[TRANS] [FRAG=1/1] [ID=%02d] ", temp_id);
        livello3_send(pdu_buffer);
        return strdup(pdu_buffer);
    }

    size_t sdu_len = strlen(sdu_from_l5);
    printf("[4] Trasporto SEND - Ricevuto SDU da L5 (len: %zu): \"%.*s\"\n", sdu_len, MTU_PAYLOAD, sdu_from_l5);

    int num_fragments = (sdu_len == 0) ? 1 : (sdu_len + MTU_PAYLOAD - 1) / MTU_PAYLOAD;
    int current_id = transport_pdu_id_counter++;
    char* first_pdu_to_return = NULL;

    for (int k = 0; k < num_fragments; k++) {
        char pdu_buffer[PDU_SIZE];
        char header_l4[75];
        
        snprintf(header_l4, sizeof(header_l4), "[TRANS] [FRAG=%d/%d] [ID=%02d] ", k + 1, num_fragments, current_id);

        size_t current_payload_offset = k * MTU_PAYLOAD;
        size_t current_payload_len;

        if (sdu_len == 0) {
            current_payload_len = 0;
        } else {
            current_payload_len = (k == num_fragments - 1) ? (sdu_len - current_payload_offset) : MTU_PAYLOAD;
        }
        
        snprintf(pdu_buffer, PDU_SIZE, "%s%.*s", header_l4, (int)current_payload_len, sdu_from_l5 + current_payload_offset);

        printf("[4] Trasporto SEND - Invio PDU (ID=%02d, Frag=%d/%d) a L3: \"%.30s...\"\n", current_id, k + 1, num_fragments, pdu_buffer);
        
        livello3_send(pdu_buffer);

        first_pdu_to_return = strdup(pdu_buffer);
        }
        return first_pdu_to_return;
}

// Initialize the reassembly arrays
void init_reassembly_arrays() {
    for (int i = 0; i < MAX_MESSAGES; i++) {
        msg_ids[i] = -1;
        msg_total_fragments[i] = 0;
        msg_fragments_received[i] = 0;
        msg_in_use[i] = 0;
        
        for (int j = 0; j < MAX_FRAGMENTS; j++) {
            msg_fragments[i][j] = NULL;
        }
    }
    reassembly_initialized = 1;
}

// Free a message from the reassembly arrays
void free_message(int index) {
    if (index < 0 || index >= MAX_MESSAGES) return;
    
    for (int j = 0; j < MAX_FRAGMENTS; j++) {
        if (msg_fragments[index][j] != NULL) {
            free(msg_fragments[index][j]);
            msg_fragments[index][j] = NULL;
        }
    }
    
    msg_in_use[index] = 0;
    msg_ids[index] = -1;
    msg_total_fragments[index] = 0;
    msg_fragments_received[index] = 0;
}

// Find a message by ID
int find_message_by_id(int id) {
    for (int i = 0; i < MAX_MESSAGES; i++) {
        if (msg_in_use[i] && msg_ids[i] == id) {
            return i;
        }
    }
    return -1;
}

// Find an empty slot in the arrays
int find_empty_slot() {
    for (int i = 0; i < MAX_MESSAGES; i++) {
        if (!msg_in_use[i]) {
            return i;
        }
    }
    return -1;
}

// Reassemble a complete message from fragments
char* reassemble_message(int msg_index) {
    if (msg_index < 0 || msg_index >= MAX_MESSAGES) return NULL;
    
    // Calculate total length needed
    size_t total_len = 0;
    for (int i = 0; i < msg_total_fragments[msg_index]; i++) {
        if (msg_fragments[msg_index][i] == NULL) {
            fprintf(stderr, "[4] Trasporto RECV ERRORE: Frammento %d mancante durante il riassemblaggio del messaggio ID=%d\n", 
                    i+1, msg_ids[msg_index]);
            return NULL;
        }
        total_len += strlen(msg_fragments[msg_index][i]);
    }
    
    // Allocate buffer for the complete message
    char* complete_message = (char*)malloc(total_len + 1);
    if (!complete_message) {
        perror("[4] Trasporto RECV ERRORE: malloc fallito per complete_message");
        return NULL;
    }
    
    // Reassemble fragments - they already have transport headers removed
    complete_message[0] = '\0';
    for (int i = 0; i < msg_total_fragments[msg_index]; i++) {
        strcat(complete_message, msg_fragments[msg_index][i]);
    }
    
    printf("[4] Trasporto RECV - Messaggio completo riassemblato (ID=%d): \"%.40s...\"\n", 
           msg_ids[msg_index], complete_message);
    
    return complete_message;
}

// Find the transport header in a PDU that may contain network headers
const char* find_transport_header(const char* pdu) {
    if (pdu == NULL) return NULL;
    
    // Look for the transport header
    const char* trans_header = strstr(pdu, "[TRANS]");
    if (trans_header == NULL) {
        fprintf(stderr, "[4] Trasporto RECV ERRORE: Transport header non trovato nel PDU\n");
        return NULL;
    }
    
    return trans_header;
}

char* livello4_receive(const char* pdu_from_l3) {
    // Initialize reassembly arrays if not already done
    if (!reassembly_initialized) {
        init_reassembly_arrays();
    }
    
    // If this is a direct call from Application/Presentation layer with a fragment
    // that shouldn't happen, but we need to handle it gracefully
    if (pdu_from_l3 == NULL) {
        fprintf(stderr, "[4] Trasporto RECV ERRORE: PDU ricevuto è NULL\n");
        return NULL;
    }
    
    // Find the transport header in the PDU (after network headers)
    const char* trans_header = find_transport_header(pdu_from_l3);
    if (trans_header == NULL) {
        // If no transport header is found, pass the PDU directly to the session layer
        return livello5_receive(pdu_from_l3);
    }
    
    int k_frag, n_total_frags, pdu_id_received;
    int header_actual_len = 0;
    
    if (sscanf(trans_header, "[TRANS] [FRAG=%d/%d] [ID=%d]%n", &k_frag, &n_total_frags, &pdu_id_received, &header_actual_len) == 3 && 
       header_actual_len > 0 && trans_header[header_actual_len - 1] == ']') {
        const char* payload_start_ptr = trans_header + header_actual_len + 1; // +1 to skip the space after header
        
        printf("[4] Trasporto RECV - Header L4 analizzato: K=%d, N=%d, ID=%d. Payload (frammento) inizia da: \"%.20s...\"\n",
               k_frag, n_total_frags, pdu_id_received, payload_start_ptr);
        
        // Log the network headers that were found
        if (trans_header != pdu_from_l3) {
            int network_header_len = trans_header - pdu_from_l3;
            char network_headers[128];
            strncpy(network_headers, pdu_from_l3, (network_header_len > 127) ? 127 : network_header_len);
            network_headers[(network_header_len > 127) ? 127 : network_header_len] = '\0';
            printf("[4] Trasporto RECV - Trovati network headers: \"%s\"\n", network_headers);
        }
        
        // Validate fragment index
        if (k_frag < 1 || k_frag > n_total_frags || n_total_frags > MAX_FRAGMENTS) {
            fprintf(stderr, "[4] Trasporto RECV ERRORE: Indice frammento non valido (K=%d, N=%d)\n", 
                    k_frag, n_total_frags);
            return NULL;
        }
        
        // Find or create message entry
        int msg_index = find_message_by_id(pdu_id_received);
        if (msg_index < 0) {
            // Message not found, create new entry
            msg_index = find_empty_slot();
            if (msg_index < 0) {
                fprintf(stderr, "[4] Trasporto RECV ERRORE: Buffer di riassemblaggio pieno\n");
                return NULL;
            }
            
            // Initialize new message entry
            msg_in_use[msg_index] = 1;
            msg_ids[msg_index] = pdu_id_received;
            msg_total_fragments[msg_index] = n_total_frags;
            msg_fragments_received[msg_index] = 0;
        } else {
            // Verify consistency
            if (msg_total_fragments[msg_index] != n_total_frags) {
                fprintf(stderr, "[4] Trasporto RECV ERRORE: Incoerenza nel numero totale di frammenti per ID=%d (%d vs %d)\n", 
                        pdu_id_received, msg_total_fragments[msg_index], n_total_frags);
                return NULL;
            }
        }
        
        // Store fragment (0-based index)
        int frag_index = k_frag - 1;
        
        // Check if fragment already received
        if (msg_fragments[msg_index][frag_index] != NULL) {
            fprintf(stderr, "[4] Trasporto RECV AVVISO: Frammento %d per ID=%d già ricevuto, sovrascrittura\n", 
                    k_frag, pdu_id_received);
            free(msg_fragments[msg_index][frag_index]);
        } else {
            msg_fragments_received[msg_index]++;
        }
        
        // Store only the fragment payload (without any headers)
        msg_fragments[msg_index][frag_index] = strdup(payload_start_ptr);
        if (!msg_fragments[msg_index][frag_index]) {
            perror("[4] Trasporto RECV ERRORE: strdup fallito per il frammento");
            return NULL;
        }
        
        printf("[4] Trasporto RECV - Stored fragment %d/%d: \"%s\"\n", 
               k_frag, n_total_frags, msg_fragments[msg_index][frag_index]);
        
        // Check if all fragments received
        if (msg_fragments_received[msg_index] == msg_total_fragments[msg_index]) {
            printf("[4] Trasporto RECV - Tutti i frammenti ricevuti per ID=%d, riassemblaggio...\n", pdu_id_received);
            
            // Reassemble complete message
            char* complete_message = reassemble_message(msg_index);
            if (!complete_message) {
                fprintf(stderr, "[4] Trasporto RECV ERRORE: Riassemblaggio fallito per ID=%d\n", pdu_id_received);
                free_message(msg_index);
                return NULL;
            }
            
            printf("[4] Trasporto RECV - Messaggio riassemblato completo: \"%s\"\n", complete_message);
            
            // Now the message should be the complete session layer PDU
            printf("[4] Trasporto RECV - Forwarding complete reassembled message to Session layer\n");
            
            // Forward reassembled message to upper layer
            char* result_from_l5 = livello5_receive(complete_message);
            
            // Cleanup
            free(complete_message);
            free_message(msg_index);
            
            return result_from_l5;
        } else {
            printf("[4] Trasporto RECV - Frammento %d/%d ricevuto per ID=%d (%d/%d frammenti)\n", 
                   k_frag, n_total_frags, pdu_id_received, 
                   msg_fragments_received[msg_index], 
                   msg_total_fragments[msg_index]);
            
            // Return NULL to indicate no complete message yet
            return NULL;
        }
    } else {
        fprintf(stderr, "[4] Trasporto RECV ERRORE: Formato transport header non valido: \"%.30s...\"\n", trans_header);
        return NULL;
    }
}
