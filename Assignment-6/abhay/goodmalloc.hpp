#ifndef _GOODMALLOC_H
#define _GOODMALLOC_H
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
using namespace std;

struct PageTableEntry
{
    uint32_t mem_offset; // index in memory
    uint32_t total_size; // total number of bits used in memory
    uint32_t next;       // 31 bits idx to the next pagetable_entry, last bit saying if this block is to not be freed or not
};

struct PageTable
{
    int head_idx;
    int tail_idx;
    uint32_t curr_size;
    uint32_t max_size;
    PageTableEntry *arr;

    void init(int, PageTableEntry *);               // constructs
    int insert(uint32_t addr, uint32_t total_size); // inserts at the tail of the list
    void remove(uint32_t idx);                      // removes entry at idx
    void unmark(uint32_t idx);                      // unmakrs entry at idx
    void print();                                   // prints the page table
};
struct StackEntry
{
    PageTableEntry *redirect; // pointer to the pagetable_entry
    int scope_tbf;            // first 31 bits scope number, last bit tells us if the entry has to be freed
    char name[20];
};

struct Stack
{
    StackEntry *arr;                                      // array implementation of Stack
    int top;                                              // index of top in arr
    int max_size;                                         // max size of the Stack
    void init(int, StackEntry *);                         // constructor
    void push(PageTableEntry *, const char *);            // pushes an entry onto the Stack
    StackEntry *pop();                                    // pops an entry from the Stack
    PageTableEntry *peek();                               // returns the top of the Stack
    PageTableEntry *getPageTableEntry(const char *, int); // returns the pagetable_entry with the given name
    void trace();                                         // prints the Stack
};

extern PageTable *PAGE_TABLE;
extern Stack *GLOBAL_STACK;
extern const int max_stack_size;
extern int *MAIN_MEMORY;
extern uint32_t main_memory_size;
extern int *BOOKKEEPING_MEMORY;
extern const int bookkeeping_memory_size;
extern int CURRENT_SCOPE;
extern int unmarkedCounter;

void createMem(int);                                   // A function to create a memory segment using malloc
void createList(const char *, int);                    // Returns the page table entry. Using this function you can create an array of the above types. These variables reside in the memory created by createMem.
void assignVal(const char *, int, uint32_t, int = -1); // Pass the page table entry. Assign values to array or array elements. Have a light typechecking, your boolean variable cannot hold an int etc
uint32_t accessVal(const char *, int = 0, int = -1);   // returns the value of the variable
void freeElem(const char * = NULL);                    // Mark the element to be freed by the garbage collector
void freeMem();                                        // Free the memory segment created by createMem // Extra                                      // Needs to be called by the programmer to indicate the end of a scope
void freeElemUtil(PageTableEntry *var);                // called by cleanBookkeepers to remove the element from the tables and the memory
void print_main_memory();                              // prints the main memory
void startScope();                                     // Needs to be called by the programmer to indicate the start of a new scope
void endScope();
int getScope();
void cleanBookkeepers();
void coalesce();
int CreatePartitionMainMemory(int size);
void FreePartitionMainMemory(int idx);

#endif