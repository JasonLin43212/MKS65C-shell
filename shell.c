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

void check_error() {
  if (errno) {
    printf("Error #%d: %s",errno,strerror(errno));
    exit(1);
  }
}

//for testing
void print_list(char ** list){
  printf("[");
  while(*list){
    printf("%s,",*list);
    list++;
  }
  printf("\b]\n");
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
  output[num_args] = NULL;

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

void change_directory_display(char output[]){
  char path[4096] = "";
  if (getcwd(path,4096) == NULL){
    printf("Cannot get current working directory.\n");
    exit(1);
  }

  char * username = getpwuid(getuid())->pw_name;
  char home[4096] = "/home/";
  strcat(home,username);
  if (strstr(path,home)){
    strcpy(output,"~");
    strcat(output,path+strlen(home));
  }
  else{
    strcpy(output,path);
  }
}

int main(){
  char * input;
  char ** args;
  int status = 1;
  char * username = getpwuid(getuid())->pw_name;
  char host_name[64];
  gethostname(host_name,64);

  char current_directory[4097] = "";
  change_directory_display(current_directory);

  while(status){
    printf("%s@%s:%s$ ",username,host_name,current_directory);
    input = read_input();
    char * cur_input = input;
    strsep(&input,";");
    while (cur_input){
      //printf("Your input now is cur:%s or input:%s\n",cur_input,input);
      args = parse_args(cur_input);
      //print_list(args);
      //Checks if the user is trying to change directory
      if (strcmp(args[0],"cd") == 0){
        //User has no args or "~"
        if (args[1] == NULL || strcmp(args[1],"~") == 0){
          char home[4096] = "/home/";
          strcat(home,username);
          chdir(home);
        }
        //User puts in one arg
        else if (args[2] == NULL){
          if (chdir(args[1]) == -1){
            printf("cd: %s: No such file or directory\n",args[1]);
          }
        }
        //User puts in more than one arg
        else {
          printf("cd: Too many arguments\n");
        }
        //Change directory that is displayed
        change_directory_display(current_directory);
      }
      else if (strcmp(args[0],"exit") == 0){
        exit(0);
      }
      else{
        status = execute_args(args);
      }
      cur_input = input;
      strsep(&input,";");
    }
  }
  return 0;
}
