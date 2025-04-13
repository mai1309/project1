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

/* example of what can be printed 
Welcome to File System Simulation
-----------------------------------
$> help
Available commands:
- ls       - List directory contents
- mkdir    - Create a new directory
- touch    - Create a new file
- rm       - Remove a file
- cd       - Change directory
- cat      - Display file contents
- exit     - Exit the simulation

$> ls
Directory: /
- home/
- user1.txt
- docs/

$> mkdir projects
Created directory: projects

$> ls
Directory: /
- home/
- user1.txt
- docs/
- projects/

$> cd projects
Changed directory to: projects/

$> touch readme.txt
Created file: readme.txt

$> ls
Directory: projects/
- readme.txt

$> cat readme.txt
File is empty.

$> echo "This is a project!" > readme.txt
Added content to readme.txt

$> cat readme.txt
This is a project!

$> rm readme.txt
Removed file: readme.txt

$> ls
Directory: projects/
- (empty)

$> cd ..
Changed directory to: /

$> exit
Exiting the simulation. Goodbye!
*/
void list_contents()
{

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
    if(rmdir(directory_name) == 0)
    {
        printf("Directory succesfully deleted.\n");
    }
    else
    {
        perror("rmdir failed");
    }
}