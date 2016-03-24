#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>

/**
 * Maximale Länge der Benutzereingabe
 */
#define MAX_INPUT 1024

/**
 * Prozessid des Kindprozesses
 */
static int pid = 0;

/**
 * Feld das den aktuellen Pfad beinhaltet
 */
static char cwd[100];

/**
 * Array für die Argumente der Benutzereingabe
 */
static char *argv[256];

/**
 * Anzahl der Argumente des derzeitigen Befehls
 */
static int argc;

/**
 * Pfad der Protokollierungsdatei
 */
static char *home;
/**
 * Datei in der die Benutzereingaben protokolliert werden
 */
FILE *fhistory;

/**
 * Gibt den Promptcode der rash am Bildschirm aus
 * @param text kann NULL sein oder auch nicht
 */
void myprintf(char *text) {
    text != NULL ?
    printf("%s@rash:%s: %s\n", getenv("USER"), getcwd(cwd, sizeof(cwd)), text) :
    printf("%s@rash:%s: ", getenv("USER"), getcwd(cwd, sizeof(cwd)));
}

/**
 * Gibt den Promptcode mit der entsprechenden Fehlermeldung aus
 */
void errprintf(char *text) {
    myprintf(NULL);
    printf("Couldn't run %s\n", text);
}

/**
 * Verarbeitet die Eingabe des Benutzers indem es die Zeichenkette vom Zeilenvorschub abtrennt
 * und anschließend in das array argv schreibt
 * Falls der Parameter input "exit" ist, wird die Protokollierungsdatei geschlossen und das Programm
 * beendet.
 * @param input, Benutzereingabe
 * @return 0, falls keine Eingabe stattfand, ansonsten 1
 */
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
        strcpy(argv[argc++], param);
        param = strtok(NULL, " ");
    }
    if (!strcmp(argv[0], "exit")) {
        fclose(fhistory);
        exit(0);
    }
    return 1;
}

/**
 * Signalverarbeitungsroutine die bei den Signalen SIGINT und SIGCHLD den Kindprozess
 * beendet und pid auf 0 setzt.
 * @param sign, Signalid
 */
void signalHandler(int sign) {
    switch (sign) {
        /**
         * Routine falls der Benutzer ^C gedrückt hat
         */
        case SIGINT: {
            if (pid != 0) {
                kill(pid, SIGKILL);
                pid = 0;
            }
            break;
        }
            /**
             * Routine falls das Kind beendet wurde
             */
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

/**
 * Methode die den Befehl des Benutzers ausführt, indem die Methode fork() aufgerufen
 * wird und der Elternprozess auf das Beenden des Kindprozesses wartet.
 */
void programs() {
    /**
     * Ändert auf den Befehl "cd" hin das Verzeichnis
     */
    if (!strcmp(argv[0], "cd")) {
        chdir(argv[1] == NULL ? getenv("HOME") : argv[1]);
    } else {
        /**
         * Ein Kindprozess wird erstellt
         */
        pid = fork();
        switch (pid) {
            case -1: {
                /**
                 * Kindprozess konnte nicht erstellt werden
                 */
                myprintf("Couldn't fork!");
                break;
            }
            case 0: {
                /**
                 * Wenn der eingebene Befehl "history" ist, wird die Protokollierungsdatei
                 * im Homeverzeichnis des Benutzers mit Hilfe von "cat" ausgegeben
                 */
                if (!strcmp(argv[0], "history")) {
                    strcpy(argv[0], "cat");
                    strcpy(argv[1], home);
                }
                /**
                 * Kindprozess wird ausgeführt
                 */
                execvp(argv[0], argv);
                /**
                 * Gibt eine Fehlermeldung aus, falls der Befehl unbekannt war
                 */
                errprintf(argv[0]);
                break;
            }
            default: {
                /**
                 * Der Elternprozess wartet auf das Ableben des Kindprozesses
                 */
                waitpid(pid, NULL, 0);
                signal(SIGCHLD, signalHandler);
                signal(SIGINT, signalHandler);
                break;
            }
        }
    }
}

/**
 * Hauptprogramm, das in einer Endlosschleife die Befehle der Benutzers einliest und ausführt
 */
int main(void) {
    signal(SIGINT, signalHandler);
    char *input = NULL;
    /**
     * Herausfinden des Homeverzeichnisses des Benutzers und Öffnen des Protokolls
     */
    struct passwd *pw = getpwuid(getuid());
    home = pw->pw_dir;
    strcat(home, "/.rash_history");
    fhistory = fopen(home, "a");

    /**
     * Ausgabe des Willkommensgrüße
     */
    myprintf("Welcome to rash!\ndeveloped by Resch Andreas 3IA");

    while (1) {
        /**
         * Ausgabe des Promptcodes vor jeder Eingabe
         */
        myprintf(NULL);

        input = (char *) malloc(sizeof(char *) * MAX_INPUT);
        int j;
        /**
         * Leeren des Feldes
         */
        for (j = 0; j < MAX_INPUT; j++) {
            input[j] = '\0';
        }
        /**
         * Einlesen von der Benutzereingabe bis ein Zeilenvorschub kommt
         */
        fgets(input, MAX_INPUT, stdin);

        /**
         * Konntrolliert ob die Eingabe nur ein Zeilenvorschub war
         */
        if (parseInput(input)) {
            /**
             * Ausführen des eingegebenen Befehls
             */
            programs();
        }

        /**
         * Freigeben des Speichers von der Schon bearbeiteten Benutzereingabe
         */
        free(input);
    }
    return 0;
}