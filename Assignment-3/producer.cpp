#include <iostream>
#include "definitions.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <random>

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
    default_random_engine generator;
    while (k--)
    {
        uniform_int_distribution<int> U(1, range);
        int x = U(generator);
        int u = 0, prefix_sum = 0;

        while (u < node_count)
        {
            prefix_sum += degree[u];
            if (prefix_sum >= x)
                break;
            u++;
        }

        if (edge_count == MAX_EDGES)
            dieWithError();
        // add node_id->u
        edges[edge_count].to = u;
        edges[edge_count].next = nodes[node_id];
        nodes[node_id] = edge_count;
        edge_count++;
        degree[u]++;

        if (edge_count == MAX_EDGES)
            dieWithError();
        // add u->node_id
        edges[edge_count].to = node_id;
        edges[edge_count].next = nodes[u];
        nodes[u] = edge_count;
        edge_count++;
        degree[node_id]++;

        range++;
    }
}

void producerProcess()
{
    cerr << "Producer waked up, adding new nodes\n";

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

    int m = 10 + rand() % 21;
    while (m--)
    {
        if (curr_node_count == MAX_NODES)
            dieWithError();
        connectNewNode(curr_node_count, degree);
        curr_node_count++;
    }
    cerr << "Producer slept\n";
}
