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
#include "execute.h"
#include "parse.h"


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

void change_directory_display(char output[]){
  char path[4096] = "";
  if (getcwd(path,4096) == NULL){
    printf("Cannot get current working directory.\n");
    exit(1);
  }

  char * home = getenv("HOME");
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

      // User enters nothing
      if (get_num_args(args) == 0){}
      //Checks if the user is trying to change directory
      else if (strcmp(args[0],"cd") == 0){
        //User has no args or "~"
        if (args[1] == NULL || strcmp(args[1],"~") == 0){
          char * home = getenv("HOME");
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

      //Exit
      else if (strcmp(args[0],"exit") == 0){
        printf("Shell Exited\n");
        exit(0);
      }

      //All Other Commands
      else{
        status = execute_args(args);
      }

      //Go to the command after the next semicolon
      cur_input = input;
      strsep(&input,";");
    }
  }
  return 0;
}
