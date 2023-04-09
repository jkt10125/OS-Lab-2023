#include <bits/stdc++.h>
#include "goodmalloc.hpp"

bool memory_is_occupied(const void* ptr, size_t size) {
    const unsigned char *p = static_cast<const unsigned char *>(ptr);
    for (size_t i = 0; i < size; ++i) {
        if (p[i] != 0) return true;
    }
    return false;
}

void *createMem(int size) {
    void *mem = malloc(size);
    // error handling
    if (mem == NULL) {
        std::cout << "Error: Memory allocation failed.\n";
        exit(1);
    }
    return mem;
}

element *createList(std::string name, int size) {
    auto &entry = PT.back();
    fInfo f = entry.first;
    void *ptr = entry.second.front().second.second;
    int s = entry.second.front().second.first;

    assert (entry.second.front().first.empty());

    element *itr = (element *)ptr, *itr_prev = NULL;
    
    // create the linked list of given size in the allocated memory space
    // and assign only the unoccupied memory blocks using FIRST FIT Strategy
    for (; size > 0; itr++) {
        if (!memory_is_occupied(itr, sizeof(element))) {
            itr->data = 0;
            itr->prev = itr_prev;
            if (itr_prev != NULL) itr_prev->next = itr;
            else {
                // updatePageTable(f, name, size, itr);
                entry.second.push_back({name, {size, itr}});
            }
            itr_prev = itr;
            size--;
        }
    }
}

void assignVal(std::string name, int index, int val) {
    auto entry = PT.back();
    fInfo f = entry.first;
    void *ptr = NULL;
    int size = 0;

    for (auto it : entry.second) {
        if (it.first == name) {
            ptr = it.second.second;
            size = it.second.first;
            break;
        }
    }

    if (ptr == NULL) {
        std::cout << "Error: No such variable exists.\n";
        exit(1);
    }

    element *itr = (element *)ptr;
    for (int i = 0; i < index; i++) {
        itr = itr->next;
    }
    itr->data = val;
}

void freeElem(std::string name) {
    auto &entry = PT.back();
    fInfo f = entry.first;
    void *ptr = NULL;
    int size = 0;

    for (auto it : entry.second) {
        if (it.first == name) {
            ptr = it.second.second;
            size = it.second.first;
            // delete the entry from the page table
            entry.second.erase(std::find(entry.second.begin(), entry.second.end(), it));
            break;
        }
    }

    if (ptr == NULL) {
        std::cout << "Error: No such variable exists.\n";
        exit(1);
    }

    element *itr = (element *)ptr;
    while (size > 1) {
        itr = itr->next;
        memset(itr->prev, 0, sizeof(element));
    }
    if (size) memset(itr, 0, sizeof(element));
}

void freeElem() {
    auto entry = PT.back();
    fInfo f = entry.first;
    void *ptr = entry.second.front().second.second;

    free(ptr);
    PT.pop_back();
}

void updatePageTable(fInfo f, int size, void *ptr) {
    // update the local Page Table entry
    PT.push_back({f, {{"", {size, ptr}}}});
}