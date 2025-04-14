#include <stdio.h>
#include <string.h>
#include <time.h>
#include "file_system.h"
#include <pthread.h>

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
            printf("Available Commands:\n");
            printf("- exit                         - Exit simulation\n");
            printf("- create <filename>            - Create a file (owned by current user)\n");
            printf("- read <filename>              - Read a file (requires read permission)\n");
            printf("- write <filename>             - Write to a file (requires write permission)\n");
            printf("- delete <filename>            - Delete a file (requires write permission)\n");
            printf("- adduser <username> <group>   - Add a new user to the system\n");
            printf("- switchuser <username>        - Switch to an existing user\n");
            printf("- whoami                       - Display the current user and group\n");
            printf("- createprocess <filename>     - Simulate a process writing to a file\n");
        }
        else if (strncmp(input, "createprocess", 13) == 0)
            {
                char file[50];
                if (sscanf(input + 14, "%s", file) == 1)
                {
                    if (search_file(file) >= 0)
                    {
                        pthread_t tid;
                        pthread_create(&tid, NULL, simulate_process, strdup(file));
                        pthread_detach(tid);
                        printf("Process started for file %s\n", file);
                    }
                    else
                    {
                        printf("File %s does not exist\n", file);
                    }
                }
                else
                {
                    printf("Usage: createprocess <filename>\n");
                }
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
        else if(strncmp(input, "adduser", 7) == 0)
            {
                char uname[20], grp[20];
                if(sscanf(input + 8, "%s %s", uname, grp) == 2)
                    add_user(uname, grp);
                else
                    printf("Usage: adduser <username> <group>\n");
            }
        else if(strncmp(input, "switchuser", 10) == 0)
            {
                char uname[20];
                if(sscanf(input + 11, "%s", uname) == 1)
                    switch_user(uname);
                else
                    printf("Usage: switchuser <username>\n");
            }
        else if(strcmp(input, "whoami") == 0)
            {
                printf("Current user: %s (group: %s)\n", current_user, current_group);
            }
        else
            printf("Unknown Command\n");
    } 
    return 0;
}