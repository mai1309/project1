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
                printf(" - help\n");
                printf(" - exit\n");
                printf(" - mkdir\n");
                printf(" - rmdir\n");
            }
            else if (strcmp(input, "ls") == 0) {
                list_contents();
            } 
            else if (strncmp(input, "mkdir",5) == 0) {
                sscanf(input + 6, "%s", name);      //gets the directory name
                create_directory(name);
            } 
            else if (strncmp(input, "rmdir",5) == 0) {
                sscanf(input + 6, "%s", name);      //gets the directory name
                delete_directory(name);
            } 
            else {
                printf("Unknown command: %s\n", input);
            }
        }
    }
    return 0;
}