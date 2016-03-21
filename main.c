#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


#define MAX_INPUT 256

static char cwd[100];

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
    printf("///%s///\n", input);
    input = strtok(input, "\n");
    printf("///%s///\n", input);
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
        printf("Input was exit\n");
        return 0;
    }
    printf("---%s---\n", argv[0]);
    printf("+++%s+++\n", input);
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
                myprintf("Erectile Dysfunction!");
                break;
            }
            case 0: {
                if (!strcmp(argv[0], "history")) {
                    system("cat .rash_history.txt");
                } else {
                    execvp(argv[0], argv);
                    errprintf(argv[0]);
                }
                break;
            }
            default: {
                waitpid(pid, NULL, 0);
                signal(SIGCHLD, signalHandler);
                break;
            }
        }
    }
    printf("Hier unten");
    return 1;
}

int main(void) {
    signal(SIGINT, signalHandler);
    //char *input = NULL;
    char input[MAX_INPUT];
    myprintf("Welcome to rash!\ndeveloped by Resch Andreas 3IA");
    fhistory = fopen(".rash_history.txt", "a");
    int end = 1;
    //while (input != NULL ? strncmp(input, "exit", strlen(input)) != 0 : 1) {
/*    while (end != -1) {
        myprintf(NULL);
        input = (char *) malloc(sizeof(char *) * MAX_INPUT);
        int j;
        for (j = 0; j < MAX_INPUT; j++) {
            input[j] = '\0';
        }
        fgets(input, MAX_INPUT, stdin);
        parseInput(input);
        printf("???%s???\n", input);
        if (*(input) != '\n') {
            end = programs();
            printf("%i\n", end);
        }
        printf("***%s***\n", input);*/
    //}
    while (end) {
        myprintf(NULL);
        if (fgets(input, MAX_INPUT, stdin) == NULL)
            break;
        end = parseInput(input);
        if (input[0] != '\n') {
            end == 1 ? programs() : fclose(fhistory);
            printf("%i\n", end);
            printf("***%s***\n", input);
        }
    }
    return 0;
}
