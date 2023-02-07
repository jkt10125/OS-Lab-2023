#include "./pipeline.hpp"
#include "./utils.hpp"
#include "./signal_handler.hpp"
#include <sys/wait.h>
using namespace std;

Pipeline::Pipeline(const string &__str) : command(__str), isBackgroundProcess(false), group_pid(-1)
{
    parse();
}

Pipeline::~Pipeline() {}

void Pipeline::parse()
{
    trim(command);
    if (command.back() == '&')
    {
        isBackgroundProcess = true;
        command.pop_back();
    }
    vector<string> chuncks = split(command, '|');
    for (int i = 0; i < chuncks.size(); i++)
    {
        components.push_back(new Command(chuncks[i]));
    }
    n_alive = components.size();
}

void Pipeline::execute()
{
    int fg_pgid = 0;
    int pp[2], oldpp[2];

    blockSIGCHLD();

    for (int i = 0; i < components.size(); i++)
    {
        if (i != components.size() - 1 && pipe(pp))
        {
            perror("pipe()");
            exit(1);
        }

        int pid = fork();
        if (pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        // Piping the commands with input output redirections.
        if (pid==0)
        {
            unblockSIGCHLD();

            components[i]->set_fd();

            if (i == 0)
            {
                fg_pgid = getpid();
                setpgrp();
                // tcsetpgrp(STDIN_FILENO, fg_pgid);
                // pipesArr.push_back(this);
            }
            else
            {
                setpgid(0, fg_pgid);
                dup2(oldpp[0], components[i]->infd);
                close(oldpp[0]);
                close(oldpp[1]);
            }
            if (i != components.size() - 1)
            {
                dup2(pp[1], components[i]->ofd);
                close(pp[0]);
                close(pp[1]);
            }
            execute_command(components[i]->args);

            cout<<1<<endl;
        }
        else
        {
            components[i]->pid = pid;
            if (i == 0)
            {
                fg_pgid = pid;
                group_pid = pid;
                setpgid(pid, fg_pgid);
                pipesArr.push_back(this);
                tcsetpgrp(STDIN_FILENO, fg_pgid);
            }
            else
            {
                setpgid(pid, fg_pgid);
            }
            if (i != 0)
            {
                close(oldpp[0]);
                close(oldpp[1]);
            }
            oldpp[0] = pp[0];
            oldpp[1] = pp[1];
            pid2index[pid] = pipesArr.size() - 1;
        }
    }
    if (isBackgroundProcess)
    {
        unblockSIGCHLD();
    }
    else
    {
        waitForForegroundProcess(fg_pgid);
        // if (pipesArr.back() == STOPPED) {  // If Ctrl-Z was sent, now send SIGCONT to continue the process immediately in the background
        //     kill(-fg_pgid, SIGCONT);
        // }
    }
    tcsetpgrp(STDIN_FILENO, getpid());
}
