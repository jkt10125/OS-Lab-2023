#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/shm.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "definitions.h"
#include "producer.h"
#include "consumer.h"
using namespace std;

void *shmptr;

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
            cerr << "Invalid edge format! Ignored\n";
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

        node_count = max({node_count, u, v});
    }

    in.close();
}

void output(const string &filepath)
{
    ofstream out(filepath);
    int &node_count = getNodeCount();
    int *nodes = getNodeArr();
    Edge *edges = getEdgeArr();
    for (int u = 0; u <= node_count; u++)
    {
        for (int index = nodes[u]; index != -1; index = edges[index].next)
        {
            out << u << " " << edges[index].to << endl;
        }
    }
    out.close();
}

void createSharedSpace()
{
    int shmid = shmget(IPC_PRIVATE, sizeof(int) + MAX_NODES * sizeof(int) + sizeof(int) + MAX_EDGES * sizeof(Edge), IPC_CREAT | 0666);
    shmptr = shmat(shmid, NULL, 0);
}

int main(int argc, char *argv[])
{
    createSharedSpace();
    input("facebook_combined.txt");
    int producerID, consumerIDs[CONSUMER_COUNT];
    producerID = fork();
    if (producerID == 0)
    {
        producerProcess();
        exit(0);
    }

    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        consumerIDs[i] = fork();
        if (consumerIDs[i] == 0)
        {
            consumerProcess(i);
            exit(0);
        }
    }
    waitpid(producerID, NULL, 0);
    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        waitpid(consumerIDs[i], NULL, 0);
    }

    return 0;
}