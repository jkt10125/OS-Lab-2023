#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/wait.h>

using namespace std;

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(IEXTEN | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void die(const string s) {
    perror(s.c_str());
    exit(0);
}

const int MAXCHAR = 100;
const int MAXARGS = 100;

int main() {
    enableRawMode();

    while (true) {
        char uname[256], hname[256], cdir[256];
        getlogin_r(uname, 256);
        gethostname(hname, 256);
        getcwd(cdir, 256);
        
        cout << "\033[1;32m" << uname << "@" << hname << ":" << "\033[1;33m" << cdir << "\033[0m" << "$ ";
        fflush(stdout);

        char input[MAXCHAR];
        char *args[MAXARGS];

        fgets(input, MAXCHAR, stdin);

        char *token = strtok(input, " \t\n");
        int ctr = 0;
        while (token != NULL) {
            args[ctr++] = token;
            token = strtok(NULL, " ");
        }
        args[ctr] = NULL;

        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            exit(0);
        }
        else if (pid < 0) {
            die("Error in forking!");
        }
        else {
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}
