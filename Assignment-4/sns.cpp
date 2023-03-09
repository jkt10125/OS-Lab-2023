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

Node *nodes[MAXNODES];
pthread_mutex_t feedQmutex[MAXNODES];
queue<Action *> actionQueue;
pthread_mutex_t actionQmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t newActionGenerated = PTHREAD_COND_INITIALIZER;
queue<Node *> feedsUpdatedQueue;
pthread_mutex_t feedsUpdatedQmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t newUpdatesPushed = PTHREAD_COND_INITIALIZER;
ofstream logfile;
pthread_mutex_t fmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t omutex = PTHREAD_MUTEX_INITIALIZER;

void readGraph(const string &filename)
{
    for (int i = 0; i < MAXNODES; i++)
    {
        nodes[i] = new Node(i, (rand() % 2) ? Node::PRIORITY : Node::CHRONOLOGICAL);
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
        nodes[u]->friendList[v] = Node::Friend(nodes[v]);
        nodes[v]->friendList[u] = Node::Friend(nodes[u]);
    }
    for (int i = 0; i < MAXNODES; i++)
    {
        // nodes[i]->initPriority();
    }
}

int main()
{
    logfile.open("sns.log");
    if (!logfile.is_open())
    {
        perror(INPUT_FILE.c_str());
        exit(1);
    }
    readGraph(INPUT_FILE);
    for (int i = 0; i < MAXNODES; i++)
    {
        feedQmutex[i] = PTHREAD_MUTEX_INITIALIZER;
    }

    pthread_t userSimulatorThread, pushUpdateThreads[MAX_PUSH_UPDATE_THREADS], readPostThreads[MAX_READ_POST_THREADS];
    pthread_create(&userSimulatorThread, NULL, userSimulatorRunner, NULL);
    for (int i = 0; i < MAX_PUSH_UPDATE_THREADS; i++)
    {
        pthread_create(&pushUpdateThreads[i], NULL, pushUpdateRunner, static_cast<void *>(&i));
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
    logfile.close();
    return 0;
}
