#include <stddef.h>
#include "../include/skilltree.h"

Graph *skilltreeCreateDefault(void) {
    Graph *g = graphCreate(10);
    if (!g) return NULL;

    graphSetSkill(g, 0, "Guerrero",       "Clase de inicio. Ya desbloqueado.");
    graphSetSkill(g, 1, "Golpe Poderoso", "Aumenta el daño cuerpo a cuerpo.");
    graphSetSkill(g, 2, "Armadura Pesada","Reduce el daño recibido.");
    graphSetSkill(g, 3, "Modo Berserker", "Velocidad y daño al costo de defensa.");
    graphSetSkill(g, 4, "Ataque Rapido",  "Incrementa la velocidad de ataque.");
    graphSetSkill(g, 5, "Evasion Total",  "Alta probabilidad de esquivar golpes.");
    graphSetSkill(g, 6, "Tiro Certero",   "Ataque a distancia de alta precision.");
    graphSetSkill(g, 7, "Flujo de Mana",  "Regeneracion de mana aumentada.");
    graphSetSkill(g, 8, "Bola de Fuego",  "Proyectil de fuego de area.");
    graphSetSkill(g, 9, "Rayo Arcano",    "Descarga electrica de largo alcance.");

    g->skill[0].unlocked = 1;

    /* Rama Fuerza */
    graphAddEdge(g, 0, 1);
    graphAddEdge(g, 1, 2);
    graphAddEdge(g, 1, 3);

    /* Rama Destreza */
    graphAddEdge(g, 0, 4);
    graphAddEdge(g, 4, 5);
    graphAddEdge(g, 4, 6);

    /* Rama Magia */
    graphAddEdge(g, 0, 7);
    graphAddEdge(g, 7, 8);
    graphAddEdge(g, 7, 9);

    return g;
}
