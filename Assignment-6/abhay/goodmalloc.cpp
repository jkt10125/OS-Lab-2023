#include "goodmalloc.hpp"

PageTable *PAGE_TABLE;
Stack *GLOBAL_STACK;
const int max_stack_size = 1e5;
int *MAIN_MEMORY = NULL;
uint32_t main_memory_size;
int *BOOKKEEPING_MEMORY = NULL;
const int bookkeeping_memory_size = 1e8;
int CURRENT_SCOPE;
int unmarkedCounter = 0;
int memoryInUse = 0;
int allocated = 0;
void startScope()
{
    CURRENT_SCOPE++;
}

void endScope()
{
    for (int i = GLOBAL_STACK->top; ~i; i--)
    {
        if ((GLOBAL_STACK->arr[i].scope_tbf >> 1) != CURRENT_SCOPE)
            break;
        printf("[endScope] freeing %s\n", GLOBAL_STACK->arr[i].name);
        GLOBAL_STACK->arr[i].scope_tbf |= 1;
        freeElem(GLOBAL_STACK->arr[i].name);
        GLOBAL_STACK->pop();
    }
    CURRENT_SCOPE--;
}

int getScope()
{
    return CURRENT_SCOPE;
}

void Stack::init(int mx, StackEntry *mem_block)
{
    this->arr = mem_block;
    this->top = -1;
    this->max_size = mx;
}

void Stack::push(PageTableEntry *redirect_ptr, const char *name)
{
    this->top++;
    if (this->top >= this->max_size)
    {
        printf("[Stack::push] Stack Overflow\n");
        return;
    }
    this->arr[this->top].redirect = redirect_ptr;
    this->arr[this->top].scope_tbf = CURRENT_SCOPE << 1;
    strcpy(this->arr[this->top].name, name);
}

StackEntry *Stack::pop()
{
    if (this->top == -1)
    {
        printf("[Stack::pop] Stack is empty\n");
        return NULL;
    }
    else
    {
        this->top--;
        auto ret = &this->arr[this->top + 1];
        printf("[Stack::pop] poped element \n");
        return ret;
    }
}
PageTableEntry *Stack::peek()
{
    if (~this->top)
    {
        auto ret = this->arr[this->top].redirect;
        return ret;
    }
    return NULL;
}

PageTableEntry *Stack::getPageTableEntry(const char *name, int scope)
{
    for (int i = this->top; ~i; i--)
    {
        if ((this->arr[i].scope_tbf >> 1) < scope)
            break;
        if ((this->arr[i].scope_tbf >> 1) == scope && !strcmp(this->arr[i].name, name))
        {
            auto ret = this->arr[i].redirect;
            return ret;
        }
    }
    return NULL;
}
void Stack::trace()
{
    printf("[Stack::trace]: Stack has %d entries, it looks like:\n", this->top + 1);
    for (int i = this->top; ~i; i--)
        printf("[Stack::trace]: Scope Number %d \t Redirect %ld \t To be free %d\n", this->arr[i].scope_tbf >> 1, this->arr[i].redirect - PAGE_TABLE->arr, this->arr[i].scope_tbf & 1);
}
void PageTable::init(int mx, PageTableEntry *mem_block)
{
    this->arr = mem_block;
    this->head_idx = 0;
    this->tail_idx = mx - 1;
    for (int i = 0; i + 1 < mx; i++)
    {
        this->arr[i].mem_offset = -1;
        this->arr[i].next = ((i + 1) << 1);
        this->arr[i].next |= 1;
    }
    this->arr[mx - 1].mem_offset = -1;
    this->arr[mx - 1].next = -1;
    this->curr_size = 0;
    this->max_size = mx;
}
int PageTable::insert(uint32_t addr, uint32_t total_size)
{
    if (this->curr_size == this->max_size)
    {
        printf("[PageTable::insert]: page table is full\n");
        return -1;
    }
    int idx = head_idx;
    // printf("[PageTable::insert]: Inserting at index %d\n", idx);
    this->arr[idx].mem_offset = addr;
    this->arr[idx].total_size = total_size;
    this->arr[idx].next |= 1; // mark as allocated
    head_idx = arr[idx].next >> 1;
    this->curr_size++;
    printf("[PageTable::insert]: Added page table entry %d, allocated at %d, new head %d\n", idx, addr, head_idx);
    return idx;
}
void PageTable::remove(uint32_t idx)
{
    if (idx >= this->max_size or this->curr_size <= 0)
    {
        return;
    }
    this->arr[idx].mem_offset = -1;
    this->arr[idx].next = -1;
    this->arr[idx].total_size = 0;
    this->arr[tail_idx].next = idx << 1;
    this->arr[tail_idx].next |= 1;
    tail_idx = idx;
    this->curr_size--;
    printf("[PageTable::remove]: Removed variable at index %d, marked it %d \n", idx, this->arr[idx].next & 1);
}
void PageTable::unmark(uint32_t idx)
{
    unmarkedCounter++;
    this->arr[idx].next &= -2;
    if (unmarkedCounter > 1000)
    {
        unmarkedCounter = 0;
        // cleanBookkeepers();
    }
}
void PageTable::print()
{
    printf("[PageTable::print]: Printing elements of page table to be deleted, cur total size %d\n", this->curr_size);
    for (uint32_t i = 0; i < this->max_size; i++)
        if (!(this->arr[i].next & 1))
            printf("[PageTable::print]: Entry %d: addr: %d, total_size: %d\n", i, this->arr[i].mem_offset, this->arr[i].total_size);
}

void cleanBookkeepers()
{
    for (int i = 0; i <= GLOBAL_STACK->top; i++) // remove all other entries which are to be freed from the Stack
    {
        if (GLOBAL_STACK->arr[i].scope_tbf & 1)
        {
            if (~PAGE_TABLE->arr[GLOBAL_STACK->arr[i].redirect - PAGE_TABLE->arr].mem_offset)
            {
                PAGE_TABLE->unmark(GLOBAL_STACK->arr[i].redirect - PAGE_TABLE->arr);
            }
            GLOBAL_STACK->top--;
            for (int j = i; j < GLOBAL_STACK->top; j++)
                GLOBAL_STACK->arr[j] = GLOBAL_STACK->arr[j + 1];
        }
    }
    for (uint32_t i = 0; i < PAGE_TABLE->max_size; i++)
    {
        if (!(PAGE_TABLE->arr[i].next & 1))
        {
            printf("[cleanBookkeepers]: Freeing page table entry %d\n", i);
            freeElemUtil(&PAGE_TABLE->arr[i]);
        }
    }
    coalesce();
    // print_main_memory();
    // printf("done with cleanBookkeepers\n");
}
int coalesceUtil()
{
    int coalesce_count = 0;
    // write coalesce once code here
    // traverse the list
    // at the first hole
    // copy the elements
    // remember the old address
    // find the entry in the page with the old address and update it
    int *p = MAIN_MEMORY;
    int *next = p + (*p >> 1);
    while (next != MAIN_MEMORY + main_memory_size)
    {
        if ((*p & 1) == 0 && (*next & 1) == 1)
        {
            int sz1 = *p >> 1;
            int sz2 = *next >> 1;
            memmove(p, next, sz2 << 2);

            for (uint32_t j = 0; j < PAGE_TABLE->max_size; j++)
            {
                if (PAGE_TABLE->arr[j].mem_offset + 1 == (next - MAIN_MEMORY))
                {
                    PAGE_TABLE->arr[j].mem_offset = (p - MAIN_MEMORY);
                    break;
                }
            }

            p = p + sz2;
            *p = sz1 << 1;
            *(p + sz1 - 1) = sz1 << 1;
            next = p + sz1;
            if (next < MAIN_MEMORY + main_memory_size and !(*next & 1)) // coalesce if the next block is free
            {
                sz1 = sz1 + (*next >> 1);
                *p = sz1 << 1;
                *(p + sz1 - 1) = sz1 << 1;
                next = p + sz1;
            }
            coalesce_count++;
            break;
        }
        else
        {
            p = next;
            next = p + (*p >> 1);
        }
    }
    if (coalesce_count)
    {
        printf("[coalesceUtil]: Compacted first hole\n");
        // print_main_memory();
    }

    return coalesce_count;
}
void coalesce()
{
    printf("[coalesce]: Before coalesceion\n");
    print_main_memory();
    while (coalesceUtil())
        ;
    printf("[coalesce]: Done coalesceing\n");
}

void createMem(int size)
{
    MAIN_MEMORY = new int[((size + 3) / 4)]();
    main_memory_size = ((size + 3) / 4);
    printf("[createMem]: Allocated %d bytes of data as requested\n", ((size + 3) / 4) * 4);

    BOOKKEEPING_MEMORY = new int[bookkeeping_memory_size]();
    MAIN_MEMORY[0] = (main_memory_size) << 1;
    MAIN_MEMORY[main_memory_size - 1] = (main_memory_size) << 1;

    printf("[createMem]: Allocated %d bytes of data for bookkeeping\n", bookkeeping_memory_size);

    GLOBAL_STACK = (Stack *)(BOOKKEEPING_MEMORY);
    GLOBAL_STACK->init(max_stack_size, (StackEntry *)(GLOBAL_STACK + 1));

    PageTable *S_TABLE_START = (PageTable *)(GLOBAL_STACK->arr + GLOBAL_STACK->max_size);
    PAGE_TABLE = S_TABLE_START;
    PAGE_TABLE->init(max_stack_size, (PageTableEntry *)(PAGE_TABLE + 1));

    printf("[createMem]: Setup Stack and page Table\n");
}

int CreatePartitionMainMemory(int size)
{
    // Format:
    // input size= bytes
    // Header: size (31 bits), free (1 bit)
    // Data: size (nearest mult of 4)
    // Footer: size (31 bits), free (1 bit)
    // source: https://courses.cs.washington.edu/courses/cse351/17au/lectures/25/CSE351-L25-memalloc-II_17au.pdf
    // returns idx of location of data in the memory
    // Uses First Fit
    int *p = MAIN_MEMORY;
    int newsize = (((size + 3) >> 2) << 2);
    newsize += 2 * sizeof(int);
    while ((p - MAIN_MEMORY < main_memory_size) && ((*p & 1) || ((*p << 1) < newsize)))
        p = p + (*p >> 1);
    if (p - MAIN_MEMORY >= main_memory_size)
    {
        cout << p - MAIN_MEMORY << " " << main_memory_size << endl;
        // printf("hello\n");
        return -1;
    }
    // found a block with size >= size wanted
    // printf("size: %d\n", newsize);
    int oldsize = *p << 1;               // old size of the block
    int words = newsize >> 2;            // number of 4 byte blocks we need
    *p = (words << 1) | 1;               // set the header of the new block, first 31 bits: words, last bit: 1 (in use)
    *(p + words - 1) = (words << 1) | 1; // footer: same as above
    if (newsize < oldsize)               // If some blocks are left
    {
        *(p + words) = (oldsize - newsize) >> 1;              // header of the new block, last bit 0 as free
        *(p + (oldsize >> 2) - 1) = (oldsize - newsize) >> 1; // footer of the new block, last bit is 0 as free
    }
    return (p - MAIN_MEMORY);
}
void FreePartitionMainMemory(int *ptr)
{
    *ptr = *ptr & -2;                                          // clear allocated flag
    *(ptr + (*(ptr) >> 1) - 1) = *ptr;                         // clear allocated flag
    int *next = ptr + (*ptr >> 1);                             // find next block
    if (next - MAIN_MEMORY < main_memory_size && !(*next & 1)) // if next block is free
    {
        *ptr += *next;                   // merge with next block
        *(ptr + (*ptr >> 1) - 1) = *ptr; // update boundary tag
    }
    if (ptr != MAIN_MEMORY) // there is a block before
    {
        int *prev = ptr - (*(ptr - 1) >> 1); // find previous block
        if (!(*prev & 1))                    // if previous block is free
        {
            *prev += *ptr; // merge with previous block
            *(prev + (*prev >> 1) - 1) = *prev;
        }
    }
}

void createList(const char *name, int sz)
{
    int main_memory_idx, total_size;

    total_size = 32 * sz;
    main_memory_idx = CreatePartitionMainMemory(total_size / 8);

    if (name == NULL)
    {
        printf("[createList]: Failed to create list due to empty name\n");
        exit(1);
    }

    for (int i = 0; i < GLOBAL_STACK->top; i++)
    {

        if (GLOBAL_STACK->arr[i].scope_tbf >> 1 == CURRENT_SCOPE)
        {
            if (strcmp(GLOBAL_STACK->arr[i].name, name) == 0)
            {
                printf("[createList]: Failed to create list due to name conflict\n");
                return;
            }
        }
    }
    int idx = PAGE_TABLE->insert(main_memory_idx + 1, total_size); // add plus one to account for header
    printf("[CreateArr]: Created array of total size %d at index %d\n", total_size, main_memory_idx);
    GLOBAL_STACK->push(PAGE_TABLE->arr + idx, name);

    allocated += sz * sizeof(int);
    memoryInUse = GLOBAL_STACK->top * sizeof(StackEntry) + PAGE_TABLE->curr_size * sizeof(PageTableEntry) + allocated;
}

uint32_t accessVal(const char *name, int idx, int scope)
{
    if (scope == -1)
        scope = CURRENT_SCOPE;
    PageTableEntry *var = GLOBAL_STACK->getPageTableEntry(name, scope);
    if (var == NULL)
    {
        printf("[AssignList]: Trying to access an undefined variable %s\n", name);
        exit(1);
    }

    int main_idx = var->mem_offset + idx;
    uint32_t val = *((int *)(MAIN_MEMORY + main_idx));
    // cout << "\t[AccessVar]: Accessed raw variable at index" << main_idx << " , it looks like " << std::bitset<32>(val) << "\t";
    return val;
}
void assignVal(const char *name, int idx, uint32_t val, int scope)
{
    if (scope == -1)
        scope = CURRENT_SCOPE;
    PageTableEntry *arr = GLOBAL_STACK->getPageTableEntry(name, scope);
    if (arr == NULL)
    {
        printf("[AssignList]: Trying to access an undefined variable %s\n", name);
        return;
    }
    *((int *)(MAIN_MEMORY + arr->mem_offset + idx)) = val;
    // printf("[AssignArr]: Assigned %d to array at index %d, it looks like %d\n", val, arr->mem_offset + idx, *((MAIN_MEMORY + arr->mem_offset + idx)));
}
void print_main_memory()
{
    printf("[PrintMainMemory]: Main Memory looks like\n");
    int *ptr = MAIN_MEMORY;
    while (ptr - MAIN_MEMORY < main_memory_size)
    {
        if (*ptr & 1) // allocated
        {
            printf("[PrintMainMemory]: Allocated block of size %d\n", (*ptr) >> 1);
        }
        else
        {
            printf("[PrintMainMemory]: Unallocated block of size %d\n", (*ptr) >> 1);
        }
        ptr = ptr + ((*ptr) >> 1);
    }
}

void freeElem(const char *name)
{
    if (name == NULL)
    {
        endScope();
        return;
    }
    PageTableEntry *var = GLOBAL_STACK->getPageTableEntry(name, CURRENT_SCOPE);
    if (var == NULL)
    {
        printf("[freeElem]: Trying to free a variable that is not declared\n");
        exit(0);
    }
    allocated -= PAGE_TABLE->arr[var - PAGE_TABLE->arr].total_size / 8;
    memoryInUse = GLOBAL_STACK->top * sizeof(StackEntry) + PAGE_TABLE->curr_size * sizeof(PageTableEntry) + allocated;
    PAGE_TABLE->unmark(var - PAGE_TABLE->arr);
}

void freeElemUtil(PageTableEntry *var)
{
    for (int i = 0; i <= GLOBAL_STACK->top; i++)
    {
        if (GLOBAL_STACK->arr[i].redirect == var)
        {
            GLOBAL_STACK->arr[i].scope_tbf |= 1;
            break;
        }
    }
    cout << "\t[freeElemUtil]: Freeing at main memory address: " << var->mem_offset - 1 << endl;
    if (var->mem_offset && var->mem_offset - 1 < main_memory_size)
    {
        FreePartitionMainMemory(MAIN_MEMORY + (var->mem_offset - 1)); // addr is one after the header so -1
        PAGE_TABLE->remove(var - PAGE_TABLE->arr);
    }
}
void freeMem()
{
    delete[] (MAIN_MEMORY);
    printf("[freeMem]: Main memory freed\n");
    delete[] (BOOKKEEPING_MEMORY);
    printf("[freeMem]: Book keeping memory freed\n");
}
