#include "definitions.h"
#include <ctime>
#include <semaphore>

using namespace std;

void initGuests(int Y)
{
    Guests.resize(Y);
    for (int i = 0; i < Guests.size(); i++)
    {
        Guests[i].Priority = Guests.size() - i;
        cout << "Guest " << i << " Priority " << Guests[i].Priority << endl;
    }
}
void updateOccupancy()
{
    pthread_mutex_lock(&availableRoomMutex);
    totalOccupancy += 1;
    cout << "Total Occupancy is " << totalOccupancy << endl;
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
        cout << "Pushing Room " << room->RoomID << " into available Queue" << endl;
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
    cout << "Thread " << ID << " checking Total Occupancy" << endl;
    pthread_mutex_lock(&availableRoomMutex);
    while (totalOccupancy == 2 * RoomSize)
    {
        cout << "Thread " << ID << " waiting for Total Occupancy to be less than 2 * RoomSize" << endl;
        pthread_cond_wait(&availableRoomCond, &availableRoomMutex);
    }
    pthread_mutex_unlock(&availableRoomMutex);
    cout << "Thread " << ID << " Total Occupancy is less than 2 * RoomSize" << endl;
}
int checkIn(Room *&room, int ID)
{
    bool acquired = false;
    pthread_mutex_lock(&availableRoomMutex);
    if (!availableRooms.empty())
    {
        room = availableRooms.front();
        availableRooms.pop();
        room->currentGuest = ID;
        acquired = true;
        cout << "Guest " << ID << " Acquired Room " << room->RoomID << " Normally at time " << time(NULL) << endl;
        cout << "Accessing Room " << room->RoomID << endl;
        sem_init(&controlSemaphore[room->RoomID], 0, 0);
    }
    pthread_mutex_unlock(&availableRoomMutex);
    return acquired;
}
int evict(Room *&room, int ID, int roomID)
{
    bool acquired = false;
    sem_post(&controlSemaphore[roomID]);
    sem_wait(&roomSemaphore[roomID]);
    cout << "Thread " << ID << " Evicting Guest " << Rooms[roomID].currentGuest << " from Room " << roomID << endl;
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
                cout << "Guest " << ID << " Acquired Evicted Room " << room->RoomID << " at time " << time(NULL) << endl;
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
    cout << "Thread " << ID << " finding best Guest to Evict" << endl;
    for (int i = 0; i < RoomSize; i++)
    {
        cout << "Thread " << ID << " Checking Room " << i << endl;
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
            if (roomID != -1)
            {
                acquired = evict(room, ID, roomID);
            }
            if (!acquired)
            {
                cout << "Thread " << ID << " Waiting for a room to be available" << endl;
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
                cout << "Guest " << ID << " Acquired Room " << room->RoomID << " at time " << time(NULL) << " after waiting" << endl;
                acquired = true;
                pthread_mutex_unlock(&availableRoomMutex);
            }
        }
        timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += stayTime;
        cout << "Guest " << ID << " Waiting for " << stayTime << " seconds" << endl;
        cout << "Guest " << ID << " is in Room " << room->RoomID << endl;
        int result = sem_timedwait(&controlSemaphore[room->RoomID], &timeout);
        if (result == -1)
        {
            cout << "Guest " << ID << " is leaving Room " << room->RoomID << " at time " << time(NULL) << endl;
        }
        else
        {
            cout << "Guest " << ID << " is kicked out of Room " << room->RoomID << " at time " << time(NULL) << endl;
        }
        updateRoom(room, stayTime);
        updateOccupancy();
    }
    exit(0);
}