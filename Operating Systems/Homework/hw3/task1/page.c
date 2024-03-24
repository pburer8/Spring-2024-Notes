#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

const int MEM_SIZE = 20;
struct Page {
    int id;
    int read; //read = 0, write = 1
};

struct DLL {
    struct Page page;
    struct DLL prev;
    struct DLL next;
};

void FIFO() {
    struct Page frames[MEM_SIZE];
    int ptr = 0;
    int faults = 0;
    int in[100];


    for (int i = 0; i < MEM_SIZE; i++) {
        struct Page fill = {.id = -1, .read = -1};
        frames[i] = fill;
    }
    for (int i = 0; i < 100; i++) {
        in[i] = 0;
    }

    FILE* fptr = fopen("page_references.txt", "r");

    char line[25];

    while (fgets(line, 25, fptr)) {
        int id = atoi(strtok(line, " "));
        char* read = atoi(strtok(NULL, " "));
        if (in[id] == 1) {
            continue;
        } else {
            faults++;
            struct Page prev_page = frames[ptr];
            if (prev_page.id != -1) {
                in[prev_page.id] = 0;
            }
            in[id] = 1;


            struct Page new_page = {.id=id, .read=read};
            frames[ptr] = new_page;
            ptr = (ptr + 1) % MEM_SIZE;
        }
    }

    printf("Number of faults with FIFO model: %d", faults);
}

void LRU() {
    struct DLL head;
    int faults = 0;
    int in[100];
    for (int i = 0; i < MEM_SIZE; i++) {
        struct Page fill = {.id = -1, .read = -1};
        
    }
    for (int i = 0; i < 100; i++) {
        in[i] = 0;
    }

    FILE* fptr = fopen("page_references.txt", "r");

    char line[25];
    
    while (fgets(line, 25, fptr)) {
        int id = atoi(strtok(line, " "));
        int read = atoi(strtok(NULL, " "));
        if (in[id] == 1) {
        }
    }
}

int main() {
    FIFO();
    return 0;
}