/* C-ISO/OSI - Datalink - Fornari Giordano at 02:53 AM
    - add MAC addresses
    - add/verifica cksum, passa payload se ok 
*/

#include "level1_fisic.h"  // livello 7
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// indirizzi mac fissi (come quelli veri)
#define SRC_MAC "00:03:93:6C:8B:EC" // mac generato da apple inc (blocco dal 2017)
#define DST_MAC "00:03:93:70:8B:8F" // mac casuale di apple inc
// format header l2
#define L2_HEADER_FORMAT "[MAC][SRC=%s][DST=%s]"

// calcola cksum (somma dei byte dei caratteri)
unsigned char calculate_checksum(const char* d) {
        unsigned int s = 0;
        for (int i = 0; d[i] != '\0'; ++i) { // ciclo fino al terminatore
                     s += (unsigned char)d[i]; // sommo il byte
        }
        return s % 256; // applico il modulo 256 (byte)
}

// ricevo il frame, verifico cksum, ritorno il payload
char* livello2_receive() {
        char* frame = livello1_receive(); // prendo il frame dal livello fisico

        if (frame == NULL) {
                     printf("opps... fatal error!\n");
                     return NULL;
        }

        char received_src_mac[18]; // 17 caratteri + terminatore
        char received_dst_mac[18];
        char r_ck[3];         // cksum in hex: 2 caratteri + terminatore
        char r_data[PDU_SIZE]; // buffer per i dati
        unsigned char do_cksum;

        int parsed_items = sscanf(frame, "[MAC][SRC=%17[^]]][DST=%17[^]]]", received_src_mac, received_dst_mac);

        if (parsed_items != 2) {
             printf("Sorry, there is a mistake.%s\n", frame);
             free(frame);
             return NULL;
        }

        char* payload_start = strstr(frame, received_dst_mac);
        if (payload_start) {
             payload_start = strchr(payload_start, ']');
             if (payload_start) {
                  payload_start++; // mi sposto all'inizio del payload (l3 pdu)
             } else {
                  printf("[2] Datalink - errore: riavviare il pc per fixare%s\n", frame);
                  free(frame);
                  return NULL;
             }
        }

        // ora cerco il marcatore del checksum "[CHK=" nel resto del frame
        char* chk_marker = strstr(payload_start, "[CHK=");
        if (!chk_marker) {
             printf("[l2] Datalink - errore: there is a MAN IN THE MIDDLE!!!!%s\n", frame);
             free(frame);
             return NULL;
        }

        // la lunghezza dei dati (r_data) è data dalla distanza tra payload_start e chk_marker
        size_t data_len = chk_marker - payload_start;

        if (data_len >= PDU_SIZE) {
                     free(frame);
                     return NULL;
        }
        strncpy(r_data, payload_start, data_len); // copio i dati
        r_data[data_len] = '\0';

        // copio i 2 caratteri del checksum
        // controllo che chk_marker + 5 non superi la fine se il frame è malformato
        strncpy(r_ck, chk_marker + 5, 2);
        r_ck[2] = '\0';
        
        do_cksum = calculate_checksum(r_data);

        // converto il checksum calcolato in esadecimale
        char h_ck[3]; // 2 caratteri + terminatore
        snprintf(h_ck, sizeof(h_ck), "%02x", do_cksum);

        // valido il checksum
        if (strcmp(h_ck, r_ck) != 0) {
             printf("[l2] Datalink - checksum non combacia! You got HACKED!!!: atteso %s, ottenuto %s. frame: %s\n", h_ck, r_ck, frame);
             // secondo le specifiche del progetto: in caso di errori si scarta il frame
             free(frame);
             return NULL; 
        }
        printf("    │    [2] Datalink - MAC: src=%s, dst=%s. payload ok, cksum %s. dati: \"%.*s...\"\n",
                 received_src_mac, received_dst_mac, h_ck, (int)(data_len > 30 ? 30 : data_len), r_data);

        char* d = strdup(r_data); // duplico solo il payload per il livello 3
        free(frame); // libero il frame completo ricevuto dal livello fisico
        return d;
}
          
// invio il frame: aggiungo mac, cksum e mando al livello fisico
char* livello2_send(const char* d) { // sdu da livello3
        // d è il pdu del livello 3
        printf(" │    [2] Datalink - payload da inviare: %s\n", d);

        char l2_header_buf[64]; // buffer per format header l2, src mac e dst mac
        snprintf(l2_header_buf, sizeof(l2_header_buf), L2_HEADER_FORMAT, SRC_MAC, DST_MAC);

        unsigned char ck = calculate_checksum(d); // calcolo il checksum
        
        char f[PDU_SIZE]; // buffer per il frame completo del livello 2
        snprintf(f, sizeof(f), "%s%s[CHK=%02x]", l2_header_buf, d, ck);
        
        livello1_send(f);
          
        return strdup(d); 
}