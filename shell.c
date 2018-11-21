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

void print_list(char ** list){
  printf("[");
  while(*list){
    printf("%s,",*list);
    list++;
  }
  printf("]\n");
}

char * read_input(){
  int max_input_size = 2048;
  int cur_index = 0;
  char * input = malloc(max_input_size*sizeof(char));

  int cur_char;

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
  //Remove whitespace in front of input
  while(*line == ' ' || *line == '\t'){
    line++;
  }
  char ** output = malloc(10*sizeof(char *));
  int num_args = 0;
  char * old = NULL;

  while(line){
    old = line;
    strsep(&line," ");
    if (*old != '\0'){
      output[num_args] = old;
      num_args++;
    }
  }

  return output;
}

int execute_args(char ** args){
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
    if (errno == 2) {
      printf("Cannot find command: %s\n",args[0]);
    }
  }
  return 1;
}


int main(){
  char * input;
  char ** args;
  int status = 1;

  while(status){
    printf("$ ");
    input = read_input();
    char * cur_input = input;
    strsep(&input,";");
    while (cur_input){
      printf("Your input now is cur:%s or input:%s\n",cur_input,input);
      args = parse_args(cur_input);
      print_list(args);
      status = execute_args(args);
      cur_input = input;
      strsep(&input,";");
    }
  }
  return 0;
}
