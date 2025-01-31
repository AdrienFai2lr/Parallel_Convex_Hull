/*
 * file              : sortm.c
 * created by        : lemarch
 * creation date     : 01/12/98
 */
/* TP algorithmique parallele
 * maitrise
 * LL
 * 07/10/2017
 */

/* 
 * sortm.c
 * algorithme maitre
 * -- tri recursif & fusion
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "pvm3.h"
#include "sort.h"

static pb_t *Q[PB];		/* la pile de problemes */
static int Q_nb;			/* l'index de tete de pile */

/*
 * remplit un tableau d'entiers aleatoirement
 * dans l'intervalle [0..99]
 * la taille du tableau est donnee
 * par DATA (cf sort.h)
 */
void set_random_data(data)
	int *data;
{
	int i;

	for (i=0; i<DATA; i++) 
		data[i] = rand()%100;
}

/*
 * initialise la file de problemes
 * chacun des PB problemes est un
 * probleme de tri de taille N
 * stocke dans le premier champ de
 * donnees de la structure.
 */

void init_queue(data)
	int *data;
{
	int i;

	for (i=0; i<PB; i++) {
		Q[i] = (pb_t *)malloc(sizeof(pb_t));
		Q[i]->taille1 = N;
		Q[i]->taille2 = 0;
		Q[i]->data1 = (int *)malloc(N*sizeof(int));
		copy_int(Q[i]->data1, data + i*N, N);
		Q[i]->data2 = NULL;
		Q[i]->type = PB_TRI;
	}
	Q_nb = PB;	
}

/*
 * empile ou depile dans la 
 * pile globale Q un probleme
 */

pb_t *depile()
{
	if (Q_nb > 0)
		return Q[--Q_nb];
	else
		return NULL;
}

void empile(pb)
	pb_t *pb;
{
	Q[Q_nb++] = pb;
}

/*
 * programme maitre
 */

main(argc, argv)
	int argc; 
	char **argv;
{
	int i;
	int tids[P];		/* tids fils */
	int data[DATA];	/* donnees */
	pb_t *pb;			/* probleme courant */
	int sender[1];

	set_random_data(data);	/* initialisation aleatoire */
	init_queue(data);				/* initialisation de la pile */
	/* lancement des P esclaves */
	pvm_spawn(EPATH "/sorts", (char**)0, 0, "", P, tids);

	/* envoi d'un probleme (de tri) a chaque esclave */
	for (i=0; Q_nb>0 && i<P; i++)
		send_pb(tids[i], depile());

	while (1) {
		// a vous
		pb = receive_pb(-1, sender);
		if (pb->taille1 == DATA) break;

		empile(pb);
		pb = depile();
		if (pb->type == PB_TRI) 
			send_pb(*sender, pb);
		else {
			pb_t *pb2 = depile();
			if (pb2 == NULL) empile(pb);
			else {
				if (pb2->type == PB_TRI) {
					send_pb(*sender, pb2);
					empile(pb);
				}
				else {
					pb->taille2 = pb2->taille1;
					pb->data2 = pb2->data1;
					send_pb(*sender, pb);
					free(pb2);
				}
			}
		}
	}

	pvm_mcast(tids, P, MSG_END); /* fin esclaves */
	pb_print(pb);
	pb_free(pb);
	pvm_exit();
	exit(0);
}
