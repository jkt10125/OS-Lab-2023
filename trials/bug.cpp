#include <bits/stdc++.h>
#include <sys/wait.h>
static const size_t kNumChildren = 5;
static size_t numDone = 0;
void reapChild(int unused)
{
    std::cout<<getpid()<<std::endl;
    while (true)
    {
        pid_t pid = waitpid(-1, NULL, WNOHANG);
        if (pid <= 0)
            break;
        numDone++;
    }
}
int main(int argc, char *argv[])
{
    std::cout<<getpid()<<std::endl;
    printf("Let my five children play while I take a nap.\n");
    signal(SIGCHLD, reapChild);
    for (size_t kid = 1; kid <= 5; kid++)
    {
        if (fork() == 0)
        {
            sleep(3*kid); // sleep emulates "play" time
            printf("Child #%zu tired... returns to dad.\n", kid);
            return 0;
        }
    }
    // code below is a continuation of that presented on the previous slide
    while (numDone < kNumChildren)
    {
        printf("At least one child still playing, so dad nods off.\n");
        sleep(5);
        printf("Dad wakes up! ");
    }
    printf("All children accounted for. Good job, dad!\n");
    return 0;
}
