#ifndef GOODMALLOC_HPP
#define GOODMALLOC_HPP

#include <string>
#include <vector>
#include <stack>
#include <map>

using namespace std;

// Structure for the callee function information
struct fInfo {
    string name;
    vector<int> data;
};

// vector<pair<fInfo, string>> SymbolTable;


struct element {
    int data;
    element *prev, *next;
};

// vector<pair<fInfo, vector<pair<string, void *>>>> PT;

vector<pair<fInfo, vector<pair<string, pair<int, void *>>>>> PT;

// variadic function 
void *createMem(int size);

element *createList(string name, int size);

void assignVal(string name, int index, int val);

void freeElem(string name);

void updatePageTable(fInfo f, int size, void *ptr);

bool memory_is_occupied(const void *, size_t);

#endif