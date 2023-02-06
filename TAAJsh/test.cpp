#include <termios.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include "command.hpp"
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#define CTRL_KEY(k) ((k)&0x1f)

struct termios orig_termios;

const int MAXCHAR = 100;
const int MAXARGS = 100;

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
    raw.c_lflag &= ~(IEXTEN | ICANON | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void die(const string s)
{
    perror(s.c_str());
    exit(0);
}
// Function to execute commands
void execute_command(vector<string> command)
{
    char *arr[command.size() + 1];
    for (int i = 0; i < command.size(); i++)
    {
        arr[i] = (char *)command[i].c_str();
    }
    arr[command.size()] = NULL;
    execvp(arr[0], arr); // Executing the command.
    cout << "Error" << endl;
    exit(0);
}

int main()
{
    // enableRawMode();
    int i = 0;
    vector<string> commands;
    commands.push_back("ls -l");
    commands.push_back("g++ test2.cpp -o test2");
    commands.push_back("./test2 ok run this");
    commands.push_back("./test2 < input.txt");
    commands.push_back("./test2 < input.txt > output.txt");
    while (i < 5)
    {
        cout << commands[i] << endl;
        pid_t pid = fork();
        if (pid == 0)
        {
            Command c(commands[i]);
            execute_command(c.args);
        }
        else if (pid < 0)
            die("fork");
        else
        {
            if (open("input.txt", O_RDONLY) < 0)
            {
                perror("open");
                exit(0);
            }
            int status;
            waitpid(pid, &status, 0);
        }
        i++;
    }

    return 0;
}
