# MKS65C-shell
By Jason Lin, Period 10
## What Is Implemented
* Allow for user input
* Remove unnecessary whitespace from input
* Execute a single command
* Execute commands separated by semicolons
* Exit from shell
* Change Directories
* Display username, hostname, and current working directory in correct format
* Redirecting stdin and stdout with >, >>, and <
* Piping
* Printing out error messages in case file does not exist or other errors (most of the time)
## Attempted But Unsuccessful Features
* Reading the user's input as it is being typed out.
## Bugs
* If there is a file with a list of commands and the contents of this file are redirected to the stdin of `./a.out`, then the correct output will print but there will be a big block of the username, hostname and current working directory.
## Every Function Header
```C
void print_list(char **);

char * read_input();

void change_directory_display(char []);

int sep_special(char *,char ***, char *, int *);

char ** parse_args(char *);

int has_special(char **, char **, char **, char **);

int execute_special(char **, char *, char **);

int get_num_args(char **);

int execute_args(char **);

void print_execute_error(int, char *);

int run_command(char **);

```
