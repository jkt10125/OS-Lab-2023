#include <iostream>
#include "definitions.hpp"
using namespace std;

void pushToFriends(Action * action){
    
}
void *pushUpdateRunner(void *param)
{
    while (1)
    {
        pthread_mutex_lock(&mutexShared);
        while (isEmptyShared == true)
            pthread_cond_wait(&emptyShared, &mutexShared);
        Action *action = generatedActions.front();
        generatedActions.pop();
        pthread_mutex_unlock(&mutexShared);
        pushToFriends(action);
    }
    pthread_exit(0);
    return param;
}