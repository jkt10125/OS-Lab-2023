#include <iostream>
#include <iomanip>
#include <math.h>
#include <set>
#include <sys/wait.h>
#include <algorithm>
#include "definitions.hpp"
using namespace std;

Action *generateAction(Node *node)
{
    Action *newAction;
    newAction = new Action(node->userId, static_cast<Action::ACTION_TYPE>(getRandom(0, 2)));
    node->wall.push(*newAction);
    return newAction;
}
void *userSimulatorRunner(void *param)
{
    srand(time(NULL));

    while (1)
    {
        int newActions;
        Node *node;
        set<int> posters;

        while (posters.size() < 100)
        {
            posters.insert(getRandom(0, MAXNODES - 1));
        }

        vector<pair<int, int>> id2countActions;
        std::transform(posters.begin(), posters.end(), std::back_inserter(id2countActions), [](int id)
                       { return pair<int, int>(id, 10 * (1 + static_cast<int>(log2(nodes[id]->getDegree())))); });
        while (!id2countActions.empty())
        {
            vector<Action *> actions;
            for (int i = 0; i < 500 && !id2countActions.empty(); i++)
            {
                int index = getRandom(0, id2countActions.size() - 1);
                actions.push_back(generateAction(nodes[id2countActions[index].first]));
                --id2countActions[index].second;
                if (id2countActions[index].second == 0)
                {
                    swap(id2countActions.back(), id2countActions[index]);
                    id2countActions.pop_back();
                }
            }

            // push action to queue of new actions
            pthread_mutex_lock(&actionQmutex);
            for (Action *newAction : actions)
                actionQueue.push(newAction);
            pthread_cond_broadcast(&newActionGenerated);
            pthread_mutex_unlock(&actionQmutex);

            // print to logfile
            pthread_mutex_lock(&fmutex);
            for (Action *newAction : actions)
                logfile << setw(20) << left << "Simulater$ " << *newAction;
            pthread_mutex_unlock(&fmutex);

            // print to console
            pthread_mutex_lock(&omutex);
            for (Action *newAction : actions)
                cout << setw(20) << left << "Simulater$ " << *newAction;
            pthread_mutex_unlock(&omutex);
        }
        sleep(120);
    }
    pthread_exit(0);
    return param;
}