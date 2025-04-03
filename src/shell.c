#include "shell.h"
#include "helpers.h"

process_control_block pcb[16];
resource_control_block rcb[4];
ready_list rl;

void init(){
    int i;
    // free all the nodes in the ready list
    for (i = 0; i < 3; i++){
        ready_list_node* current = rl.levels[i];
        while (current != NULL){
            ready_list_node* temp = current;
            current = current->next;
            free(temp);
        }
        rl.levels[i] = NULL;
    }

    // free all the nodes in the resource control block
    for (i = 0; i < 16; i++){
        // free children list
        node_t* current = pcb[i].children;
        while (current != NULL){
            node_t* temp = current;
            current = current->next;
            free(temp);
        }
        pcb[i].children = NULL;

        // free resources list
        current = pcb[i].resources;
        while (current != NULL){
            node_t* temp = current;
            current = current->next;
            free(temp);
        }
        pcb[i].children = NULL;
    }

    // free all the nodes in the resource control block
    for (i = 0; i < 4; i++){
        node_t* current = rcb[i].waitlist;
        while (current != NULL){
            node_t* temp = current;
            current = current->next;
            free(temp);
        }
        rcb[i].waitlist = NULL;
    }

    // initialize PCB
    for (i = 0; i < 16; i++){
        pcb[i].state = FREE;
        pcb[i].parent = -1;
        pcb[i].children = NULL;
        pcb[i].resources = NULL;
        pcb[i].priority = 0;
    }

    // initialize RCB
    rcb[0].inventory = 1;
    rcb[1].inventory = 1;
    rcb[2].inventory = 2;
    rcb[3].inventory = 3;

    for (i = 0; i < 4; i++){
        rcb[i].state = rcb[i].inventory;
        rcb[i].waitlist = NULL;
    }

    // initialize ready list
    for (i = 0; i < 3; i++){
        rl.levels[i] = NULL;
    }

    // initialize process 0
    pcb[0].state = READY;
    pcb[i].parent = -1;
    pcb[i].children = NULL;
    pcb[i].resources = NULL;
    pcb[i].priority = 0;

    // add process 0 to ready list
    addToRL(0, 0);
}

void scheduler(){
    // find the highest priority process in the ready list and set it to running
    int i = 0;
    int highest_priority_process = -1;
    for (i = 2; i >= 0; i--){
        if (rl.levels[i] != NULL){
            highest_priority_process = rl.levels[i]->pid;
            break;
        }
    }

    if (highest_priority_process != -1){
        printf("%s", highest_priority_process);
    }
}

int create(int p){
    int parent = getRunningProcess();
    if (parent = -1){
        return -1;
    }

    if (p < 0 || p > 2){
        return -1;
    }

    int child = -1;
    int i;
    for (i = 0; i < 16; i++){
        if (pcb[i].state == FREE){
            child = 1;
            break;
        }
    }

    if (child == 1){
        return -1;
    }

    pcb[child].state = READY;
    pcb[child].parent = parent;
    pcb[child].children = NULL;
    pcb[child].resources = NULL;
    pcb[child].priority = p;

    addToList(&pcb[parent].children, child, 0);
    addToRL(child, p);

    if (p > pcb[parent].priority){
        scheduler();
    }
    return child;
}

int main() {
    
}