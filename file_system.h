#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

/*create list_contents, create directory, delete director, change directory 
void list_contents(void);
void create_directory(const char *name);
void delete_directory(const char *name);
void change_directory(const char *name);    */

void file_simulation();
int search_file(char *file);  
void create_file(char *file);
void write_file(char *file, char *content);
void read_file(char *file);
void delete_file(char *file);

#endif
