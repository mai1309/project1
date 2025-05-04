#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <time.h>       //time
#include <unistd.h>     //symlink

typedef struct {             //symlink struct 
    char linkname[50];       //name of the symlink
    char target[50];         //target filename or directory name
    int exists;
} SymLink;

typedef struct File {          //file metadata struct
    char filename[20];
    int size;
    time_t creation_date; 
    char permissions[10];
    char content[100];
    int exists;
    char owner[20]; 
    char group[20];   
} File;

typedef struct Directory{        //directory metadata struct
    char dirname[20];
    struct Directory *parent;
    struct Directory *subdirs[10];
    File *files[50];
    int subdir_count;
    int file_count;
    SymLink* symlinks[20];  
    int symlink_count;
    int exists;
} Directory;


void chmod_file(char *file, const char *new_perms);

void file_simulation();

int search_file(char *file);  
void create_file(char *file);
void write_file(char *file);        
void read_file(char *file);
void delete_file(char *file);

void list_contents();

int search_directory(Directory* current_dir, const char *name);
void create_directory(const char *name);
void delete_directory(const char *name);
void switchdir(const char *name);

void create_symlink(char *linkname, char *target);
char* resolve_symlink(char *name);
void delete_symlink(char *linkname);

void add_user(const char *username, const char *group);
void switch_user(const char *username);
void list_users();
extern char current_user[20];
extern char current_group[20];

void* simulate_process(void *arg);

#endif
