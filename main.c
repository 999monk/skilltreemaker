#include <stdio.h>

#include "include/graph.h"
#include "include/skilltree.h"

int main(void) {
    Graph *tree = skilltreeCreateDefault();
    if (!tree) return 1;

    graphPrint(tree);

    /* Demo: misma secuencia que el original */
    graphUnlock(tree, 1);   /* vecino de 0 (Guerrero) → OK          */
    graphUnlock(tree, 3);   /* vecino de 1, pero 1 no está desbloqueado aún → falla */
    graphUnlock(tree, 8);   /* rama Magia: 7 no desbloqueado → falla */
    graphUnlock(tree, 7);   /* vecino de 0 → OK                      */
    graphUnlock(tree, 8);   /* vecino de 7, ahora sí → OK            */

    graphPrint(tree);

    graphFree(tree);
    return 0;
}
