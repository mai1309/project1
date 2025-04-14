#include "fileui.h"
#include <stdio.h>      //printf
#include <sys/stat.h>   //mkdir
#include <sys/types.h>  //mode_t (0777)
#include <unistd.h>     //rmdir
#include <dirent.h>     //open,read,closedir
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct {          //File metadata struct
    char filename[20];
    int size;
    time_t creation_date; 
    char permissions[10];
    char content[100];
    int exists;
} File;

File all_files[100];
int file_count = 0;

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

int search_file(char *file)  //returns file location if found
{
    for(int i = 0; i < file_count; i++)
    {
        if(strcmp(all_files[i].filename, file) == 0 && all_files[i].exists)
            return i;
    }
    return -1;
}

create_file(char *file)
{
    if(search_file(file) >= 0)
    {
        printf("%s already exists\n", file);
        return;
    }
    if(file_count < 100)
    {
        //set up struct instance info
        strcpy(all_files[file_count].filename, file);
        all_files[file_count].creation_date = time(NULL);
        strcpy(all_files[file_count].permissions,"rw-rw-r");      //default permissions
        all_files[file_count].exists = 1;
        printf("%s has been created\n", file); 
        file_count++;
        return;
    }

    printf("Cannot make more files!\n");
    return;
} 

void write_file(char *file, char *content)
{
    int index = search_file(file);
    if(index >= 0)
    {
        strcpy(all_files[index].content, content);
        all_files[index].size = strlen(all_files[index].content);
        printf("wrote to %s\n", file);
        return;
    }
    
   printf("%s does not exist\n", file);
   return;

}

void read_file(char *file)
{
    int index = search_file(file);
    if(index  >= 0)
    {
        printf("%s\n", file);
        printf("%s\n", all_files[index].content);
        return;
    }

    printf("%s does not exist\n", file);
    return;
}

void delete_file(char *file)
{
    int index = search_file(file);
    if(index >= 0)
    {
        all_files[index].exists = 0;   
        printf("%s has been deleted\n", file);
        return;
    }

    printf("%s does not exist\n", file);
    return;
}