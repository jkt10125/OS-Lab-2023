#include <iostream>
using namespace std;

void *readPostRunner(void *param)
{
    int id = *static_cast<int *>(param);
    pthread_exit(0);
    return param;
}