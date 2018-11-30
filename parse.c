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

#include "parse.h"

int sep_special(char ** old, char *** output, char * special, int * arg_index){
  if (strstr(*old,special)){
    char * super_old = *old;
    strsep(&(*old),special);
    if (*super_old){
      (*output)[*arg_index] = super_old;
      (*arg_index)++;
    }
    (*output)[*arg_index] = special;
    (*arg_index)++;
    *old += strlen(special)-1;
    if (**old){
      (*output)[*arg_index] = *old;
      (*arg_index)++;
      *old += strlen(*old);
    }
    return 1;
  }
  return 0;
}

char ** parse_args(char * line){
  //Remove whitespace in front of input
  while(*line == ' ' || *line == '\t'){
    line++;
  }
  char ** output = malloc(10*sizeof(char *));
  int arg_index = 0;
  char * old = NULL;

  while(line){
    old = line;
    strsep(&line," ");
    int num_special = 0;
    //Checks for redirects
    num_special += sep_special(&old,&output,">>",&arg_index);
    num_special += sep_special(&old,&output,"<<",&arg_index);
    num_special += sep_special(&old,&output,">",&arg_index);
    num_special += sep_special(&old,&output,"<",&arg_index);
    num_special += sep_special(&old,&output,"|",&arg_index);
    if (num_special < 1 && *old){
      output[arg_index] = old;
      arg_index++;
    }
  }
  output[arg_index] = NULL;

  return output;
}
