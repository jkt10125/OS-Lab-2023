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

int &getNodeCount();
int *getNodeArr();
int &getEdgeCount();
Edge *getEdgeArr();
void detach();
void dieWithError();
void producerProcess();
void consumerProcess(int id, bool optimize);

#endif