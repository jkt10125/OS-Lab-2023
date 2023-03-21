#include "definitions.h"

using namespace std;

void initRooms(int N)
{
    Rooms.resize(N);
    for (int i = 0; i < Rooms.size(); i++)
    {
        Rooms[i].firstGuestTime = -1;
        Rooms[i].secondGuestTime = -1;
        Rooms[i].currentGuest = -1;
        Rooms[i].RoomID = i;
        availableRooms.push(&Rooms[i]);
    }
}
void clean(int T)
{
    sleep(T);
}
void *simulateCleaners(void *params)
{
    while (true)
    {
        pthread_mutex_lock(&availableRoomMutex);
        while (totalOccupancy != 2 * RoomSize)
        {
            pthread_cond_wait(&availableRoomCond, &availableRoomMutex);
        }
        pthread_mutex_unlock(&availableRoomMutex);
        cout<<"Cleaner "<<*(int *)params<<" started cleaning at time "<<time(NULL)<<endl;
        int ID = *(int *)params;
        for (int i = 0; i < Rooms.size(); i++)
        {
            if (i % ID == 0)
            {
                clean(Rooms[i].firstGuestTime + Rooms[i].secondGuestTime);
                Rooms[i].firstGuestTime = -1;
                Rooms[i].secondGuestTime = -1;
                Rooms[i].currentGuest = -1;
            }
        }
        cout<<"Cleaner "<<*(int *)params<<" finished cleaning at time "<<time(NULL)<<endl;
        pthread_mutex_lock(&CleanerMutex);
        CleanerDone++;
        if (CleanerDone == Cleaners)
        {
            CleanerDone = 0;
            pthread_mutex_lock(&availableRoomMutex);
            totalOccupancy = 0;
            pthread_mutex_unlock(&availableRoomMutex);

            sem_post(&cleanerSemaphore);
            pthread_mutex_unlock(&CleanerMutex);
        }
        else
        {
            pthread_mutex_unlock(&CleanerMutex);
            sem_wait(&cleanerSemaphore);
            sem_post(&cleanerSemaphore);
        }
    }
}