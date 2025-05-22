/* C-ISO-OSI - Application layer - Functions */
#include "level7_application.h"
#include "level6_presentation.h"

char* livello7_send(const char* dati) {
    return livello6_send(dati);
}

char* livello7_receive(const char* pdu) {
    return livello6_receive(pdu);
}
