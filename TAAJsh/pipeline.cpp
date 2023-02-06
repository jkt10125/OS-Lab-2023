#include "./pipeline.hpp"
#include "./utils.hpp"
#include <sys/wait.h>
using namespace std;

Pipeline::Pipeline(const string &__str) : command(__str), isBackgroundProcess(false)
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
}

void Pipeline::execute()
{
    int pp[2], oldpp[2];
    for (int i = 0; i < components.size(); i++)
    {
        if (i != components.size() - 1)
            pipe(pp);
        int pid = fork();
        // Piping the commands with input output redirections.
        if (!pid)
        {
            if (i == 0 || i == components.size() - 1)
                components[i]->set_fd();
            if (i != 0)
            {
                dup2(oldpp[0], STDIN_FILENO);
                close(oldpp[0]);
                close(oldpp[1]);
            }
            if (i != components.size() - 1)
            {
                close(pp[0]);
                dup2(pp[1], STDOUT_FILENO);
                close(pp[1]);
            }
            components[i]->pid = getpid();
            execute_command(components[i]->args);
        }
        if (i != 0)
        {
            close(oldpp[0]);
            close(oldpp[1]);
        }
        if (i != components.size() - 1)
        {
            oldpp[0] = pp[0];
            oldpp[1] = pp[1];
        }
        if (!isBackgroundProcess)
        {
            int status;
            waitpid(pid, &status, WUNTRACED);
            if(WIFSTOPPED(status)){
                kill(pid, SIGCONT);
            }
        }
    }
}

