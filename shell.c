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
  Arguments: The print_list function takes in a char ** (aka an array of strings).
  Return Value(s): No return value.
  Purpose: Prints out the contents of a string array. This is used to test what
  my program actually sees when I input something into it.
*/
void print_list(char ** list){
  printf("[");
  while(*list){
    printf("%s,",*list);
    list++;
  }
  //Removes the last comma and replaces it with a right bracket.
  printf("\b]\n");
}

/*
  Arguments: The read_input function does not take in any arguments.
  Return Value(s): Returns a char * (a string)
  Purpose: When this function is called, it waits for the user to input something.
  Once the user pressed <ENTER>, it will return everything that the user inputs as a string.
*/
char * read_input(){
  int max_input_size = 2048;
  int cur_index = 0;
  char * input = malloc(max_input_size*sizeof(char));

  int cur_char;

  while(1){
    cur_char = getchar();
    //User presses ENTER
    if (cur_char=='\n'){
      input[cur_index] = '\0';
      return input;
    }
    //User doesn't press enter
    else {
      input[cur_index] = cur_char;
    }
    cur_index++;
    //Expand the size of the accumulator as necessary
    if (cur_index >= max_input_size){
      input = realloc(input,2*max_input_size);
      if (errno){
        printf("Error #%d: %s",errno,strerror(errno));
        exit(1);
      }
    }
  }
}

/*
  Arguments: The function change_directory_display takes in a char []
  Return Value(s): No return value.
  Purpose: Puts the current working directory into the char [] to be used
  outside of this function.
*/
void change_directory_display(char output[]){
  char path[4096] = "";
  if (getcwd(path,4096) == NULL){
    printf("Cannot get current working directory.\n");
    exit(1);
  }

  char * home = getenv("HOME");
  //If the current working directory is a subdirectory of the home directory
  if (strstr(path,home)){
    strcpy(output,"~");
    strcat(output,path+strlen(home));
  }
  // Current working directory not in home directory
  else{
    strcpy(output,path);
  }
}

/*
  Arguments: No arguments
  Return Value: Returns 0
  Purpose: Responsible for displaying the username, host name, and current
  working directory. Calls the read_input function to read what the user
  types in and separates the commands by the semicolon. It calls the parse_args
  function to separate the arguments into an array. After, it checks if the user
  called cd or exit and executes those commands accordingly. Else, it calls
  the execute_args command using the parsed arguments.
 */
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
    if (isatty(0)){
      printf("%s@%s:%s$ ",username,host_name,current_directory);
    }
    //printf("$ ");
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
            if (errno == 2){
              printf("cd: %s: No such file or directory\n",args[1]);
            }
            else {
              printf("cd: %s\n",strerror(errno));
            }

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
        //printf("Shell Exited\n");
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
