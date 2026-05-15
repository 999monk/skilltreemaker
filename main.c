#include <stdio.h>
#include <string.h>

#include "include/graph.h"
#include "include/cli.h"
#include "include/persist.h"

/*
 * Uso:
 *   ./skilltree              → arranca con árbol vacío
 *   ./skilltree <archivo>    → carga el archivo y arranca la CLI
 */
int main(int argc, char *argv[]) {
    Graph *g = NULL;

    if (argc >= 2) {
        g = persistLoad(argv[1]);
        if (!g) {
            fprintf(stderr, "No se pudo cargar \"%s\". Iniciando vacío.\n",
                    argv[1]);
        }
    }

    if (!g) g = graphCreate(0);
    if (!g) { fprintf(stderr, "Error al inicializar el grafo.\n"); return 1; }

    cliRun(g);

    graphFree(g);
    return 0;
}
