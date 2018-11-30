int has_special(char **, char **, char **, char **);

int get_num_args(char **);

int execute_args(char **);

/*
  execute_args takes in a list of arguments and executes it by
  first forking to create a child process and then using
  execvp to execute that command in the child process
*/
int execute_args(char **);
