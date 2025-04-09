#include <stdio.h>
#include <string.h>
#include <stdlib.h>     //for exit()
#include "fileui.c"

#define MAX_INPUT 100

int main() {
    file_simulation();
    char input[MAX_INPUT];

    while(1) {
        printf("$$$> ");

        if(fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\nExiting...\n");
            break;
        }
    }
    
    // Removes trailing newline so input can read correctly
    input[strcspn(input, "\n")] = 0;

    if (strcmp(input, "exit") == 0) {
        exit(0);
    } else if (strcmp(input, "help") == 0) {
        printf("Available commands:\n");
        printf(" - help\n");
        printf(" - exit\n");
        printf(" - (Add your own commands here...)\n");
    } else {
        printf("Unknown command: %s\n", input);
    }
    return 0;
}