#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/persist.h"

/*
 * Formato del archivo:
 *
 *   NODES <n>
 *   NODE <id> <unlocked> <name> | <desc>
 *   ...
 *   EDGES
 *   EDGE <a> <b>
 *   ...
 *   END
 *
 * Se guardan solo las aristas en una dirección (a < b) para evitar duplicados.
 * El separador '|' en la línea NODE permite nombres y descripciones con espacios.
 */

int persistSave(const Graph *g, const char *path) {
    if (!g || !path) return 0;

    FILE *f = fopen(path, "w");
    if (!f) { perror("persistSave: fopen"); return 0; }

    fprintf(f, "NODES %d\n", g->numNodes);
    for (int i = 0; i < g->numNodes; i++) {
        const Skill *s = &g->skill[i];
        fprintf(f, "NODE %d %d %s | %s\n",
                s->id, s->unlocked, s->name, s->desc);
    }

    fprintf(f, "EDGES\n");
    for (int i = 0; i < g->numNodes; i++) {
        NodeAdj *v = g->adj[i];
        while (v) {
            if (v->neighborId > i)          /* solo a < b */
                fprintf(f, "EDGE %d %d\n", i, v->neighborId);
            v = v->nextNode;
        }
    }

    fprintf(f, "END\n");
    fclose(f);
    printf("Guardado en \"%s\".\n", path);
    return 1;
}

Graph *persistLoad(const char *path) {
    if (!path) return NULL;

    FILE *f = fopen(path, "r");
    if (!f) { perror("persistLoad: fopen"); return NULL; }

    char line[512];
    Graph *g = NULL;

    /* Primera línea: NODES <n> */
    if (!fgets(line, sizeof(line), f)) goto fail;
    int numNodes = 0;
    if (sscanf(line, "NODES %d", &numNodes) != 1) goto fail;

    g = graphCreate(numNodes);
    if (!g) goto fail;

    /* Líneas NODE */
    for (int i = 0; i < numNodes; i++) {
        if (!fgets(line, sizeof(line), f)) goto fail;

        int id, unlocked;
        char name[MAX_NAME], desc[MAX_DESC];

        /* Parseo manual para soportar espacios en nombre y desc */
        char *p = line;

        /* "NODE " */
        if (strncmp(p, "NODE ", 5) != 0) goto fail;
        p += 5;

        /* id */
        id = (int)strtol(p, &p, 10);
        /* unlocked */
        unlocked = (int)strtol(p, &p, 10);
        /* espacio */
        if (*p == ' ') p++;

        /* nombre hasta ' | ' */
        char *sep = strstr(p, " | ");
        if (!sep) goto fail;
        int nameLen = (int)(sep - p);
        if (nameLen >= MAX_NAME) nameLen = MAX_NAME - 1;
        strncpy(name, p, nameLen);
        name[nameLen] = '\0';

        /* descripción (resto, sin el \n final) */
        p = sep + 3;
        strncpy(desc, p, MAX_DESC - 1);
        desc[MAX_DESC - 1] = '\0';
        /* strip newline */
        char *nl = strchr(desc, '\n');
        if (nl) *nl = '\0';

        graphSetSkill(g, id, name, desc);
        g->skill[id].unlocked = unlocked;
    }

    /* "EDGES" */
    if (!fgets(line, sizeof(line), f)) goto fail;
    if (strncmp(line, "EDGES", 5) != 0) goto fail;

    /* Líneas EDGE hasta END */
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "END", 3) == 0) break;
        int a, b;
        if (sscanf(line, "EDGE %d %d", &a, &b) == 2)
            graphAddEdge(g, a, b);
    }

    fclose(f);
    printf("Cargado desde \"%s\" (%d nodos).\n", path, g->numNodes);
    return g;

fail:
    fprintf(stderr, "persistLoad: formato inválido en \"%s\".\n", path);
    fclose(f);
    graphFree(g);
    return NULL;
}
