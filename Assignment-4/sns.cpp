#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/wait.h>
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
vector<queue<Node *>> feedsUpdatedQueue(MAX_READ_POST_THREADS);
pthread_mutex_t feedsUpdatedQmutex[MAX_READ_POST_THREADS];
pthread_cond_t newUpdatesPushed[MAX_READ_POST_THREADS];
// queue<Node *> feedsUpdatedQueue;
// pthread_mutex_t feedsUpdatedQmutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t newUpdatesPushed = PTHREAD_COND_INITIALIZER;
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

    ofstream gout("users.log");
    if (!gout.is_open())
    {
        perror("users.log");
        return;
    }
    for (int i = 0; i < MAXNODES; i++)
    {
        gout << *nodes[i];
    }
    gout.close();
}

long long PUops0[MAX_PUSH_UPDATE_THREADS], PUops1[MAX_PUSH_UPDATE_THREADS], RDops[MAX_READ_POST_THREADS];
void *monitorThreadRunner(void *param)
{
    ofstream tout("time.log");
    if (!tout.is_open())
    {
        perror("time.log");
        pthread_exit((void *)(-1));
        return param;
    }
    while (1)
    {
        sleep(1);
        tout << string(60, '-') << "\n                   Interval of 1 seconds                  \n" << string(60, '-') << endl;
        for (int i = 0; i < MAX_PUSH_UPDATE_THREADS; i++)
        {
            if (PUops0[i] != 0)
            {
                tout << "FeedUpdater#" << i << " processed " << PUops0[i] << " actions and pushed to " << PUops1[i] << " users." << endl;
                PUops0[i] = PUops1[i] = 0;
            }
        }
        for (int i = 0; i < MAX_READ_POST_THREADS; i++)
        {
            if (RDops[i] != 0)
            {
                tout << "FeedReader#" << i << " read " << RDops[i] << " actions." << endl;
                RDops[i] = 0;
            }
        }
        tout << endl;
    }
    tout.close();
    return param;
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
    for (int i = 0; i< MAX_READ_POST_THREADS; i++)
    {
        feedsUpdatedQmutex[i] = PTHREAD_MUTEX_INITIALIZER;
        newUpdatesPushed[i] = PTHREAD_COND_INITIALIZER;
    }
    pthread_t userSimulatorThread, pushUpdateThreads[MAX_PUSH_UPDATE_THREADS], readPostThreads[MAX_READ_POST_THREADS];
    pthread_create(&userSimulatorThread, NULL, userSimulatorRunner, NULL);
    for (int i = 0; i < MAX_PUSH_UPDATE_THREADS; i++)
    {
        pthread_create(&pushUpdateThreads[i], NULL, pushUpdateRunner, static_cast<void *>(new int(i)));
    }
    for (int i = 0; i < MAX_READ_POST_THREADS; i++)
    {
        pthread_create(&readPostThreads[i], NULL, readPostRunner, static_cast<void *>(new int(i)));
    }
    pthread_t monitorThread;
    pthread_create(&monitorThread, NULL, monitorThreadRunner, NULL);

    pthread_join(userSimulatorThread, NULL);
    for (int i = 0; i < MAX_PUSH_UPDATE_THREADS; i++)
    {
        pthread_join(pushUpdateThreads[i], NULL);
    }
    for (int i = 0; i < MAX_READ_POST_THREADS; i++)
    {
        pthread_join(readPostThreads[i], NULL);
    }
    pthread_join(monitorThread, NULL);

    logfile.close();
    return 0;
}
