#include "file_system.h"
#include <stdio.h>      //printf & scanf
#include <string.h>     //strcmp
#include <time.h>       //time
#include <stdlib.h>     //malloc

struct File;
struct Directory;

File all_files[100];
int file_count = 0;
Directory* current_dir;

void file_simulation()
{
     //memory for the root directory
     current_dir = (Directory*)malloc(sizeof(Directory));
     if (current_dir == NULL) {
         printf("Failed to initialize root directory. Memory allocation error.\n");
         return;
     }
    strcpy(current_dir->dirname, "/");
     current_dir->parent = NULL;
     current_dir->subdir_count = 0;
     current_dir->file_count = 0;
     current_dir->exists = 1;
 
    printf("Welcome to File Simulation!\n");
    printf("Type help to see a list of commands or exit to leave simulation.\n");
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

void create_file(char *file)
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
        printf("Wrote to %s\n", file);
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
        printf("Reading file: %s\n", file);
        if(strlen(all_files[index].content) ==0)
            printf("File is empty.\n");
        else
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

void list_contents()
{
    printf("Listing Content:\n");
    if(file_count == 0)
    {
        printf("No files available.\n");
        return;
    }
    for (int i = 0; i < file_count; i++) 
    {
        if (all_files[i].exists) 
            printf("- %s\n", all_files[i].filename);
    }
}
int search_directory(Directory* current_dir, const char *name) 
{
    for(int i = 0; i < current_dir->subdir_count; i++)
    {
        if (current_dir->subdirs[i]->exists && strcmp(current_dir->subdirs[i]->dirname, name) == 0) 
            return i;
    }
    return -1;
}
void create_directory(const char *name)
{
    if(search_directory(current_dir, name) >= 0)
    {
        printf("%s directory already exists\n", name);
        return;
    }
    if (current_dir->subdir_count >= 10) 
    {
        printf("Cannot create more subdirectories in '%s'\n", current_dir->dirname);
        return;
    }
    //creating new directory
    Directory* new_dir = (Directory*)malloc(sizeof(Directory));
    if (new_dir == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    strcpy(new_dir->dirname, name);
    new_dir->parent = current_dir;
    new_dir->subdir_count = 0;
    new_dir->file_count = 0;
    new_dir->exists = 1;

    current_dir->subdirs[current_dir->subdir_count++] = new_dir;

    printf("Directory '%s' created successfully.\n", name);
    
}
//make sure you delete the directory recursively so all the files 
//it also delete 
void delete_directory(const char *name)
{

}