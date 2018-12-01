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

void print_execute_error(int status, char * cmd_name){
  if (status){
    if (status == 2){
      printf("Cannot find command: %s\n",cmd_name);
      exit(2);
    }
    else if (status != 256 && status != 512) {
      printf("%d: %s\n",status,strerror(status));
      exit(2);
    }
  }
}

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


int execute_args(char ** args){
  int num_args = get_num_args(args);
  char ** first_arg = malloc(num_args*sizeof(char *));
  char * special;
  char ** second_arg = malloc(num_args*sizeof(char *));

  // If the command has redirection or piping
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
