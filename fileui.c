#include <stdio.h>

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