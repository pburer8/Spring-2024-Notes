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
    struct Process high_priority_queue[10];
    struct Process medium_priority_queue[10];
    struct Process low_priority_queue[10];
    int high_size = 0, medium_size = 0, low_size = 0;

    // Distribute processes to respective queues based on priority
    for (int i = 0; i < num_processes; ++i) {
        if (processes[i].priority == 1) {
            high_priority_queue[high_size++] = processes[i];
        } else if (processes[i].priority == 2) {
            medium_priority_queue[medium_size++] = processes[i];
        } else {
            low_priority_queue[low_size++] = processes[i];
        }
    }

    
    // Execute processes from each queue
    printf("Process Execution Order:\n");

    //Let's only use SRJF, 
    //but we only consider medium and low priority when we aren't doing anything else
    //and higher priority can interrupt low and medium
    
    //Remaining times
    int high_remaining_time[10];
    int medium_remaining_time[10];
    int low_remaining_time[10];

    for (int i = 0; i < 10; i++) {
        if (i < high_size) {
            high_remaining_time[i] = high_priority_queue[i].burst_time;
        }
        if (i < medium_size) {
            medium_remaining_time[i] = medium_priority_queue[i].burst_time;
        }
        if (i < low_size) {
            low_remaining_time[i] = low_priority_queue[i].burst_time;
        }
    }

    int high_arrival_times[high_size];
    int medium_arrival_times[medium_size];
    int low_arrival_times[low_size];

    int nexts[3] = {0,0,0};

    for (int i = 0; i < 10; i++) {
        if (i < high_size) {
            high_arrival_times[i] = high_priority_queue[i].arrival_time;
        }
        if (i < medium_size) {
            medium_arrival_times[i] = medium_priority_queue[i].arrival_time;
        }
        if (i < low_size) {
            low_arrival_times[i] = low_priority_queue[i].arrival_time;
        }
    }

    //sort arrival times
    qsort(high_arrival_times, high_size, sizeof(int), c_compare);
    qsort(medium_arrival_times, medium_size, sizeof(int), c_compare);
    qsort(low_arrival_times, low_size, sizeof(int), c_compare);

    int completed_processes = 0;

    //find the first arrival and increment time to that
    //(no need to go through a loop while doing nothing)
    int current_time = high_arrival_times[nexts[0]];
    
    if (medium_arrival_times[nexts[1]] < current_time) {
        current_time = medium_arrival_times[nexts[1]];
    }

    if (low_arrival_times[nexts[2]] < current_time) {
        current_time = low_arrival_times[nexts[2]];
    }

    //{what queue, what process in said queue}
    int current_process[2] = {0, -1};
    

    //grab available processes from high queue
    //then medium if we haven't found anything
    //then finally low
    for (int i = 0; i < high_size; i++) {
        if (high_priority_queue[i].arrival_time > current_time) {
            continue;
        }

        if (current_process[1] == -1 || high_priority_queue[i].burst_time < high_priority_queue[current_process[1]].burst_time) {
            current_process[0] = 1;
            current_process[1] = i;
        }
    }

    if (current_process[0] == 0) {
        for (int i = 0; i < medium_size; i++) {
            if (medium_priority_queue[i].arrival_time > current_time) {
                continue;
            }
            
            if (current_process[1] == -1 || medium_priority_queue[i].burst_time < medium_priority_queue[current_process[1]].burst_time) {
                current_process[0] = 2;
                current_process[1] = i;
            }
        }
    }

    if (current_process[0] == 0) {
        for (int i = 0; i < low_size; i++) {
            if (low_priority_queue[i].arrival_time > current_time) {
                continue;
            }

            if (current_process[1] == -1 || low_priority_queue[i].burst_time < low_priority_queue[current_process[1]].burst_time) {
                current_process[0] = 3;
                current_process[1] = i;
            }
        }
    }

    //get the next arrival
    while (nexts[0] < high_size && high_arrival_times[nexts[0]] <= current_time) {
        nexts[0]++;
    }

    while (nexts[1] < medium_size && medium_arrival_times[nexts[1]] <= current_time) {
        nexts[1]++;
    }

    while (nexts[2] < low_size && low_arrival_times[nexts[2]] <= current_time) {
        nexts[2]++;
    }

    //create turnaround and response sums to track avg times
    int high_turnaround = 0;
    int high_response[high_size];
    int medium_turnaround = 0;
    int medium_response[medium_size];
    int low_turnaround = 0;
    int low_response[low_size];
    
    //while we still have processes to do
    while (completed_processes < num_processes) {
        
        //flag if we need to reevaluate working process
        bool new_process = false;

        if (current_process[0] == 1) {
            //if we're done with the current process
            if (high_remaining_time[current_process[1]] == 0) {
                new_process = true;
            }

            //if there's another high process that just arrived
            if (nexts[0] < high_size && high_arrival_times[nexts[0]] == current_time) {
                new_process = true;
            }
        } else if (current_process[0] == 2) {
            //if we're done with the process
            if (medium_remaining_time[current_process[1]] == 0) {
                new_process = true;
            }

            //if theres another high or medium process that just arrived
            if (nexts[0] < high_size && high_arrival_times[nexts[0]] == current_time) {
                new_process = true;
            }

            if (nexts[1] < medium_size && medium_arrival_times[nexts[1]] == current_time) {
                new_process = true;
            }
        } else if (current_process[0] == 3) {

            //if we're done with the process
            if (low_remaining_time[current_process[1]] == 0) {
                new_process = true;
            }

            //if any other process arrived
            if (nexts[0] < high_size && high_arrival_times[nexts[0]] == current_time) {
                new_process = true;
            }

            if (nexts[1] < medium_size && medium_arrival_times[nexts[1]] == current_time) {
                new_process = true;
            }

            if (nexts[2] < low_size && low_arrival_times[nexts[2]] == current_time) {
                new_process = true;
            }
        } else {
            new_process = true;
        }

        //if we need to reevaluate, do process selection again
        if (new_process) {
            current_process[0] = 0;
            current_process[1] = -1;

            for (int i = 0; i < high_size; i++) {
                if (high_priority_queue[i].arrival_time > current_time) {
                    continue;
                }

                if (high_remaining_time[i] == 0) {
                    continue;
                }

                if (current_process[1] == -1 || high_priority_queue[i].burst_time < high_priority_queue[current_process[1]].burst_time) {
                    current_process[0] = 1;
                    current_process[1] = i;
                }
            }

            if (current_process[0] == 0) {
                for (int i = 0; i < medium_size; i++) {
                    if (medium_priority_queue[i].arrival_time > current_time) {
                        continue;
                    }

                    if (medium_remaining_time[i] == 0) {
                        continue;
                    }
            
                    if (current_process[1] == -1 || medium_priority_queue[i].burst_time < medium_priority_queue[current_process[1]].burst_time) {
                        current_process[0] = 2;
                        current_process[1] = i;
                    }
                }
            }

            if (current_process[0] == 0) {
                for (int i = 0; i < low_size; i++) {
                    if (low_priority_queue[i].arrival_time > current_time) {
                        continue;
                    }

                    if (low_remaining_time[i] == 0) {
                        continue;
                    }

                    if (current_process[1] == -1 || low_priority_queue[i].burst_time < low_priority_queue[current_process[1]].burst_time) {
                        current_process[0] = 3;
                        current_process[1] = i;
                    }
                }
            }

            //this is only true if the current time has no processes (i.e. less than all next arrival times)
            if (current_process[0] == 0)  {
                if (high_arrival_times[nexts[0]] < medium_arrival_times[nexts[1]] &&  high_arrival_times[nexts[0]] < low_arrival_times[nexts[2]]) {
                    current_time = high_arrival_times[nexts[0]];
                } else if (medium_arrival_times[nexts[1]] < high_arrival_times[nexts[0]] && medium_arrival_times[nexts[1]] < low_arrival_times[nexts[2]]) {
                    current_time = medium_arrival_times[nexts[1]];
                } else if (low_arrival_times[nexts[2]] < high_arrival_times[nexts[0]] && low_arrival_times[nexts[2]] < medium_arrival_times[nexts[1]]) {
                    current_time = low_arrival_times[nexts[1]];
                }
            }
        }

        //get the next arrival
        while (nexts[0] < high_size && high_arrival_times[nexts[0]] <= current_time) {
            nexts[0]++;
        }

        while (nexts[1] < medium_size && medium_arrival_times[nexts[1]] <= current_time) {
            nexts[1]++;
        }

        while (nexts[2] < low_size && low_arrival_times[nexts[2]] <= current_time) {
            nexts[2]++;
        }

        if (current_process[0] == 0) {
            continue;
        }

        //time step
        current_time++;

        //variables to reduce code duplication mostly
        int remaining = -1;
        int pid = -1;
        int arrival_time = -1;
        int burst_time = -1;
        int response_time = -1;

        if (current_process[0] == 1) {
            //if this is an untouched process
            if (high_remaining_time[current_process[1]] == high_priority_queue[current_process[1]].burst_time) {
                high_response[current_process[1]] = (current_time - high_priority_queue[current_process[1]].arrival_time);
            }

            high_remaining_time[current_process[1]]--;
            remaining = high_remaining_time[current_process[1]];

            if (remaining == 0) {
                pid = high_priority_queue[current_process[1]].pid;
                arrival_time = high_priority_queue[current_process[1]].arrival_time;
                burst_time = high_priority_queue[current_process[1]].burst_time;
                response_time = high_response[current_process[1]];

                high_turnaround += (current_time - arrival_time);
            }
        } else if (current_process[0] == 2) {
            
            if (medium_remaining_time[current_process[1]] == medium_priority_queue[current_process[1]].burst_time) {
                medium_response[current_process[1]] = (current_time - medium_priority_queue[current_process[1]].arrival_time);
            }

            medium_remaining_time[current_process[1]]--;
            remaining = medium_remaining_time[current_process[1]];

            if (remaining == 0) {
                pid = medium_priority_queue[current_process[1]].pid;
                arrival_time = medium_priority_queue[current_process[1]].arrival_time;
                burst_time = medium_priority_queue[current_process[1]].burst_time;
                response_time = medium_response[current_process[1]];

                medium_turnaround += (current_time - arrival_time);
            }
        } else if (current_process[0] == 3) {

            if (low_remaining_time[current_process[1]] == low_priority_queue[current_process[1]].burst_time) {
                low_response[current_process[1]] = (current_time - low_priority_queue[current_process[1]].arrival_time);
            }

            low_remaining_time[current_process[1]]--;
            remaining = low_remaining_time[current_process[1]];

            if (remaining == 0) {
                pid = low_priority_queue[current_process[1]].pid;
                arrival_time = low_priority_queue[current_process[1]].arrival_time;
                burst_time = low_priority_queue[current_process[1]].burst_time;
                response_time = low_response[current_process[1]];

                low_turnaround += (current_time - arrival_time);
            }
        }

        //print info
        if (remaining == 0) {
            printf("Executing Priority %d Process %d (Burst Time: %d)\n", current_process[0], pid, burst_time);
            int turnaround_time = current_time - arrival_time;
            printf("Turnaround Time for Process %d: %d\n", pid, turnaround_time);
            printf("Response Time for Process %d: %d\n\n", pid, response_time);
            completed_processes++;
        }
    }

    int high_response_ = 0;
    int med_response_ = 0;
    int low_response_ = 0;

    for (int i = 0; i < num_processes; i++) {
        if (i < high_size) {
            high_response_ += high_response[i];
        }
        if (i < medium_size) {
            med_response_ += medium_response[i];
        }
        if (i < low_size) {
            low_response_ += low_response[i];
        }
    }
    float average_high_t = (float) high_turnaround / (float) high_size;
    float average_high_r = (float) high_response_ / (float) high_size;

    printf("Average Turnaround for High Priority: %f\n", average_high_t);
    printf("Average Response for High Priority: %f\n\n", average_high_r);

    float average_med_t = (float) medium_turnaround / (float) medium_size;
    float average_med_r = (float) med_response_ / (float) medium_size;

    printf("Average Turnaround for Medium Priority: %f\n", average_med_t);
    printf("Average Response for Medium Priority: %f\n\n", average_med_r);

    float average_low_t = (float) low_turnaround / (float) low_size;
    float average_low_r = (float) low_response_ / (float) low_size;

    printf("Average Turnaround for Low Priority: %f\n", average_low_t);
    printf("Average Response for Low Priority: %f\n\n", average_low_r);

    int total_turnaround = high_turnaround + medium_turnaround + low_turnaround;
    int total_response = high_response_ + med_response_ + low_response_;

    float average_total_t = (float) total_turnaround / (float) num_processes;
    float average_total_r = (float) total_turnaround / (float) num_processes;

    printf("Average Turnaround: %f\n", average_total_t);
    printf("Average Response: %f\n\n", average_total_r);
}

int main() {
    int num_processes = sizeof(processes) / sizeof(struct Process);
    schedule_processes(processes, num_processes);
    return 0;
}
