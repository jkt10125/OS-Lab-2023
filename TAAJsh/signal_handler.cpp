#include "./signal_handler.hpp"
#include <signal.h>
#include <sys/wait.h>
#include <readline/readline.h>

void reapProcesses(int signum)
{
    while (true)
    {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (pid <= 0)
        {
            break;
        }

        if (pid2index.find(pid) == pid2index.end())
        {
            // std::cerr << "Unknown process terminated" << std::endl;
            if (fgpid == pid)
                fgpid = 0;
            break;
        }
        Pipeline *pipeline = pipesArr[pid2index[pid]];

        if (WIFSIGNALED(status) || WIFEXITED(status))
        {
            pipeline->n_alive--;
        }
        else if (WIFSTOPPED(status))
        {
            pipeline->n_alive--;
        }
        else if (WIFCONTINUED(status))
        {
            pipeline->n_alive++;
        }

        if (pipeline->group_pid == fgpid && !WIFCONTINUED(status) && pipeline->n_alive == 0)
        {
            fgpid = 0;
            if (WIFSTOPPED(status))
                pipeline->isBackgroundProcess = true;
        }
    }
}

void toggleSIGCHLDBlock(int how)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(how, &mask, NULL);
}

void blockSIGCHLD()
{
    toggleSIGCHLDBlock(SIG_BLOCK);
}

void unblockSIGCHLD()
{
    toggleSIGCHLDBlock(SIG_UNBLOCK);
}

void waitForForegroundProcess(pid_t pid)
{
    fgpid = pid;
    sigset_t empty;

    sigemptyset(&empty);
    while (fgpid == pid)
    {
        sigsuspend(&empty);
    }
    unblockSIGCHLD();
}

void ctrlChandler(int sig)
{
    if (rootpid == getpid())
    {
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
    else
        kill(getpid(), SIGKILL);
}

void ctrlZhandler(int sig)
{
    if (rootpid == getpid())
    {
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
    else
        kill(getpid(), SIGTSTP);
}
