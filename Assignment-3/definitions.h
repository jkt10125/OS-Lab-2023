#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <iostream>

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