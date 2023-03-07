#include <iostream>
#include <fstream>
#include <sstream>
#include "definitions.hpp"

using namespace std;
const int MAXNODES = 37700;
const int MAXEDGES = 289003;
const int MAX_PUSH_UPDATE_THREADS = 25;
const int MAX_READ_POST_THREADS = 10;
const string INPUT_FILE = "musae_git_edges.csv";

Node **readGraph(const string &filename)
{
    Node **tempNodes = new Node *[MAXNODES];
    for (int i = 0; i < MAXNODES; i++)
    {
        tempNodes[i] = new Node(i, (rand() % 2) ? Node::PRIORITY : Node::CHRONOLOGICAL);
    }
    ifstream in(filename);
    string line;
    int u, v;
    getline(in, line);
    while (getline(in, line))
    {
        stringstream ss(line);
        ss >> u;
        ss.ignore();
        ss >> v;
        tempNodes[u]->friendList[v] = Node::Friend(tempNodes[v]);
        tempNodes[v]->friendList[u] = Node::Friend(tempNodes[u]);
    }
    for (int i = 0; i < MAXNODES; i++)
    {
        tempNodes[i]->initPriority();
    }
    return tempNodes;
}

Node **nodes;
queue<Action *> actionQueue;
int countNewActions = 0;
pthread_cond_t newActionGenerated;
pthread_mutex_t mutex;

int main()
{
    nodes = readGraph(INPUT_FILE);

    pthread_t userSimulatorThread, pushUpdateThreads[MAX_PUSH_UPDATE_THREADS], readPostThreads[MAX_READ_POST_THREADS];
    pthread_create(&userSimulatorThread, NULL, userSimulatorRunner, NULL);
    for (int i = 0; i < MAX_PUSH_UPDATE_THREADS; i++)
    {
        pthread_create(&pushUpdateThreads[i], NULL, pushUpdateRunner, NULL);
    }
    for (int i = 0; i < MAX_READ_POST_THREADS; i++)
    {
        pthread_create(&readPostThreads[i], NULL, readPostRunner, static_cast<void *>(&i));
    }

    pthread_join(userSimulatorThread, NULL);
    for (int i = 0; i < MAX_PUSH_UPDATE_THREADS; i++)
    {
        pthread_join(pushUpdateThreads[i], NULL);
    }
    for (int i = 0; i < MAX_READ_POST_THREADS; i++)
    {
        pthread_join(readPostThreads[i], NULL);
    }
    return 0;
}
