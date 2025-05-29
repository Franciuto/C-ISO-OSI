/* C-ISO-OSI - Presentation layer - Functions */

// Include guard
#ifndef LEVEL6_PRESENTATION_H
#define LEVEL6_PRESENTATION_H

// Layer functions
char* livello6_send(const char* dati, const char* enc_type);
char* livello6_receive(const char* pdu);
// Fucntions to encrypt/decrypt
char* rot13_encrypt(const char* input);
char* rot13_decrypt(const char* input);
// Funtions to encode/decode using base64
char* base64_encode(const char* input);
char* base64_decode(const char* input);
#endif
