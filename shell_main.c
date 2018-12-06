#define _BSD_SOURCE
#define _XOPEN_SOURCE
#define _SVID_SOURCE
#include "command_line.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>


#define MAX_LINE_LENGTH 512
char path[PATH_MAX];
int reap;

void handle_sigchld(int sig) {
	int saved_errno = errno;
	while (waitpid((pid_t)(-1), &reap, WNOHANG) > 0) {}
	errno = saved_errno;
	
	if (WIFEXITED(reap)){
		int exit_status = WEXITSTATUS(reap);
		printf("Exit status was : %d\n", exit_status);
	}else if (WTERMSIG(reap)){
		int sig_status = WTERMSIG(reap);
		printf("Signal terminated process: %d\n", sig_status);
	}
}

void myHandler(int sig){
/*	pid_t pid = fork();

	if (pid == 0){
		kill(pid, sig);
	}
*/
}

int eval(char **args, bool background)
{


	pid_t pid, wpid ;
	int status;



	pid = fork();
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("error");
	}else{
		signal(SIGCHLD, handle_sigchld);

		do {
			if(background == 0){
				wpid = waitpid(pid, &status, WUNTRACED);
			}

			
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	}

	return 1;
}


int cd(char **args)
{
	if (args[1] == NULL) {
		chdir(getenv("HOME"));
	} else {
		if (chdir(args[1]) != 0) {
			perror("cd");
		}
	}
	return 1;
}
int parse(char **args, bool background)
{
	getcwd(path, sizeof(path));
	if (args[0] == NULL) {
		return 1;
	}

	if (strcmp(args[0], "cd") == 0) {
		return cd(args);
	}


	return eval(args, background);
}





int main(int argc, const char **argv)
{
	struct sigaction sa;
	sa.sa_handler = &handle_sigchld;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, 0) == -1) {
		printf("hello");
		perror(0);
		exit(1);
	}





	signal(SIGINT, myHandler);
	char cmdline[MAX_LINE_LENGTH];
	struct CommandLine command;
	for (;;)
	{
		printf("> ");
		fgets(cmdline, MAX_LINE_LENGTH, stdin);
		if (feof(stdin)) 
		{
			exit(0);
		}

		bool gotLine = parseLine(&command, cmdline);
		if (gotLine) 
		{
			parse(command.arguments, command.background);
			char cwd[PATH_MAX];
			getcwd(cwd, sizeof(cwd));
			printf("Current directory: %s\n", cwd);
			printCommand(&command);
			freeCommand(&command);
		}
	}
}
