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
  Arguments: The function sep_special takes in a string that represents an
  argument from the input, a pointer to an string array from the outside, a string that
  represents the special character(s) (>,<,>>,|) and the index of the array.
  Return Values: Returns 1 if there is a special character, and 0 if there isn't one.
  Purpose: TLDR- Separates the special characters from any other args if the user does
  not uses spaces to separate the args from the special characters (Ex: ls -l>file.txt)
  Given a string, it will check if any special characters are in that string. It will
  separate the special characters from any args that are attatched to it and adds the
  args and the special character to the array of strings (array of args)
 */
int sep_special(char * str, char *** output, char * special, int * arg_index){
  //Checks if there is a special character
  if (strstr(str,special)){
    char * super_str = str;
    strsep(&str,special);
    //Adds the arg before the special character only if it was attatched
    // Ex: ls -l>file.txt  It will add "-l" into the string array
    // But for ls -l > file.txt It will not add the "-l" in this function
    if (*super_str){
      (*output)[*arg_index] = super_str;
      (*arg_index)++;
    }
    //Adds the special character to the array
    (*output)[*arg_index] = special;
    (*arg_index)++;
    str += strlen(special)-1;
    //Adds the arg after the special character only if it was attached
    // Ex.ls -l>file.txt  It will add "file.txt" into the string array
    if (*str){
      (*output)[*arg_index] = str;
      (*arg_index)++;
      str += strlen(str);
    }
    return 1;
  }
  return 0;
}


/*
  Arguments: The function parse_args takes in a string of the input
  Return Value: Returns a (char **) string array
  Purpose: Separates the string of all the arguments into an array of strings
 */
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
    //Checks for special characters and adds them into the string array
    num_special += sep_special(old,&output,">>",&arg_index);
    num_special += sep_special(old,&output,">",&arg_index);
    num_special += sep_special(old,&output,"<",&arg_index);
    num_special += sep_special(old,&output,"|",&arg_index);
    //If there was not anything special about the current arg, then just add it
    if (num_special < 1 && *old){
      output[arg_index] = old;
      arg_index++;
    }
  }
  //Null at end of array
  output[arg_index] = NULL;

  return output;
}
