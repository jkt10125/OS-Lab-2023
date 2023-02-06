#include <termios.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include "./utils.hpp"
#include "./command.hpp"
using namespace std;

const int MAXCHAR = 100;
const int MAXARGS = 100;

int main() {
    enableRawMode();

    while (true) {

        shellPrompt();
        string input = ReadLine();
        pid_t pid = fork();
        if (pid == 0) {
            Command cmd(input);
            execute_command(cmd.args);
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
