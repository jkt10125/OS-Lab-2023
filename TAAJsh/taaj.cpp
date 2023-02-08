#include <termios.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include "./utils.hpp"
#include "./pipeline.hpp"
#include "./history.hpp"
#include "./signal_handler.hpp"

using namespace std;

const int MAXCHAR = 100;
const int MAXARGS = 100;

pid_t pid;

pid_t fgpid = 0;
std::vector<Pipeline*> pipesArr;
std::map<pid_t, int> pid2index;

History history;

int main()
{
    signal(SIGCHLD, reapProcesses);
    signal(SIGTTOU, SIG_IGN);
    
    while (true)
    {
        history.resetHistory();
        string input = ReadLine();
        trim(input);
        if (input.empty())
        {
            continue;
        }
        history.addHistory(input);

        Pipeline* p = new Pipeline(input);

        if(p->components[0]->args[0] == "cd"){
            if(chdir(p->components[0]->args[1].c_str()) < 0){
                perror("cd failed()");
            }
            continue;
        }
        if(p->components[0]->args[0] == "exit"){
            break;
        }
        p->execute();
    }

    return 0;
}
