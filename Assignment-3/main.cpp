#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/shm.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include "definitions.h"
using namespace std;

int shmid;
void *shmptr;
const pid_t mainID = getpid(); 

int &getNodeCount()
{
    return *(static_cast<int *>(shmptr));
}

int *getNodeArr()
{
    return static_cast<int *>(shmptr) + 1;
}

int &getEdgeCount()
{
    return *(static_cast<int *>(shmptr) + 1 + MAX_NODES);
}

Edge *getEdgeArr()
{
    return reinterpret_cast<Edge *>(static_cast<int *>(shmptr) + 1 + MAX_NODES + 1);
}

void detach()
{
    shmdt(shmptr);
}

void dieWithError()
{
    cerr << "Shared memory limit exceeded\n";
    cerr << "Process: " << getpid() << " terminated\n";
    detach();
    exit(0);
}

void init()
{
    getNodeCount() = 0;
    getEdgeCount() = 0;
    memset(getNodeArr(), -1, MAX_NODES * sizeof(int));
    memset(getEdgeArr(), -1, MAX_EDGES * sizeof(Edge));
}

void input(const string &filepath)
{
    init();

    ifstream in(filepath);

    if (!in.is_open())
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int *nodes = getNodeArr();
    int &node_count = getNodeCount();
    Edge *edges = getEdgeArr();
    int &edge_count = getEdgeCount();
    string line;
    int u, v;

    while (getline(in, line))
    {
        istringstream iss(line);

        if (!(iss >> u >> v))
        {
            cerr << "Invalid edge format! Ignored" << endl;
            continue;
        }

        // add u->v
        edges[edge_count].to = v;
        edges[edge_count].next = nodes[u];
        nodes[u] = edge_count;
        edge_count++;

        // add v->u
        edges[edge_count].to = u;
        edges[edge_count].next = nodes[v];
        nodes[v] = edge_count;
        edge_count++;

        node_count = max({node_count, u + 1, v + 1});
    }

    in.close();
}

void createSharedSpace()
{
    shmid = shmget(IPC_PRIVATE, sizeof(int) + MAX_NODES * sizeof(int) + sizeof(int) + MAX_EDGES * sizeof(Edge), IPC_CREAT | 0666);
    shmptr = shmat(shmid, NULL, 0);
}

void ctrlChandler(int signal)
{
    detach();
    if (getpid() == mainID)
        shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, ctrlChandler);
    createSharedSpace();
    input("facebook_combined.txt");
    int producerID, consumerIDs[CONSUMER_COUNT];
    producerID = fork();
    if (producerID == 0)
    {
        srand(static_cast<unsigned>(time(NULL)));
        while (1)
        {
            sleep(50);
            producerProcess();
        }
        detach();
        exit(0);
    }

    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        consumerIDs[i] = fork();
        if (consumerIDs[i] == 0)
        {
            while (1)
            {
                consumerProcess(i);
                sleep(30);
            }
            detach();
            exit(0);
        }
    }
    waitpid(producerID, NULL, 0);
    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        waitpid(consumerIDs[i], NULL, 0);
    }
    detach();
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}