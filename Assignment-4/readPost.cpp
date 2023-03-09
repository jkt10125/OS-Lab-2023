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
        pthread_mutex_lock(&feedsUpdatedQmutex[tid-1]);
        while (feedsUpdatedQueue[tid-1].empty())
        {
            pthread_cond_wait(&newUpdatesPushed[tid-1], &feedsUpdatedQmutex[tid-1]);
        }
        node = feedsUpdatedQueue[tid-1].front();
        feedsUpdatedQueue[tid-1].pop();
        pthread_mutex_unlock(&feedsUpdatedQmutex[tid-1]);

        // pop an update from the feed of selected node
        pthread_mutex_lock(&feedQmutex[node->userId]);
        action = nodes[node->userId]->feed.top();
        nodes[node->userId]->feed.pop();
        pthread_mutex_unlock(&feedQmutex[node->userId]);

        // print to log file
        pthread_mutex_lock(&fmutex);
        logfile << setw(20) << left << ("FeedReader-" + to_string(tid) + "$ ");
        logfile << "user#" << node->userId << " ";
        logfile << (action.actionType == Action::POST ? "viewed a post " : (action.actionType == Action::LIKE ? "viewed a like " : "read a comment "));
        logfile << "(ID: " << action.actionId << ") ";
        logfile << "by user#" << action.userId << endl;
        pthread_mutex_unlock(&fmutex);

        // print to console
        pthread_mutex_lock(&omutex);
        cerr << setw(20) << left << ("FeedReader-" + to_string(tid) + "$ ");
        cerr << "user#" << node->userId << " ";
        cerr << (action.actionType == Action::POST ? "viewed a post " : (action.actionType == Action::LIKE ? "viewed a like " : "read a comment "));
        cerr << "(ID: " << action.actionId << ") ";
        cerr << "by user#" << action.userId << endl;
        pthread_mutex_unlock(&omutex);
    }

    pthread_exit(0);
    return param;
}