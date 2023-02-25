#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <iostream>
#include <sys/ipc.h>

const int max_node_size = (1 << 13);
const int max_edge_size = (1 << 21);

const int key = ftok("main.cpp", 0);
class Edge{
    int to;
    int next;
public:
    Edge(int dest, int nxt): to(dest), next(nxt){}
    ~Edge(){}
};

class SegmentTree{
    static const int MAX_SIZE = (1<<13);
    int arr[MAX_SIZE];
    int lazy[MAX_SIZE];
    public:
    SegmentTree(int* data, size_t size);
};

#endif