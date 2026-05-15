#ifndef SKILL_H
#define SKILL_H

#define MAX_NAME 32
#define MAX_DESC 128

typedef struct {
    int  id;
    char name[MAX_NAME];
    char desc[MAX_DESC];
    int  unlocked;
} Skill;

#endif /* SKILL_H */
