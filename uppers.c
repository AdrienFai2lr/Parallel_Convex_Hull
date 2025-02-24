#include "pvm3.h"
#include "point.h"
#include <stdlib.h>

int main() {
    int parent = pvm_parent();
    int size;
    pvm_recv(parent, -1);

    // Deserialize points
    pvm_upkint(&size, 1, 1);
    int *data = malloc(size * sizeof(int));
    pvm_upkint(data, size, 1);
    point *pts = point_deserialize(data, size);
    free(data);

    // Compute upper hull
    upper_hull(pts);

    // Serialize and send back
    int *result_data = point_serialize(pts, &size);
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&size, 1, 1);
    pvm_pkint(result_data, size, 1);
    pvm_send(parent, 0);
    free(result_data);
    point_free(pts);

    pvm_exit();
    return 0;
}