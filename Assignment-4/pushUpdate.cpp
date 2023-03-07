#include <iostream>
#include "definitions.hpp"
using namespace std;

void pushToFriends(Action *action)
{
    for (auto const &[id, f] : nodes[action->userId]->friendList)
    {
        f.node->feed.push(*action);
    }
    cout << "Pushed: \n" << *action << endl;
}
void *pushUpdateRunner(void *param)
{
    while (1)
    {
        Action *action;

        /** Critical Section */
        pthread_mutex_lock(&mutex);
        while (countNewActions == 0){
            // cout << countNewActions << endl;
            pthread_cond_wait(&newActionGenerated, &mutex);
        }
        action = actionQueue.front();
        actionQueue.pop();
        countNewActions--;
        pushToFriends(action);
        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(0);
    return param;
}