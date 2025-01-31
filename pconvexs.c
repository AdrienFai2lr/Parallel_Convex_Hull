/* TP algorithmique parallele
 * maitrise
 * LL
 * 07/10/97
 */

/* 
 * sorts.c
 * algorithme esclave
 * -- tri fusion maitre esclave
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "pvm3.h"
#include "sort.h"

/*
 * trie un tableau de taille N
 * compare_int pour qsort
 */

static int compare_int(a, b) int *a, *b; { return *a - *b; }
void sort_data(pb_t *pb)
{
	qsort(pb->data1, N, sizeof(int), compare_int);
	pb->type = PB_FUS;
}

/*
 * fusion (cf tp1)
 */

void merge_data(pb)
	pb_t *pb;
{
	int idx1, idx2, idx;
	int *data;

	data = (int *)malloc((pb->taille1+pb->taille2)*sizeof(int));
	idx1 = idx2 = idx = 0;

	while (idx1<pb->taille1 && idx2<pb->taille2) 
		if (pb->data1[idx1] < pb->data2[idx2])
			data[idx++] = pb->data1[idx1++];
		else
			data[idx++] = pb->data2[idx2++];

	while (idx1<pb->taille1)
		data[idx++] = pb->data1[idx1++];
	while (idx2<pb->taille2)
		data[idx++] = pb->data2[idx2++];

	free(pb->data1);
	pb->data1 = data;
	free(pb->data2);
	pb->data2 = NULL;
	pb->taille1 += pb->taille2;
	pb->taille2 = 0;
}

/*
 * programme esclave
 */

main()
{
	extern pb_t *receive_pb();
	int parent, sender[1];	/* pere et envoyeur (non utilise) */
	pb_t *pb;					/* probleme courant */

	parent = pvm_parent();

	/* tant que l'on recoit un probleme a traiter ... */
	while ((pb = receive_pb(parent, sender)) != 0) {
		if (pb->type == PB_TRI) sort_data(pb);
		else merge_data(pb);
		send_pb(parent, pb);
	}

	pvm_exit();
	exit(0);
}

