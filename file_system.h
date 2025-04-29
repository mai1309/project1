#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <time.h>       //time

/*check why create directory doesnt show correct directory name
void change_directory(const char *name);    */

typedef struct File {          //File metadata struct
    char filename[20];
    int size;
    time_t creation_date; 
    char permissions[10];
    char content[100];
    int exists;
    char owner[20]; 
    char group[20];   
} File;


typedef struct Directory{        //Directory metadata struct
    char dirname[20];
    struct Directory *parent;
    struct Directory *subdirs[10];
    File *files[50];
    int subdir_count;
    int file_count;
    int exists;
} Directory;

void file_simulation();
int search_file(char *file);  
void create_file(char *file);
void write_file(char *file, char *content);
void read_file(char *file);
void delete_file(char *file);
void list_contents();

int search_directory(Directory* current_dir, const char *name);
void create_directory(const char *name);
void delete_directory(const char *name);
void switchdir(const char *name);

void add_user(const char *username, const char *group);
void switch_user(const char *username);
void list_users();
extern char current_user[20];
extern char current_group[20];

void* simulate_process(void *arg);

#endif
