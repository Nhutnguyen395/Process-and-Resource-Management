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

int destroy(int pid){
    int running_process = getRunningProcess();
    if (running_process == -1){
        return -1;
    }

    // check if process exists and is not free
    if (pid < 0 || pid >= 16 || pcb[pid].state == FREE){
        return -1;
    }

    // check if the process is a child of the current running process or is the running process
    if (pid != running_process && pcb[pid].parent != running_process){
        return -1;
    }

    int destroy_children_count = 0;
    destroy_children_count = destroyChildren(pid);

    if (pid == running_process){
        scheduler();
    } else {
        int highest_priority = getRunningProcess();
        if (highest_priority != -1 && pcb[highest_priority].priority > pcb[running_process].priority) {
            scheduler();
        }
    }
    return destroy_children_count;
}

int request(int rid, int units){
    int pid = getRunningProcess();
    if (pid == -1){
        return -1;
    }

    // resource 0 cannot request for resource
    if (pid == 0){
        return -1;
    }

    // check if units is valid
    if (units <= 0 || units > rcb[rid].inventory){
        return -1;
    }

    // cannot request already holding resource
    int current_units = 0;
    if (isInList(pcb[pid].resources, rid, &current_units)){
        if ((current_units + units) > rcb[rid].inventory){
            return -1;
        }
    }

    // check if resource is available
    if (rcb[rid].state >= units){
        // allocate if available
        rcb[rid].state -= units;
        if (current_units > 0){
            node_t* current = pcb[pid].resources;
            while (current != NULL){
                if (current->id == rid){
                    current->units += units;
                    break;
                }
                current = current->next;
            }
        } else {
            addToList(&pcb[pid].resources, rid, units);
        }
        return 0;
    } else {
        // if resource already allocated then block the process
        pcb[pid].state = BLOCKED;
        removeFromRL(pid);
        // add the process to the waitlist
        addToList(&rcb[rid].waitlist, pid, units);
        scheduler();
        return 0;
    }
}

void timeout(){
    int pid = getRunningProcess();
    if (pid == -1){
        return;
    }

    // remove process from head of the list
    int i;
    for (i = 0; i < 3; i++){
        if (rl.levels[i] != NULL && rl.levels[i]->pid == pid){
            ready_list_node* temp = rl.levels[i];
            rl.levels[i] = rl.levels[i]->next;
            free(temp);
            break;
        }
    }
    addToRL(pid, pcb[pid].priority);

    scheduler();
}

int main() {
    
}