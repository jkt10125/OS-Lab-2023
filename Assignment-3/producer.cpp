#include <iostream>
#include "definitions.h"
#include "producer.h"
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <algorithm>
#include <numeric>
#include "producer.h"
#include "definitions.h"

using namespace std;

void connectNewNode(int node_id, vector<int> &degree)
{
    const int &node_count = getNodeCount();
    int *nodes = getNodeArr();
    int &edge_count = getEdgeCount();
    Edge *edges = getEdgeArr();
    int range = accumulate(degree.begin(), degree.end(), 0);
    degree.push_back(0);

    int k = 1 + rand() % 20;
    while (k--)
    {
        int x = 1 + rand() % range;
        int u = 0, prefix_sum = 0;

        while (u < node_count)
        {
            prefix_sum += degree[u];
            if (prefix_sum >= x)
                break;
            u++;
        }

        // add node_id->u
        edges[edge_count].to = u;
        edges[edge_count].next = nodes[node_id];
        nodes[node_id] = edge_count;
        edge_count++;
        degree[u]++;

        // add u->node_id
        edges[edge_count].to = node_id;
        edges[edge_count].next = nodes[u];
        nodes[u] = edge_count;
        edge_count++;
        degree[node_id]++;

        range++;
    }
}
void printDegrees()
{
    const int &curr_node_count = getNodeCount();
    int *nodes = getNodeArr();
    Edge *edges = getEdgeArr();
    int degree;
    ofstream out("degree.txt");
    for (int u = 0; u < curr_node_count; u++)
    {
        degree = 0;
        for (int index = nodes[u]; index != -1; index = edges[index].next)
        {
            degree++;
        }
        out << u << ": " << degree << endl;
    }
    out.close();
}

void producerProcess()
{
    int &curr_node_count = getNodeCount();
    int *nodes = getNodeArr();
    Edge *edges = getEdgeArr();
    vector<int> degree(curr_node_count, 0);

    for (int u = 0; u < curr_node_count; u++)
    {
        for (int index = nodes[u]; index != -1; index = edges[index].next)
        {
            degree[u]++;
        }
    }

    srand(static_cast<unsigned>(time(NULL)));
    int m = 10 + rand() % 21;
    while (m--)
    {
        connectNewNode(curr_node_count, degree);
        curr_node_count++;
    }
    printDegrees();
}
