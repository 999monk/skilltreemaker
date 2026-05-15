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
static Graph *createInitialTree(void) {
    printf("Ingresa el nombre del nodo raíz: ");
    fflush(stdout);

    char name[MAX_NAME];
    if (!fgets(name, sizeof(name), stdin)) {
        fprintf(stderr, "Error al leer nombre.\n");
        return NULL;
    }
    char *nl = strchr(name, '\n');
    if (nl) *nl = '\0';
    if (name[0] == '\0') {
        fprintf(stderr, "El nombre no puede estar vacío.\n");
        return NULL;
    }

    printf("Ingresa la descripción (Enter para dejar vacía): ");
    fflush(stdout);
    char desc[MAX_DESC];
    if (!fgets(desc, sizeof(desc), stdin)) desc[0] = '\0';
    nl = strchr(desc, '\n');
    if (nl) *nl = '\0';
    if (desc[0] == '\0') strncpy(desc, "Nodo raíz", MAX_DESC - 1);

    Graph *g = graphCreate(1);
    if (!g) { fprintf(stderr, "Error al crear el grafo.\n"); return NULL; }

    graphSetSkill(g, 0, name, desc);
    printf("Árbol creado con nodo raíz: \"%s\"\n", name);
    return g;
}

int main(int argc, char *argv[]) {
    Graph *g = NULL;

    if (argc >= 2) {
        g = persistLoad(argv[1]);
        if (!g) {
            fprintf(stderr, "No se pudo cargar \"%s\". Creando nuevo árbol.\n",
                    argv[1]);
        }
    }

    if (!g) g = createInitialTree();
    if (!g) { fprintf(stderr, "Error al inicializar el grafo.\n"); return 1; }

    cliRun(g);

    graphFree(g);
    return 0;
}
