#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void myprintf(char *text) {
    text != NULL ? printf("rash: %s\n", text) : printf("rash: ");
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
	//if (strcmp(argv[0], "cat") == 0) {
	//	execlp(argv[0], argv[1]);
	//} else {
	//	myprintf("Command not supported");
	//}
	int pid = fork();
	switch(pid) {
		case -1: {
			myprintf("Couldn't fork"); break;
		}
		case 0: {
			execlp(argv[0], argv[1]); myprintf("Child couldn't run"); break;
		}
		default: {
			wait(NULL); break;
		}	
	}
}

int main(void) {
	FILE *history = fopen(".rash_history.txt", "a+");
	char *input = NULL;
	myprintf("Welcome to rash!\n\t\u00A9 Resch Andreas");
	while(input != NULL ? strncmp(input, "exit\n", strlen(input)) != 0 : 1) {
		myprintf(NULL);
		input = (char *) malloc(sizeof(char) * 256);
		fgets(input, 256, stdin);
		if (*(input) != '\n' && strncmp(input, "exit\n", strlen(input))) {
			fprintf(history, "%s", input);
			parseInput(input);
            supported();
		}
	}
	fclose(history);
	return 0;
}
