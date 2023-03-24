#include "definitions.h"
#include <ctime>
#include <semaphore>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;

/**
 * @brief Initializes Y Guests each with distinct priority randomly allocated in [1, Y]
 * @param Y Number of Guests
 */
void initGuests(int Y)
{
    int arr[Y];
    for (int i = 0; i < Y; i++)
    {
        arr[i] = i + 1;
    }
    shuffle(arr, arr + Y, default_random_engine(time(NULL)));

    Guests.resize(Y);
    for (int i = 0; i < static_cast<int>(Guests.size()); i++)
    {
        Guests[i].GuestID = i;
        Guests[i].Priority = arr[i];
        cout << "Guest " << Guests[i].GuestID << " Priority " << Guests[i].Priority << endl;
    }
}
void updateOccupancy()
{
    pthread_mutex_lock(&availableRoomMutex);
    totalOccupancy += 1;
    pthread_mutex_lock(&omutex);
    cout << "Total Occupancy is " << totalOccupancy << endl;
    pthread_mutex_unlock(&omutex);
    if (totalOccupancy == 2 * RoomSize)
    {
        sem_init(&cleanerSemaphore, 0, 0);
        pthread_cond_broadcast(&availableRoomCond);
    }
    pthread_mutex_unlock(&availableRoomMutex);
}
void updateRoom(Room *&room, int stayTime)
{
    pthread_mutex_lock(&targetedRoomMutex);
    sem_wait(&roomSemaphore[room->RoomID]);
    room->currentGuest = -1;
    if (room->firstGuestTime == -1)
    {
        room->firstGuestTime = stayTime;
    }
    else
    {
        room->secondGuestTime = stayTime;
    }
    if (targetedRooms.find(room->RoomID) == targetedRooms.end() && room->secondGuestTime == -1)
    {
        pthread_mutex_lock(&omutex);
        cout << "Pushing Room " << room->RoomID << " into available Queue" << endl;
        pthread_mutex_unlock(&omutex);
        pthread_mutex_lock(&availableRoomMutex);
        availableRooms.push(room);
        pthread_cond_broadcast(&availableRoomCond);
        pthread_mutex_unlock(&availableRoomMutex);
    }
    else
    {
        targetedRooms.erase(room->RoomID);
    }
    pthread_mutex_unlock(&targetedRoomMutex);
    sem_post(&roomSemaphore[room->RoomID]);
}
void checkOccupancy(int ID)
{
    pthread_mutex_lock(&omutex);
    cout << "Guest " << ID << " requesting for a room" << endl;
    pthread_mutex_unlock(&omutex);
    pthread_mutex_lock(&availableRoomMutex);
    while (totalOccupancy == 2 * RoomSize)
    {
        pthread_mutex_lock(&omutex);
        cout << "Guest " << ID << " waiting for cleaning staff to finish" << endl;
        pthread_mutex_unlock(&omutex);
        pthread_cond_wait(&availableRoomCond, &availableRoomMutex);
    }
    pthread_mutex_unlock(&availableRoomMutex);
}
int checkIn(Room *&room, int ID)
{
    bool acquired = false;
    time_t now;
    pthread_mutex_lock(&availableRoomMutex);
    if (!availableRooms.empty())
    {
        room = availableRooms.front();
        availableRooms.pop();
        room->currentGuest = ID;
        acquired = true;
        pthread_mutex_lock(&omutex);
        cout << "Guest " << ID << " acquired Room " << room->RoomID << " at " << put_time(localtime(&(now = time(NULL))), "%H:%M:%S") << endl;
        pthread_mutex_unlock(&omutex);
        sem_init(&controlSemaphore[room->RoomID], 0, 0);
    }
    else
    {
        pthread_mutex_lock(&omutex);
        cout << "No empty room available for Guest " << ID << endl;
        pthread_mutex_unlock(&omutex);
    }
    pthread_mutex_unlock(&availableRoomMutex);
    return acquired;
}
int evict(Room *&room, int ID, int roomID)
{
    bool acquired = false;
    time_t now;
    sem_post(&controlSemaphore[roomID]);
    sem_wait(&roomSemaphore[roomID]);
    pthread_mutex_lock(&omutex);
    cout << "Guest " << ID << " evicting Guest " << Rooms[roomID].currentGuest << " from Room " << roomID << endl;
    pthread_mutex_unlock(&omutex);
    sem_post(&roomSemaphore[roomID]);
    while (true)
    {
        sem_wait(&roomSemaphore[roomID]);
        if (Rooms[roomID].currentGuest == -1)
        {
            if (Rooms[roomID].secondGuestTime == -1)
            {
                Rooms[roomID].currentGuest = ID;
                room = &Rooms[roomID];
                pthread_mutex_lock(&omutex);
                cout << "Guest " << ID << " acquired Evicted Room " << room->RoomID << " at " << put_time(localtime(&(now = time(NULL))), "%H:%M:%S") << endl;
                pthread_mutex_unlock(&omutex);
                sem_init(&controlSemaphore[room->RoomID], 0, 0);
                acquired = true;
            }
            sem_post(&roomSemaphore[roomID]);
            break;
        }
        sem_post(&roomSemaphore[roomID]);
    }
    return acquired;
}
int identifyGuestToEvict(int ID)
{
    int roomID = -1;
    int priority = -1;
    pthread_mutex_lock(&targetedRoomMutex);
    pthread_mutex_lock(&omutex);
    cout << "Guest " << ID << " finding a guest to evict" << endl;
    pthread_mutex_unlock(&omutex);
    for (int i = 0; i < RoomSize; i++)
    {
        sem_wait(&roomSemaphore[i]);
        if (Rooms[i].currentGuest >= 0 && Guests[Rooms[i].currentGuest].Priority < Guests[ID].Priority && Guests[Rooms[i].currentGuest].Priority > priority && targetedRooms.find(i) == targetedRooms.end())
        {
            priority = Guests[Rooms[i].currentGuest].Priority;
            roomID = i;
        }
        sem_post(&roomSemaphore[i]);
    }
    if (roomID != -1)
    {
        targetedRooms.insert(roomID);
    }
    pthread_mutex_unlock(&targetedRoomMutex);
    return roomID;
}
void *simulateGuests(void *params)
{
    int ID = *(int *)params;
    srand(ID);
    Room *room;
    time_t now;

    while (true)
    {
        sleep(rand() % 11 + 10);
        int stayTime = rand() % 21 + 10;

        checkOccupancy(ID);

        bool acquired = false;
        int roomID = -1;

        acquired = checkIn(room, ID);

        if (!acquired)
        {
            roomID = identifyGuestToEvict(ID);
            if (roomID == -1)
            {
                pthread_mutex_lock(&omutex);
                cout << "Guest " << ID << " can't evict any guest" << endl;
                pthread_mutex_unlock(&omutex);
            }
            else
            {
                acquired = evict(room, ID, roomID);
            }
            if (!acquired)
            {
                pthread_mutex_lock(&omutex);
                cout << "Guest " << ID << " waiting for a room to be available" << endl;
                pthread_mutex_unlock(&omutex);
                pthread_mutex_lock(&availableRoomMutex);
                while (availableRooms.empty() && totalOccupancy != 2 * RoomSize)
                {
                    pthread_cond_wait(&availableRoomCond, &availableRoomMutex);
                }
                if (totalOccupancy == 2 * RoomSize)
                {
                    pthread_mutex_unlock(&availableRoomMutex);
                    continue;
                }
                room = availableRooms.front();
                availableRooms.pop();
                room->currentGuest = ID;
                sem_init(&controlSemaphore[room->RoomID], 0, 0);
                pthread_mutex_lock(&omutex);
                cout << "Guest " << ID << " acquired Room " << room->RoomID << " at " << put_time(localtime(&(now = time(NULL))), "%H:%M:%S") << " after waiting" << endl;
                pthread_mutex_unlock(&omutex);
                acquired = true;
                pthread_mutex_unlock(&availableRoomMutex);
            }
        }
        timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += stayTime;
        pthread_mutex_lock(&omutex);
        cout << "Guest " << ID << " staying for " << stayTime << " seconds" << endl;
        pthread_mutex_unlock(&omutex);
        int result = sem_timedwait(&controlSemaphore[room->RoomID], &timeout);
        pthread_mutex_lock(&omutex);
        if (result == -1 && errno == ETIMEDOUT)
        {
            cout << "Guest " << ID << " is leaving Room " << room->RoomID << " at " << put_time(localtime(&(now = time(NULL))), "%H:%M:%S") << endl;
        }
        else
        {
            cout << "Guest " << ID << " is kicked out of Room " << room->RoomID << " at " << put_time(localtime(&(now = time(NULL))), "%H:%M:%S") << endl;
        }
        pthread_mutex_unlock(&omutex);
        updateRoom(room, stayTime);
        updateOccupancy();
    }
    pthread_exit(0);
    return params;
}