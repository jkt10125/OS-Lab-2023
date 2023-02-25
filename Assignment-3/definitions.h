#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <iostream>

const int MAX_NODES = (1 << 13);
const int MAX_EDGES = (1 << 21);
const int CONSUMER_COUNT = 10;

struct Edge{
    int to;
    int next;
};

class SegmentTree{
    static const int MAX_SIZE = (1<<13);
    int arr[MAX_SIZE];
    int lazy[MAX_SIZE];
    public:
    SegmentTree(int* data, size_t size);
};

#endif