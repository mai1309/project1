#include "file_system.h"
#include <stdio.h>      //printf & scanf
#include <string.h>     //strcmp
#include <time.h>       //time

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

