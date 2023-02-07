#include <termios.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include "./utils.hpp"
#include "./pipeline.hpp"
#include "./history.hpp"

using namespace std;

const int MAXCHAR = 100;
const int MAXARGS = 100;

pid_t pid;

int main()
{
    enableRawMode();
    History history;
    while (true)
    {
        history.resetHistory();
        shellPrompt();
        string input = ReadLine(history);
        trim(input);
        if (input.empty())
        {
            continue;
        }
        history.addHistory(input);
        if (input == "exit")
            break;
        if (input.substr(0, 2) == "cd")
        {
            continue;
        }
        Pipeline p(input);
        p.execute();
    }

    return 0;
}
