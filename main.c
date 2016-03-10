#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void myprintf(char *text) {
    text != NULL ? printf("%s@rash: %s\n",getenv("USER"), text) : printf("%s@rash: ", getenv("USER"));
}

static char *argv[256];
static int argc;

void parseInput(char *input) {
	for(argc = 0; argc < 256; argc++) {
		argv[argc] = NULL;
	}
	argc = 0;
	char *param = strtok(input, " ");
	while (param) {
		argv[argc++] = param;
		param = strtok(NULL, " ");
	}
	printf("argc ist: %i\nargv[0] ist: %s\nargv[1] ist: %s\n", argc, argv[0], argv[1]);
}

void signalHandler(int sign) {
	switch(sign) {
		case SIGCHLD: {
			myprintf("I don't know what to do now!");
		}
	}
}

void supported() {
	int pid = fork();
	switch(pid) {
		case -1: {
			myprintf("Couldn't fork"); break;
		}
		case 0: {
			execvp(argv[0], argv);
			exit(1);
			myprintf("Child couldn't run");
			break;
		}
		default: {
			waitpid(pid, NULL, 0); break;
		}	
	}
}

int main(void) {
	FILE *history = fopen(".rash_history.txt", "a+");
	char *input = NULL;
	myprintf("Welcome to rash!\n\t\u00A9 Resch Andreas");
	while(input != NULL ? strncmp(input, "exit", strlen(input)) != 0 : 1) {
		myprintf(NULL);
		input = (char *) malloc(sizeof(char) * 256);
		fgets(input, 256, stdin);
		char *temp = NULL;
		temp = (char *) malloc(sizeof(char) * 256);
		printf("|%s|%i|\n", input, (int) strlen(input));
		strncpy(temp, input, strlen(input) - 1);
		printf("|%s***%i|\n", temp, (int) strlen(temp));
		strncpy(input, temp, (int) strlen(temp));
		if (*(input) != '\n' && strncmp(input, "exit", strlen(input))) {
			fprintf(history, "%s", input);
			parseInput(input);
            supported();
		}
	}
	fclose(history);
	return 0;
}
