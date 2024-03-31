#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

const int MEM_SIZE = 20;

struct Page {
    int id;
    int second;
    int write;
};

struct node {
    struct Page* data;
    struct node* prev;
    struct node* next;
};

struct node* create_node(struct Page* data) {
    struct node* n = malloc(sizeof(struct node));

    n->data = data;
    n->prev = NULL;
    n->next = NULL;

    return n;
}

struct node* append(struct node* tail, struct Page* data) {
    struct node* appendee = create_node(data);

    tail->next = appendee;
    appendee->prev = tail;

    return appendee;
}

void use(struct node** head, struct node** tail, struct node** node) {

    if (!(*node)->prev) {
        (*head) = (*node);
    } else if (!(*node)->next) {
        (*tail)->prev->next = NULL;
        (*tail) = (*tail)->prev;

        (*head)->prev = (*node);
        (*node)->prev->next = (*node)->next;
        (*node)->next = (*head);
        (*node)->prev = NULL;      

        (*head) = (*node);  
    } else {
        (*head)->prev = (*node);
        (*node)->prev->next = (*node)->next;
        (*node)->next->prev = (*node)->prev;
        (*node)->next = (*head);
        (*node)->prev = NULL;
        
        (*head) = (*node);
    }
}

void swap(struct node** head, struct node** tail, struct Page* data) {

    struct node* n = create_node(data);

    (*head)->prev = n;
    n->next = (*head);
    (*head) = n;

    (*tail)->prev->next = NULL;
    (*tail) = (*tail)->prev;
}

void print_list(struct node* head) {

    while (head) {
        struct Page p = *(head->data);
        printf("Node #%d ", p.id);
    

        head = head->next;
        if (head) {
            printf("<--> \n");
        }
    }
    printf("\n");
}

void FIFO(char* filename) {
    struct Page frames[MEM_SIZE];
    int ptr = 0;
    int faults = 0;
    int in[100];


    for (int i = 0; i < MEM_SIZE; i++) {
        struct Page fill = {.id = -1};
        frames[i] = fill;
    }
    for (int i = 0; i < 100; i++) {
        in[i] = 0;
    }

    FILE* fptr = fopen(filename, "r");

    char line[25];

    int page_request_count = 0;
    while (fgets(line, 25, fptr)) {
        int id = atoi(strtok(line, " "));
        
        if (in[id] != 1) {
            faults++;
            struct Page prev_page = frames[ptr];
            if (prev_page.id != -1) {
                in[prev_page.id] = 0;
            }
            in[id] = 1;


            struct Page new_page = {.id=id};
            frames[ptr] = new_page;
            ptr = (ptr + 1) % MEM_SIZE;
        }
        page_request_count++;
    }

    printf("Number of faults with FIFO model: %d\n", faults);
    float percent_rate = (float) faults / (float) page_request_count;
    printf("Percentage rate: %.2f%%\n", percent_rate*100);
}

void LRU(char* filename) {
    struct Page filler = {.id = -1};
    struct node* head = create_node(&filler);
    
    struct node* tail = head;

    for (int i = 2; i <= MEM_SIZE; i++) {
        struct Page* f = malloc(sizeof(struct Page));

        f->id = i * -1;

        tail = append(tail, f);
    }

    int faults = 0;
    int in[100];

    for (int i = 0; i < 100; i++) {
        in[i] = 0;
    }

    FILE* fptr = fopen(filename, "r");
    char line[25];

    int page_request_count = 0;
    while (fgets(line, 25, fptr)) {
        int id = atoi(strtok(line, " "));

        if (in[id] == 1) {
            struct node* node = malloc(sizeof(struct node));
            node = head;
            while (node->data->id != id) {
                node = node->next;
            }

            use(&head, &tail, &node);
        } else {
            faults++;

            struct Page* p = malloc(sizeof(struct Page));
            p->id = id;

            int prev_id = tail->data->id;

            if (prev_id >= 0) {
                in[prev_id] = 0;
            }
            in[id] = 1;

            swap(&head, &tail, p);
        }
        page_request_count++;
    }
    printf("Number of faults with LRU model: %d\n", faults);
    float percent_rate = (float) faults / (float) page_request_count;
    printf("Percent rate: %.2f%%\n", percent_rate*100);
}

void SECOND_CHANCE(char* filename) {
    struct Page frames[MEM_SIZE];

    for (int i = 0; i < MEM_SIZE; i++) {
        struct Page p = {.id = -1, .second=-1};

        frames[i] = p;
    }

    int ptr = 0;
    int faults = 0;
    int page_request_count = 0;

    int in[100];
    for (int i = 0; i < 100; i++) {
        in[i] = -1;
    }

    FILE* fptr = fopen(filename, "r");
    char line[25];

    while (fgets(line, 25, fptr)) {
        int id = atoi(strtok(line, " "));

        if (in[id] != -1) {
            frames[in[id]].second = 1;
        } else {
            faults++;

            if (frames[ptr].id == -1) {
                in[id] = ptr;

                struct Page p = {.id = id, .second = 0};

                frames[ptr] = p;
                ptr = (ptr + 1) % MEM_SIZE;
            } else {


                while (frames[ptr].second == 1) {
                    frames[ptr].second = 0;
                    ptr = (ptr + 1) % MEM_SIZE;
                }

                in[id] = ptr;

                in[frames[ptr].id] = -1;

                struct Page p = {.id=id, .second = 0};
                frames[ptr] = p;
                ptr = (ptr + 1) % MEM_SIZE;
            }
        }

        page_request_count++;
    }

    printf("Number of faults with second chance model: %d\n", faults);
    float percent_rate = (float) faults / (float) page_request_count;
    printf("Perent rate: %.2f%%\n", percent_rate * 100);
}

void ENHANCED_SECOND_CHANCE(char* filename) {
    struct Page frames[MEM_SIZE];

    for (int i = 0; i < MEM_SIZE; i++) {
        struct Page p = {.id = -1, .second=-1, .write = -1};

        frames[i] = p;
    }

    int ptr = 0;
    int faults = 0;
    int writes = 0;
    int page_request_count = 0;

    int in[100];
    for (int i = 0; i < 100; i++) {
        in[i] = -1;
    }

    FILE* fptr = fopen(filename, "r");
    char line[25];

    while (fgets(line, 25, fptr)) {
        int id = atoi(strtok(line, " "));
        char rw = strtok(NULL, " ")[0];

        if (in[id] != -1) {
            if (rw == 'r') {
                frames[in[id]].second = 1; 
            } else {
                frames[in[id]].second = 1;
                frames[in[id]].write = 1;
            }
        } else {
            faults++;

            if (frames[ptr].id == -1) {
                in[id] = ptr;

                if (rw == 'r') {
                    struct Page p = {.id = id, .second = 0, .write = 0};
                    frames[ptr] = p;
                } else {
                    struct Page p = {.id = id, .second = 0, .write = 1};
                    frames[ptr] = p;
                    writes++;
                }

            
                ptr = (ptr + 1) % MEM_SIZE;
            } else {
                while (frames[ptr].second == 1 || frames[ptr].write == 1) {
                    if (frames[ptr].second == 1) {
                        frames[ptr].second = 0;
                    } else {
                        frames[ptr].write = 0;
                    }
                    ptr = (ptr + 1) % MEM_SIZE;
                }

                in[id] = ptr;

                in[frames[ptr].id] = -1;

                if (rw == 'r') {
                    struct Page p = {.id = id, .second = 0, .write = 0};
                    frames[ptr] = p;
                } else {
                    struct Page p = {.id = id, .second = 0, .write = 1};
                    frames[ptr] = p;
                    writes++;
                }
                
                ptr = (ptr + 1) % MEM_SIZE;
            }
        }

        page_request_count++;
    }

    printf("Number of faults with enhanced second chance model: %d\n", faults);
    float percent_rate = (float) faults / (float) page_request_count;
    printf("Percent rate: %.2f%%\n", percent_rate*100);
    printf("Number of writes on faults: %d\n", writes);
    float write_rate = (float) writes / (float) faults;
    printf("Percent rate: %.2f%%\n", write_rate*100);
}
int main() {
    char* file = "page_references.txt";
    FIFO(file);
    LRU(file);
    SECOND_CHANCE(file);
    ENHANCED_SECOND_CHANCE(file);
    return 0;
}