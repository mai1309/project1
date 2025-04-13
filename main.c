#include <stdio.h>
#include <string.h>
#include <stdlib.h>     //for exit()
#include <sys/stat.h>
#include <sys/types.h>
#include "fileui.c"

#define MAX_INPUT 100

int main() {
    file_simulation();
    char input[MAX_INPUT];
    char name[100];

    while(1) {
        printf("$$$> ");

        if(fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\nExiting...\n");
            break;
        }
        else
        {
            // Removes trailing newline so input can read correctly
            input[strcspn(input, "\n")] = 0;

            if (strcmp(input, "exit") == 0) {
             exit(0);
            } 
            else if (strcmp(input, "help") == 0) {
                printf("Available commands:\n");
                printf("- exit     - Exit the simulation\n");
                printf("- mkdir    - Create a new directory\n");
                printf("- rmdir    - Delete an existing directory\n");
                printf("- ls       - List directory contents\n");
                printf("- cd       - Change directory\n");
            }
            else if (strcmp(input, "ls") == 0) {
                list_contents();
            } 
            else if (strncmp(input, "mkdir",5) == 0) {
                if(sscanf(input + 6, "%s", name)==1)      //gets the directory name
                {
                    create_directory(name);
                }
                else
                {
                    printf("Invalid directory name.\n");
                }
            } 
            else if (strncmp(input, "rmdir",5) == 0) {
                if(sscanf(input + 6, "%s", name)==1)      //gets the directory name
                {
                    delete_directory(name);
                }
                else
                {
                    printf("Invalid directory name.\n");
                }
            } 
            else if (strncmp(input, "cd",2) == 0) {
                if(sscanf(input + 3, "%s", name)==1)      //gets the directory name
                {
                    change_directory(name);
                }
                else
                {
                    printf("In the same directory, add a name to change.\n");
                }
            } 
            else {
                printf("Unknown command: %s\n", input);
            }
        }
    }
    return 0;
}