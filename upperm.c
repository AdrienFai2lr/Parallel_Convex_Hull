#include "pvm3.h"
#include "point.h"
#include <stdlib.h>

#define MAX_SLAVES 8

int main(int argc, char **argv) {
    int tids[MAX_SLAVES], n_slaves = 4; // Adjust n_slaves as needed (power of two)
    point *pts = point_random(atoi(argv[1]));
    point *partitions[MAX_SLAVES];
    int num_partitions = 1;

    // Split points into n_slaves contiguous partitions
    partitions[0] = pts;
    while (num_partitions < n_slaves) {
        int current = num_partitions;
        for (int i = 0; i < current; i++) {
            if (num_partitions >= n_slaves) break;
            point *mid = point_part(partitions[i]);
            partitions[num_partitions++] = mid;
        }
    }

    // Spawn slaves
    pvm_spawn("uppers", (char**)0, 0, "", n_slaves, tids);

    // Send partitions to slaves
    for (int i = 0; i < n_slaves; i++) {
        int size, *data = point_serialize(partitions[i], &size);
        pvm_initsend(PvmDataDefault);
        pvm_pkint(&size, 1, 1);
        pvm_pkint(data, size, 1);
        pvm_send(tids[i], 0);
        free(data);
    }

    // Receive and merge results
    point *result = NULL;
    for (int i = 0; i < n_slaves; i++) {
        int size;
        pvm_recv(tids[i], -1);
        pvm_upkint(&size, 1, 1);
        int *data = malloc(size * sizeof(int));
        pvm_upkint(data, size, 1);
        point *hull = point_deserialize(data, size);
        free(data);
        result = (result == NULL) ? hull : point_merge_UH(result, hull);
    }

    // Output final hull
    point_print_gnuplot(result, 1);
    point_free(result);
    pvm_mcast(tids, n_slaves, 9);
    pvm_exit();
    return 0;
}