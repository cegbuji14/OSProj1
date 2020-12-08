#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = "$ \t\r\n";
extern char **environ;


char *trimwhitespace(char *str){
  char *end;  
  // Trim leading space 
  while(isspace((unsigned char)*str)){ 
    str++; 
  }
  if(*str == 0){  // All spaces?   
    return str;
  }
  // Trim trailing space
  end = str + strlen(str) - 1;  
  while(end > str && isspace((unsigned char)*end)){
    end--;
  }
  // Write new null terminator character
  end[1] = '\0';
  
  return str;
}

char *myGetCWD(){
  char *cwd;
  char *buf;
  int size;
  
  size = pathconf(".", _PC_PATH_MAX);
  buf = (char *) malloc((size_t)size);
  
  cwd = getcwd(buf, (size_t)size);
  
  return cwd;
}

static void run_getenv(const char* name){
  char* value;
  value = getenv(name);
  if (!value){
    printf("'%s' is not set.\n", name);
  }
  else {
    printf("%s = %s\n", name, value);
  }
}

int *status;//for parent process

int main() {
  //char cmd[100], command[100], *parameters[20];
  int count = 0;
  int i = 0;
  int j = 0;
  char *array[100]; 
  char *pch;
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];

  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
    
    char *envp[] = {(char *) "PATH=/bin", 0};
    while (true) {
      
        do{ 
            char cwd[256];
            // Print the shell prompt.
            printf("%s%s", myGetCWD(), prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
               }
          
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 
        
			  // 0. Modify the prompt to print the current working directory			  
			
        // 1. Tokenize the command line input (split it on whitespace)        
        // // get the first token      
        arguments[0] = strtok(command_line, delimiters); //trim whitespace       
        // walk through other tokens
        i = 0;       
        while( arguments[i] != NULL ) {
          arguments[++i] = strtok(NULL, delimiters);
        }
        j = 0;
        while (j < i - 1){
          arguments[j] = trimwhitespace(arguments[j]);
          j++;
        }
        

      
        // 2. Implement Built-In Commands
        if (strcmp(arguments[0],"exit")== 0){
          exit(0);
        }
        else if (strcmp(arguments[0],"pwd")== 0){
          printf("%s\n",myGetCWD());
        }
        else if (strcmp(arguments[0],"echo")== 0){
          i = 1;
          while (arguments[i] != NULL){
            printf("%s", arguments[i]);
            i++;
          }
          printf("/n");
        }
        else if (strcmp(arguments[0],"env")== 0){
          j = 1;
          char *v = *environ;
          for (; v; j++){
            printf("%s\n", v);
            v = *(environ+j);
          }
        }
        else if (strcmp(arguments[0],"setenv")== 0){
          setenv(arguments[1], arguments[2], 1);//to overwrite
        }
        else if (strcmp(arguments[0],"cd")== 0){
          chdir(arguments[1]);
        }
        else {
          // 3. Create a child process which will execute the command line input
          pid_t pid;
          pid = fork();
          if (pid < 0){
            printf("Fork error.\n");
          }
          if (pid == 0){//child process
            char *arg_arr[] = {arguments[0], arguments[1], arguments[2], NULL};
            execvp(arg_arr[0], arg_arr);
            exit(0);
            if (strcmp(arguments[1], "&")== 0){
              char *arg_arr[] = {arguments[0], NULL};
              execvp(arg_arr[0], arg_arr);
            }
            
          }
          // 4. The parent process should wait for the child to complete unless its a background process
          pid = wait(status);
        }
      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    }
    // This should never be reached.
    return -1;
}
