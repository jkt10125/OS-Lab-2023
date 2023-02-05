#include <unistd.h>
#include <termios.h>
// #include <stdio.h>
#include <string>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <sys/wait.h>
using namespace std;
const int MAX_LINE = 4096;
struct termios orig_termios;

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(IEXTEN | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void die(const string &s)
{
    perror(s.c_str());
    exit(1);
}

int main()
{
    enableRawMode();

    char *args[MAX_LINE / 2 + 1]; /* Command line arguments */
    char input[MAX_LINE];         /* User input */
    int should_run = 1;           /* Flag to determine when to exit program */

    while (should_run)
    {
        char username[256], hostname[256], curr_dir[256];
        getlogin_r(username, 256);
        gethostname(hostname, 256);
        getcwd(curr_dir, 256);
        cout << "\033[1;32m" << username << "@" << hostname << "\033[0m"
             << ":"
             << "\033[1;31m" << curr_dir << "\033[0m"
             << "$";
        fflush(stdout);
        // cout << u8"\u276f" << endl;
        fgets(input, MAX_LINE, stdin);
        // cin.getline(input, MAX_LINE, '\r');
        getchar();
        cout<<input<<endl;
        input[strcspn(input, "\n")] = 0;

        int i = 0;
        char *token = strtok(input, " \t\n");
        while (token != NULL)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        pid_t pid = fork();
        if (pid == 0)
        {
            /* Child process */
            execvp(args[0], args);
            perror("Error executing command");
            exit(1);
        }
        else if (pid < 0)
        {
            /* Error forking */
            perror("Error forking");
            exit(1);
        }
        else
        {
            /* Parent process */
            int status;
            waitpid(pid, &status, 0);
        }
    }
    return 0;
}