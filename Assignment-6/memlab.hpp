#ifndef MEMLAB_H
#define MEMLAB_H
#include <iostream>
#include <stdio.h>
#include <bitset>
#include <csignal>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
using namespace std;

struct PageTableEntry
{
    uint32_t addr_in_mem; // index in memory
    uint32_t unit_size;   // size of a unit in bits, eg bool=1, int = 32, char = 8, medium_int = 24
    uint32_t total_size;  // total number of bits used in memory
    uint32_t next;        //  31 bits idx to the next stable_entry, last bit saying if this block is to not be freed or not
    int is_free()
    {
        return this->next & 1;
    }
};
struct PageTable
{
    int head_idx;
    int tail_idx;
    uint32_t cur_size;
    uint32_t mx_size;
    PageTableEntry *arr;

    void init(int, PageTableEntry *);                                   // constructs
    int insert(uint32_t addr, uint32_t unit_size, uint32_t total_size); // inserts at the tail of the list
    void remove(uint32_t idx);                                          // removes entry at idx
    void unmark(uint32_t idx);                                          // unmakrs entry at idx
    void print();                                                       // prints the symbol table
};
struct StackEntry
{
    char name[20];
    PageTableEntry *redirect; // pointer to the stable_entry
    int scope_tbf;            // first 31 bits scope number, last bit tells us if the entry has to be freed
};
// Linked list of stable_entries to make the symbol table
// create Stack out of StackEntry
struct Stack
{
    StackEntry *arr;                                      // array implementation of Stack
    int top;                                              // index of top in arr
    int max_size;                                         // max size of the Stack
    void init(int, StackEntry *);                         // constructor
    void push(PageTableEntry *, const char *);            // pushes an entry onto the Stack
    StackEntry *pop();                                    // pops an entry from the Stack
    PageTableEntry *peek();                               // returns the top of the Stack
    PageTableEntry *getPageTableEntry(const char *, int); // returns the stable_entry with the given name
    void trace();                                         // prints the Stack
};

void CreateMemory(int);                                  // A function to create a memory segment using malloc
void CreateArray(const char *, int);          // Returns the symbol table entry. Using this function you can create an array of the above types. These variables reside in the memory created by createMem.
void AssignArray(const char *, int, uint32_t, int = -1); // Pass the symbol table entry. Assign values to array or array elements. Have a light typechecking, your boolean variable cannot hold an int etc
uint32_t accessVar(const char *, int = 0, int = -1);     // returns the value of the variable
void freeElem(const char *);                             // Mark the element to be freed by the garbage collector
void freeMem();                                          // Free the memory segment created by createMem // Extra                                      // Needs to be called by the programmer to indicate the end of a scope
void freeElem_inner(PageTableEntry *var);                // called by gc_run_inner to remove the element from the tables and the memory
extern Stack *GLOBAL_STACK;
extern PageTable *SYMBOL_TABLE;
extern uint32_t big_memory_sz;
extern int *BIG_MEMORY;                                                                             // Pointer to the start of the BIG_MEMORY, int for enforcing word allignment
extern int *BOOKKEEP_MEMORY;                                                                        // Pointer to the memory segment used for bookkeeping data structures
extern pthread_mutex_t symbol_table_mutex, stack_mutex, memory_mutex, gc_active_mutex, print_mutex; // Locks for synchronisation
extern const int bookkeeping_memory_size;                                                           // max size of bookkeeping memory
extern const int max_stack_size;                                                                    // also max size of symbol table
extern int CURRENT_SCOPE;                                                                           // current scope
extern int GC_ACTIVE;                                                                               // 0 if garbage collector is not active, 1 if active
void print_big_memory();                                                                            // prints the big memory
void startScope();                                       // Needs to be called by the programmer to indicate the start of a new scope
void endScope();   
int getScope();
void gc_run_inner();  // does the actual sweep and deletion of memory, called by gc_run
void compact_total(); // compacts the memory space until it is compacted
int compact_once();   // compacts the memory space once, removes the first gap it finds in the memory, returns 0 if nothing to compact, 1 if compacted

int CreatePartitionMainMemory(int size);
void FreePartitionMainMemory(int idx);

#endif