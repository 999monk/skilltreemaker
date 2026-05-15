#ifndef GRAPH_H
#define GRAPH_H

#include "skill.h"

#define MAX_NODES 64

/* Lista de adyacencia: un nodo vecino */
typedef struct NodeAdj {
    int             neighborId;
    struct NodeAdj *nextNode;
} NodeAdj;

/* Grafo principal */
typedef struct {
    int     numNodes;
    Skill   skill[MAX_NODES];
    NodeAdj *adj[MAX_NODES];
} Graph;

/* Ciclo de vida */
Graph *graphCreate(int numNodes);
void   graphFree(Graph *g);

/* Construcción */
int  graphAddNode(Graph *g, const char *name, const char *desc);
void graphSetSkill(Graph *g, int id, const char *name, const char *desc);
void graphAddEdge(Graph *g, int a, int b);

/* Lógica del skill tree */
int  graphUnlock(Graph *g, int id);

/* Visualización en terminal */
void graphPrint(const Graph *g);

#endif /* GRAPH_H */
