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
    {1, 0, 8},
    {2, 1, 5},
    {3, 2, 10},
    {4, 3, 3},
    {5, 4, 6},
    {6, 5, 7},
    {7, 6, 4},
    {8, 7, 9},
    {9, 8, 2},
    {10, 9, 5},
    {11, 10, 6},
    {12, 11, 8},
    {13, 12, 4},
    {14, 13, 7},
    {15, 14, 3},
    {16, 15, 6},
    {17, 16, 5},
    {18, 17, 4},
    {19, 18, 4},
    {20, 19, 9}
    // Add more processes here
};

void schedule_processes_FIFO(struct Process processes[], int num_processes) {
    int current_time = 0; // Initialize current time
    printf("Process Execution Order:\n\n");
    int turnaround_sum = 0;
    int response_sum = 0;

    for (int i = 0; i < num_processes; ++i) {
        // Wait until the process arrives (if needed)
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        int response_time = current_time - processes[i].arrival_time;

        // Execute the process
        printf("Executing Process %d (Burst Time: %d)\n", processes[i].pid, processes[i].burst_time);
        current_time += processes[i].burst_time;

        // Calculate turnaround time and response time
        int turnaround_time = current_time - processes[i].arrival_time;
        turnaround_sum += turnaround_time;
        printf("Turnaround Time for Process %d: %d\n", processes[i].pid, turnaround_time);
        
        response_sum += response_time;
        printf("Response Time for Process %d: %d\n\n", processes[i].pid, response_time);
    }

    float average_turnaround = ((float) turnaround_sum) / (float) num_processes;
    printf("Average Turnaround Time: %f\n", average_turnaround);
    float average_response = ((float) response_sum) / (float) num_processes;
    printf("Average Response Time: %f\n", average_response);
}

void schedule_processes_SJF(struct Process processes[], int num_processes) {

    int current_time = 0;
    int completed_processes = 0;
    int turnaround_sum = 0;
    int response_sum = 0;

    int completed[num_processes];
    for (int i = 0; i < num_processes; i++) {
        completed[i] == 0;
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

        int response_time = (current_time - processes[current_process].arrival_time);
        response_sum += (current_time - processes[current_process].arrival_time);
        printf("Executing Process %d (Burst Time: %d)\n", processes[current_process].pid, processes[current_process].burst_time);
        current_time += processes[current_process].burst_time;
        completed[current_process] = 1;

        int turnaround_time = current_time - processes[current_process].arrival_time;
        turnaround_sum += turnaround_time;
        printf("Turnaround Time for Process %d: %d\n", processes[current_process].pid, turnaround_time);
        printf("Response Time for Process %d: %d\n\n", processes[current_process].pid, response_time);
        completed_processes++;
    }

    //Calculate average turnaround and response
    float average_turnaround = ((float) turnaround_sum) / (float) num_processes;
    printf("Average Turnaround Time: %f\n", average_turnaround);

    float average_response = ((float) response_sum) / (float) num_processes;
    printf("Average Response Time: %f\n", average_response);
}

void schedule_processes_SRJF(struct Process processes[], int num_processes) {
    //Create a remaining time array
    //Arrival time array
    //Response time array
    //And a sum for turnaround times

    int remaining_time[num_processes];
    int arrival_times[num_processes];
    int response_times[num_processes];
    int turnaround_sum = 0;

    //Initialize arrays
    for (int i = 0; i < num_processes; i++) {
        remaining_time[i] = processes[i].burst_time;
        arrival_times[i] = processes[i].arrival_time;
        response_times[i] = -1;
    }

    //Sort arrival times
    qsort(arrival_times, num_processes, sizeof(int), c_compare);
    
    //Current time is the first arrival time
    int current_time = arrival_times[0];
    //next arrival indexes arrival times
    int next_arrival = 0;
    //Flag variable for how we know we're done
    int completed_processes = 0;
    //Which process are we working on
    int current_process = -1;

    //select the first and shortest process that arrives
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time > arrival_times[next_arrival]) {
            continue;
        }
        if (current_process == -1 || remaining_time[i] < remaining_time[current_process]) {
            current_process = i;
        }
    }
    
    //Get the next distinct arrival time
    while (arrival_times[next_arrival] == current_time) {
        next_arrival++;
    }

    //While we are still executing processes
    while (completed_processes < num_processes) {

        //if we are finished with the current process, or new processes arrive
        if (remaining_time[current_process] == 0 || (next_arrival < num_processes && current_time == arrival_times[next_arrival])) {
            current_process = -1;
            for (int i = 0; i < num_processes; i++) {
                if (processes[i].arrival_time > current_time || remaining_time[i] == 0) {
                    continue;
                }
                if (current_process == -1 || remaining_time[i] < remaining_time[current_process]) {
                    current_process = i;
                }
            }

            if (current_process == -1) {
                current_time = arrival_times[next_arrival];
            }
        }

        //get next distinct arrival time
        while (next_arrival < num_processes && arrival_times[next_arrival] == current_time) {
            next_arrival++;
        }

        if (current_process == -1) {
            continue;
        }

        //increment arrival time
        current_time++;
        remaining_time[current_process]--;
        //if this is our first time with current process, set response time
        if (response_times[current_process] == -1) {
            response_times[current_process] = (current_time - processes[current_process].arrival_time - 1);
        }

        //if we are finished, print out info
        if (remaining_time[current_process] == 0) {
            completed_processes++;
            printf("Executing Process %d (Burst Time: %d)\n", processes[current_process].pid, processes[current_process].burst_time);
            int turnaround_time = current_time - processes[current_process].arrival_time;
            turnaround_sum += turnaround_time;
            printf("Turnaround Time for Process %d: %d\n",processes[current_process].pid, turnaround_time);
            printf("Response Time for Process %d: %d\n\n", processes[current_process].pid, response_times[current_process]);
        }
    }

    //calculate averages
    int response_sum = 0;
    for (int i = 0; i < num_processes; i++) {
        response_sum += response_times[i];
    }
    float average_turnaround = ((float) turnaround_sum) / (float) num_processes;
    printf("Average Turnaround Time: %f\n", average_turnaround);
    float average_response = ((float) response_sum) / (float) num_processes;
    printf("Average Response Time: %f\n", average_response);
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
