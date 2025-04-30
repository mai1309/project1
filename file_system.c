#include "file_system.h"
#include <stdio.h>      //printf & scanf
#include <string.h>     //strcmp
#include <time.h>       //time
#include <stdlib.h>     //malloc
#include <unistd.h>     //symlink

Directory* current_dir;

//this will handle user metadata 
typedef struct {
    char username[20];
    char group[20];
} User;

User users[10];
int user_count = 0;

void add_user(const char *username, const char *group);
void switch_user(const char *username);


char current_user[20] = "admin";
char current_group[20] = "admin";

void file_simulation()
{
    //add admin user automatically.
    add_user("admin", "admin");

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

int search_file(char *file) {
    for (int i = 0; i < current_dir->file_count; i++) {
        if (current_dir->files[i]->exists && strcmp(current_dir->files[i]->filename, file) == 0) {
            return i;
        }
    }
    return -1;
}


void create_file(char *file) {
    // Check if file already exists in current directory
    for (int i = 0; i < current_dir->file_count; i++) {
        if (current_dir->files[i]->exists && strcmp(current_dir->files[i]->filename, file) == 0) {
            printf("%s already exists in %s\n", file, current_dir->dirname);
            return;
        }
    }

    if (current_dir->file_count >= 50) {
        printf("Cannot create more files in '%s'\n", current_dir->dirname);
        return;
    }

    File *new_file = (File*)malloc(sizeof(File));
    if (new_file == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    strcpy(new_file->filename, file);
    new_file->creation_date = time(NULL);
    strcpy(new_file->permissions, "rw-rw-r");
    strcpy(new_file->owner, current_user);
    strcpy(new_file->group, current_group);
    new_file->exists = 1;
    new_file->size = 0;
    new_file->content[0] = '\0';

    current_dir->files[current_dir->file_count++] = new_file;

    printf("%s has been created by %s in %s\n", file, current_user, current_dir->dirname);
}


//this function is made to check if the user has certain permission whicvh will be used in read_file, write_file, and delete_file functions. 
int has_permission(File *file, const char *mode) {
    int index = 6; //this is the default number for "other"

    if (strcmp(current_user, file->owner) == 0) {
        index = 0;
    } else if (strcmp(current_group, file->group) == 0) {
        index = 3;
    }

    if(strcmp(mode, "r") == 0 && file->permissions[index] == 'r') {
        return 1;
    }
    if(strcmp(mode, "w") == 0 && file->permissions[index +1 ] == 'w') {
        return 1;
    }
    if(strcmp(mode, "x") == 0 && file->permissions[index + 2 ] == 'x') {
        return 1;
    }

    return 0; //if it returns 0 then this means permission denied.
    

}

void write_file(char *file, char *content) {
    file = resolve_symlink(file);
    int index = search_file(file);
    if (index >= 0) {
        File *target = current_dir->files[index];

        if (!has_permission(target, "w")) {
            printf("Permission denied: Cannot write to %s\n", file);
            return;
        }

        strcpy(target->content, content);
        target->size = strlen(content);
        printf("Wrote to %s\n", file);
        return;
    }
    printf("%s does not exist in %s\n", file, current_dir->dirname);
}


void read_file(char *file) {
    file = resolve_symlink(file);
    int index = search_file(file);
    if (index >= 0) {
        File *target = current_dir->files[index];

        if (!has_permission(target, "r")) {
            printf("Permission denied: Cannot read %s\n", file);
            return;
        }

        printf("Reading file: %s\n", file);
        if (strlen(target->content) == 0)
            printf("File is empty.\n");
        else
            printf("%s\n", target->content);
        return;
    }
    printf("%s does not exist in %s\n", file, current_dir->dirname);
}


void delete_file(char *file) {
    file = resolve_symlink(file);
    int index = search_file(file);
    if (index >= 0) {
        File *target = current_dir->files[index];

        if (!has_permission(target, "w")) {
            printf("Permission denied: Cannot delete %s\n", file);
            return;
        }

        target->exists = 0;
        printf("%s has been deleted from %s\n", file, current_dir->dirname);
        return;
    }
    printf("%s does not exist in %s\n", file, current_dir->dirname);
}


void list_contents() {
    printf("Listing contents in %s:\n", current_dir->dirname);

    if (current_dir->subdir_count == 0) {
        printf("No directories.\n");
    } else {
        printf("Directories:\n");
        for (int i = 0; i < current_dir->subdir_count; i++) {
            if (current_dir->subdirs[i]->exists) {
                printf("- %s/\n", current_dir->subdirs[i]->dirname);
            }
        }
    }

    if (current_dir->file_count == 0) {
        printf("No files.\n");
    } else {
        printf("Files:\n");
        for (int i = 0; i < current_dir->file_count; i++) {
            if (current_dir->files[i]->exists) {
                printf("- %s\n", current_dir->files[i]->filename);
            }
        }
    }

    if (current_dir->symlink_count == 0) {
        printf("No symlinks.\n");
    } else {
        printf("Symlinks:\n");
        for (int i = 0; i < current_dir->symlink_count; i++) {
            if (current_dir->symlinks[i]->exists) {
                printf("- %s -> %s\n", current_dir->symlinks[i]->linkname, current_dir->symlinks[i]->target);
            }
        }
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
void delete_directory(const char *name) {
    int index = search_directory(current_dir, name);

    if (index == -1) {
        printf("Directory '%s' not found.\n", name);
        return;
    }

    Directory* dir_to_delete = current_dir->subdirs[index];

    //deletes all files in this directory
    for (int i = 0; i < dir_to_delete->file_count; i++) {
        if (dir_to_delete->files[i]->exists) {
            free(dir_to_delete->files[i]);
        }
    }
    dir_to_delete->file_count = 0;

    //recursively delete all subdirectories
    for (int i = 0; i < dir_to_delete->subdir_count; i++) {
        if (dir_to_delete->subdirs[i]->exists) {
            delete_directory(dir_to_delete->subdirs[i]->dirname);
        }
    }
    dir_to_delete->subdir_count = 0;

    free(dir_to_delete);

    //shift the remaining subdirectories to fill the gap
    for (int i = index; i < current_dir->subdir_count - 1; i++) {
        current_dir->subdirs[i] = current_dir->subdirs[i + 1];
    }
    current_dir->subdir_count--;

    printf("Directory '%s' has been deleted.\n", name);
}

void switchdir(const char *name) {
    //checking for ..
    if (strcmp(name, "..") == 0) {
        if (current_dir->parent != NULL) {
            current_dir = current_dir->parent;
            printf("Switched to parent directory: %s\n", current_dir->dirname);
        } else {
            printf("Already at the root directory.\n");
        }
        return;
    }

    //checking if it's a symlink
    for (int i = 0; i < current_dir->symlink_count; i++) {
        if (current_dir->symlinks[i]->exists && strcmp(current_dir->symlinks[i]->linkname, name) == 0) {
            char *target = current_dir->symlinks[i]->target;
            
            int index = search_directory(current_dir, target);
            if (index != -1) {
                current_dir = current_dir->subdirs[index];
                printf("Switched to symlinked directory: %s -> %s\n", name, target);
                return;
            } else {
                printf("Target directory '%s' not found.\n", target);
                return;
            }
        }
    }

    int index = search_directory(current_dir, name);
    if (index == -1) {
        printf("Directory '%s' not found in '%s'\n", name, current_dir->dirname);
        return;
    }

    current_dir = current_dir->subdirs[index];
    printf("Switched to directory: %s\n", current_dir->dirname);
}

void create_symlink(char *linkname, char *target) {
    if (current_dir->symlink_count >= 20) {
        printf("Cannot create more symlinks in '%s'\n", current_dir->dirname);
        return;
    }

    SymLink* new_link = (SymLink*)malloc(sizeof(SymLink));
    if (new_link == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    strcpy(new_link->linkname, linkname);
    strcpy(new_link->target, target);
    new_link->exists = 1;

    current_dir->symlinks[current_dir->symlink_count++] = new_link;

    printf("Symlink '%s' -> '%s' created successfully.\n", linkname, target);
}

char* resolve_symlink(char *name) {
    for (int i = 0; i < current_dir->symlink_count; i++) {
        if (current_dir->symlinks[i]->exists && strcmp(current_dir->symlinks[i]->linkname, name) == 0) {
            return current_dir->symlinks[i]->target;
        }
    }
    return name;  //not a symlink, return original name
}

void delete_symlink(char *linkname) {
    for (int i = 0; i < current_dir->symlink_count; i++) {
        if (current_dir->symlinks[i]->exists && strcmp(current_dir->symlinks[i]->linkname, linkname) == 0) {
            free(current_dir->symlinks[i]);
            for (int j = i; j < current_dir->symlink_count - 1; j++) {
                current_dir->symlinks[j] = current_dir->symlinks[j+1];
            }
            current_dir->symlink_count--;
            printf("Symlink '%s' has been deleted.\n", linkname);
            return;
        }
    }
    printf("Symlink '%s' not found.\n", linkname);
}

void add_user(const char *username, const char *group) {
    if (user_count < 10) {
        strcpy(users[user_count].username, username);
        strcpy(users[user_count].group, group);
        printf("User %s added to group %s\n", username, group);
        user_count++;
    }
}

void switch_user(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            strcpy(current_user, users[i].username);
            strcpy(current_group, users[i].group);
            printf("Switched to user %s (group: %s)\n", current_user, current_group);
            return;
        }
    }
    printf("User not found\n");
}

void* simulate_process(void *arg) {
    char *file = (char *)arg;
    printf("Process is writing to file %s\n", file);
    write_file(file, "Simulated concurrent write.\n");
    return NULL;
}

void list_users() {
    printf("List of users:\n");
    if (user_count == 0) {
        printf("No users found.\n");
        return;
    }

    for (int i = 0; i < user_count; i++) {
        printf("- Username: %s | Group: %s\n", users[i].username, users[i].group);
    }
}

