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
    }
  }
}

void run_bash(){
  char * input;

  while(1){
    printf("$ ");
    input = read_input();
    printf("This is input: %s\n",input);
  }
}

int main(){

  run_bash();

  return 0;
}
