#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>
#include <set>
#include <chrono>
#include "definitions.h"

using namespace std;
using namespace std::chrono;

int seen_nodes = 0;
vector<int> seen_dist;
vector<int> seen_par;

void update_graph(vector<int> new_nodes_sorted, int *NodeArr, Edge *EdgeArr, int node_size, int edge_size){
    for(int i = 0 ; i < new_nodes_sorted.size(); i++){
        int node = new_nodes_sorted[i];
        int nodePtr = NodeArr[node];
        while (nodePtr != -1) {
            int node2 = EdgeArr[nodePtr].to;
            if(seen_dist[node] > seen_dist[node2] + 1){
                seen_dist[node] = seen_dist[node2] + 1;
                seen_par[node] = node2;
            }
            nodePtr = EdgeArr[nodePtr].next;
        }
        nodePtr = NodeArr[node];
        
        while (nodePtr != -1) {
            int node2 = EdgeArr[nodePtr].to;
            if(seen_dist[node2] > seen_dist[node] + 1){
                vector<bool> visited(node_size, false);
                visited[node] = true;
                visited[node2] = true;
                seen_dist[node2] = seen_dist[node] + 1;
                seen_par[node2] = node;
                // doing bfs from node2 on nodes with seen_dist value greater than seen_dist[node] + 1
                queue<int> q;
                q.push(node2);
                while(!q.empty()){
                    int node3 = q.front();
                    q.pop();
                    int nodePtr2 = NodeArr[node3];
                    while (nodePtr2 != -1) {
                        int node4 = EdgeArr[nodePtr2].to;
                        if(!visited[node4] && seen_dist[node4] > seen_dist[node3] + 1){
                            visited[node4] = true;
                            seen_dist[node4] = seen_dist[node3] + 1;
                            seen_par[node4] = node3;
                            q.push(node4);
                        }
                        nodePtr2 = EdgeArr[nodePtr2].next;
                    }
                }
            }
            nodePtr = EdgeArr[nodePtr].next;
        }
    }
}

void consumerProcess(int id, bool optimize) {

    string filename = "consumer_" + to_string(id) + ".txt";
    ofstream myfile(filename.c_str(), ios::app);

    int node_size = getNodeCount();
    int *NodeArr = getNodeArr();

    int edge_size = getEdgeCount();
    Edge *EdgeArr = getEdgeArr();

    if (node_size == 0) {
        myfile << "No nodes in graph" << endl;
        myfile.close();
        return;
    }

    auto dijkstra = [&node_size, &NodeArr, &edge_size, &EdgeArr] (int id) {
        set<pair<int, int>> s; // {dist[u], u}
        vector<int> dist(node_size , INT32_MAX);
        vector<int> par(node_size , -1);
        int limit = node_size / 10 * (id + 1);
        if(id+1==CONSUMER_COUNT) limit = node_size;
        for (int i = node_size / 10 * id; i < limit; i++) {
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
        seen_dist = dist;
        seen_par = par;
        return par;
    };

    auto sort_new_nodes = [&node_size, &NodeArr, &edge_size, &EdgeArr] (int id) {
        vector<pair<int,int>> new_nodes(node_size - seen_nodes);
        for (int i = 0 ; i < new_nodes.size(); i++){
            new_nodes[i].first = 0;
            new_nodes[i].second = i+seen_nodes;
            int nodePtr = NodeArr[i+seen_nodes];
            while (nodePtr != -1) {
                int node = EdgeArr[nodePtr].to;
                if(node < seen_nodes){
                    new_nodes[i].first++;
                }
                nodePtr = EdgeArr[nodePtr].next;
            }
        }
        sort(new_nodes.begin(), new_nodes.end(), greater<pair<int,int>>());
        vector<int> new_nodes_sorted(node_size - seen_nodes);
        for (int i = 0 ; i < new_nodes.size(); i++){
            new_nodes_sorted[i] = new_nodes[i].second;
        }
        return new_nodes_sorted;
    };

    vector<int> par;

    if(!optimize || seen_nodes == 0){
        auto start = high_resolution_clock::now();
        par = dijkstra(id);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by Consumer "<<id<<": "<< duration.count() << " microseconds" << endl;
    }
    else{
        auto start = high_resolution_clock::now();
        seen_par.resize(node_size);
        seen_dist.resize(node_size);
        vector<int> new_sources;
        set<int> new_source_set;
        int j = 0;
        for(int i = seen_nodes; i < node_size; i++){
            if(i%10 == id){
                new_sources.push_back(i);
                seen_dist[new_sources[j]] = 0;
                seen_par[new_sources[j]] = new_sources[j];
                new_source_set.insert(new_sources[j]);
                j++;
            }
        }
        update_graph(new_sources, NodeArr, EdgeArr, node_size, edge_size);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        start = high_resolution_clock::now();
        vector<int> new_nodes_sorted = sort_new_nodes(id);
        
        for(int i = 0 ; i< new_nodes_sorted.size(); i++){
            if(new_source_set.find(new_nodes_sorted[i]) != new_source_set.end()){
                new_nodes_sorted.erase(new_nodes_sorted.begin() + i);
                i--;
                continue;
            }
            seen_dist[new_nodes_sorted[i]] = INT32_MAX;
            seen_par[new_nodes_sorted[i]] = -1;
        }
        update_graph(new_nodes_sorted, NodeArr, EdgeArr, node_size, edge_size);
        stop = high_resolution_clock::now();
        auto duration2 = duration_cast<microseconds>(stop - start);
        cout << "Time taken by Consumer "<<id<<": "<< duration.count() << " microseconds for part 1 and "<<duration2.count()<<" microseconds for process 2" << endl;
    }
    
    seen_nodes = node_size;

    if (!myfile.is_open()) {
        perror("Unable to open file");
        return;
    }
    
    for (int i = 0; i < node_size; i++) {
        int node = i, distance = 0;
        myfile << "Node " << node << ": " << node;
        while (node != seen_par[node] && node != -1) {
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