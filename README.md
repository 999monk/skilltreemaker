# SkillTreeMaker

Herramienta CLI escrita en C que permite crear un árbol de habilidades (o un grafo sin más). Es un proyecto recreativo, realizado mientras incursiono sobre C y grafos.

## Compilar

```bash
make
```

## Ejecutar

```bash
./skilltree              # Crear árbol desde cero
./skilltree <archivo>    # Cargar árbol existente
```

## Comandos

- `add <nombre>` - Agregar nodo
- `connect <id1> <id2>` - Conectar nodos
- `unlock <id>` - Desbloquear nodo (requiere vecino desbloqueado)
- `save <archivo>` - Guardar
- `load <archivo>` - Cargar
- `help` - Ver todos los comandos

---
