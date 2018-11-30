/*
  check_error will print out the errno with error message
  when there is a change in the errno and exits the program
*/
void check_error();

/*
  print_list takes in the list of arguments and prints them out
  for testing
*/
void print_list(char **);

/*
  read_input will read in whatever the user types into the shell
  one character at a time so that it will know when they press enter to
  execute the command
*/
char * read_input();

void change_directory_display(char []);
