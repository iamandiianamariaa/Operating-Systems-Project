  #include <sys/types.h>
  #include <sys/stat.h>
  #include <sys/wait.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include <dirent.h>
  #include <stdbool.h>
  #include <fcntl.h>
  #include <errno.h>


  // check if it is the same inode on the same device
  #define SAME_INODE(a, b) ((a).st_ino == (b).st_ino && (a).st_dev == (b).st_dev)


//declaratii functii

  int lsh_cd(char **args);
  int lsh_help(char **args);
  int lsh_exit(char **args);
  int lsh_makedir(char **args);
  int lsh_rmdir(char **args);
  int lsh_cp(char **argv);
  int lsh_mv(char **argv);
  int lsh_echo(char **args);
  int lsh_pwd();
  int lsh_cat(char **args);



  bool isDirectoryEmpty(char *dirname);                   // checks if a directory is empty
  char *get_current_dir_name(void);                       // gets directory name


  char *builtin_str[] = {
    "cd",                                     
    "help",
    "exit",
    "mkdir",
    "rmdir",
    "cp",
    "mv",
    "echo",
    "pwd",
    "cat"
  };

  int (*builtin_func[]) (char **) = {         //array of pointers to functions
    &lsh_cd,                                  // fucntions that execute our commands
    &lsh_help,
    &lsh_exit,
    &lsh_makedir,
    &lsh_rmdir,
    &lsh_cp,
    &lsh_mv,
    &lsh_echo,
    &lsh_pwd,
    &lsh_cat
  };

  int lsh_num_builtins() {                             // size
    return sizeof(builtin_str) / sizeof(char *);
  }

  int lsh_echo(char **args)
  {
    if (args[1]==NULL) {
      printf("Error: expected argument to echo \n");
      return 1;
    }
    else {
      printf("%s\n", args[1]);
    }
    return 1;
  }

  int lsh_pwd()
  {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));       //returneaza un string cu numele directorului curent
    puts("Path info: ");
    printf("\tWorkdir: %s\n", cwd);
    return 1;
  }

  int lsh_cat(char **args)
  {
    if(args[1]==NULL)
      {
        printf("Error: expected argument to cat \n");
        return 1;
      }
    else {
      int n=1;                   
      while(args[n]!=NULL){
      FILE*fptr = fopen(args[n], "r"); 
      if (fptr ==NULL) 
      { 
          printf("Cannot open file \n"); 
          n++;
          return 1;
      } 
      char c = fgetc(fptr); 
      while ((c = fgetc(fptr))!= EOF)                  //citeste caracter cu caracter si afiseaza continutul fisierului
      { 
          printf ("%c", c); 
      } 
    
      fclose(fptr); 
      n++;
     } 
    }
    return 1;
  }

  int lsh_cd(char **args)
  {


  char command[50];                       // size of command
  char* cwd;                              // a pointer to find out the current directory
  char buff[PATH_MAX + 1];

  if (args[1] == NULL) {                // if no argument is given to cd, it returns an error with an example statement
          printf("Error: expected argument to cd e.g.  'cd Desktop' etc\n ");        // serves as an example
          free(cwd);      // frees the pointer we created previously
          return 1;       // returns to our shell
  }else if(strcmp(args[1],"..")==0){   // checks if argv[1] == ".." using strcmp
          chdir(".."); //changes directory, goes a directory back
          
         
         
    }else if(chdir(args[1]) != 0) {                       //checks if the directory mentioned exists or not!
               printf("Error: directory does not exist!\n");  // prints error if directory aforementioned exists or not.
         }
             
      

      
      cwd = get_current_dir_name();   // gets current directory name
      if( cwd != NULL ) {             //checks if current directory exists
          printf( "My working directory is: \n" ); // prints the message before command line argument
          sprintf(command, cwd);   // give command string a value using sprintf.
          printf("%s",cwd);                        // prints the directory path right before the argument '>'
          chdir(command);                          //executes the value that we passed on to the command string
      }
    free(cwd);                    // frees the pointer
    return 1;                     // return to our shell
  }

  int lsh_cp(char **argv)       //copiaza continutul unui fisier in alt fisier
  {
      char buffer[1024];              //cp source destination
      int files[2];
      ssize_t count;

      if(argv[2] == NULL){                                                        // checks if two arguments was provided     
        printf("Error: expected argument to cp\n");               // if arguments was not provided, prints the following error message
        return -1;                                                               // return to the program
       }
       
      files[0] = open(argv[1], O_RDONLY);                               //open the first file argument with read-only
      if (files[0] == -1){  
          printf("cannot open %s with permission", argv[1]);
          return -1;
      }
      files[1] = open(argv[2], O_WRONLY);                                //open the second file argument with write-only
      if (files[1] == -1)                                               
      {   
          printf("cannot open %s with permission", argv[2]);
          close(files[0]);
          return -1;
      }

      while ((count = read(files[0], buffer, sizeof(buffer))) != 0){          //write the content of first file to second file 
          write(files[1], buffer, count);
      }
      
      printf("success cp\n");
      return 1;
  }

  int lsh_mv(char **argv)         //copiaza continutul primului in al doilea si il sterge pe primul
  {

    struct stat statbuf_src, statbuf_dest;
    char *src, *dest, *new_src, *new_dest;
    char *current_directory;

    if(argv[2] == NULL){                                                        // checks if two arguments was provided     
        printf("Error: expected argument to mv\n");               // if arguments was not provided, prints the following error message
        return -1;                                                               // return to the program
   }
                                                                                  
    src = (char*)malloc(strlen(argv[1]) + 1);                                    //allocate memory space and make a copy of two files
    dest = (char*)malloc(strlen(argv[2]) + 1);
    strcpy(src, argv[1]);
    strcpy(dest, argv[2]);

    stat(src, &statbuf_src);                        //returns information about a file, in the buffer pointed to by st  
    stat(dest, &statbuf_dest);

                                                                             // check if two arguments is a file or directory 
    printf("\"%s\" is a ", src);
    if (S_ISREG(statbuf_src.st_mode)) {
      puts("a regular file");
    }
    if (S_ISDIR(statbuf_src.st_mode)) {
      puts("a directory");
    }

    printf("\"%s\" is a ", dest);
    if (S_ISREG(statbuf_dest.st_mode)) {
      puts("a regular file");
    }
    if (S_ISDIR(statbuf_dest.st_mode)) {
      puts("a directory");
    }

    if (SAME_INODE(statbuf_dest, statbuf_src)) {                                     //check if two arguments are the same file 
      printf("%s and %s are identical\n", src, dest);
    }
    
    
    current_directory = getenv("HOME");                                               //get the current postion directory
    
    printf("current directory is \"%s\"\n", current_directory);

    
    new_src =(char*) malloc(strlen(src) + 1 + strlen(current_directory) + 1);        //set the directory of first file 
    strcpy(new_src,current_directory);
    strcat(new_src,"/");
    strcat(new_src,src);
    

    new_dest = (char*)malloc(strlen(dest) + 1 + strlen(current_directory) + 1);              //set the directory of second file 
    strcpy(new_dest,current_directory);
    strcat(new_dest,"/");
    strcat(new_dest,dest);
    

    if(rename(new_src,new_dest) != 0){                                                      //change the name of file2 to file 1 
      fprintf(stderr,"rename failed with error %s\n",strerror(errno));
    }

    free(new_src);
    free(new_dest);
    free(src);
    free(dest);
    return 1;

  }


  int lsh_rmdir(char **args){                                              // remove directory command

  int n=1;                                                                       // initializes n that will be used later in the while lopp to check arguments starts with one as args[0] is mkdir
   
                                   
       if(args[1] == NULL){                                                        // checks if argument was provided     
       printf("Error: expected argument to rmdir e.g. rmdir a\n");                   // if argument was not provided, prints the following error message
        return 1;                                                               // return to the program
       }


  while(args[n]!=NULL){

         if(isDirectoryEmpty(args[n])==false){    // checks if the directry we wish to remove is empty or not
            printf("Directory is not empty!\n");  // prints the following message.
            return 1;                             // returns to shell, program continues
         }

         if(chdir(args[n]) == 0 && isDirectoryEmpty(args[n]) == true){          // if directory exists, gets into it!
            printf("Directory %s exists!\n", args[n]);// prints directory          
            printf("SUCCESS! Directory %s removed!\n",args[n]);   // prints that the requested directory has been removed
            chdir(".."); // gets out of directory to remain in previous directory
            rmdir(args[n]);  // removes the directory
            n++;    // checks other arguments  so we can remove multiple directories at once
            continue;   // back to the while loop!
         }
   
         if(chdir(args[n]) != 0){           // if the directory does not exist
         
            printf(" %s Directory does not exist!\n",args[n]);   // prints the following message
            n++;    // goes to the next following request
         } 
     } 
      

     return 1;               // program continues

  }



  int lsh_makedir(char **args){                                                  // make directory command

  int n=1;                                                                       // initializes n that will be used later in the while lopp to check arguments starts with one as args[0] is mkdir
   
                                   
    if(args[1] == NULL){                                                        // checks if argument was provided     
        printf("Error: expected argument to mkdir\n");                   // if argument was not provided, prints the following error message
        return 1;                                                               // return to the program
    }


  while(args[n]!=NULL){

      if(chdir(args[n]) == 0){          // if directory exists, gets into it!
         printf("Directory %s already exists!\n", args[n]); // prints directory!
         n++;
         chdir(".."); // gets out of directory to remain in previous directory
         continue;   // back to the while loop!
      }

      if(chdir(args[n]) != 0){           // if the directory does not exist
         struct stat st = {0};                                                                                       
                                                                                                                     
              if (stat(args[n], &st) == -1) {         //returns information about a file, in the buffer pointed to by st                                                            
                mkdir(args[n], ACCESSPERMS); //accessperms grants all access rights to everyone                      
              }                                                                                                 
     
         printf("SUCCESS! %s Directory is created!\n",args[n]); // prints a success messgae
         n++;  // follows up to the next argument. 
      } 
  } 
      
  return 1;              
  }

  int lsh_help(char **args)                                    // help function prints out all our commands to help the user
  {
    int i;
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    printf("the num size : %d\n", lsh_num_builtins());
    for (i = 0; i < lsh_num_builtins(); i++) {
      printf("  %s\n", builtin_str[i]);
    }

    printf("Use the 'man' command for information on other programs.\n");  
    return 1;
  }

  int lsh_exit(char **args)  //shut program
  {
    return 0;
  }

  int lsh_launch(char **args)
  {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
      // Child process
      if (execvp(args[0], args) == -1) {
        perror("Error");
      }
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      // Error forking
      perror("Error");
    } else {
      // Parent process
      do {
        wpid = waitpid(pid, &status, WUNTRACED);         //suspends execution of the calling process until a child specified by pid argument has changed state
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));   //WIFEXITED returns true if the child terminated normally, that is, by calling exit, or by returning from main().
    }                                                         //returns true if the child process was terminated by a signal.

    return 1;
  }

  int lsh_execute(char **args)
  {
    int i;

    if (args[0] == NULL) {
      // An empty command was entered.
      return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++) {
      if (strcmp(args[0], builtin_str[i]) == 0) {
        return (*builtin_func[i])(args);
      }
    }

    return lsh_launch(args);
  }

  #define LSH_RL_BUFSIZE 1024
  
  char *lsh_read_line(void)
  {
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = (char*)malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
      fprintf(stderr, "Error: allocation error\n");
      exit(EXIT_FAILURE);
    }

    while (1) {
      // Read a character
      c = getchar();

      // If we hit EOF, replace it with a null character and return.
      if (c == EOF || c == '\n') {
        buffer[position] = '\0';
        return buffer;
      } else {
        buffer[position] = c;
      }
      position++;

      // If we have exceeded the buffer, reallocate.
      if (position >= bufsize) {
        bufsize += LSH_RL_BUFSIZE;
        buffer = (char*)realloc(buffer, bufsize);
        if (!buffer) {
          fprintf(stderr, "Error: allocation error\n");
          exit(EXIT_FAILURE);
        }
      }
    }
  }

  #define LSH_TOK_BUFSIZE 64
  #define LSH_TOK_DELIM " \t\r\n\a"

  char **lsh_split_line(char *line)
  {
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = (char**)malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
      printf("Error: allocation error\n");
      exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
      tokens[position] = token;
      position++;

      if (position >= bufsize) {
        bufsize += LSH_TOK_BUFSIZE;
        tokens = (char**)realloc(tokens, bufsize * sizeof(char*));
        if (!tokens) {
          printf("Error: allocation error\n");
          exit(EXIT_FAILURE);
        }
      }

      token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
  }


  void lsh_loop(void)
  {
    char *line;
    char **args;
    int status;

    chdir("..");
    chdir("..");
    printf("\e[1;1H\e[2J");                                // clears screen for the user
    printf("The commands we have worked on are cd, exit, mkdir, rmdir and help.\n\n");
    printf("CURRENT DIRECTORY:\n");             
    system("ls");                        // prints the contents to the current directory
    do {
      printf("> ");
      line = lsh_read_line();
      args = lsh_split_line(line);
      status = lsh_execute(args);

      free(line);
      free(args);
    } while (status);
  }

  bool isDirectoryEmpty(char *dirname) {   //If its an empty directory, readdir will stop after the entries '.' and '..' empty if n<=2. If its empty or doesn't exist, it should return 1, else return 0
    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(dirname);
    if (dir == NULL) //Not a directory or doesn't exist
      return 1;
    while ((d = readdir(dir)) != NULL) {         //numara cate directoare sunt in directorul dat
      if(++n > 2)
        break;
    }
    closedir(dir);
    if (n <= 2) //Directory Empty
      
      return true;
    else
      return false;
  }

  int main(int argc, char **argv)
  {
    lsh_loop();
    return EXIT_SUCCESS;
  }