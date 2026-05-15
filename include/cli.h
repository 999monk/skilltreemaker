#ifndef CLI_H
#define CLI_H

#include "graph.h"

/*
 * Inicia el loop interactivo.
 * Recibe un grafo ya inicializado (puede estar vacío).
 * El loop termina cuando el usuario escribe "exit" o "quit".
 */
void cliRun(Graph *g);

#endif /* CLI_H */
