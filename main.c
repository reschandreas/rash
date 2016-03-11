#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static char cwd[100];

void myprintf(char *text) {
    text != NULL ? printf("%s@rash:%s %s\n",getenv("USER"), getcwd(cwd, sizeof(cwd)), text) : printf("%s@rash:%s ", getenv("USER"), getcwd(cwd, sizeof(cwd)));
}

static char *argv[256];
static int argc;
static int pid;
void parseInput(char *input) {
	input = strtok(input, "\n");
	for(argc = 0; argc < 256; argc++) {
		argv[argc] = NULL;
	}
	argc = 0;
	char *param = strtok(input, " ");
	while (param) {
		argv[argc++] = param;
		param = strtok(NULL, " ");
	}
}

void signalHandler(int sign) {
	switch(sign) {
		case SIGINT: {
			kill(pid, SIGKILL);
			break;
		}
		case SIGCHLD: {
			myprintf("I don't know what to do now!");
			break;
		}
	}
}

void supported() {
	if (!strcmp(argv[0], "cd")) {
		chdir(argv[1] == NULL ? getenv("HOME") : argv[1]);
	} else {
		pid = fork();
		switch(pid) {
			case -1: {
				myprintf("Erectile Dysfunction!"); break;
			}
			case 0: {
				execvp(argv[0], argv);
				exit(1);
				myprintf("Child couldn't run");
				break;
			}
			default: {
				break;
			}	
		}
	}
}

int main(void) {
	signal(SIGINT, signalHandler);
	FILE *history = fopen(".rash_history.txt", "a+");
	char *input = NULL;
	myprintf("Welcome to rash!\n\t\u00A9 Resch Andreas");
	while(input != NULL ? strncmp(input, "exit", strlen(input)) != 0 : 1) {
		myprintf(NULL);
		input = (char *) malloc(sizeof(char) * 256);
		fgets(input, 256, stdin);
		if (*(input) != '\n' && strncmp(input, "exit", strlen(input))) {
			fprintf(history, "%s", input);
			parseInput(input);
            supported();
		}
	}
	fclose(history);
	return 0;
}
