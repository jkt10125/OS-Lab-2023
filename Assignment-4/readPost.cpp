#include <iostream>
#include <iomanip>
using namespace std;
#include "definitions.hpp"

void *readPostRunner(void *param)
{
    int tid = *static_cast<int *>(param);
    Node *node;
    Action action;
    while (1)
    {
        RDops[tid]++;

        // pop a node whose feed is updated
        pthread_mutex_lock(&feedsUpdatedQmutex[tid]);
        while (feedsUpdatedQueue[tid].empty())
        {
            pthread_cond_wait(&newUpdatesPushed[tid], &feedsUpdatedQmutex[tid]);
        }
        node = feedsUpdatedQueue[tid].front();
        feedsUpdatedQueue[tid].pop();
        pthread_mutex_unlock(&feedsUpdatedQmutex[tid]);

        // pop a node whose feed is updated
        // pthread_mutex_lock(&feedsUpdatedQmutex);
        // while (feedsUpdatedQueue.empty())
        // {
        //     pthread_cond_wait(&newUpdatesPushed, &feedsUpdatedQmutex);
        // }
        // node = feedsUpdatedQueue.front();
        // feedsUpdatedQueue.pop();
        // pthread_mutex_unlock(&feedsUpdatedQmutex);

        vector<Action> actions;
        // pop an update from the feed of selected node
        pthread_mutex_lock(&feedQmutex[node->userId]);
        while (!nodes[node->userId]->feed.empty())
        {
            actions.emplace_back(nodes[node->userId]->feed.top());
            nodes[node->userId]->feed.pop();
        }
        pthread_mutex_unlock(&feedQmutex[node->userId]);

        // print to log file
        pthread_mutex_lock(&fmutex);
        for (const Action &action : actions)
        {
            logfile << setw(20) << left << ("FeedReader-" + to_string(tid) + "$ ");
            logfile << "user#" << node->userId << " ";
            logfile << (action.actionType == Action::POST ? "viewed a post " : (action.actionType == Action::LIKE ? "viewed a like " : "read a comment "));
            logfile << "(ID: " << action.actionId << ") ";
            logfile << "by user#" << action.userId << endl;
        }
        pthread_mutex_unlock(&fmutex);

        // print to console
        pthread_mutex_lock(&omutex);
        for (const Action &action : actions)
        {
            cout << setw(20) << left << ("FeedReader-" + to_string(tid) + "$ ");
            cout << "user#" << node->userId << " ";
            cout << (action.actionType == Action::POST ? "viewed a post " : (action.actionType == Action::LIKE ? "viewed a like " : "read a comment "));
            cout << "(ID: " << action.actionId << ") ";
            cout << "by user#" << action.userId << endl;
        }
        pthread_mutex_unlock(&omutex);
    }

    pthread_exit(0);
    return param;
}