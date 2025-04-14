#include <stdio.h>
#include <string.h>
#include <time.h>
#include "file_system.c"

#define MAX_INPUT 100

int main()
{
    int i = 0;
    char operation[10];
    char file[20];
    char content[100];
    while(1)
    {
        printf("What would you like to do?\n");
        scanf("%s", operation);
        printf("File name: ");
        scanf("%s", file);

        if(strcmp(operation, "create") == 0)
            create_file(file);
        else if(strcmp(operation, "read") == 0)
            read_file(file);
        else if(strcmp(operation, "write") == 0)
        {
            printf("Write: ");
            scanf("%[^\n]", content);
            write_file(file, content);
        }
        else if(strcmp(operation, "delete") == 0)
            delete_file(file);
        else if(strcmp(operation, "exit") == 0)
            break;
    } 
    
    return 0;
}