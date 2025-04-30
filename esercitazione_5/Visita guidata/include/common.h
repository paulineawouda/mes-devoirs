// include/common.h

#ifndef COMMON_H
#define COMMON_H

#define SERVER_PORT 5050
#define BUFFER_SIZE 1024
#define MAX_GUIDE 20
#define MAX_VISITATORI 50
#define MAX_NAME_LEN 30

// Messaggio dalla guida al server
typedef struct {
    char nome[MAX_NAME_LEN];
    int min_visite;
    int max_visite;
} GuidaInfo;

// Messaggio dal visitatore al server
typedef struct {
    int numero_persone;
} VisitatoreInfo;

#endif
