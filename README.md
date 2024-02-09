User Manual
Introduction
	Creator: Sean Byrne
		This programs purpose was to create a simple shell based on the Linux shell. The main functionality of how this program works is a 4-step process. The first is that there is a while loop that constantly asks the user for commands. The second step is to read the users input in. In the third step the users command is then parsed by the spaces in between the words as a delimiter and each induvial word is put into an index into an array.  The fourth and final step is to send the users parsed command through various checks to figure out how the command will be processed and outputted.

Internal Commands
	Help 
		Executed by typing “help” in the shell with no arguments. Executing the help command will print the user manual with the read more filter. The read more filter will print out only 10 lines at a time of the user manual and prompting you to press enter to read more of the file which is printing out the next 10 lines. This task is accomplished by opening the file for user manual, reading in 10 lines from file and printing them out, and then asking the user to hit enter to get the next 10 lines to be printed until EOF.
	Directory
		The directory command is executed by itself by typing “dir” or “dir” with arguments following the command. If the command is executed with no arguments, then it will print out all the directory contents in the current directory. If the command is with preceding arguments (“dir -arg”) then the command will print all the contents from the directory path you specified with “arg”. The way this command functions is that it looks to see if the user specified a directory to search in. if not then the opendir() method would be called to open the directory and the readir() method will also be called in a loop with a print statement to print all the contents with your current directory as the parameter. When the user specifies the directory to search in then the same steps will be done but with the specified directory as the parameter.

	Environ
		Executed by typing “env” in the shell with no arguments preceding.  When executed all the user’s environment variables will be printed to the screen. To accomplish this task when the command is executed is printing the external eviron array from the unstd.h library in a loop until empty.

	Exit
		Executed by typing “quit” in the shell with no arguments. This command exits the terminal and ending the program. To do this this command sets the main loops Boolean variable to false.

	Pause
		Executed by typing “pause” in the shell with no arguments. This command pause the entire program by calling the method getchar() and waits for the user to hit enter to continue the program.

	Change Directory
		This command is executed with arguments by “cd -arg”. When the command is executed it changes the current directory to what you specify in your directory path for the program and children forked. Accomplishing this task is to call chdir() method the argument that was typed as the parameter

	Echo
		Executed by typing “echo” with preceding string like “echo Hello world!”. This command will print out whatever the user types out after echo. The way this command works is that it takes the string you printed that got stored in a array and prints out each element.


	Clear
		The command clear is executed with no preceding arguments and is simply executed with typing “clear”. The user will also be able to see what was cleared by using the scroll bar up until your back at the output. When the command is executed, it will empty the screen and only leave “myShell>” prompt at the top of the screen.  To produce this outcome when “clear” command is typed is by printing out enough newlines to empty the screen and moving the cursor to the top of the screen.

External Commands
		Example of external commands are “ps”, “ls”, “cat”, etc. These commands could either entered in with arguments or no arguments. The main program will fork() to create a child process to execute these commands with execvp() and the main program (the parent) will then wait for child process to finish to run again. The function execvp() takes your command in the form of a char** array with the last index as NULL to be able to tell when down, as 2 parameters. The first parameter is “array[0]” which is the users command and the second as the entire array “array” which is the arguments. Also when execvp() is called it will search for this program in the path environment, and execute the command.

I/O Redirection
		For input /output redirection to work is to first open the file or creating a new file with using the open() method depending on creating, making, or appending to a file. Then the file descriptor of the file is duplicated with the method dup2(). This is so the input or output will be either sent/inputted instead of stdout/stdin. Once dup2() is finished then the file is then closed with close(). To implement redirection the users command is searched through multiple if statements to figure out what kind of redirection will take place. It will be searched for symbols like “>” (for output overwriting), “>>” (for output appending), and ”<” (for input). Once one of those symbols is found, it would be removed from the parsed users command that was obtained by the parsing method. Then the command will be executed accordingly with execvp() method by sending the new parsed users command in the form of an array to the method in 2 parts like external commands for a external commands. If the command is a internal then it will just be printed instead of using execvp() at the end. 

Piping
		To execute this command type in the terminal “|” between 2 commands (“cmd1 | cmd2”). Piping works by sending the output of one command to the input of another, by allowing the stdout of one command to be connected to the stdin of another command. So, the command on the left of the “|” is the output of the command you want to go into the command on the right.  To implement pipe in the shell is by first taking the users parsed command and separating the commands into their on parsed array and removing the “|” symbol. Then calling the pipe() method to create the pipe with 2 file descriptors. Then fork() is called to create a child process. In the parent process the on of the file descriptors from the pipe method is duplicated with dup2() and the STDOUT_FILENO creating pipe out and then closed. Afterwards command on the left side of the pipe is executed with execvp(). In the child process the same steps occur but instead it duplicates the other pipe created file descriptor with STDIN_FILENO to get the output of the other command as input for the second command when its executed with execvp().

Background Execution
		To execute a command in the background is by ending the command with the “&” symbol like “cmd &”. While a process is running in the background the user will prompted to enter in a new command instead of waiting for the command to finish to be prompted again.  To accomplish this task was first checking if the users parsed command has the “&” at the end and If so then an integer flag will change to 1.  So when executing the external command with the fork() method the parent will not wait for the child to terminate. Also, the child’s standard output will be closed and be redirected somewhere else other than the terminals by calling  close(0), close(1), close(2), open("/dev/null", O_RDONLY), open("/dev/null", O_WRONLY), and open("/dev/null", O_WRONLY).

Batch mode
		Batch mode is executed in 2 ways. The first way is by invoking your program with a command line argument like (“./myshell batchFile.sh”). The second way is by running the batch file while already in the shell like(“./batchFile.sh”). to accomplish running a batch file in the already running shell is by checking users command to see if it is not a valid internal or external command. If, so then the method for the check will return a 1 and Then will check to see if it is a valid file name that can be executed. If it is a valid file name, then read the batch file line by line and parse the commands like originally inputted user commands and send the through the same path through the program too.

 

