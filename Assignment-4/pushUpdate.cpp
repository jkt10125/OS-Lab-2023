#include <iostream>
#include <iomanip>
#include "definitions.hpp"
using namespace std;

void pushToFriends(int tid, Action *action)
{
    int i = 0;
    for (auto const &[id, f] : nodes[action->userId]->friendList)
    {
        PUops1[tid]++;
        // update feed of node f
        pthread_mutex_lock(&feedQmutex[f.node->userId]);
        f.node->feed.push(*action);
        pthread_mutex_unlock(&feedQmutex[f.node->userId]);

        // push node f to queue of nodes which are updated
        pthread_mutex_lock(&feedsUpdatedQmutex[(action->actionId + i)%MAX_READ_POST_THREADS]);
        feedsUpdatedQueue[(action->actionId + i)%MAX_READ_POST_THREADS].push(f.node);
        pthread_cond_broadcast(&newUpdatesPushed[(action->actionId + i)%MAX_READ_POST_THREADS]);
        pthread_mutex_unlock(&feedsUpdatedQmutex[(action->actionId + i)%MAX_READ_POST_THREADS]);
        i++;
        // push node f to queue of nodes which are updated
        // pthread_mutex_lock(&feedsUpdatedQmutex);
        // feedsUpdatedQueue.push(f.node);
        // pthread_cond_broadcast(&newUpdatesPushed);
        // pthread_mutex_unlock(&feedsUpdatedQmutex);

        // print to logfile
        string str = "FeedUpdater-" + to_string(tid) + "$ ";
        pthread_mutex_lock(&fmutex);
        logfile << setw(20) << left << str;
        logfile << (action->actionType == Action::POST ? "pushed a post " : (action->actionType == Action::LIKE ? "pushed a like " : "pushed a comment "));
        logfile << "(ID: " << action->actionId << ") ";
        logfile << "from user#" << action->userId << " ";
        logfile << "to user#" << f.node->userId << endl;
        pthread_mutex_unlock(&fmutex);

        // print to console
        pthread_mutex_lock(&omutex);
        cerr << setw(20) << left << str;
        cerr << (action->actionType == Action::POST ? "pushed a post " : (action->actionType == Action::LIKE ? "pushed a like " : "pushed a comment "));
        cerr << "(ID: " << action->actionId << ") ";
        cerr << "from user#" << action->userId << " ";
        cerr << "to user#" << f.node->userId << endl;
        pthread_mutex_unlock(&omutex);
    }
}

void *pushUpdateRunner(void *param)
{
    int tid = *static_cast<int *>(param);
    while (1)
    {
        PUops0[tid]++;

        Action *action;

        // pop an action from the queue of new actions
        pthread_mutex_lock(&actionQmutex);
        while (actionQueue.empty())
        {
            pthread_cond_wait(&newActionGenerated, &actionQmutex);
        }
        
        action = actionQueue.front();
        actionQueue.pop();
        
        pthread_mutex_unlock(&actionQmutex);
        
        pushToFriends(tid, action);
    }
    pthread_exit(0);
    return param;
}