#include "definitions.h"
#include <iomanip>
using namespace std;

void initRooms(int N)
{
    Rooms.resize(N);
    for (int i = 0; i < static_cast<int>(Rooms.size()); i++)
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
    int cleanerID = *(int *)params;
    time_t now;
    while (true)
    {
        pthread_mutex_lock(&availableRoomMutex);
        while (totalOccupancy != 2 * RoomSize)
        {
            pthread_cond_wait(&availableRoomCond, &availableRoomMutex);
        }
        pthread_mutex_unlock(&availableRoomMutex);
        pthread_mutex_lock(&omutex);
        cout << "Cleaner " << cleanerID << " started cleaning at " << put_time(localtime(&(now = time(NULL))), "%H:%M:%S") << endl;
        pthread_mutex_unlock(&omutex);

        while (true)
        {
            sem_wait(&cleanerCountSemaphore);
            int curr = roomsDone;
            if(roomsDone == RoomSize){
                sem_post(&cleanerCountSemaphore);
                break;
            }
            roomsDone++;
            sem_post(&cleanerCountSemaphore);
            int cleaningTime = static_cast<int>(ALPHA * (Rooms[curr].firstGuestTime + Rooms[curr].secondGuestTime));
            pthread_mutex_lock(&omutex);
            cout << "Cleaner " << cleanerID << " cleaning Room " << curr << " for " << cleaningTime << " seconds." << endl;
            pthread_mutex_unlock(&omutex);
            clean(cleaningTime);
            Rooms[curr].firstGuestTime = -1;
            Rooms[curr].secondGuestTime = -1;
            Rooms[curr].currentGuest = -1;
            pthread_mutex_lock(&availableRoomMutex);
            availableRooms.push(&Rooms[curr]);
            pthread_mutex_unlock(&availableRoomMutex);
        }

        pthread_mutex_lock(&omutex);
        cout << "Cleaner " << cleanerID << " finished cleaning at " << put_time(localtime(&(now = time(NULL))), "%H:%M:%S") << endl;
        pthread_mutex_unlock(&omutex);

        pthread_mutex_lock(&CleanerMutex);
        CleanerDone++;
        if (CleanerDone == Cleaners)
        {
            pthread_mutex_lock(&omutex);
            cout << "Cleaning staff finished cleaning at " << put_time(localtime(&(now = time(NULL))), "%H:%M:%S") << endl;
            pthread_mutex_unlock(&omutex);
            CleanerDone = 0;
            roomsDone = 0;
            pthread_mutex_lock(&availableRoomMutex);
            totalOccupancy = 0;
            pthread_cond_broadcast(&availableRoomCond);
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
    pthread_exit(0);
    return params;
}