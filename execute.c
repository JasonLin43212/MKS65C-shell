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

#include "execute.h"
#include "shell.h"

int get_num_args(char ** args){
  int output = 0;
  while(*args){
    output++;
    args++;
  }
  return output;
}

int has_special(char ** args, char ** first_args,
                char ** special, char ** second_args ){
  char * match = "<<>>|";
  int arg_num = 0;
  int inner_index = 0;
  int return_val = 0;
  while (*args){
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

void run_command(char ** args){
  int child_pid = fork();
  if (child_pid == -1){
    printf("Failed to fork child.\n");
    exit(1);
  }

  //Parent
  if (child_pid){
    int status = 0;
    wait(&status);
  }
  //Command
  else {
    execvp(args[0],args);
    printf("Cannot find command: %s\n",args[0]);
    exit(2);
  }
}

void execute_special(char ** first, char * special, char ** second){
  // Overwrite into file
  if (strcmp(special,">") == 0){
    int copy = dup(1);
    int fd = open(second[0], O_WRONLY | O_CREAT, 0666);
    dup2(fd,1);
    run_command(first);
    dup2(copy,1);
    close(fd);
  }
  // Append into file
  else if (strcmp(special,">>") == 0){
    int copy = dup(1);
    int fd = open(second[0], O_WRONLY | O_APPEND | O_CREAT, 0666);
    dup2(fd,1);
    run_command(first);
    dup2(copy,1);
    close(fd);
  }
  // Put file contents into stdin
  else if (strcmp(special,"<") == 0){
    int copy = dup(0);
    int fd = open(second[0], O_RDONLY);
    dup2(fd,0);
    run_command(first);
    dup2(copy,0);
    close(fd);
  }
  else if (strcmp(special,"<<") == 0){

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
      dup2(pipe_fd[1],1);
      close(pipe_fd[0]);
      close(pipe_fd[1]);
      execvp(first[0],first);
      if (errno == 2){
        printf("Cannot find command: %s\n",first[0]);
        exit(2);
      }
    }
    // Parent
    else {
      child_pid = fork();

      // Second child
      if (child_pid == 0){
        dup2(pipe_fd[0],0);
        close(pipe_fd[1]);
        close(pipe_fd[0]);
        execvp(second[0],second);
        if (errno == 2){
          printf("Cannot find command: %s\n",first[0]);
          exit(2);
        }
      }
      //Parent
      else {
        int status = 0;
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        wait(&status);

      }
    }
    dup2(in,0);
    dup2(out,1);
  }
}


int execute_args(char ** args){
  int num_args = get_num_args(args);
  char ** first_arg = malloc(num_args*sizeof(char *));
  char * special;
  char ** second_arg = malloc(num_args*sizeof(char *));

  // If the command has redirection or piping
  if (has_special(args,first_arg,&special,second_arg)){
    execute_special(first_arg,special,second_arg);
  }
  //Normal Command without redirection or piping
  else {
    run_command(args);
  }

  return 1;
}
