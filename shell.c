#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

void check_error() {
  if (errno) {
    printf("Error #%d: %s",errno,strerror(errno));
    exit(1);
  }
}

char * read_input(){
  int max_input_size = 2048;
  int cur_index = 0;
  char * input = malloc(max_input_size*sizeof(char));
  int cur_char;
  check_error();

  while(1){
    cur_char = getchar();
    if (cur_char=='\n'){
      input[cur_index] = '\0';
      return input;
    }
    else {
      input[cur_index] = cur_char;
    }
    cur_index++;
    if (cur_index >= max_input_size){
      input = realloc(input,2*max_input_size);
      check_error();
    }
  }
}

char ** parse_args(char * line){
   char ** output = malloc(10*sizeof(char *));
  int num_args = 0;
  char * old = NULL;

  while(line){
    old = line;
    strsep(&line," ");
    output[num_args] = old;
    num_args++;
  }

  return output;
}

int execute_args(char ** args){
  int child_pid = fork();
  check_error();

  //Parent
  if (child_pid){
    int status = 0;
    wait(&status);
  }
  else {
    execvp(args[0],args);
  }
  return 1;
}

void run_bash(){
  char * input;
  char ** args;
  int status = 1;

  while(status){
    printf("$ ");
    input = read_input();
    args = parse_args(input);
    status = execute_args(args);
  }
}

int main(){

  run_bash();

  return 0;
}
