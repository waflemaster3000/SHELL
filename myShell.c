#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define space 1000
// compiled with cygwin64 terminal
int externalCommands(char** parsedCommand);
void clear(char** parsedCommand);
void echo(char** parsedCommand);
void changeDirectory(char** parsedCommand);
void quit(char** parsedCommand);
void listDir(char** parsedCommand);
void eviron(char** parsedCommand);
void help(char** parsedCommand);
void halt(char** parsedCommand);


struct dirent *d;
DIR *getDir;
char buf[space];
//for help--------
char *line = NULL;
size_t len = 100;
//----------------
//for redirect----
char* fileName;
int inside = 0;
//----------------
//for piping------
char** cmd1;
char** cmd2;
//----------------
extern char **environ;
int pipeCheck = 0;
int redirectCheck =0;
size_t size = 100;
bool run = true;
pid_t child_pid;
pid_t child_pid2;
int stat_loc;
char* Direct;
int backgroundCheck = 0;
char** parsedCommand;
FILE *file;
char c[1000];
char *internal[] = {"env","dir","cd","help","pause","quit","clear","echo"};
//array of internal function names----
void (*internalFunc[])(char **) = {&eviron,&listDir,&changeDirectory,&help,&halt,&quit,&clear,&echo};

/*
method to parse the users commands by the dilemeter of spces.
checks for internal commands to know what to do during I/O redirection
checks for "&" to indicate the command should be run in the background
*/
char** strip(char* input){
		 char** userCom = malloc(sizeof(char*)*50);
		 char* tmp;
		 int spot = 0;

		 tmp = strtok(input," ");
		 while (tmp != NULL) {

			 userCom[spot] = malloc(sizeof(*tmp));
			 userCom[spot]=tmp;

		/*	 printf("%d\n",strlen(tmp));
			 printf("|%s|\n",tmp);
			 printf("|%s|\n",userCom[spot]);*/
			 spot++;
			 tmp = strtok(NULL," ");
		 }
		 userCom[spot] = NULL;
   if(userCom[0] != NULL){
     for(int i = 0; i <= 8; i++){
       if(strcmp(userCom[0],internal[i]) == 0){
         inside = 1;
       }
     }
   spot = spot-1;
     if(spot != 0){
       if(strcmp("&",userCom[spot])== 0){
         userCom[spot] = NULL;
         backgroundCheck = 1;
       }
     }
 }
	 	 return userCom;
 }
/*
 method to read in what ever the user typed with the help of fgets
 stores users input in a char*
 removes unwanted spaces that getline() cause at the end of the read
 */
char* readLine(){
	 char* userInput = malloc(200);
	 if (fgets(userInput, size, stdin) == NULL) {
	 			free(userInput);
	 			return NULL;
	 	} else if (userInput[0] == '\n' ){
			free(userInput);
			return NULL;
		}else if (userInput[1] == '\n' && userInput[0]=='\r'){
			free(userInput);
			return NULL;
		}
		//printf("strcmp %d\n", userInput[0]=='\r');
	 		int j = strlen(userInput)-1;
	 		if( userInput[j] == '\n'){
	 		  userInput[j] = '\0';
			}
	 		if( userInput[j-1] == '\r'){
	 		  userInput[j-1] = '\0';
			}
	 		return userInput;

 }

//Cd, CLR, Pause, Quit, Eviron, Echo, Help, Dir - internal commands
/*
clears the screen of what was printed with printing 60 newlines
moves the curser back to the top of the screen
*/
void clear(char** parsedCommand){
	for(int i=0; i<=60;i++){
		printf("\n");
	}
	printf("\033[H\033[2J");
  return;
}
/*
quits the ternimal when the user types quit
*/
void quit(char** parsedCommand){
	
  exit(1);
  run = false;
  return;
}
/*
takes in parsed array and prints all the elements in parsed users command excluding echo
*/
void echo(char** parsedCommand){

  int i = 1;
  while(parsedCommand[i] != NULL){
    printf("%s ",parsedCommand[i]);
    i++;
  }
  printf("\n");
  return;
}
/*
changes the directory specified by the user using the chdir() with the directory specified as the parameter
*/
void changeDirectory(char** parsedCommand){

  if(chdir(parsedCommand[1]) < 0){
    printf("error changing directory\n");
  }
  Direct = getcwd(buf, sizeof(buf));
  printf("%s\n",Direct);
  return;
}
/*
opens directory specified
reads from opened directory in while and prints the contents
*/
void listDir(char** parsedCommand){

  getDir = opendir(parsedCommand[1]); 
  if(getDir == NULL){
	printf("invalid Directory\n");
	return;
  }

  while((d = readdir(getDir)) != NULL){
     printf("%s\n", d->d_name);
  } 
return;
} 
/*
  prints all the environment variables stored in the external array eviron[]
  */
void eviron(char** parsedCommand){

  int i = 0;
  while(environ[i]) {
    printf("%s\n", environ[i++]);
    }
    return;
}
/*
opens help file for reading
prints out 10 lines of the help file
waits for the user to press enter to print more until finished
*/
void help(char** parsedCommand){
  if(redirectCheck > 0){
    char word;
    file = fopen("readme","r");
    while((word = fgetc(file)) != EOF)
      printf("%c", word);
   fclose(file);
    return;
  }else{

  file = fopen("readme","r");
  int i = 0;
  int printCheck = 0;
  int endCheck;

  while(printCheck == 0){
    while(i < 10){
      endCheck = getline(&line, &len, file);
      if(endCheck != -1 ){
        printf("%s\n",line);
      }else{
        i = 11;
        printCheck = 1;
      }
      i++;
    }
    i = 0;

    printf("press enter for more\n");
    getchar();
  }
  fclose(file);
    return;
  }
  fclose(file);
  return;
}
/*
pauses the terminal until the user hits enter using getchar()
*/
void halt(char** parsedCommand){
  printf("Press enter to continue\n");
  getchar();
  return;
}
/*
checks to see if the parsed command has a redirection symbol (>,>>,<,|) through various if statements
sets flags if symbol exists
also if redirection symbol exists, it is removed from userscommand and NULL is set equal to last lastIndex
if it was a I/O redirection that is the end of the method for that command
if pipe redirect then each command on either side of the pipe is sent to their own char** arrays
*/
void pipeAndRedirectionCheck(char** parsedCommand){
  int i = 0;
  while(parsedCommand[i] != NULL){
    if(strcmp("<",parsedCommand[i]) == 0){
      redirectCheck = 1;
      parsedCommand[i] = parsedCommand[i+1];
      parsedCommand[i+1] = NULL;
      fileName = parsedCommand[i];
      if(inside == 1){
	  printf("can't use internal commands\n");
	}

    }
    if(strcmp(">",parsedCommand[i]) == 0){
      redirectCheck = 2;
      parsedCommand[i] = parsedCommand[i+1];
      parsedCommand[i+1] = NULL;
      fileName = parsedCommand[i];
      if(inside == 1){
      parsedCommand[i] = NULL;
      }
      return;
    }

    if(strcmp(">>",parsedCommand[i]) == 0){
      redirectCheck = 4;
      parsedCommand[i] = parsedCommand[i+1];
      parsedCommand[i+1] = NULL;
      fileName = parsedCommand[i];
      if(inside == 1){
      parsedCommand[i] = NULL;
      }
      return;
    }
    if(strcmp("|",parsedCommand[i]) == 0){
      cmd1 = malloc(sizeof(char*)*50);
      cmd2 = malloc(sizeof(char*)*50);
      pipeCheck = 1;
      int j = 0;
      int k = 0;
      int pipeAgain = 0;
      while(parsedCommand[j] != NULL){
        if(strcmp("|",parsedCommand[j])==0){
          cmd1[j] = NULL;
          pipeAgain = 1;
          j++;
         }
        if(pipeAgain == 0){
          cmd1[j]=parsedCommand[j];
          j++;
          }
        if(pipeAgain == 1){
          cmd2[k] = parsedCommand[j];
          k++;
          j++;
        }
      }
      cmd2[k] = NULL;
      j = 0;
      k = 0;
      pipeAgain = 0;
      return;
    }

    i++;
  }

}
/*
for external command with no redirection
forks() to create child to run the command with execvp()
the parent then waits for the child to complete
if command was supposed to be executed in the background then the code in the if-statment that has the flag will be executed
the code inside if-statment doesn't allow the child to print output to the terminal
and the parent will not wait for the child
*/
int externalCommands(char** parsedCommand){
	child_pid = fork();
	if(child_pid < 0){
		printf("error forking\n");
		return 0;
	}
	if (child_pid == 0) {
          if(backgroundCheck == 1){
      		close(0);
      		close(1);       
      		close(2);
      		open("/dev/null", O_RDONLY);
      		open("/dev/null", O_WRONLY);
     		open("/dev/null", O_WRONLY);
     		}
		execvp(parsedCommand[0], parsedCommand);
		return 1;
	} else {
		if(backgroundCheck == 0){
			waitpid(child_pid, &stat_loc, WUNTRACED);
		}
	}
   backgroundCheck =0;
   return 0;
}
/*
chesks to see if the command was internal with comparing the user input to internal[]
if internal then *internalFunc[] will called the apporiate method
if not internal then leave function
*/
int internalCheck(char** parsedCommand){
	if (parsedCommand[0] == NULL) {
		//printf("Parsed command is NULL!\n");
			return 0;
	  }
	for(int i = 0; i <= 8; i++){
		if(strcmp(parsedCommand[0],internal[i]) == 0){
			(*internalFunc[i])(parsedCommand);
      return 0;
		}
	}
	return externalCommands(parsedCommand);
}
/*
child is created by fork()
checks to see what redirection flag was set
duplicates the file discriptor so the input or output will be either sent/inputted instead of stdout/stdin
checks if internal command flag was set
if so then the internalFunc[] will be called to run the apporiate method
if external command then execvp() is called to execute it
parent will wait till childs finished or if background flag was set then the method will handle it like the external() method
*/
void redirectionFunc(char ** parsedCommand){
  int fileDisc1;

	child_pid = fork();
	if(child_pid < 0){
		printf("error forking\n");
		return;
	}
   
	if (child_pid == 0) {
    if(redirectCheck == 1){ // "<"
      fileDisc1 = open(fileName, O_RDONLY, 0644);
      if(fileDisc1 <0){
	printf("|%s|",fileName);
	printf("Error opening file\n");
	return;
        }
      if(dup2(fileDisc1, 0)<0){
	  printf("ERROR dupping\n");
	  return;
	}
      close(fileDisc1);
    }
    if(redirectCheck == 2){  // ">"
      fileDisc1 = open(fileName, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
	if(fileDisc1 <0){
	    printf("Error opening file\n");
	    return;
	}
      //close(1);
      if(dup2(fileDisc1,1)<0){
	  printf("ERROR dupping\n");
	  return;
	}
      close(fileDisc1);
    }

    if(redirectCheck == 4){  // ">>"
      fileDisc1 = open(fileName, O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
	if(fileDisc1 <0){
	    printf("Error opening file\n");
	    return;
	}
      //close(1);
      if(dup2(fileDisc1,1)<0){
	  printf("ERROR dupping\n");
	  return;
	}
      close(fileDisc1);
    }
    if(inside == 1){
      for(int i = 0; i <= 8; i++){
          if(strcmp(parsedCommand[0],internal[i]) == 0){
               (*internalFunc[i])(parsedCommand);
               exit(0);
          }
       }
    }
    if(inside == 0){
	  execvp(parsedCommand[0], parsedCommand);
		printf("ERROR invalid command\n");
    }
	} else {

		if(backgroundCheck == 0){
			waitpid(child_pid, &stat_loc, WUNTRACED);
    		}
	 backgroundCheck = 0;
   	 inside = 0;
	}
	redirectCheck = 0;
	
}

/*
calls the pipe() method to create the pipe with 2 file descriptors
dup2() and the STDOUT_FILENO creating pipe out and then closed.
 Afterwards command on the left side (stored in another array by pipeAndRedirectionCheck()) of the pipe is executed with execvp().
 then another child is made
duplicate the other pipe created file descriptor with STDIN_FILENO to get the output of the other command (stored in another array by pipeAndRedirectionCheck())
if background execution then its handled just like esternal()
*/
void pipeFunc(){
    int discriptor[2];
  pipe(discriptor);
  child_pid = fork();

	if(child_pid < 0){
		printf("error forking\n");
		return;
	}
	if (child_pid == 0) {
    if(dup2(discriptor[1], 1)<0){
	  printf("ERROR dupping\n");
	  return;
	}
    close(discriptor[0]);
   if(execvp(cmd2[0],cmd2)<0){
    	perror("failed pipe command 1\n");
	close(discriptor[0]);
  	close(discriptor[1]);
	exit(0);
   	 return;
     }
  }
  child_pid2 = fork();
	if(child_pid2 < 0){
		printf("error forking\n");
		return;
	}
	if (child_pid2 == 0) {
    if(dup2(discriptor[0], 0)<0){
	  printf("ERROR dupping\n");
	  return;
	}
    close(discriptor[1]);
     if(execvp(cmd2[0],cmd2)<0){
    	perror("failed pipe command 1\n");
	close(discriptor[0]);
  	close(discriptor[1]);
	exit(0);
   	 return;
     }
  }
  close(discriptor[0]);
  close(discriptor[1]);
  if(backgroundCheck == 0){
  	waitpid(child_pid, &stat_loc, WUNTRACED);
  	waitpid(child_pid2, &stat_loc, WUNTRACED);
   }
backgroundCheck = 0;
 pipeCheck = 0;
}

/*
main while loop of the terminal
prompts user for input with "myShell>""
calls readline() to read in user input
then calls strip to parse user input
then calls apporiate methods corresponding to what flags where set
frees all pointers after use
also checks for batch file
*/

int main(int argc, char * argv[], char * envp[]) {
  Direct = getcwd(buf, sizeof(buf));
  int batchCommand = 0;
  int cmdBatchCommand = 0;
  int endCheck = 0;
  char * userInput;
  char * batchFilePath;
  FILE * batchFile;
  FILE * cmdbatchFile = NULL;
  if (argv != NULL) {

    cmdbatchFile = fopen(argv[1], "r");
  }
  while (run) {

    if (cmdbatchFile != NULL) {
      userInput = malloc(sizeof(char * ) * len);

      endCheck = getline( & userInput, & len, cmdbatchFile);

      cmdBatchCommand = 1;
      if (endCheck != -1) {
        userInput[strlen(userInput) - 2] = 0;
      } else {
        fclose(cmdbatchFile);
        break;
      }
    }

    if (cmdBatchCommand == 0 && batchCommand == 0) {
      printf("myshell> ");
      fflush(stdout);
      userInput = readLine();

    } else if (cmdBatchCommand == 0) {

      userInput = malloc(sizeof(char * ) * len);
      endCheck = getline( & userInput, & len, batchFile);
	

      if (endCheck != -1) {
	printf("|%s|",userInput);
	fflush(stdout);
	strip(userInput);
        //userInput[strlen(userInput) - 2] = 0;

      } else {

        fclose(batchFile);
        batchCommand = 0;
        userInput = NULL;
      }
    }

    if (userInput != NULL) {

      parsedCommand = strip(userInput);

      pipeAndRedirectionCheck(parsedCommand);

      if (pipeCheck == 0 && redirectCheck == 0) {
//internalCheck returns 1 if parsed command is not a valid internal or external command 
//then will check to see if it is a valid file name that can be executed
        if (internalCheck(parsedCommand) == 1) {

          batchFilePath = userInput;
          batchFile = fopen(batchFilePath, "r");
	 batchCommand = 1;
//handle if batchFile path is not a real file 
//if batchFilePath does not exist print invalid entry and set batchcommand = 0 to prompt for input next iterations
          if(batchFile == NULL){
		printf("invalid command\n");
		fflush(stdout);
		batchCommand =0;
	   }
          userInput = NULL;
        }
      }
      if (pipeCheck == 1) {
        pipeFunc();
      }
      if (redirectCheck > 0) {
        redirectionFunc(parsedCommand);
      }
      //printf("%d\n", redirectCheck);
      //printf("%d\n", pipeCheck);
	//printf("%d\n", backgroundCheck);

      redirectCheck = 0;
      pipeCheck = 0;

    }

    if (userInput != NULL) {
      free(userInput);
      userInput = NULL;
    }

    if (cmd1 != NULL) {
      free(cmd1);
      cmd1 = NULL;
    }
    if (cmd2 != NULL) {
      free(cmd2);
      cmd2 = NULL;
    }
    if (parsedCommand != NULL) {
      free(parsedCommand);
      parsedCommand = NULL;
    }

  }

  

  return 0;
}


