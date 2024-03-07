#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority; // Priority level (higher value indicates higher priority)
};

// Example test processes
struct Process processes[] = {
    // ... (previous processes)
    {11, 10, 6, 1},
    {12, 11, 8, 3},
    {13, 12, 4, 2},
    {14, 13, 7, 1},
    {15, 14, 3, 3},
    {16, 15, 6, 2},
    {17, 16, 5, 1},
    {18, 17, 4, 3},
    {19, 18, 7, 2},
    {20, 19, 9, 1},
    
};

int c_compare(const void* a, const void* b) {
    return (*(int*) a- *(int*) b);
}

void schedule_processes(struct Process processes[], int num_processes) {
    // Separate queues for different priority levels
    struct Process queue[3][10];
    int size[3] = {0,0,0};
    int remaining_time[3][10];
    int arrival_times[3][10];
    int nexts[3] = {0,0,0};
    int turnaround[3] = {0,0,0};
    int response[3][10];

    int completed_processes = 0;
    int current_time = 0;
    int current_process[2] = {-1, -1};

    // Distribute processes to respective queues based on priority
    for (int i = 0; i < num_processes; ++i) {
        if (processes[i].priority == 1) {
            queue[0][size[0]++] = processes[i];
        } else if (processes[i].priority == 2) {
            queue[1][size[1]++] = processes[i];
        } else {
            queue[2][size[2]++] = processes[i];
        }
    }

    //populate remaining time
    for (int i = 0; i < 10; i++) {
        if (i < size[0]) {
            remaining_time[0][i] = queue[0][i].burst_time;
        }
        if (i < size[1]) {
            remaining_time[1][i] = queue[1][i].burst_time;
        }
        if (i < size[2]) {
            remaining_time[2][i] = queue[2][i].burst_time;
        }
    }

    //initialize response time
    for (int i = 0; i < 10; i++) {
        response[0][i] = 0;
        response[1][i] = 0;
        response[2][i] = 0;
    }

    //populate arrival time
    for (int i = 0; i < 10; i++) {
        arrival_times[0][i] = -1;
        arrival_times[1][i] = -1;
        arrival_times[2][i] = -1;
    }

    for (int i = 0; i < 10; i++) {
        if (i < size[0]) {
            arrival_times[0][i] = queue[0][i].arrival_time;
        }
        if (i < size[1]) {
            arrival_times[1][i] = queue[1][i].arrival_time;
        }
        if (i < size[2]) {
            arrival_times[2][i] = queue[2][i].arrival_time;
        }
    }

    //sort arrival time
    qsort(arrival_times[0], size[0], sizeof(int), c_compare);
    qsort(arrival_times[1], size[1], sizeof(int), c_compare);
    qsort(arrival_times[2], size[2], sizeof(int), c_compare);

    //start current time at first process to arrive
    if (nexts[0] < size[0]) {
        current_time = arrival_times[0][nexts[0]];
    }

    if (nexts[1] < size[1] && arrival_times[1][nexts[1]] < current_time) {
        current_time = arrival_times[1][nexts[1]];
    }

    if (nexts[2] < size[2] && arrival_times[2][nexts[2]] < current_time) {
        current_time = arrival_times[2][nexts[2]];
    }

    // Execute processes from each queue

    //Let's only use SRJF, 
    //but we only consider medium and low priority when we aren't doing anything else
    //and higher priority can interrupt low and medium
    
    //while we still have processes to do
    while (completed_processes < num_processes) {
        
        //flag if we need to reevaluate working process
        bool new_process = false;

        if (current_process[0] != -1) {
            if (remaining_time[current_process[0]][current_process[1]] == 0) {
                new_process = true;
            }

            if (nexts[0] < size[0] && arrival_times[0][nexts[0]] == current_time) {
                new_process = true;
            }

            if (current_process[0] > 0 && nexts[1] < size[1] && arrival_times[1][nexts[1]] == current_time) {
                new_process = true;
            }

            if (current_process[0] > 1 && nexts[2] < size[2] && arrival_times[2][nexts[2]] == current_time) {
                new_process = true;
            }
        } else {
            new_process = true;
        }

        //if we need to reevaluate, do process selection again
        if (new_process) {
            current_process[0] = -1;
            current_process[1] = -1;

            for (int i = 0; i < 3; i++) {
                if (current_process[0] == -1 || current_process[0] == i) {
                    for (int j = 0; j < size[i]; j++) {
                        //printf("%d\n", queue[i][j].pid);
                        if (queue[i][j].arrival_time > current_time) {
                            continue;
                        }

                        if (remaining_time[i][j] == 0) {
                            continue;
                        }

                        if (current_process[1] == -1 || queue[i][j].burst_time < queue[i][current_process[1]].burst_time) {
                            current_process[0] = i;
                            current_process[1] = j;
                        }
                    }
                }
            }

            //this is only true if the current time has no processes (i.e. less than all next arrival times)
            if (current_process[0] == -1)  {
                if (arrival_times[0][nexts[0]] != -1) {
                    current_time = arrival_times[0][nexts[0]];
                }
                
                if (arrival_times[1][nexts[1]] != -1 && arrival_times[1][nexts[1]] < current_time) {
                    current_time = arrival_times[1][nexts[1]];
                }

                if (arrival_times[2][nexts[2]] != -1 && arrival_times[2][nexts[2]] < current_time) {
                    current_time = arrival_times[2][nexts[2]];
                }
            }
        }

        //get the next arrival
        while (nexts[0] < size[0] && arrival_times[0][nexts[0]] <= current_time) {
            nexts[0]++;
        }

        while (nexts[1] < size[1] && arrival_times[1][nexts[1]] <= current_time) {
            nexts[1]++;
        }

        while (nexts[2] < size[2] && arrival_times[2][nexts[2]] <= current_time) {
            nexts[2]++;
        }

        if (current_process[0] == -1) {
            continue;
        }

        //time step
        current_time++;

        int remaining = -1;

        int priority = current_process[0];
        int place = current_process[1];
        struct Process process = queue[priority][place];

        //record response time
        if (remaining_time[priority][place] == process.burst_time) {
            response[priority][place] = (current_time - process.arrival_time - 1);
        }

        remaining_time[priority][place]--;
        remaining = remaining_time[priority][place];

        //print info
        if (remaining == 0) {
            printf("Executing Priority %d Process %d (Burst Time: %d)\n", priority + 1, process.pid, process.burst_time);
            int turnaround_time = current_time - process.arrival_time;
            turnaround[priority] += turnaround_time;
            printf("Turnaround Time for Process %d: %d\n", process.pid, turnaround_time);
            printf("Response Time for Process %d: %d\n\n", process.pid, response[priority][place]);
            completed_processes++;
        }
    }

    int responses[3] = {0,0,0};

    for (int i = 0; i < num_processes; i++) {
        responses[0] += response[0][i];
        responses[1] += response[1][i];
        responses[2] += response[2][i];
    }
    float average_high_t = (float) turnaround[0] / (float) size[0];
    float average_high_r = (float) responses[0] / (float) size[0];

    printf("Average Turnaround for High Priority: %f\n", average_high_t);
    printf("Average Response for High Priority: %f\n\n", average_high_r);

    float average_med_t = (float) turnaround[1] / (float) size[1];
    float average_med_r = (float) responses[1] / (float) size[1];

    printf("Average Turnaround for Medium Priority: %f\n", average_med_t);
    printf("Average Response for Medium Priority: %f\n\n", average_med_r);

    float average_low_t = (float) turnaround[2] / (float) size[2];
    float average_low_r = (float) responses[2] / (float) size[2];

    printf("Average Turnaround for Low Priority: %f\n", average_low_t);
    printf("Average Response for Low Priority: %f\n\n", average_low_r);

    int total_turnaround = turnaround[0] + turnaround[1] + turnaround[2];
    int total_response = responses[0] + responses[1] + responses[2];

    float average_total_t = (float) total_turnaround / (float) num_processes;
    float average_total_r = (float) total_response / (float) num_processes;

    printf("Average Turnaround: %f\n", average_total_t);
    printf("Average Response: %f\n\n", average_total_r);
}

int main() {
    int num_processes = sizeof(processes) / sizeof(struct Process);
    schedule_processes(processes, num_processes);
    return 0;
}
