#ifndef HELPERS_H
#define HELPERS_H

#include "shell.h"

node_t* createNode(int id, int units);
void addToList(node_t** head, int id, int units);
int removeFromList(node_t** head, int id, int* units);
int isInList(node_t* head, int id, int* units);
void addToRL(int pid, int priority);
int removeFromRL(int pid);
void freeList(node_t** head);
int getRunningProcess();

#endif 