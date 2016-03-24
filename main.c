#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>


#define MAX_INPUT 256

static char cwd[100];

static char* home;

void myprintf(char *text) {
    text != NULL ?
    printf("%s@rash:%s: %s\n", getenv("USER"), getcwd(cwd, sizeof(cwd)), text) :
    printf("%s@rash:%s: ", getenv("USER"), getcwd(cwd, sizeof(cwd)));
}

void errprintf(char *text) {
    myprintf(NULL);
    printf("Couldn't run %s\n", text);
}

static char *argv[256];
static int argc;
static int pid = 0;

FILE *fhistory;

int parseInput(char *input) {
    if (!strncmp(input, "\n", sizeof("\n"))) {
        return 0;
    }
    input = strtok(input, "\n");
    for (argc = 0; argc < MAX_INPUT; argc++) {
        argv[argc] = NULL;
    }
    argc = 0;
    fprintf(fhistory, "%s\n", input);
    char *param = strtok(input, " ");
    while (param) {
        argv[argc++] = param;
        param = strtok(NULL, " ");
    }
    if (!strcmp(argv[0], "exit")) {
        fclose(fhistory);
        exit(0);
    }
    return 1;
}

void signalHandler(int sign) {
    switch (sign) {
        case SIGINT: {
            if (pid != 0) {
                kill(pid, SIGKILL);
                pid = 0;
            }
            break;
        }
        case SIGCHLD: {
            if (pid != 0) {
                kill(pid, SIGKILL);
                pid = 0;
            }
            break;
        }
        default:
            break;
    }
}

int programs() {
    if (!strcmp(argv[0], "cd")) {
        chdir(argv[1] == NULL ? getenv("HOME") : argv[1]);
    } else {
        pid = fork();
        switch (pid) {
            case -1: {
                myprintf("Couldn't fork!");
                break;
            }
            case 0: {
                if (!strcmp(argv[0], "history")) {
                    strcpy(argv[0], "cat");
                    strcpy(argv[1], home);
                }
                    execvp(argv[0], argv);
                    errprintf(argv[0]);
                break;
            }
            default: {
                waitpid(pid, NULL, 0);
                signal(SIGCHLD, signalHandler);
                break;
            }
        }
    }
    return 1;
}

int main(void) {
    signal(SIGINT, signalHandler);
    char *input = NULL;
    struct passwd *pw = getpwuid(getuid());
    home = pw->pw_dir;
    strcat(home, "/.rash_history");
    printf("%s", home);
    myprintf("Welcome to rash!\ndeveloped by Resch Andreas 3IA");
    fhistory = fopen(home, "a");
    while (1) {
        myprintf(NULL);
        input = (char *) malloc(sizeof(char *) * MAX_INPUT);
        int j;
        for (j = 0; j < MAX_INPUT; j++) {
            input[j] = '\0';
        }
        fgets(input, MAX_INPUT, stdin);
        if (parseInput(input)) {
            programs();
        }
        free(input);
    }
    return 0;
}