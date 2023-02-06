#include <termios.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include "./utils.hpp"
#include "./pipeline.hpp"

using namespace std;

const int MAXCHAR = 100;
const int MAXARGS = 100;

pid_t pid;

int main() {
    enableRawMode();

    while (true) {

        shellPrompt();
        string input = ReadLine();
        trim(input);
        if(input.empty()){
            continue;
        }
        pid = fork();
        if (pid == 0) {
            Pipeline p(input);
            p.execute();
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
