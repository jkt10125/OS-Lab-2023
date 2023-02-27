#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <queue>
#include <set>
#include <chrono>
#include <iomanip>
#include "definitions.h"

using namespace std;
using namespace std::chrono;

int seen_nodes = 0;
int iterations = 0;
double avg_time_taken = 0;
vector<int> seen_dist;
vector<int> seen_par;

void optimized_dijkstra(const vector<int> &sources, const int *NodeArr, const Edge *EdgeArr, int node_size, int edge_size)
{
    for (int source : sources)
    {
        for (int nodePtr = NodeArr[source]; nodePtr != -1; nodePtr = EdgeArr[nodePtr].next)
        {
            int neighbour = EdgeArr[nodePtr].to;
            if (seen_dist[source] - 1 > seen_dist[neighbour])
            {
                seen_dist[source] = seen_dist[neighbour] + 1;
                seen_par[source] = neighbour;
            }
        }

        for (int nodePtr = NodeArr[source]; nodePtr != -1; nodePtr = EdgeArr[nodePtr].next)
        {
            int neighbour = EdgeArr[nodePtr].to;
            if (seen_dist[neighbour] - 1 <= seen_dist[source])
                continue;

            // doing restricted bfs from neighbours with seen_dist value greater than seen_dist[source] + 1
            queue<int> q;
            vector<bool> visited(node_size, false);
            visited[source] = true;
            visited[neighbour] = true;
            seen_dist[neighbour] = seen_dist[source] + 1;
            seen_par[neighbour] = source;
            q.push(neighbour);

            while (!q.empty())
            {
                int u = q.front();
                q.pop();
                for (int nodePtr = NodeArr[u]; nodePtr != -1; nodePtr = EdgeArr[nodePtr].next)
                {
                    int v = EdgeArr[nodePtr].to;
                    if (!visited[v] && seen_dist[v] - 1 > seen_dist[u])
                    {
                        visited[v] = true;
                        seen_dist[v] = seen_dist[u] + 1;
                        seen_par[v] = u;
                        q.push(v);
                    }
                }
            }
        }
    }
}

void consumerProcess(int id, bool optimize)
{

    string filename = "consumer_" + to_string(id) + ".txt";
    ofstream myfile(filename.c_str(), ios::app);

    int node_size = getNodeCount();
    int *NodeArr = getNodeArr();

    int edge_size = getEdgeCount();
    Edge *EdgeArr = getEdgeArr();

    if (node_size == 0)
    {
        myfile << "No nodes in graph\n"
               << endl;
        myfile.close();
        return;
    }

    auto dijkstra = [&node_size, &NodeArr, &edge_size, &EdgeArr](int id)
    {
        set<pair<int, int>> s; // {dist[u], u}
        vector<int> dist(node_size, INT32_MAX);
        vector<int> par(node_size, -1);
        int limit = node_size / 10 * (id + 1);
        if (id + 1 == CONSUMER_COUNT)
            limit = node_size;
        for (int i = node_size / 10 * id; i < limit; i++)
        {
            dist[i] = 0;
            par[i] = i;
            s.insert({dist[i], i});
        }

        while (!s.empty())
        {
            int u = s.begin()->second;
            s.erase({dist[u], u});
            int nodePtr = NodeArr[u];
            while (nodePtr != -1)
            {
                int node = EdgeArr[nodePtr].to;

                if (dist[node] > dist[u] + 1)
                {
                    s.erase({dist[node], node});
                    dist[node] = dist[u] + 1;
                    s.insert({dist[node], node});
                    par[node] = u;
                }
                nodePtr = EdgeArr[nodePtr].next;
            }
        }
        seen_dist = dist;
        seen_par = par;
    };

    auto BFS = [&node_size, &NodeArr, &edge_size, &EdgeArr](const vector<int> &sources)
    {
        queue<int> Q;
        vector<bool> visited(node_size, false);

        for (int source : sources)
            visited[source] = true, Q.push(source);
        while (!Q.empty())
        {
            int u = Q.front();
            Q.pop();
            int edgePtr = NodeArr[u];
            while (edgePtr != -1)
            {
                int v = EdgeArr[edgePtr].to;
                if (!visited[v] && seen_dist[v] > seen_dist[u] + 1)
                {
                    seen_dist[v] = seen_dist[u] + 1;
                    seen_par[v] = u;
                    visited[v] = true;
                    Q.push(v);
                }
                edgePtr = EdgeArr[edgePtr].next;
            }
        }
    };

    auto set_priority = [&node_size, &NodeArr, &edge_size, &EdgeArr](vector<int> &non_sources)
    {
        vector<pair<int, int>> new_nodes;
        for (int i = 0; i < non_sources.size(); i++)
        {
            int nodePtr = NodeArr[non_sources[i]], affinity = 0;
            while (nodePtr != -1)
            {
                int node = EdgeArr[nodePtr].to;
                if (node < seen_nodes)
                {
                    affinity++;
                }
                nodePtr = EdgeArr[nodePtr].next;
            }
            new_nodes.push_back({affinity, non_sources[i]});
        }
        sort(new_nodes.begin(), new_nodes.end(), greater<pair<int, int>>());

        for (int i = 0; i < new_nodes.size(); i++)
        {
            non_sources[i] = new_nodes[i].second;
        }
    };
    auto start = high_resolution_clock::now();
    if (seen_nodes == node_size)
    {
        // do nothing
    }
    else if (!optimize || seen_nodes == 0)
    {
        dijkstra(id);
        auto stop = high_resolution_clock::now();
    }
    else
    {
        seen_par.resize(node_size);
        seen_dist.resize(node_size);
        vector<int> new_sources, new_non_sources;

        for (int i = seen_nodes; i < node_size; i++)
        {
            if (i % 10 == id)
            {
                new_sources.push_back(i);
                seen_dist[new_sources.back()] = 0;
                seen_par[new_sources.back()] = new_sources.back();
            }
            else
            {
                new_non_sources.push_back(i);
                seen_dist[new_non_sources.back()] = INT32_MAX;
                seen_par[new_non_sources.back()] = -1;
            }
        }

        BFS(new_sources);

        set_priority(new_non_sources);

        optimized_dijkstra(new_non_sources, NodeArr, EdgeArr, node_size, edge_size);
    }
    seen_nodes = node_size;

    auto stop = high_resolution_clock::now();
    avg_time_taken = (avg_time_taken * iterations + duration_cast<microseconds>(stop - start).count()) / (iterations + 1);
    cout << "Time taken by Consumer " << id << ": " << setw(10) << avg_time_taken << " microseconds" << endl;
    iterations++;

    if (!myfile.is_open())
    {
        perror("Unable to open file");
        return;
    }

    for (int i = 0; i < node_size; i++)
    {
        int node = i, distance = 0;
        myfile << "Node " << node << ": " << node;
        while (node != seen_par[node] && node != -1)
        {
            node = seen_par[node];
            distance++;
            myfile << " -> " << node;
        }
        myfile << endl;
        myfile << "Distance: " << distance << endl;
    }

    myfile << string(100, '-') << endl;
    myfile << string(100, '-') << endl;

    myfile.close();
}