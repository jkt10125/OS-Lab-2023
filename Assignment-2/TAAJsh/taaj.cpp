#include <iostream>
#include <signal.h>
#include <unistd.h>
#include "./utils.hpp"
#include "./pipeline.hpp"
#include "./history.hpp"
#include "./signal_handler.hpp"
#include "./flock.hpp"
#include "./malware_detection.hpp"

using namespace std;

pid_t fgpid = 0;
const pid_t rootpid = getpid();
std::vector<Pipeline *> pipesArr;
std::map<pid_t, int> pid2index;
History history;

int main()
{
    signal(SIGCHLD, reapProcesses);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGINT, ctrlChandler);
    signal(SIGTSTP, ctrlZhandler);

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
        if(p->components[0]->args[0] == "delep"){
            delep(p->components[0]->args[1]);
            continue;
        }
        if(p->components[0]->args[0] == "sb"){
            detect(p->components[0]->args);
            continue;
        }
        p->execute();
    }
    history.updateHistory();
    return 0;
}
