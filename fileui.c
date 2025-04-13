#include "fileui.h"
#include <stdio.h>      //printf
#include <sys/stat.h>   //mkdir
#include <sys/types.h>  //mode_t (0777)
#include <unistd.h>     //rmdir
#include <dirent.h>     //open,read,closedir

void file_simulation()
{
    printf("Welcome to the File System Simulation!\n");
    printf("Type 'help' for a list of commands, or 'exit' to quit.\n");
}

/* example of what can be printed, add these 
Welcome to File System Simulation
-----------------------------------
$> help
Available commands:
- touch    - Create a new file
- rm       - Remove a file
- cat      - Display file contents

$> touch readme.txt
Created file: readme.txt

$> cat readme.txt
File is empty.

$> echo "This is a project!" > readme.txt
Added content to readme.txt

$> rm readme.txt
Removed file: readme.txt

*/
void list_contents()
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Unable to open current directory");
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("- %s\n", entry->d_name);
        }
    }
    closedir(dir);
}

void create_directory(const char *name)
{
    const char *directory_name = name;
    if(mkdir(directory_name, 0777) == 0)
    {
        printf("Directory succesfully created.\n");
    }
    else
    {
        perror("mkdir failed");
    }
}

void delete_directory(const char *name)
{
    const char *directory_name = name;
    //check if directory exists 
    DIR *dir = opendir(directory_name);
    if(dir)
    {
        closedir(dir);
        if(rmdir(directory_name) == 0)
        {
            printf("Directory succesfully deleted.\n");
        }
        else
        {
            perror("rmdir failed");
        }
    }
    else
    {
        perror("Directory does not exist");
    }
}

void change_directory(const char *name)
{
    const char *directory_name = name;
    DIR *dir = opendir(directory_name);
    if(dir)
    {
        closedir(dir);
        if(chdir(directory_name)==0)
        {
            char cwd[1024]; //stores full path to current directory 
            if(getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("Directory changed.\n");
            }
        }
        else
        {
            perror("cd failed");
        }
    }
    else
    {
        perror("Directory does not exist.\n");
    }
    
}