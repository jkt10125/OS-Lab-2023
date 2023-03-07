#include <iostream>
#include <math.h>
#include <sys/wait.h>
#include "definitions.hpp"
using namespace std;

void generateAction(Node *node)
{
    Action::actionCount++;
    Action *newAction = new Action(Action::actionCount, static_cast<Action::ACTION_TYPE>(getRandom(0, 2)));
    node->wall.push(*newAction);
    generatedActions.push(newAction);
}
void *userSimulatorRunner(void *param)
{
    while (1)
    {
        for (int i = 0; i < 100; i++)
        {
            Node *node = nodes[getRandom(0, MAXNODES - 1)];
            int newActions = static_cast<int>(log2(node->getDegree()));
            for (int i = 0; i < newActions; i++)
            {
                generateAction(node);
            }
        }
        sleep(120);
    }
    pthread_exit(NULL);
    return param;
}