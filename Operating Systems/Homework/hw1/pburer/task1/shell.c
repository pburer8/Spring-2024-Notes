#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_SIZE 1024

void parse(char* input, char** margs)
{
    char* token = strtok(input, " ");
    int i = 0;

    while (token != NULL)
    {
        margs[i] = malloc(sizeof(char)*strlen(token+1));
        strcpy(margs[i], token);
        token = strtok(NULL, " ");
        i++; 
    }
}

int arg_c(char* input)
{
    int args = 1;
    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] == ' ')
        {
            args++;
        }
    }
    return args;
}

int cd_check(char** myargs)
{
    if (!strcmp(myargs[0], "cd"))
    {
        if (myargs[1] == NULL)
        {
            fprintf(stderr, "Expected argument for cd\n");
        } else if (chdir(myargs[1]) != 0)
        {
            perror("error");
        }
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{

    while(true)
    {
        printf(">>> ");
        char* input_line = malloc(sizeof(char)*MAX_SIZE);
        scanf("%[^\n]%*c", input_line);

        int arg_count = arg_c(input_line);

        char* myargs[arg_count+1];

        parse(input_line, myargs);

        myargs[arg_count] = NULL;

        if (!strcmp(myargs[0], "exit"))
        {
            printf("Exiting...\n");
            exit(0);
        }

        if (cd_check(myargs) == 1)
        {
            continue;
        }
        
        int rc = fork();
        
        if (rc < 0)
        {
            perror("fork fail");
            exit(1);
        } else if (rc == 0)
        {
            if (execvp(myargs[0], myargs) == -1)
            {
                perror("error");
            }
        } else
        {
            int wc = wait(NULL);
        }
        
    }
    
    return 0;
}