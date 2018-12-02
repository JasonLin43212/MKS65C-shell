void print_list(char **);

char * read_input();

void change_directory_display(char []);

int sep_special(char *,char ***, char *, int *);

char ** parse_args(char *);

int has_special(char **, char **, char **, char **);

int get_num_args(char **);

int execute_args(char **);

void print_execute_error(int, char *);
