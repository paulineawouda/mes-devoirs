#ifndef _LIST_H
#define _LIST_H

#define BOOL int
#define FALSE 0
#define TRUE (!FALSE)

#define MAX_SIZE 128

#define TIPO_AGENTE 0
#define TIPO_INVESTITORE 1

/*** Elemento base contenuto nella lista ***/
typedef struct {
    char nome[MAX_SIZE];
    int titolo_quantita;
    int titolo_prezzo_corrente;
    int titolo_prezzo_minimo;
    int sockd;
    int ricavo_totale;
} ItemType;

struct Messaggio {
    int tipo;
    ItemType item;
};

/* Nodo della lista (per uso interno) */
struct LINKED_LIST_NODE {
    ItemType item;
    struct LINKED_LIST_NODE* next;
};

/* Per semplicità... */
typedef struct LINKED_LIST_NODE NODE;

/* La lista non è altro che il puntatore associato al primo elemento */
typedef NODE* LIST;

int itemCompare(ItemType item1, ItemType item2);

/*** Costruttore/Distruttore ***/

/* Inizializza una nuova lista */
LIST NewList();

/* Azzera la lista */
LIST DeleteList(LIST l);

/*** Predicati ***/

/* Restituisce TRUE se la lista e' vuota */
BOOL isEmpty(LIST l);

/* Restituisce il numero di elementi presenti nella lista */
int getLength(LIST l);

/*** Selettori ***/

/* Restituisce l'elemento in testa alla lista, senza modificarla */
ItemType getHead(LIST l);

/* Restituisce l'elemento in coda alla lista, senza modificarla */
ItemType getTail(LIST l);

/* Cerca l'elemento item nella lista e ne restituisce il puntatore
   ritorna NULL se non lo trova */
ItemType* Find(LIST l, ItemType item);

/*** Trasformatori ***/

/* Inserisce un elemento nella prima posizione della lista */
LIST EnqueueFirst(LIST l, ItemType item);

/* Inserisce un elemento nell'ultima posizione della lista */
LIST EnqueueLast(LIST l, ItemType item);

/* Inserisce un elemento mantenendo la lista ordinata */
LIST EnqueueOrdered(LIST l, ItemType item);

/* Toglie il primo elemento della lista (se non e' vuota) */
LIST DequeueFirst(LIST l);

/* Toglie l'ultimo elemento della lista (se non e' vuota) */
LIST DequeueLast(LIST l);

/* Toglie l'elemento item dalla lista (se e' presente) */
LIST Dequeue(LIST l, ItemType item);

/*** Stampe ***/

/* Stampa a video un elemento della lista */
void PrintItem(ItemType item);

/* Stampa a video la lista (esegue PrintItem() su tutti gli elementi) */
void PrintList(LIST l);

#endif
