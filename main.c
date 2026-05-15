#include <stdio.h>
#include <string.h>

#include "include/graph.h"
#include "include/cli.h"
#include "include/persist.h"

static void printUsage(const char *prog) {
    fprintf(stderr,
        "Usage: %s <command> [options]\n\n"
        "Commands:\n"
        "  init <name>     Create new tree with root node named <name>\n"
        "  <file>          Load tree from file\n\n"
        "Options (when used with init):\n"
        "  --save <file>   Save tree after creation\n",
        prog);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    Graph *g = NULL;
    const char *saveFile = NULL;

    if (strcmp(argv[1], "init") == 0) {
        if (argc < 3) {
            fprintf(stderr, "init: missing root node name\n");
            printUsage(argv[0]);
            return 1;
        }

        g = graphCreate(1);
        if (!g) { fprintf(stderr, "Error creating graph.\n"); return 1; }
        graphSetSkill(g, 0, argv[2], "No description");

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "--save") == 0) {
                if (i + 1 < argc) saveFile = argv[++i];
            }
        }
    } else {
        g = persistLoad(argv[1]);
        if (!g) {
            fprintf(stderr, "Error: could not load \"%s\"\n", argv[1]);
            return 1;
        }
    }

    if (saveFile) {
        persistSave(g, saveFile);
        printf("Saved to \"%s\"\n", saveFile);
    }

    cliRun(g);

    graphFree(g);
    return 0;
}