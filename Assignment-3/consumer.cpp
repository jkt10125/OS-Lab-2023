#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "definitions.h"

using namespace std;

void consumerProcess(int id) {

    string filename = "consumer_" + to_string(id) + ".txt";
    ofstream myfile(filename.c_str(), ios::app);


    int node_size = getNodeCount();
    int *NodeArr = getNodeArr();

    int edge_size = getEdgeCount();
    Edge *EdgeArr = getEdgeArr();

    auto dijkstra = [&node_size, &NodeArr, &edge_size, &EdgeArr] (int id) {
        set<pair<int, int>> s; // {dist[u], u}
        vector<int> dist(node_size + 1, INT32_MAX);
        vector<int> par(node_size + 1, -1);
        for (int i = node_size / 10 * id; i < node_size / 10 * (id + 1); i++) {
            dist[i] = 0;
            par[i] = i;
            s.insert({dist[i], i});
        }
        
        while (!s.empty()) {
            int u = s.begin()->second;
            s.erase({dist[u], u});
            int nodePtr = NodeArr[u];
            while (nodePtr != -1) {
                int node = EdgeArr[nodePtr].to;

                if (dist[node] > dist[u] + 1) {
                    s.erase({dist[node], node});
                    dist[node] = dist[u] + 1;
                    s.insert({dist[node], node});
                    par[node] = u;
                }
                nodePtr = EdgeArr[nodePtr].next;
            }
        }
        
        return par;
    };

    // vector<int> par(node_size);
    vector<int> par = dijkstra(id);
    
    if (!myfile.is_open()) {
        perror("Unable to open file");
        return;
    }
    
    for (int i = 0; i < node_size; i++) {
        int node = i;
        myfile << node << " : " << node;
        while (node != par[node]) {
            node = par[node];
            myfile << ", " << node;
        }
        myfile << endl;
    }

    myfile << string(100, '-') << endl;

    myfile.close();

}