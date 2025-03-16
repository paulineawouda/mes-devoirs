#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"





/************* Funzioni locali *********************************/

int itemCompare( itemType item1, itemType item2 )
{
	return strcmp(item1.nome, item2.nome);
}





/* Alloca un nuovo nodo per la lista, copiando all'interno l'elemento item */
NODE* createNode( itemType item )
{
	NODE * p = (NODE*)malloc(sizeof(NODE));
	assert( p != NULL );

	p->item = item;
	p->next = NULL;
	return p;
}





/* Dealloca il nodo p */
void deleteNode( NODE* p )
{
	free(p);
}





/********** Funzioni standard *******************************/

 /* Inizializza una nuova lista */
LIST NewList()
{
	return NULL;
}





/* Azzera la lista */
LIST DeleteList( LIST l )
{
	NODE* tmp = l;

	while ( tmp != NULL )
	{
		NODE* todel = tmp;
		tmp = tmp->next;
		deleteNode( todel );
	}
	
	return NewList();
}





/* Restituisce TRUE se la lista e' vuota */
BOOL isEmpty( LIST l )
{
	return l == NULL;
}





/* Restituisce il numero di elementi presenti nella lista */
int getLength(LIST l)
{ 
	int size = 0;
	NODE* tmp = l; 

	while ( tmp != NULL )
	{
		size++;
		tmp = tmp->next;
	}

	return size;
}





/* Restituisce l'elemento in testa alla lista, senza modificarla */
itemType getHead(LIST l) 
{
	assert( !isEmpty(l) );
	return l->item;
}





/* Restituisce l'elemento in coda alla lista, senza modificarla */
itemType getTail(LIST l)
{
	assert( !isEmpty(l) );
	NODE* tmp = l;

	while( tmp->next != NULL )
		tmp = tmp->next;

	return tmp->item;
}



itemType getItemAtPos(LIST l, int pos)
{
	LIST tmp = l;
	int i = 0;
	while (tmp != NULL && i < pos) 
	{
		tmp = tmp->next;
		i++;
	}
	
	assert(tmp != NULL);
		
	return tmp->item;
}



  /* Cerca l'elemento item nella lista e ne restituisce il puntatore.
   Restituisce NULL se non lo trova */
itemType* Find( LIST l, itemType item )
{
	NODE* tmp = l;
	while( tmp != NULL && itemCompare(tmp->item, item) != 0 )
		tmp = tmp->next;

	if ( tmp == NULL ) {
		return NULL;
	}

	return &(tmp->item);
}



/* Inserisce un elemento nella prima posizione della lista */
LIST EnqueueFirst (LIST l, itemType item)
{  
	NODE* new_node = createNode(item);
	new_node->next = l;
	return new_node;
}





/* Inserisce un elemento nell'ultima posizione della lista */
LIST EnqueueLast (LIST l, itemType item) 
{
	if ( isEmpty( l ) )
	{
		/* In caso la lista sia vuota, eseguo un inserimento in testa */
		l = EnqueueFirst(l, item);  
	}
	else
	{
		/* Altrimenti devo scorrere la lista per inserire l'elemento nell'ultima posizione */
		NODE* tmp = l;
		NODE* new_node = createNode(item);
		while ( tmp->next != NULL ) {
			tmp = tmp->next;
		}
		tmp->next = new_node;
	}
	
	return l;
}





/* Inserisce un elemento mantenendo la lista ordinata */
LIST EnqueueOrdered(LIST l, itemType item )
{
	if ( isEmpty(l) || itemCompare(l->item, item) >= 0 ) {    
		/* In caso la lista sia vuota o il primo elemento della lista sia maggiore dell'elemento da inserire, eseguo un inserimento in testa */
		l = EnqueueFirst(l, item);    
	}

	else {
		/* Altrimenti devo scorrere la lista per inserire l'elemento nella posizione corretta */

		NODE* tmp = l;
		NODE* new_node = createNode(item);

		while( tmp->next != NULL && itemCompare(tmp->next->item,item) < 0 ) {
			tmp = tmp->next;
		}

		new_node->next = tmp->next;
		tmp->next = new_node;
	}

	return l;
}





 /* Toglie il primo elemento della lista (se non e' vuota) */
LIST DequeueFirst( LIST l )
{
	if ( !isEmpty( l ) )
	{
		NODE* todel = l;
		l = l->next;
		deleteNode( todel );
	}
	return l;
}





/* Toglie l'ultimo elemento della lista (se non e' vuota) */
LIST DequeueLast( LIST l )
{
	if ( !isEmpty( l ) )
	{
		if ( l->next == NULL ) {
			/* Se la lista è composta da un solo elemento, rimuovere in coda equivale a rimuovere in testa */
			l = DequeueFirst(l);
		}
		else {
			/* Altrimenti scorro la lista fino al PENULTIMO elemento in modo da deallocare il nodo seguente e impostare a NULL il puntatore ad esso */
			NODE* tmp = l;      
			while(tmp->next->next != NULL) {
				tmp = tmp->next;
			}
			deleteNode(tmp->next);
			tmp->next = NULL;
		}
	}

	return l;
}





/* Toglie l'elemento item dalla lista (se e' presente) */
LIST Dequeue( LIST l, itemType item )
{
	if ( ! isEmpty( l ) )
	{
		if ( itemCompare( l->item, item ) == 0 )
		{
			/* eliminazione dell'elemento in testa */
			NODE* todel = l;
			l = l->next;
			deleteNode( todel );
		}
		else
		{
			NODE* tmp = l;

			while ( tmp->next != NULL && itemCompare( tmp->next->item, item ) != 0 ) {
				tmp = tmp->next;
			}

			if ( tmp->next != NULL )
			{
				/* l'elemento e' stato trovato nella lista (viene eliminato) */
				NODE* todel = tmp->next;
				tmp->next = tmp->next->next;
				deleteNode( todel );
			}
		}
	}
	return l;
}





/* Stampa a video un elemento della lista */
void PrintItem( itemType item )
{
	printf("%s, punti %d, partite giocate %d\n",item.nome,item.punteggio, item.num_partite);
}





/* Stampa a video la lista (esegue PrintItem() su tutti gli elementi) */
void PrintList( LIST l )
{
	NODE* tmp = l;
	while ( tmp != NULL ) 
	{
		PrintItem( tmp->item );
		tmp = tmp->next;
	}
}


