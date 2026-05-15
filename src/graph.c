#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/graph.h"

/* ---------- ciclo de vida ---------- */

Graph *graphCreate(int numNodes) {
    if (numNodes <= 0 || numNodes > MAX_NODES) {
        fprintf(stderr, "graphCreate: numNodes debe estar entre 1 y %d.\n",
                MAX_NODES);
        return NULL;
    }

    Graph *g = malloc(sizeof(Graph));
    if (!g) { perror("malloc"); return NULL; }

    g->numNodes = numNodes;
    for (int i = 0; i < numNodes; i++) {
        g->adj[i]           = NULL;
        g->skill[i].id      = i;
        g->skill[i].unlocked = 0;
        strncpy(g->skill[i].name, "???",      MAX_NAME - 1);
        strncpy(g->skill[i].desc, "Sin desc.", MAX_DESC - 1);
        g->skill[i].name[MAX_NAME - 1] = '\0';
        g->skill[i].desc[MAX_DESC - 1] = '\0';
    }
    return g;
}

void graphFree(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->numNodes; i++) {
        NodeAdj *cur = g->adj[i];
        while (cur) {
            NodeAdj *tmp = cur;
            cur = cur->nextNode;
            free(tmp);
        }
    }
    free(g);
}

/* ---------- construcción ---------- */

/*
 * Agrega un nodo nuevo al grafo de forma dinámica.
 * Devuelve el id asignado, o -1 si no hay espacio.
 */
int graphAddNode(Graph *g, const char *name, const char *desc) {
    if (!g || g->numNodes >= MAX_NODES) {
        fprintf(stderr, "graphAddNode: grafo lleno (max %d nodos).\n",
                MAX_NODES);
        return -1;
    }
    int id = g->numNodes++;
    g->adj[id]            = NULL;
    g->skill[id].id       = id;
    g->skill[id].unlocked = 0;
    graphSetSkill(g, id, name, desc);
    return id;
}

void graphSetSkill(Graph *g, int id, const char *name, const char *desc) {
    if (!g || id < 0 || id >= g->numNodes) return;
    strncpy(g->skill[id].name, name, MAX_NAME - 1);
    strncpy(g->skill[id].desc, desc, MAX_DESC - 1);
    g->skill[id].name[MAX_NAME - 1] = '\0';
    g->skill[id].desc[MAX_DESC - 1] = '\0';
}

static void addNeighbor(Graph *g, int origin, int neighborId) {
    NodeAdj *node = malloc(sizeof(NodeAdj));
    if (!node) { perror("malloc"); return; }
    node->neighborId = neighborId;
    node->nextNode   = g->adj[origin];
    g->adj[origin]   = node;
}

void graphAddEdge(Graph *g, int a, int b) {
    if (!g || a < 0 || a >= g->numNodes ||
              b < 0 || b >= g->numNodes || a == b) {
        fprintf(stderr, "graphAddEdge: IDs invalidos (%d, %d).\n", a, b);
        return;
    }
    addNeighbor(g, a, b);
    addNeighbor(g, b, a);
}

/* ---------- lógica del skill tree ---------- */

/*
 * BUG ORIGINAL: la condición era `id <= g->numNodes` (debería ser >=).
 * Con esa condición todos los IDs válidos eran rechazados.
 */
int graphUnlock(Graph *g, int id) {
    if (!g || id < 0 || id >= g->numNodes) {
        fprintf(stderr, "graphUnlock: ID %d invalido.\n", id);
        return 0;
    }
    if (g->skill[id].unlocked) {
        printf("\"%s\" ya estaba desbloqueado.\n", g->skill[id].name);
        return 0;
    }

    /* Buscar al menos un vecino desbloqueado */
    NodeAdj *neighbor = g->adj[id];
    while (neighbor) {
        if (g->skill[neighbor->neighborId].unlocked) {
            g->skill[id].unlocked = 1;
            printf("Desbloqueado: \"%s\"\n", g->skill[id].name);
            return 1;
        }
        neighbor = neighbor->nextNode;
    }

    printf("No podés desbloquear \"%s\": ningún vecino activo.\n",
           g->skill[id].name);
    return 0;
}

/* ---------- visualización ---------- */

void graphPrint(const Graph *g) {
    if (!g) return;
    printf("\n=== SKILL TREE ===\n");
    for (int i = 0; i < g->numNodes; i++) {
        const Skill *s = &g->skill[i];
        printf("[%s] id:%-2d  %s\n",
               s->unlocked ? "★" : " ", s->id, s->name);
        if (s->desc[0] != '\0')
            printf("          %s\n", s->desc);

        printf("          Conectado con: ");
        NodeAdj *v = g->adj[i];
        if (!v) { printf("(ninguno)"); }
        while (v) {
            printf("%s", g->skill[v->neighborId].name);
            if (v->nextNode) printf(", ");
            v = v->nextNode;
        }
        printf("\n");
    }
    printf("==================\n\n");
}
