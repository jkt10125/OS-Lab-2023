#include "memlab.hpp"
Stack *GLOBAL_STACK;
PageTable *SYMBOL_TABLE;
uint32_t big_memory_sz;
int *BIG_MEMORY = NULL;                                                                      // Pointer to the start of the BIG_MEMORY, int for enforcing word allignment
int *BOOKKEEP_MEMORY = NULL;                                                                 // Pointer to the memory segment used for bookkeeping data structures
const int bookkeeping_memory_size = 1e8;                                                     // max size of bookkeeping memory
const int max_stack_size = 1e5;                                                              // also max size of symbol table
int CURRENT_SCOPE = 0;                                                                       // current scope
int GC_ACTIVE = 0;                                                                           // 0 if garbage collector is not active, 1 if active

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
        return ret; // should be processed before the next push else race condition
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
        // if((this->arr[i].scope_tbf >> 1 )!= CURRENT_SCOPE)
        //     continue;
        if ((this->arr[i].scope_tbf >> 1) == scope && !strcmp(this->arr[i].name, name))
        {
            auto ret = this->arr[i].redirect;
            return ret;
        }
    }
    return NULL;
}
void startScope()
{
    CURRENT_SCOPE++;
}
void endScope()
{
    for (int i = GLOBAL_STACK->top; ~i; i--)
    {
        if ((GLOBAL_STACK->arr[i].scope_tbf >> 1) == CURRENT_SCOPE)
        {
            printf("[endScope] freeing %s\n", GLOBAL_STACK->arr[i].name);
            SYMBOL_TABLE->unmark(GLOBAL_STACK->arr[i].redirect - SYMBOL_TABLE->arr);
            GLOBAL_STACK->arr[i].scope_tbf |= 1;
            // freeElem(GLOBAL_STACK->arr[i].name);
            GLOBAL_STACK->pop();
        }
    }
    CURRENT_SCOPE--;
}

int getScope()
{
    return CURRENT_SCOPE;
}

void Stack::trace()
{
    printf("[Stack::trace]: Stack has %d entries, it looks like:\n", this->top + 1);
    for (int i = this->top; ~i; i--)
        printf("[Stack::trace]: Scope Number %d \t Redirect %ld \t To be free %d\n", this->arr[i].scope_tbf >> 1, this->arr[i].redirect - SYMBOL_TABLE->arr, this->arr[i].scope_tbf & 1);
}
void PageTable::init(int mx, PageTableEntry *mem_block)
{
    this->arr = mem_block;
    this->head_idx = 0;
    this->tail_idx = mx - 1;
    for (int i = 0; i + 1 < mx; i++)
    {
        this->arr[i].addr_in_mem = -1;
        this->arr[i].next = ((i + 1) << 1);
        this->arr[i].next |= 1;
    }
    this->arr[mx - 1].addr_in_mem = -1;
    this->arr[mx - 1].next = -1;
    this->cur_size = 0;
    this->mx_size = mx;
}
int PageTable::insert(uint32_t addr, uint32_t unit_size, uint32_t total_size)
{
    if (this->cur_size == this->mx_size)
    {
        printf("[PageTable::insert]: Symbol table is full\n");
        return -1;
    }
    int idx = head_idx;
    // printf("[PageTable::insert]: Inserting at index %d\n", idx);
    this->arr[idx].addr_in_mem = addr;
    this->arr[idx].unit_size = unit_size;
    this->arr[idx].total_size = total_size;
    this->arr[idx].next |= 1; // mark as allocated
    head_idx = arr[idx].next >> 1;
    this->cur_size++;
    printf("[PageTable::insert]: Added symbol table entry %d, allocated at %d, new head %d\n", idx, addr, head_idx);
    return idx;
}
void PageTable::remove(uint32_t idx)
{
    if (idx >= this->mx_size or this->cur_size <= 0)
    {
        return;
    }
    this->arr[idx].addr_in_mem = -1;
    this->arr[idx].next = -1;
    this->arr[idx].total_size = 0;
    this->arr[idx].unit_size = 0;
    this->arr[tail_idx].next = idx << 1;
    this->arr[tail_idx].next |= 1;
    tail_idx = idx;
    this->cur_size--;
    printf("[PageTable::remove]: Removed variable at index %d, marked it %d \n", idx, this->arr[idx].next & 1);
}
void PageTable::unmark(uint32_t idx)
{
    this->arr[idx].next &= -2;
}
void PageTable::print()
{
    printf("[PageTable::print]: Printing elements of symbol table to be deleted, cur total size %d\n", this->cur_size);
    for (uint32_t i = 0; i < this->mx_size; i++)
        if (!(this->arr[i].next & 1))
            printf("[PageTable::print]: Entry %d: addr: %d, unit_size: %d, total_size: %d\n", i, this->arr[i].addr_in_mem, this->arr[i].unit_size, this->arr[i].total_size);
}

void gc_run_inner()
{
    for (int i = 0; i <= GLOBAL_STACK->top; i++) // remove all other entries which are to be freed from the Stack
    {
        if (GLOBAL_STACK->arr[i].scope_tbf & 1)
        {
            if (~SYMBOL_TABLE->arr[GLOBAL_STACK->arr[i].redirect - SYMBOL_TABLE->arr].addr_in_mem)
            {
                SYMBOL_TABLE->unmark(GLOBAL_STACK->arr[i].redirect - SYMBOL_TABLE->arr);
            }
            GLOBAL_STACK->top--;
            for (int j = i; j < GLOBAL_STACK->top; j++)
                GLOBAL_STACK->arr[j] = GLOBAL_STACK->arr[j + 1];
        }
    }
    for (uint32_t i = 0; i < SYMBOL_TABLE->mx_size; i++)
    {
        if (!(SYMBOL_TABLE->arr[i].next & 1))
        {
            printf("[gc_run_inner]: Freeing symbol table entry %d\n", i);
            freeElem_inner(&SYMBOL_TABLE->arr[i]);
        }
    }
    compact_total();
    // print_big_memory();
    // printf("done with gc_run_inner\n");
}
int compact_once()
{
    int compact_count = 0;
    // write compact once code here
    // traverse the list
    // at the first hole
    // copy the elements
    // remember the old address
    // find the entry in the symbol with the old address and update it
    int *p = BIG_MEMORY;
    int *next = p + (*p >> 1);
    while (next != BIG_MEMORY + big_memory_sz)
    {
        if ((*p & 1) == 0 && (*next & 1) == 1)
        {
            int sz1 = *p >> 1;
            int sz2 = *next >> 1;
            memmove(p, next, sz2 << 2);

            for (uint32_t j = 0; j < SYMBOL_TABLE->mx_size; j++)
            {
                if (SYMBOL_TABLE->arr[j].addr_in_mem + 1 == (next - BIG_MEMORY))
                {
                    SYMBOL_TABLE->arr[j].addr_in_mem = (p - BIG_MEMORY);
                    break;
                }
            }

            p = p + sz2;
            *p = sz1 << 1;
            *(p + sz1 - 1) = sz1 << 1;
            next = p + sz1;
            if (next < BIG_MEMORY + big_memory_sz and !(*next & 1)) // coalesce if the next block is free
            {
                sz1 = sz1 + (*next >> 1);
                *p = sz1 << 1;
                *(p + sz1 - 1) = sz1 << 1;
                next = p + sz1;
            }
            compact_count++;
            break;
        }
        else
        {
            p = next;
            next = p + (*p >> 1);
        }
    }
    if (compact_count)
    {
        printf("[compact_once]: Compacted first hole\n");
        // print_big_memory();
    }

    return compact_count;
}
void compact_total()
{
    printf("[compact_total]: Before compaction\n");
    print_big_memory();
    while (compact_once())
        ;
    printf("[compact_total]: Done compacting\n");
}

void CreateMemory(int size)
{
    // BIG_MEMORY = (int *)calloc(((size + 3) / 4), sizeof(int));
    BIG_MEMORY = new int[((size + 3) / 4)]();
    big_memory_sz = ((size + 3) / 4); // nearest multiple of 4 to size
    printf("[CreateMemory]: Allocated %d bytes of data as requested\n", ((size + 3) / 4) * 4);

    // BOOKKEEP_MEMORY = (int *)calloc(bookkeeping_memory_size, sizeof(int));
    BOOKKEEP_MEMORY = new int[bookkeeping_memory_size]();
    BIG_MEMORY[0] = (big_memory_sz) << 1;
    BIG_MEMORY[big_memory_sz - 1] = (big_memory_sz) << 1;
    // printf("[CreateMemory]: Big memory header and footer %d\n", BIG_MEMORY[0]);

    printf("[CreateMemory]: Allocated %d bytes of data for bookkeeping\n", bookkeeping_memory_size);

    GLOBAL_STACK = (Stack *)(BOOKKEEP_MEMORY);
    GLOBAL_STACK->init(max_stack_size, (StackEntry *)(GLOBAL_STACK + 1));

    PageTable *S_TABLE_START = (PageTable *)(GLOBAL_STACK->arr + GLOBAL_STACK->max_size);
    SYMBOL_TABLE = S_TABLE_START;
    SYMBOL_TABLE->init(max_stack_size, (PageTableEntry *)(SYMBOL_TABLE + 1));

    printf("[CreateMemory]: Setup Stack and Symbol Table\n");
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
    int *p = BIG_MEMORY;
    int newsize = (((size + 3) >> 2) << 2);
    newsize += 2 * sizeof(int);
    while ((p - BIG_MEMORY < big_memory_sz) && ((*p & 1) || ((*p << 1) < newsize)))
        p = p + (*p >> 1);
    if (p - BIG_MEMORY >= big_memory_sz)
    {
        cout << p - BIG_MEMORY << " " << big_memory_sz << endl;
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
    return (p - BIG_MEMORY);
}
void FreePartitionMainMemory(int *ptr)
{
    *ptr = *ptr & -2;                                      // clear allocated flag
    *(ptr + (*(ptr) >> 1) - 1) = *ptr;                     // clear allocated flag
    int *next = ptr + (*ptr >> 1);                         // find next block
    if (next - BIG_MEMORY < big_memory_sz && !(*next & 1)) // if next block is free
    {
        *ptr += *next;                   // merge with next block
        *(ptr + (*ptr >> 1) - 1) = *ptr; // update boundary tag
    }
    if (ptr != BIG_MEMORY) // there is a block before
    {
        int *prev = ptr - (*(ptr - 1) >> 1); // find previous block
        if (!(*prev & 1))                    // if previous block is free
        {
            *prev += *ptr; // merge with previous block
            *(prev + (*prev >> 1) - 1) = *prev;
        }
    }
}

void CreateArray(const char *name, int sz)
{
    int main_memory_idx, unit_size, total_size;

    unit_size = 32;
    total_size = unit_size * sz;
    main_memory_idx = CreatePartitionMainMemory(total_size / 8);

    if (name == NULL)
    {
        printf("[CreateList]: Failed to create list due to empty name\n");
        exit(1);
    }

    for (int i = 0; i < GLOBAL_STACK->top; i++)
    {

        if (GLOBAL_STACK->arr[i].scope_tbf >> 1 == CURRENT_SCOPE)
        {
            if (strcmp(GLOBAL_STACK->arr[i].name, name) == 0)
            {
                printf("[CreateList]: Failed to create list due to name conflict\n");
                return;
            }
        }
    }
    int idx = SYMBOL_TABLE->insert(main_memory_idx + 1, unit_size, total_size); // add plus one to account for header
    printf("[CreateArr]: Created array of size %d, total size %d at index %d\n", unit_size, total_size, main_memory_idx);
    GLOBAL_STACK->push(SYMBOL_TABLE->arr + idx, name);

}

uint32_t accessVar(const char *name, int idx, int scope)
{
    if (scope == -1)
        scope = CURRENT_SCOPE;
    PageTableEntry *var = GLOBAL_STACK->getPageTableEntry(name, scope);
    if (var == NULL)
    {
        printf("[AssignList]: Trying to access an undefined variable %s\n", name);
        exit(1);
    }

    int correct_unit_size = var->unit_size;

    int main_idx = var->addr_in_mem + (idx * correct_unit_size) / 32;
    // cout << "main idx: " << main_idx << endl;
    int offset = (idx * correct_unit_size) % 32;
    int end_offset = (offset + correct_unit_size - 1) % 32;
    uint32_t val = *((int *)(BIG_MEMORY + main_idx));
    // printf(, main_idx, val);
    // cout << "\t[AccessVar]: Accessed raw variable at index" << main_idx << " , it looks like " << std::bitset<32>(val) << "\t";
    // cout<<"...."<<offset<<" "<<end_offset<<".....\n";
    // start from offset and read till unit_size
    val = ~((1L << (31 - end_offset)) - 1) & val; // remove all bits after offset  val looks like  ......usefulf000000
    // cout<< std::bitset<32>(val)<<"\n";
    val = val << offset; // shift left to align          val looks liek useful000000000000
    // cout<< std::bitset<32>(val)<<"\n";
    int ret = val;
    ret = ret >> (32 - var->unit_size); // shift right to align         val looks like 0000000000useful
    // cout<< std::bitset<32>(val)<<"\n";
    return ret;
}
void AssignArray(const char *name, int idx, uint32_t val, int scope)
{
    if (scope == -1)
        scope = CURRENT_SCOPE;
    PageTableEntry *arr = GLOBAL_STACK->getPageTableEntry(name, scope);
    if (arr == NULL)
    {
        printf("[AssignList]: Trying to access an undefined variable %s\n", name);
        return;
    }

    // cout<<arr->unit_size<<endl;
    if (arr->unit_size == 32)
    {
        *((int *)(BIG_MEMORY + arr->addr_in_mem + (idx * arr->unit_size) / 32)) = val;
        // printf("[AssignArr]: Assigned %d to array at index %d, it looks like %d\n", val, arr->addr_in_mem + (idx * arr->unit_size) / 32, *((BIG_MEMORY + arr->addr_in_mem + (idx * arr->unit_size) / 32)));
    }
}
void print_big_memory()
{
    printf("[PrintBigMemory]: Big Memory looks like\n");
    int *ptr = BIG_MEMORY;
    while (ptr - BIG_MEMORY < big_memory_sz)
    {
        if (*ptr & 1) // allocated
        {
            printf("[PrintBigMemory]: Allocated block of size %d\n", (*ptr) >> 1);
            // cout << "\t Header looks like" << bitset<32>(*ptr) << "\n";
            // cout << "\t";

            // for (int i = 1; i + 1 < (*ptr >> 1); i++)
            // {
            //     cout << std::bitset<32>(ptr[i]) << "\t";
            // }
            // cout << endl;
        }
        else
        {
            printf("[PrintBigMemory]: Unallocated block of size %d\n", (*ptr) >> 1);
        }
        ptr = ptr + ((*ptr) >> 1);
    }
    // printf("\n");
}

void freeElem(const char *name)
{
    PageTableEntry *var = GLOBAL_STACK->getPageTableEntry(name, CURRENT_SCOPE);
    if (var == NULL)
    {
        printf("[freeElem]: Trying to free a variable that is not declared\n");
        return;
    }
    SYMBOL_TABLE->unmark(var - SYMBOL_TABLE->arr);
    gc_run_inner();
}
void freeElem_inner(PageTableEntry *var)
{
    for (int i = 0; i <= GLOBAL_STACK->top; i++)
    {
        if (GLOBAL_STACK->arr[i].redirect == var)
        {
            GLOBAL_STACK->arr[i].scope_tbf |= 1;
            break;
        }
    }
    cout << "\t[freeElem_inner]: Freeing at big memory address: " << var->addr_in_mem - 1 << endl;
    if (var->addr_in_mem and var->addr_in_mem - 1 < big_memory_sz)
    {
        FreePartitionMainMemory(BIG_MEMORY + (var->addr_in_mem - 1)); // addr is one after the header so -1
        SYMBOL_TABLE->remove(var - SYMBOL_TABLE->arr);
        // cout<<"hi2\n";
    }
    // print_big_memory();
}
void freeMem()
{
    delete[] (BIG_MEMORY);
    printf("[freeMem]: Big memory freed\n");
    delete[] (BOOKKEEP_MEMORY);
    printf("[freeMem]: Book keeping memory freed\n");
}
