#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

int main (int argc, char *argv[]) {

	struct timeval start, end;
	char *cmd="ls";

	pid_t pid;
	pid = fork();

	if(pid==0) {
		gettimeofday(&start, NULL);
		printf("Start child process\n");
		printf("Hi, my name is Xiaqila.\n");
		printf("exec() will run ls -l.\n");
		int sts_cd = execvp(cmd, argv);
		printf("There is an issues with the running command\n");
		if (sts_cd == -1) {
			printf("execvp error! \n");
			return 1;
		}
	}
	else if ( pid > 0 ) {
		int wc = wait(NULL);
		gettimeofday(&end, NULL);
		printf("Parent process of %d, (wc:%d), (pid:%d)\n", pid, wc, (int)getpid());
		printf("Child process: %d startTime: %ld seconds %ld micro seconds, endTime: %ld seconds %ld micro seconds , elapsedTime:  %ld micro seconds\n",
			 wc, start.tv_sec, start.tv_usec, end.tv_sec, end.tv_usec, ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	}
	else {
		printf("Error while forking \n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

