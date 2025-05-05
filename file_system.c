#include "file_system.h"
#include <stdio.h>      //printf
#include <string.h>     //strcmp
#include <time.h>       //time
#include <stdlib.h>     //malloc
#include <unistd.h>     //symlink
#include <pthread.h>
#include <signal.h>   

pthread_mutex_t fs_lock = PTHREAD_MUTEX_INITIALIZER;
int pipefd[2];  //global pipe that needs mutex

typedef struct {
    int role;        // 0 = writer, 1 = reader
    char *filename;
} thread_arg_t;


Directory* current_dir;

//this will handle user metadata 
typedef struct {
    char username[20];
    char group[20];
    Directory* root_dir;
} User;

User users[10];
int user_count = 0;

void add_user(const char *username, const char *group);
void switch_user(const char *username);


char current_user[20] = "admin";
char current_group[20] = "admin";

void handler(int sig) //Signal handler 
{
	if (sig == SIGINT) 
	{
        	printf("\n Caught SIGINT. Shutting down...\n");
        	exit(0);
    	}
}

void file_simulation()
{
    // Create a global root directory shared by all users
    Directory* global_root = (Directory*)malloc(sizeof(Directory));
    strcpy(global_root->dirname, "/");
    global_root->parent = NULL;
    global_root->subdir_count = 0;
    global_root->file_count = 0;
    global_root->symlink_count = 0;
    global_root->exists = 1;

    // Create a shared directory under the global root
    Directory* shared_dir = (Directory*)malloc(sizeof(Directory));
    strcpy(shared_dir->dirname, "shared");
    shared_dir->parent = global_root;
    shared_dir->subdir_count = 0;
    shared_dir->file_count = 0;
    shared_dir->symlink_count = 0;
    shared_dir->exists = 1;

    global_root->subdirs[global_root->subdir_count++] = shared_dir;

    // Add users and assign them to the same global root
    add_user("admin", "admin");
    users[0].root_dir = global_root;

    add_user("john", "admin");
    users[1].root_dir = global_root;

    // Start as admin
    switch_user("admin");

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
    pthread_mutex_lock(&fs_lock); //Lock before file access

    // Check if file already exists in current directory
    for (int i = 0; i < current_dir->file_count; i++) {
        if (current_dir->files[i]->exists && strcmp(current_dir->files[i]->filename, file) == 0) {
            printf("%s already exists in %s\n", file, current_dir->dirname);
            pthread_mutex_unlock(&fs_lock); // Unlock
            return;
        }
    }

    if (current_dir->file_count >= 50) {
        printf("Cannot create more files in '%s'\n", current_dir->dirname);
        pthread_mutex_unlock(&fs_lock); // Unlock
        return;
    }

    File *new_file = (File*)malloc(sizeof(File));
    if (new_file == NULL) {
        printf("Memory allocation failed.\n");
        pthread_mutex_unlock(&fs_lock); // Unlock
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

    pthread_mutex_unlock(&fs_lock); //Unlock after done
}


//this function is made to check if the user has certain permission whicvh will be used in read_file, write_file, and delete_file functions. 
int has_permission(File *file, const char *mode) {
    int index = 6; // default to 'other'

    if (strcmp(current_user, file->owner) == 0) {
        index = 0;
    } else if (strcmp(current_group, file->group) == 0) {
        index = 3;
    }

    printf("[DEBUG] Checking '%s' permission for %s. Permissions: %s, Index: %d\n", 
           mode, current_user, file->permissions, index);

    if(strcmp(mode, "r") == 0 && file->permissions[index] == 'r') {
        return 1;
    }
    if(strcmp(mode, "w") == 0 && file->permissions[index + 1] == 'w') {
        return 1;
    }
    if(strcmp(mode, "x") == 0 && file->permissions[index + 2] == 'x') {
        return 1;
    }

    return 0;
}


void write_file(char *file) {
    pthread_mutex_lock(&fs_lock);

    file = resolve_symlink(file);
    int index = search_file(file);
    if (index >= 0) {
        File *target = current_dir->files[index];

        if (!has_permission(target, "w")) {
            printf("Permission denied: Cannot write to %s\n", file);
            pthread_mutex_unlock(&fs_lock);
            return;
        }

        char content[1024];

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);

        printf("Thread is ready. Please enter content for file '%s':\n", file);

        if (fgets(content, sizeof(content), stdin) != NULL) {
            content[strcspn(content, "\n")] = '\0';
            strcpy(target->content, content);
            target->size = strlen(content);
            printf("Wrote to %s\n", file);
        } else {
            printf("Error reading input for %s\n", file);
        }
    } else {
        printf("%s does not exist in %s\n", file, current_dir->dirname);
    }

    pthread_mutex_unlock(&fs_lock);
}


void chmod_file(char *file, const char *new_perms) {
    pthread_mutex_lock(&fs_lock);

    file = resolve_symlink(file);
    int index = search_file(file);
    if (index >= 0) {
        File *target = current_dir->files[index];

        if (strlen(new_perms) != 9) {
            printf("Invalid permission format. Use something like rwxr--r--\n");
            pthread_mutex_unlock(&fs_lock);
            return;
        }

        strcpy(target->permissions, new_perms);
        printf("Permissions for %s updated to %s\n", file, new_perms);
    } else {
        printf("%s does not exist in %s\n", file, current_dir->dirname);
    }

    pthread_mutex_unlock(&fs_lock);
}



void read_file(char *file) {
    pthread_mutex_lock(&fs_lock);

    file = resolve_symlink(file);
    int index = search_file(file);
    if (index >= 0) {
        File *target = current_dir->files[index];

        if (!has_permission(target, "r")) {
            printf("Permission denied: Cannot read %s\n", file);
            pthread_mutex_unlock(&fs_lock); // Unlock before return
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

    pthread_mutex_unlock(&fs_lock); //Unlock after done
}


void delete_file(char *file) {
    pthread_mutex_lock(&fs_lock); //Lock before file access

    file = resolve_symlink(file);
    int index = search_file(file);
    if (index >= 0) {
        File *target = current_dir->files[index];

        if (!has_permission(target, "w")) {
            printf("Permission denied: Cannot delete %s\n", file);
            pthread_mutex_unlock(&fs_lock); //unlock before return
            return;
        }

        target->exists = 0;
        printf("%s has been deleted from %s\n", file, current_dir->dirname);
        pthread_mutex_unlock(&fs_lock); //Unlock after done
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
        int files_found = 0;
        for (int i = 0; i < current_dir->file_count; i++) {
            if (current_dir->files[i]->exists) {
                if (files_found == 0)
                    printf("Files:\n");
                printf("- %s\n", current_dir->files[i]->filename);
                files_found = 1;
            }
        }
        if (files_found == 0)
            printf("No files.\n");
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

        // Create user's root directory
        Directory* user_root = (Directory*)malloc(sizeof(Directory));
        if (user_root == NULL) {
            printf("Failed to initialize root directory for %s. Memory allocation error.\n", username);
            return;
        }
        strcpy(user_root->dirname, "/");
        user_root->parent = NULL;
        user_root->subdir_count = 0;
        user_root->file_count = 0;
        user_root->symlink_count = 0;
        user_root->exists = 1;

        users[user_count].root_dir = user_root;

        printf("User %s added to group %s\n", username, group);
        user_count++;
    }
}

void delete_user(const char *username, const char *group){
    int found = 0;
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].group, group) == 0)
        {
            if (strcmp(current_user, username) == 0)
            {
                printf("Cannot delete the currently logged-in user. Please switch users first.\n");
                return;
            }
            if (users[i].root_dir != NULL)
            {
                free(users[i].root_dir);
                users[i].root_dir = NULL;
            }
            for (int j = i; j < user_count - 1; j++)
            {
                users[j] = users[j + 1];
            }
            user_count--;
            found = 1;
            printf("User '%s' (group '%s') deleted successfully.\n", username, group);
            break;
        }
    }

    if (!found)
    {
        printf("User '%s' (group '%s') not found.\n", username, group);
    }
}

void switch_user(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            strcpy(current_user, users[i].username);
            strcpy(current_group, users[i].group);
            current_dir = users[i].root_dir;  // Switch to this user's root dir
            printf("Switched to user %s (group: %s)\n", current_user, current_group);
            return;
        }
    }
    printf("User not found\n");
}

void* process(void *arg) 
{
    thread_arg_t *targ = (thread_arg_t *)arg;
    char *file = targ->filename;
    char MSG[5] = {0};

    	if (targ->role == 0) //Checking if "parent process"
	{
        	write_file(file);
        	pthread_mutex_lock(&fs_lock);  //Locking before accessing and writing to global pipe
        	write(pipefd[1], "Done", 5);    //Letting child know that parent is done writing to file
        	pthread_mutex_unlock(&fs_lock);
    	} else {            //"Child process"
		int n = read(pipefd[0], MSG, 5);

    		while (n != 5) 
		{
     			n = read(pipefd[0], MSG, 5);
               		usleep(10000); // Sleep 10ms before trying again
    		}
		if (n > 0 && strcmp(MSG, "Done") == 0)   //Checking if parent is done writing to file
		{	
			read_file(file);   
		}

    	}

    free(file);
    free(targ);
    return NULL;
}

void* simulate_process(void *arg) {
    char *file = strdup((char *)arg);  // Make a safe copy
    pthread_t processes[2];
    if (file == NULL) {
        perror("Failed to duplicate filename");
        return NULL;
    }
    if (signal(SIGINT, handler) == SIG_ERR)   //Register SIGINT handler
    perror("signal error");

    if (pipe(pipefd) == -1) 
    {
        perror("Pipe failure");
        return NULL;
    }

    for (int i = 0; i < 2; i++) 
    {
        thread_arg_t *targ = malloc(sizeof(thread_arg_t));
        if (!targ) 
        {
                perror("Failed to allocate thread arg");
                continue;
        }
           targ->role = i;
        targ->filename = strdup(file);

        pthread_create(&processes[i], NULL, process, (void *)targ);   //Creating "processes" through threads
    }

    for (int i = 0; i < 2; i++)
        pthread_join(processes[i], NULL);
    close(pipefd[0]);
    close(pipefd[1]);
    printf("Simulation finished\n");
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