#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// process states
#define READY 1
#define BLOCKED 0
#define FREE -1

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
    struct ready_list_node* next;
} ready_list_node;

typedef struct ready_list {
    ready_list_node* levels[3];
} ready_list;

void init();
int create(int p);
int destroy(int pid);
int request(int rid, int units);
int release(int rid, int units);
void timeout();
void scheduler();
int processCommand(char* command);

#endif