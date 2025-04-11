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

//use to test - can be deleted later
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