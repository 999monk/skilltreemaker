#ifndef PERSIST_H
#define PERSIST_H

#include "graph.h"

/*
 * Guarda el grafo en un archivo de texto con formato propio.
 * Devuelve 1 si tuvo éxito, 0 si falló.
 */
int persistSave(const Graph *g, const char *path);

/*
 * Carga un grafo desde un archivo guardado con persistSave.
 * Devuelve un Graph* nuevo (debe liberarse con graphFree),
 * o NULL si falló.
 */
Graph *persistLoad(const char *path);

#endif /* PERSIST_H */
