#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>

void display_info(int pid)
{
    
}

void display_pids(char* input)
{
    DIR *directory = opendir("/proc");
    struct dirent *ep;

    if (directory != NULL)
    {
        int i = 0;
        while ((ep = readdir(directory)) != NULL)
        {
            char* pid = malloc(sizeof(char)*strlen(ep->d_name));
            pid = ep->d_name;

            if (isdigit(pid[0]))
            { 
                if (i%4 == 0)
                {
                    printf("\n");
                }
                printf("id = %s\t", pid);
                i++;
            }
        }
        printf("\n");
        (void) closedir(directory);
    } else
    {
        perror("could not open proc");
        exit(-1);
    }
}
int main(int argc, char* argv[])
{
    DIR *directory = opendir("/proc");
    struct dirent *ep;

    int uid = getuid();

    if (directory != NULL)
    {
        while ((ep = readdir(directory)) != NULL)
        {
            int pid = atoi(ep->d_name);
                
            char filename[1000];
            sprintf(filename, "/proc/%d/stat", pid);

            struct stat info;
            stat(filename, &info);

            if (info.st_uid == uid)
            {
                printf("pid = %d\n", pid);
                char status[1000];
                sprintf(status, "/proc/%d/status", pid);

                FILE *f = fopen(status, "r");

                int linecount = 0;
                char line[1000];
                while (fgets(line, sizeof line, f) != NULL)
                {
                    linecount++;
                }

                fclose(f);

                f = fopen(status, "r");

                int current_line = 0;
                while (fgets(line, sizeof line, f) != NULL)
                {
                    if (current_line == 0)
                    {
                        printf("%s", line);
                    } else if (current_line == linecount - 2)
                    {
                        printf("%s", line);
                    } else if (current_line == linecount - 1)
                    {
                        printf("%s", line);
                    }
                    current_line++;
                }

                char exe_path[1000];
                char dummy[1000];
                sprintf(dummy, "/proc/%d/exe", pid);
                readlink(dummy, exe_path, 1000);
                printf("Executable path: %s\n\n", exe_path);
            }
        }
        (void) closedir(directory); 
    } else 
    {
        perror("could not open /proc");
        exit(-1);
    }
    return 0;
}