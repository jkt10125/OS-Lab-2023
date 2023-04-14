#include <iostream>
#include <time.h>
#include <iomanip>
#include "goodmalloc.hpp"
#include <fstream>
#define MAX_SIZE 50000
using namespace std;

void merge(const char *arr, int scope, int l, int m, int r)
{
    startScope();
    // create temp arrays
    int n1 = m - l + 1;
    int n2 = r - m;
    createList("a", n1);
    createList("b", n2);
    // copy data to temp arrays L[] and R[]
    for (int i = 0; i < n1; i++)
    {
        assignVal("a", i, accessVal(arr, l + i, scope));
    }
    for (int j = 0; j < n2; j++)
    {
        assignVal("b", j, accessVal(arr, m + 1 + j, scope));
    }
    // merge the temp arrays back into arr[l..r]
    int i = 0; // initial index of first subarray
    int j = 0; // initial index of second subarray
    int k = l; // initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (accessVal("a", i) <= accessVal("b", j))
        {
            assignVal(arr, k, accessVal("a", i), scope);
            i++;
        }
        else
        {
            assignVal(arr, k, accessVal("b", j), scope);
            j++;
        }
        k++;
    }
    // copy the remaining elements of "a"[], if there are any
    while (i < n1)
    {
        assignVal(arr, k, accessVal("a", i), scope);
        i++;
        k++;
    }
    // copy the remaining elements of "b"[], if there are any
    while (j < n2)
    {
        assignVal(arr, k, accessVal("b", j), scope);
        j++;
        k++;
    }
    freeElem();
    
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
    freeElem();
}

int main()
{

    createMem(250*1024*1024);

    startScope();
    createList("c", MAX_SIZE);
    for (int i = 0; i < MAX_SIZE; i++)
    {
        assignVal("c", i, rand() % 100000+1);
    }
    
    ofstream out("output.txt");
    
    out<<"Before Sorting:"<<endl;
    for (int i = 0; i < MAX_SIZE; i++)
    {
        out << accessVal("c", i) << " ";
    }
    
    out << endl;
    merge_sort("c", getScope(), 0, MAX_SIZE - 1);
    out<<"After Sorting:"<<endl;
    for (int i = 0; i < MAX_SIZE; i++)
    {
        out << accessVal("c", i) << " ";
    }
    out << endl;
    out.close();
    freeElem();
    freeMem();
    return 0;
}
