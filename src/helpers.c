#include "helpers.h"
#include "shell.h"

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

int removeFromList(node_t** head, int id, int* units){
    if (*head == NULL){
        return 0;
    }

    if ((*head)->id == id){
        node_t* temp = *head;
        *head = (*head)->next;
        if (units != NULL){
            *units = temp->units;
        }
        free(temp);
        return 1;
    }

    node_t* current = *head;
    while (current->next != NULL && current->next->id != id){
        current = current->next;
    }

    if (current->next == NULL){
        return 0;
    }

    node_t* temp = current->next;
    current->next = temp->next;
    if (units != NULL){
        *units = temp->units;;
    }
    free(temp);
    return 1;
}

int isInList(node_t* head, int id, int* units){
    node_t* current = head;
    while (current != NULL){
        if (current->id == id){
            if (units != NULL){
                *units = current->units;
            }
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void addToRL(int pid, int priority) {
    ready_list_node* newNode = (ready_list_node*)malloc(sizeof(ready_list_node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    newNode->pid = pid;
    newNode->next = NULL;
    
    if (rl.levels[priority] == NULL) {
        rl.levels[priority] = newNode;
        return;
    }

    ready_list_node* current = rl.levels[priority];
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
}

int removeFromRL(int pid){
    int i;
    for (i = 0; i < 3; i++){
        ready_list_node* current = rl.levels[i];
        ready_list_node* prev = NULL;
        while (current != NULL){
            if (current->pid == pid){
                if (prev == NULL){
                    rl.levels[i] = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
                return 1;
            }
            prev = current;
            current = current->next;
        }
    }
    return 0;
}

int getRunningProcess(){
    // get the highest priority running process
    int i;
    for (i = 2; i >= 0; i--){
        if (rl.levels[i] != NULL){
            return rl.levels[i]->pid;
        }
    }
    return -1;
}

void freeList(node_t** head){
    node_t* current = *head;
    node_t* nextNode;
    while (current != NULL){
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
    *head = NULL;
}

int destroyChildren(int pid){
    int count = 1;

    // recursively destroy all children
    node_t* current = pcb[pid].children;
    while (current != NULL){
        int cid = current->id;
        node_t* next_child = current->next;
        count += destroyChildren(cid);
        current = next_child;
    }

    // remove from parent's list
    if (pcb[pid].parent != -1){
        removeFromList(&pcb[pcb[pid].parent].children, pid, NULL);
    }

    // remove from ready list or waitlist
    if (pcb[pid].state == READY){
        removeFromRL(pid);
    } else if (pcb[pid].state == BLOCKED){
        // if blocked find which resource its blocked on
        int j;
        for (j = 0; j < 16; j++){
            node_t* current = rcb[j].waitlist;
            node_t* prev = NULL;
            while (current != NULL){
                if (current->id == pid){
                    if (prev == NULL){
                        rcb[j].waitlist = current->next;
                    } else {
                        prev->next = current->next;
                    }
                    free(current);
                    break;
                }
                prev = current;
                current = current->next;
            }
        }
    }

    // release all resources held by the process
    while (pcb[pid].resources != NULL){
        int rid = pcb[pid].resources->id;
        int units = pcb[pid].resources->units;
        removeFromList(&pcb[pid].resources, rid, NULL);
        rcb[rid].state += units;

        // checks if process waiting for freed resources can be unblocked
        node_t* current = rcb[rid].waitlist;
        node_t* prev = NULL;
        while(current != NULL && rcb[rid].state > 0){
            if (rcb[rid].state >= current->units){
                int waiting_process = current->id;
                int requested_units = current->units;

                // remove the process from waitlist if resources is free
                node_t* temp = current;
                if (prev == NULL){
                    rcb[rid].waitlist = current->next;
                    current = current->next;
                    free(temp);
                } else {
                    prev->next = current->next;
                    current = prev->next;
                    free(temp);
                }

                // allocate resources to the process
                rcb[rid].state -= requested_units;
                addToList(&pcb[waiting_process].resources, rid, requested_units);
                pcb[waiting_process].state = READY;
                addToRL(waiting_process, pcb[waiting_process].priority);
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
    pcb[pid].state = FREE;
    pcb[pid].parent = -1;
    pcb[pid].children = NULL;
    pcb[pid].resources = NULL;
    return count;
}