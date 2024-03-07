#include <stdio.h>
#include <stdlib.h>

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
};

int c_compare(const void* a, const void* b) {
    return (*(int*) a- *(int*) b);
}

// Example test processes
struct Process processes[] = {
    {1, 0, 10},
    {2, 2, 5},
    {3, 4, 8}
    // Add more processes here
};

void schedule_processes_FIFO(struct Process processes[], int num_processes) {
    int current_time = 0; // Initialize current time
    printf("Process Execution Order:\n");

    for (int i = 0; i < num_processes; ++i) {
        // Wait until the process arrives (if needed)
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        // Execute the process
        printf("Executing Process %d (Burst Time: %d)\n", processes[i].pid, processes[i].burst_time);
        current_time += processes[i].burst_time;

        // Calculate turnaround time and response time
        int turnaround_time = current_time - processes[i].arrival_time;
        printf("Turnaround Time for Process %d: %d\n\n", processes[i].pid, turnaround_time);
        // Response time measurement need to be implemented.
    }
}

void schedule_processes_SJF(struct Process processes[], int num_processes) {
    int current_time = 0;
    int completed_processes = 0;
    int completed[num_processes];
    for (int i = 0; i < num_processes; i++) {
        completed[i]= 0;
    }
    
    while (completed_processes < num_processes) {
        int current_process = -1;
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time > current_time) {
                continue;
            }

            if (completed[i] == 1) {
                continue;
            }

            if (current_process == -1 || processes[i].burst_time < processes[current_process].burst_time) {
                current_process = i;
            }
        }

        if (current_process == -1) {
            for (int i = 0; i < num_processes; i++) {
                if (completed[i] == 1) {
                    continue;
                }

                if (current_process == -1 || processes[i].burst_time < processes[current_process].burst_time) {
                    current_process = i;
                }
            }
            current_time = processes[current_process].arrival_time;
        }

        printf("Executing Process %d (Burst Time: %d)\n", processes[current_process].pid, processes[current_process].burst_time);
        current_time += processes[current_process].burst_time;
        completed[current_process] = 1;

        int turnaround_time = current_time - processes[current_process].arrival_time;
        printf("Turnaround Time for Process %d: %d\n\n", processes[current_process].pid, turnaround_time);
        completed_processes++;
    }
}

void schedule_processes_SRJF(struct Process processes[], int num_processes) {
    int remaining_time[num_processes];
    int arrival_times[num_processes];
    for (int i = 0; i < num_processes; i++) {
        remaining_time[i] = processes[i].burst_time;
        arrival_times[i] = processes[i].arrival_time;
    }

    qsort(arrival_times, num_processes, sizeof(int), c_compare);
    
    int current_time = arrival_times[0];
    int next_arrival = 0;
    int completed_processes = 0;
    int current_process = -1;

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time > arrival_times[next_arrival]) {
            continue;
        }
        if (current_process == -1 || remaining_time[i] < remaining_time[current_process]) {
            current_process = i;
        }
    }
    
    while (arrival_times[next_arrival] == current_time) {
        next_arrival++;
    }

    while (completed_processes < num_processes) {
        if (remaining_time[current_process]== 0 || (next_arrival < num_processes && current_time == arrival_times[next_arrival])) {
            current_process = -1;
            for (int i = 0; i < num_processes; i++) {
                if (processes[i].arrival_time > current_time || remaining_time[i] == 0) {
                    continue;
                }
                if (current_process == -1 || remaining_time[i] < remaining_time[current_process]) {
                    current_process = i;
                }
            }
        }

        while (next_arrival < num_processes && arrival_times[next_arrival] == current_time) {
            next_arrival++;
        }

        current_time++;
        remaining_time[current_process]--;

        if (remaining_time[current_process] == 0) {
            completed_processes++;
            printf("Executing Process %d (Burst Time: %d)\n", processes[current_process].pid, processes[current_process].burst_time);
            //printf("Current Time: %d\n", current_time);
            //printf("Arrival Time: %d\n", processes[current_process].arrival_time);
            int turnaround_time = current_time - processes[current_process].arrival_time;
            printf("Turnaround Time for Process %d: %d\n\n", processes[current_process].pid,turnaround_time);
        }
    }
    
}

int main() {
    int num_processes = sizeof(processes) / sizeof(struct Process);
    printf("FIFO scheduling\n");
    schedule_processes_FIFO(processes, num_processes);
    printf("\nSJF scheduling\n");
    schedule_processes_SJF(processes, num_processes);
    printf("\nSRJF scheduling\n");
    schedule_processes_SRJF(processes, num_processes);
    return 0;
}
