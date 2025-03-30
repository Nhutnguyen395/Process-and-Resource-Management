#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    int id;
    int units;
    struct node* next;
} node_t;

typedef struct {
    int state;
    int parent;
    node_t* children;
    node_t* resources;
    int priority;
} process_control_block;

typedef struct rcb {
    int state;
    int inventory;
    node_t* waitlist;
} resource_control_block;

typedef struct ready_list_node {
    int pid;
    struct ready_list* next;
} ready_list_node;

typedef struct ready_list {
    ready_list_node* levels[3];
} ready_list;

void init();
int create(int priority);
int destroy(int pid);
int request(int rid, int units);
int release(int rid, int units);
void timeout();
void scheduler();
int getRunningProcess();

#endif