#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/cli.h"
#include "../include/persist.h"

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

/* Lee una línea desde stdin; devuelve 0 en EOF. */
static int readLine(char *buf, int size) {
    printf("> ");
    fflush(stdout);
    if (!fgets(buf, size, stdin)) return 0;
    /* strip newline */
    char *nl = strchr(buf, '\n');
    if (nl) *nl = '\0';
    return 1;
}

/*
 * Divide buf en tokens separados por espacios.
 * Devuelve el número de tokens encontrados (máx. maxTokens).
 * ATENCIÓN: modifica buf en su lugar.
 */
static int tokenize(char *buf, char *tokens[], int maxTokens) {
    int count = 0;
    char *p = buf;
    while (*p && count < maxTokens) {
        /* saltar espacios */
        while (*p == ' ') p++;
        if (!*p) break;
        tokens[count++] = p;
        /* avanzar hasta el próximo espacio */
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
    }
    return count;
}

/*
 * Extrae el "resto de la línea" a partir del token tIdx
 * (tokens[tIdx] .. tokens[n-1]), reconectando los espacios.
 * Escribe el resultado en out (maxLen bytes).
 */
static void joinTokens(char *tokens[], int from, int n,
                       char *out, int maxLen) {
    out[0] = '\0';
    for (int i = from; i < n; i++) {
        if (i > from) strncat(out, " ", maxLen - (int)strlen(out) - 1);
        strncat(out, tokens[i], maxLen - (int)strlen(out) - 1);
    }
}

/* ------------------------------------------------------------------ */
/* Ayuda                                                               */
/* ------------------------------------------------------------------ */

static void printHelp(void) {
    printf(
        "\nComandos disponibles:\n"
        "  add <nombre>           Agrega un nodo (pide descripción interactivamente)\n"
        "  desc <id> <texto>      Cambia la descripción de un nodo\n"
        "  name <id> <nombre>     Cambia el nombre de un nodo\n"
        "  connect <id1> <id2>    Conecta dos nodos con una arista\n"
        "  unlock <id>            Desbloquea un nodo si tiene vecino activo\n"
        "  lock <id>              Vuelve a bloquear un nodo\n"
        "  info <id>              Muestra detalles de un nodo\n"
        "  list                   Lista todos los nodos\n"
        "  view                   Muestra el árbol completo\n"
        "  save <archivo>         Guarda el árbol en un archivo\n"
        "  load <archivo>         Carga un árbol desde un archivo\n"
        "  reset                  Descarta el árbol actual y empieza uno nuevo\n"
        "  help                   Muestra esta ayuda\n"
        "  exit / quit            Sale del programa\n\n"
    );
}

/* ------------------------------------------------------------------ */
/* Comandos                                                            */
/* ------------------------------------------------------------------ */

static void cmdAdd(Graph *g, char *tokens[], int n) {
    if (n < 2) { printf("Usage: add <name>\n"); return; }

    char name[MAX_NAME];
    joinTokens(tokens, 1, n, name, MAX_NAME);

    int id = graphAddNode(g, name, "No description");
    if (id >= 0)
        printf("Created node: id=%d \"%s\"\n", id, name);
}

static void cmdDesc(Graph *g, char *tokens[], int n) {
    if (n < 3) { printf("Uso: desc <id> <texto>\n"); return; }

    int id = atoi(tokens[1]);
    char desc[MAX_DESC];
    joinTokens(tokens, 2, n, desc, MAX_DESC);

    if (id < 0 || id >= g->numNodes) {
        printf("ID %d inválido.\n", id); return;
    }
    strncpy(g->skill[id].desc, desc, MAX_DESC - 1);
    g->skill[id].desc[MAX_DESC - 1] = '\0';
    printf("Descripción de \"%s\" actualizada.\n", g->skill[id].name);
}

static void cmdName(Graph *g, char *tokens[], int n) {
    if (n < 3) { printf("Uso: name <id> <nombre>\n"); return; }

    int id = atoi(tokens[1]);
    char name[MAX_NAME];
    joinTokens(tokens, 2, n, name, MAX_NAME);

    if (id < 0 || id >= g->numNodes) {
        printf("ID %d inválido.\n", id); return;
    }
    strncpy(g->skill[id].name, name, MAX_NAME - 1);
    g->skill[id].name[MAX_NAME - 1] = '\0';
    printf("Nodo %d renombrado a \"%s\".\n", id, name);
}

static void cmdConnect(Graph *g, char *tokens[], int n) {
    if (n < 3) { printf("Uso: connect <id1> <id2>\n"); return; }
    int a = atoi(tokens[1]);
    int b = atoi(tokens[2]);
    graphAddEdge(g, a, b);
    if (a >= 0 && a < g->numNodes && b >= 0 && b < g->numNodes && a != b)
        printf("Conectados: \"%s\" ↔ \"%s\"\n",
               g->skill[a].name, g->skill[b].name);
}

static void cmdUnlock(Graph *g, char *tokens[], int n) {
    if (n < 2) { printf("Uso: unlock <id>\n"); return; }
    graphUnlock(g, atoi(tokens[1]));
}

static void cmdLock(Graph *g, char *tokens[], int n) {
    if (n < 2) { printf("Uso: lock <id>\n"); return; }
    int id = atoi(tokens[1]);
    if (id < 0 || id >= g->numNodes) { printf("ID %d inválido.\n", id); return; }
    g->skill[id].unlocked = 0;
    printf("Bloqueado: \"%s\"\n", g->skill[id].name);
}

static void cmdInfo(Graph *g, char *tokens[], int n) {
    if (n < 2) { printf("Uso: info <id>\n"); return; }
    int id = atoi(tokens[1]);
    if (id < 0 || id >= g->numNodes) { printf("ID %d inválido.\n", id); return; }

    const Skill *s = &g->skill[id];
    printf("\n  ID       : %d\n", s->id);
    printf("  Nombre   : %s\n", s->name);
    printf("  Desc     : %s\n", s->desc);
    printf("  Estado   : %s\n", s->unlocked ? "★ Desbloqueado" : "  Bloqueado");
    printf("  Vecinos  : ");
    NodeAdj *v = g->adj[id];
    if (!v) { printf("(ninguno)"); }
    while (v) {
        printf("[%d] %s", v->neighborId, g->skill[v->neighborId].name);
        if (v->nextNode) printf(", ");
        v = v->nextNode;
    }
    printf("\n\n");
}

static void cmdList(const Graph *g) {
    printf("\n  %-4s %-3s %-20s %s\n", "ID", "Est", "Nombre", "Descripción");
    printf("  %-4s %-3s %-20s %s\n", "---", "---", "--------------------",
           "--------------------");
    for (int i = 0; i < g->numNodes; i++) {
        const Skill *s = &g->skill[i];
        printf("  %-4d %-3s %-20s %s\n",
               s->id,
               s->unlocked ? "[★]" : "[ ]",
               s->name,
               s->desc);
    }
    printf("\n");
}

static void cmdSave(Graph *g, char *tokens[], int n) {
    if (n < 2) { printf("Uso: save <archivo>\n"); return; }
    persistSave(g, tokens[1]);
}

/* Devuelve el nuevo grafo si load tuvo éxito, o el original si falló */
static Graph *cmdLoad(Graph *g, char *tokens[], int n) {
    if (n < 2) { printf("Uso: load <archivo>\n"); return g; }
    Graph *loaded = persistLoad(tokens[1]);
    if (loaded) {
        graphFree(g);
        return loaded;
    }
    return g;
}

static Graph *cmdReset(Graph *g) {
    printf("¿Confirmar reset? Se perderán los cambios no guardados. (s/n): ");
    fflush(stdout);
    char resp[8];
    if (!fgets(resp, sizeof(resp), stdin)) return g;
    if (resp[0] == 's' || resp[0] == 'S') {
        graphFree(g);
        g = graphCreate(1);
        if (g) {
            graphSetSkill(g, 0, "Raíz", "Nodo raíz");
            printf("Árbol reiniciado.\n");
        }
    }
    return g;
}

/* ------------------------------------------------------------------ */
/* Loop principal                                                      */
/* ------------------------------------------------------------------ */

void cliRun(Graph *g) {
    printf("\nSkill Tree CLI — escribí \"help\" para ver los comandos.\n");

    char line[512];
    while (readLine(line, sizeof(line))) {
        if (line[0] == '\0') continue;

        char *tokens[32];
        int n = tokenize(line, tokens, 32);
        if (n == 0) continue;

        const char *cmd = tokens[0];

        if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
            printf("Saliendo.\n");
            break;
        } else if (strcmp(cmd, "help")    == 0) { printHelp(); }
        else if (strcmp(cmd, "add")     == 0) { cmdAdd(g, tokens, n); }
        else if (strcmp(cmd, "desc")    == 0) { cmdDesc(g, tokens, n); }
        else if (strcmp(cmd, "name")    == 0) { cmdName(g, tokens, n); }
        else if (strcmp(cmd, "connect") == 0) { cmdConnect(g, tokens, n); }
        else if (strcmp(cmd, "unlock")  == 0) { cmdUnlock(g, tokens, n); }
        else if (strcmp(cmd, "lock")    == 0) { cmdLock(g, tokens, n); }
        else if (strcmp(cmd, "info")    == 0) { cmdInfo(g, tokens, n); }
        else if (strcmp(cmd, "list")    == 0) { cmdList(g); }
        else if (strcmp(cmd, "view")    == 0) { graphPrint(g); }
        else if (strcmp(cmd, "save")    == 0) { cmdSave(g, tokens, n); }
        else if (strcmp(cmd, "load")    == 0) { g = cmdLoad(g, tokens, n); }
        else if (strcmp(cmd, "reset")   == 0) { g = cmdReset(g); }
        else {
            printf("Comando desconocido: \"%s\". Escribí \"help\".\n", cmd);
        }
    }
}
