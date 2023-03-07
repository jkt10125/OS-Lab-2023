#include <iostream>
#include <math.h>
#include <set>
#include <sys/wait.h>
#include "definitions.hpp"
using namespace std;

void generateAction(Node *node)
{
    Action *newAction;

    /** Critical Section */
    pthread_mutex_lock(&mutex);
    Action::actionCount++;
    newAction = new Action(node->userId, static_cast<Action::ACTION_TYPE>(getRandom(0, 2)));
    actionQueue.push(newAction);
    countNewActions++;
    pthread_cond_broadcast(&newActionGenerated);
    cout << "Generated: \n"
         << *newAction << endl;
    pthread_mutex_unlock(&mutex);

    node->wall.push(*newAction);
}
void *userSimulatorRunner(void *param)
{
    while (1)
    {
        int newActions;
        Node *node;
        set<int> posters;

        while (posters.size() < 100)
        {
            posters.insert(getRandom(0, MAXNODES - 1));
        }

        for (int userId : posters)
        {
            node = nodes[userId];
            newActions = static_cast<int>(log2(node->getDegree()));
            while (newActions--)
            {
                generateAction(node);
            }
        }
        // sleep(120);
        pthread_exit(0);
    }
    return param;
}