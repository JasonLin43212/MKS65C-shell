#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>

#include "shell.h"

/*
  Arguments: The print_execute_error takes in an integer that represents the status
  (errno) and a string of the command name
  Return Values: No return value
  Purpose: To print out error messages if the command is not found or the errno messages
  based on the status
 */
void print_execute_error(int status, char * cmd_name){
  if (status){
    if (status == 2){
      printf("Cannot find command: %s\n",cmd_name);
      exit(1);
    }
    //Some commands give errno 256 or 512 which are unknown
    else if (status != 256 && status != 512) {
      printf("%d: %s\n",status,strerror(status));
      exit(1);
    }
  }
}

/*
  Arguments: The function get_num_args takes in a string array
  Return Values: Returns the number of elements in that array
  Purpose: To count the number of args in the input
 */
int get_num_args(char ** args){
  int output = 0;
  while(*args){
    output++;
    args++;
  }
  return output;
}

/*
  Arguments: The function has_special takes in an array of args (strings), an array for
  storing the args of the first command, a pointer to a string that stores the special
  character, and an array for storing the args of the second command/part
  Return Values: Returns 0 if there is no special characters and 1 if there is
  Purpose: (1) To know if there are special characters in the arguments
  (2) To separate the arguments into an array with only the args necessary for the
  first command, a string to know which special character (ie >,>>,<,|) was in the
  command, and an array with args necessary for the second command/part
 */
int has_special(char ** args, char ** first_args,
                char ** special, char ** second_args ){
  char * match = "<<>>|";
  int arg_num = 0;
  int inner_index = 0;
  int return_val = 0;
  while (*args){
    // Checks if the arg is any of the special characters
    if (strstr(match,*args)){
      *special = *args;
      inner_index = 0;
      arg_num++;
      return_val = 1;
    }
    else {
      if (arg_num){
        second_args[inner_index] = *args;
      }
      else {
        first_args[inner_index] = *args;
      }
      inner_index++;
    }
    args++;
  }
  return return_val;
}

/*
  Arguments: The function run_command takes in a string array (char **) of the arguments
  Return Values: Returns the errno after executing the command
  Purpose: To use the arguments to run the command, but if the command fails, it will
  return the errno for processing by the print_execute_error function.
 */
int run_command(char ** args){
  int child_pid = fork();
  if (child_pid == -1){
    printf("Failed to fork child.\n");
    exit(1);
  }

  //Parent
  if (child_pid){
    int status = 0;
    wait(&status);
    return status;
  }
  //Child
  else {
    execvp(args[0],args);
    return errno;
  }
}

/*
  Arguments: The execute_special command takes in an array of args for the first command,
  the string of the special character, and the array of args for the second command.
  Return Value: Returns the errno after execution
  Purpose: To handle the different cases of special characters (>,>>,<,|) and to return
  the appropriate errno if an error occurs.
 */
int execute_special(char ** first, char * special, char ** second){
  // Overwrite into file
  if (strcmp(special,">") == 0){
    int fd = open(second[0], O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if (fd == -1){
      printf("Cannot find file or directory: %s\n",second[0]);
      return 0;
    }
    int copy = dup(1);
    dup2(fd,1);
    int status = run_command(first);
    dup2(copy,1);
    close(fd);
    return status;
  }
  // Append into file
  else if (strcmp(special,">>") == 0){
    int fd = open(second[0], O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (fd == -1){
      printf("Cannot find file or directory: %s\n",second[0]);
      return 0;
    }
    int copy = dup(1);
    dup2(fd,1);
    int status = run_command(first);
    dup2(copy,1);
    close(fd);
    return status;
  }
  // Put file contents into stdin
  else if (strcmp(special,"<") == 0){
    int fd = open(second[0], O_RDONLY);
    if (fd == -1){
      printf("Cannot find file or directory: %s\n",second[0]);
      return 0;
    }
    int copy = dup(0);
    dup2(fd,0);
    int status = run_command(first);
    dup2(copy,0);
    close(fd);
    return status;
  }
  else if (strcmp(special,"|") == 0){
    int pipe_fd[2];
    // 0: Read, 1: Write
    pipe(pipe_fd);
    int in = dup(0);
    int out = dup(1);

    int child_pid = fork();

    // First Child
    if (child_pid == 0){
      child_pid = fork();

      // Second child
      if (child_pid == 0){
        dup2(pipe_fd[1],1);
        close(pipe_fd[1]);
        close(pipe_fd[0]);
        execvp(first[0],first);
        return errno;
      }
      //First child
      else{
        wait(NULL);
        dup2(pipe_fd[0],0);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execvp(second[0],second);
        return errno*-1;
      }
    }
    // Parent
    else {
      int status = 0;
      close(pipe_fd[0]);
      close(pipe_fd[1]);
      wait(&status);
      dup2(in,0);
      dup2(out,1);
    }

  }
  return 0;
}

/*
  Arguments: The function execute_args takes in an array of strings (char **)
  Return Values: Returns 1
  Purpose: Given an array of args, it executes it. The two cases are if it has
  special characters or if it doesn't. It also handles the cases where the commands
  do not exist and prints the appropriate error messages.
 */
int execute_args(char ** args){
  int num_args = get_num_args(args);
  char ** first_arg = malloc(num_args*sizeof(char *));
  char * special;
  char ** second_arg = malloc(num_args*sizeof(char *));

  // Checks if the args contain special characters
  if (has_special(args,first_arg,&special,second_arg)){
    int status = execute_special(first_arg,special,second_arg);
    if (status < 0){
      print_execute_error(-1*status,second_arg[0]);
    }
    else if (status > 0){
      print_execute_error(status,first_arg[0]);
    }

  }
  //Normal Command without redirection or piping
  else {
    int status = run_command(args);
    print_execute_error(status,args[0]);
  }
  return 1;
}
