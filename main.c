#include <stdio.h>
#include <string.h>
#include <time.h>
#include "file_system.c"

int main()
{
    int i = 0;
    char input[50];
    char file[50];
    char content[100];
    
    file_simulation();
    while(1)
    {
        printf("$$$> ");

        // Read user input, including spaces, and automatically handles newlines
        scanf(" %[^\n]", input);  // This reads until a newline is encountered

        if(strcmp(input, "help") == 0)
        {
            printf("Avaliable Commands:\n");
            printf("- exit    - Exit simulation\n");
            printf("- create  - Create file\n");
            printf("- read    - Read file\n");
            printf("- write   - Write to file\n");
            printf("- delete  - Delete file\n");
        }
        else if(strncmp(input, "create", 6) == 0)       //extracts the file name after the input word
        {
            if(sscanf(input + 7, "%s", file) == 1)
                create_file(file);
            else    
                printf("Missing file name.\n");
        }
        else if(strncmp(input, "read", 4) == 0)
        {
            if(sscanf(input + 5, "%s", file) == 1)
                read_file(file);
            else    
                printf("Missing file name.\n");
        }
        else if(strncmp(input, "write", 5) == 0)
        {
            if(sscanf(input + 6, "%s", file) == 1)
            {
                int index = search_file(file); 
                if(index >= 0)
                {
                    getchar();
                    printf("Write: ");
                    scanf("%[^\n]", content);   //reads content with spaces
                    write_file(file, content);
                }
                else
                    printf("%s does not exist\n", file);
            }
            else    
                printf("Missing file name.\n");
        }
        else if(strncmp(input, "delete", 6) == 0)
        {
            if(sscanf(input + 7, "%s", file) == 1)
                delete_file(file);
            else    
                printf("Missing file name.\n");
        }
        else if(strcmp(input, "exit") == 0)
            break;
        else
            printf("Unknown Command\n");
    } 
    return 0;
}