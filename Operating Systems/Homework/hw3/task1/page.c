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
    struct Page* data;
    struct DLL* prev;
    struct DLL* next;
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
        char read = strtok(NULL, " ")[0];
        int r = 0;
        if (read == 'w') {
            r = 1;
        }
        
        if (in[id] == 1) {
            continue;
        } else {
            faults++;
            struct Page prev_page = frames[ptr];
            if (prev_page.id != -1) {
                in[prev_page.id] = 0;
            }
            in[id] = 1;


            struct Page new_page = {.id=id, .read=r};
            frames[ptr] = new_page;
            ptr = (ptr + 1) % MEM_SIZE;
        }
    }

    printf("Number of faults with FIFO model: %d\n", faults);
}

struct DLL* create_node(struct Page* data) {
    struct DLL* new_node = (struct DLL*) malloc(sizeof(struct DLL));
    new_node->data = data;
    new_node->prev = NULL;
    new_node->next = NULL;
    return new_node;
}

void append(struct DLL* head, struct Page* data) {
    struct DLL* new_node = create_node(data);

    while (head->next != NULL) {
        head = head->next;
    }

    head->next = new_node;
    new_node->prev = head;
}

void LRU() {
    printf("here\n");
    struct Page d = {.id = -1, .read = -1};
    struct DLL* head = create_node(&d);
    int faults = 0;
    int in[100];

    for (int i = 0; i < 19; i++) {
        struct Page fill = {.id=-1, .read=-1};
        append(head, &fill);
    }

    for (int i = 0; i < 100; i++) {
        in[i] = 0;
    }
    struct DLL* tail = head;
    while (tail->next != NULL) {
        tail = tail->next;
    }

    FILE* fptr = fopen("page_references.txt", "r");

    char line[25];
    printf("here2\n");
    while (fgets(line, 25, fptr)) {
        int id = atoi(strtok(line, " "));
        char read = strtok(NULL, " ")[0];
        int r = 0;
        if (read == 'w') {
            r = 1;
        }
        if (in[id] == 1) {
            struct DLL* ptr = head;
            while(ptr->data->id != id) {
                ptr = ptr->next;
            }

            if (ptr->prev != NULL) {
                ptr->prev->next = ptr->next;
            }
            if (ptr->next != NULL) {
                ptr->next->prev = ptr->prev;
            } else {
                tail = ptr->prev;
            }
            ptr->prev = NULL;
            ptr->next = head;
            head->prev = ptr;
            head = ptr;
        } else {
            faults++;

            tail = tail->prev;
            struct Page p = *tail->next->data;
            in[p.id] = 0;
            tail->next = NULL;

            struct Page np = {.id=id, .read=r};
            struct DLL* new = create_node(&np);
            in[id] = 1;

            new->next = head;
            head->prev = new;
            head = new;
        }
        printf("here3\n");
    }

    printf("Number of faults with LRU model: %d\n", faults);
}

int main() {
    //FIFO();
    LRU();
    return 0;
}