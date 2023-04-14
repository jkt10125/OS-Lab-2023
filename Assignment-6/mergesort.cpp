#include <iostream>
#include <time.h>
#include <iomanip>
#include "memlab.hpp"
#include <fstream>
#define MAX_SIZE 500
using namespace std;

void merge(const char *arr, int scope, int l, int m, int r)
{
    startScope();
    // create temp arrays
    int n1 = m - l + 1;
    int n2 = r - m;
    CreateArray("a", n1);
    CreateArray("b", n2);
    // copy data to temp arrays L[] and R[]
    for (int i = 0; i < n1; i++)
    {
        AssignArray("a", i, accessVar(arr, l + i, scope));
    }
    for (int j = 0; j < n2; j++)
    {
        AssignArray("b", j, accessVar(arr, m + 1 + j, scope));
    }
    // merge the temp arrays back into arr[l..r]
    int i = 0; // initial index of first subarray
    int j = 0; // initial index of second subarray
    int k = l; // initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (accessVar("a", i) <= accessVar("b", j))
        {
            AssignArray(arr, k, accessVar("a", i), scope);
            i++;
        }
        else
        {
            AssignArray(arr, k, accessVar("b", j), scope);
            j++;
        }
        k++;
    }
    // copy the remaining elements of "a"[], if there are any
    while (i < n1)
    {
        AssignArray(arr, k, accessVar("a", i), scope);
        i++;
        k++;
    }
    // copy the remaining elements of "b"[], if there are any
    while (j < n2)
    {
        AssignArray(arr, k, accessVar("b", j), scope);
        j++;
        k++;
    }
    freeElem("a");
    freeElem("b");
    endScope();
    
}

void merge_sort(const char *arr, int scope, int l, int r)
{
    startScope();
    if (l < r)
    {
        int m = l + (r - l) / 2;
        merge_sort(arr, scope, l, m);
        merge_sort(arr, scope, m + 1, r);
        merge(arr, scope, l, m, r);
    }
    endScope();
}

int main()
{

    CreateMemory(250*1024*1024);

    startScope();
    CreateArray("c", MAX_SIZE);
    for (int i = 0; i < MAX_SIZE; i++)
    {
        AssignArray("c", i, rand() % 100000+1);
    }
    
    ofstream out("output.txt");
    
    out<<"Before Sorting:"<<endl;
    for (int i = 0; i < MAX_SIZE; i++)
    {
        out << accessVar("c", i) << " ";
    }
    
    out << endl;
    merge_sort("c", getScope(), 0, MAX_SIZE - 1);
    out<<"After Sorting:"<<endl;
    for (int i = 0; i < MAX_SIZE; i++)
    {
        out << accessVar("c", i) << " ";
    }
    out << endl;
    out.close();
    freeElem("c");
    endScope();
    freeMem();
    return 0;
}

// void merge(string name, int l, int m, int r) {
//     element *itr = (element *)createMem((r - l + 1) * sizeof(element));
//     updatePageTable({__FUNCTION__, {l, m, r}}, (r - l + 1) * sizeof(element), itr);

//     CreateArray ("tmp", r - l + 1);
//     int p1 = l, p2 = m + 1, p3 = 0;
//     while (p1 <= m && p2 <= r) {
//         int v1 = getVal(name, p1);
//         int v2 = getVal(name, p2);
//         if (v1 < v2) {
//             assignVal("tmp", p3++, v1);
//             p1++;
//         }
//         else {
//             assignVal("tmp", p3++, v2);
//             p2++;
//         }
//     }
//     while (p1 <= m) {
//         assignVal("tmp", p3++, getVal(name, p1++));
//     }
//     while (p2 <= r) {
//         assignVal("tmp", p3++, getVal(name, p2++));
//     }
//     for (int i = l; i <= r; i++) {
//         assignVal(name, i, getVal("tmp", i - l));
//     }

//     freeElem("tmp");
//     freeElem();
// }

// void mergesort(string name, int l, int r) {
//     if(l >= r) return;
//     int m = l + (r - l) / 2;
//     mergesort(name, l, m);
//     mergesort(name, m + 1, r);
//     merge(name, l, m, r);
// }

// void bubblesort(string name, int l, int r) {
//     for (int i = l; i <= r; i++) {
//         for (int j = r - 1; j > l; j--) {
//             if (getVal(name, j) < getVal(name, j - 1)) {
//                 int temp = getVal(name, j);
//                 assignVal(name, j, getVal(name, j - 1));
//                 assignVal(name, j - 1, temp);
//             }
//         }
//     }
// }

// int main() {
//     updatePageTable({"global", {}}, 0, NULL);


//     element *itr = (element *)createMem(MAX_SIZE * sizeof(element));
//     updatePageTable({__FUNCTION__, {}}, MAX_SIZE * sizeof(element), itr);
    
//     CreateArray("A", MAX_SIZE);

//     printPageTable();

//     for (int i = 0; i < MAX_SIZE; i++) {
//         assignVal("A", i, rand());
//     }

//     // for (int i = 0; i < 49999; i++) {
//     //     std::cout << getVal("A", i) << " \n"[i+1==49999];
//     // }
//     time_t timestamp = time(NULL);
//     cerr << put_time(localtime(&timestamp), "at %H:%M:%S on %B %d, %Y") << endl;
//     mergesort("A", 0, 49999);
//     cerr << put_time(localtime(&(timestamp=time(NULL))), "at %H:%M:%S on %B %d, %Y") << endl;

//     // bubblesort("A", 0, 14);

//     // for (int i = 0; i < 49999; i++) {
//     //     std::cout << getVal("A", i) << " \n"[i+1==49999];
//     // }

//     freeElem();

// }