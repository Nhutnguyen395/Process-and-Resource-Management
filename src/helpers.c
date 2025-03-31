#include "../include/helpers.h"
#include "../include/shell.h"

extern process_control_block pcb[16];
extern resource_control_block rcb[4];
extern ready_list rl;

node_t* createNode(int id, int units){
    node_t* newNode = (node_t*)malloc(sizeof(node_t));
    if (newNode == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    newNode->id = id;
    newNode->units = units;
    newNode->next = NULL;
    return newNode;
}

void addToList(node_t** head, int id, int units){
    node_t* newNode = createNode(id, units);
    if (*head == NULL){
        *head = newNode;
        return;
    } 
    
    node_t* current = *head;
    while (current->next != NULL){
        current = current->next;
    }
    current->next = newNode;
}