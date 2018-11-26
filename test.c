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
#include <readline/readline.h>
#include <readline/history.h>

char * read_input(){
  int max_input_size = 2048;
  int cur_index = 0;
  char * input = malloc(max_input_size*sizeof(char));

  char * cur_char = "";

  while(1){
    cur_char = readline("test>");
    printf("%s",cur_char);
    if (strcmp(cur_char,"\n")){
      input[cur_index] = '\0';
      return input;
    }
    else {
      printf("Your current char is: %s\n",cur_char);
      input[cur_index] = *cur_char;
    }
    cur_index++;
    if (cur_index >= max_input_size){
      input = realloc(input,2*max_input_size);
    }
  }
}

int main(){
  char * input;
  //char ** args;
  int status = 1;



  while(status){
    input = read_input();
    printf("The input was: %s\n",input);
  }
  return 0;
}
