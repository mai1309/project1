#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "file_system.h"

int main()
{
    int i = 0;
    char input[50];
    char file[50];  //used for files
    char name[50];  //used for directories
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
            
            printf("- exit                              - Exit simulation\n");
            printf("- create <filename>                 - Create a file (owned by current user)\n");
            printf("- read <filename>                   - Read a file (requires read permission)\n");
            printf("- write <filename>                  - Write to a file (requires write permission)\n");
            printf("- delete <filename>                 - Delete a file (requires write permission)\n");
            printf("- adduser <username> <group>        - Add a new user to the system\n");
            printf("- deleteuser <username> <group>     - Remove a user from the system\n");
            printf("- switchuser <username>             - Switch to an existing user\n");
            printf("- listusers                         - List users in the system\n");
            printf("- whoami                            - Display the current user and group\n");
            printf("- createprocess <filename>          - Simulate a process writing to a file\n");
            printf("- list                              - List content in directory\n");
            printf("- createdir <dirname>               - Creates directory\n");
            printf("- deletedir <dirname>               - Deletes directory\n");
            printf("- switchdir <dirname>               - Changes directory, use .. to move backwards\n");
            printf("- link <linkname> <target>          - Creates Symlink\n");
            printf("- unlink <linkname>                 - Deletes Symlink\n");
            printf("- resolve <linkname>                - Look up what symlink points to\n");
            printf("- chmod <filename> <permissions>    - Look up what symlink points to\n");
        }
        else if (strncmp(input, "createprocess ", 14) == 0)
        {
            char file[50];
            if (sscanf(input + 14, "%s", file) == 1)
            {
                if (search_file(file) >= 0)
                {
                    pthread_t tid;
                    char *file_copy = strdup(file); // Safe string copy
                    pthread_create(&tid, NULL, simulate_process, file_copy);
                    pthread_join(tid, NULL);        // Wait for the thread to finish

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
        else if(strncmp(input, "create ", 7) == 0)       //extracts the file name after the input word
        {
            if(sscanf(input + 7, "%s", file) == 1)
                create_file(file);
            else    
                printf("Missing file name.\n");
        }
        else if(strncmp(input, "read ", 5) == 0)
        {
            if(sscanf(input + 5, "%s", file) == 1)
                read_file(file);
            else    
                printf("Missing file name.\n");
        }
        else if (strncmp(input, "write ", 6) == 0)
        {
            if (sscanf(input + 6, "%s", file) == 1)
            {
                write_file(file);
            }
            else    
            {
                printf("Missing file name.\n");
            }
        }
        else if(strncmp(input, "delete ", 7) == 0)
        {
            if(sscanf(input + 7, "%s", file) == 1)
                delete_file(file);
            else    
                printf("Missing file name.\n");
        }
        else if(strcmp(input, "exit") == 0)
        {
            printf("Exiting File Simulation. Goodbye!\n");
            break;
        }
        else if(strcmp(input, "list") == 0)
            list_contents();
        else if(strncmp(input, "createdir ", 10) == 0)       //extracts the file name after the input word
        {
            if(sscanf(input + 10, "%s", name) == 1)
                create_directory(name);
            else    
                printf("Missing directory name.\n");
        }
        else if(strncmp(input, "deletedir ", 10) == 0)
        {
            if(sscanf(input + 10, "%s", name) == 1)
                delete_directory(name);
            else    
                printf("Missing directory name.\n");
        }
        else if(strncmp(input, "switchdir ", 10) == 0)
        {
            if(sscanf(input + 10, "%s", name) == 1)
                switchdir(name);
            else    
                printf("Missing directory name.\n");
        }
        else if (strncmp(input, "link ", 5) == 0)
        {
            char linkname[50], target[50];
            if (sscanf(input + 5, "%s %s", linkname, target) == 2)
                create_symlink(linkname, target);
            else
                printf("Usage: link <linkname> <target>\n");
        }
        else if (strncmp(input, "unlink ", 7) == 0)
        {
            char linkname[50];
            if (sscanf(input + 7, "%s", linkname) == 1)
                delete_symlink(linkname);
            else
                printf("Usage: unlink <linkname>\n");
        }
        else if (strncmp(input, "resolve ", 8) == 0)
        {
            char linkname[50];
            if (sscanf(input + 8, "%s", linkname) == 1)
            {
                char *target = resolve_symlink(linkname);
                if (target)
                    printf("Symlink '%s' points to '%s'\n", linkname, target);
                else
                    printf("Symlink '%s' not found.\n", linkname);
            }
            else
                printf("Usage: resolve <linkname>\n");
        }
        else if(strncmp(input, "adduser ", 8) == 0)
        {
            char uname[20], grp[20];
            if(sscanf(input + 8, "%s %s", uname, grp) == 2)
                add_user(uname, grp);
            else
                printf("Usage: adduser <username> <group>\n");
        }
        else if(strncmp(input, "deleteuser ", 10) == 0)
        {
            char uname[20], grp[20];
            if(sscanf(input + 10, "%s %s", uname, grp) == 2)
                delete_user(uname, grp);
            else
                printf("Usage: deleteuser <username> <group>\n");
        }
        else if(strncmp(input, "switchuser ", 11) == 0)
        {
            char uname[20];
            if(sscanf(input + 11, "%s", uname) == 1)
                switch_user(uname);
            else
                printf("Usage: switchuser <username>\n");
        }
        else if(strcmp(input, "whoami") == 0)
            printf("Current user: %s (group: %s)\n", current_user, current_group);
        else if(strcmp(input, "listusers") == 0)
            list_users();
        else if (strncmp(input, "chmod ", 6) == 0)
        {
            char fname[50], perms[10];
            if (sscanf(input + 6, "%s %s", fname, perms) == 2)
                chmod_file(fname, perms);
            else
                printf("Usage: chmod <filename> <permissions>\n");
        }            
        else
            printf("Unknown Command\n");
    } 
    return 0;
}