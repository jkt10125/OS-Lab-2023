#include "definitions.h"
#include <ctime>
#include <semaphore>

using namespace std;

void initGuests(int Y)
{
    srand(time(NULL));
    Guests.resize(Y);
    for (int i = 0; i < Guests.size(); i++)
    {
        Guests[i].Priority = rand() % Y + 1;
        cout<< "Guest "<<i<<" Priority "<<Guests[i].Priority<<endl;
    }
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
        cout << "Thread " << ID << " checking Total Occupancy" << endl;
        pthread_mutex_lock(&availableRoomMutex);
        while (totalOccupancy == 2 * RoomSize)
        {
            cout << "Thread " << ID << " waiting for Total Occupancy to be less than 2 * RoomSize" << endl;
            pthread_cond_wait(&availableRoomCond, &availableRoomMutex);
        }
        pthread_mutex_unlock(&availableRoomMutex);
        cout << "Thread " << ID << " Total Occupancy is less than 2 * RoomSize" << endl;
        // acquire room part
        bool acquired = false;
        int roomID = -1;
        int priority = -1;

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

        if (!acquired)
        {
            pthread_mutex_lock(&targetedRoomMutex);
            cout << "Thread " << ID << " finding best Guest to Evict" << endl;
            for (int i = 0; i < RoomSize; i++)
            {
                cout<<"Thread "<<ID<<" Checking Room "<<i<<endl;
                sem_wait(&roomSemaphore[i]);
                if (Rooms[i].currentGuest >= 0 && Guests[Rooms[i].currentGuest].Priority < Guests[ID].Priority && Guests[Rooms[i].currentGuest].Priority > priority && targetedRooms.find(i) == targetedRooms.end())
                {
                    priority = Guests[Rooms[i].currentGuest].Priority;
                    roomID = i;
                }
                sem_post(&roomSemaphore[i]);
            }
            if (roomID != -1)
                targetedRooms.insert(roomID);
            pthread_mutex_unlock(&targetedRoomMutex);
            if (roomID != -1)
            {
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
                        pthread_mutex_lock(&targetedRoomMutex);
                        targetedRooms.erase(roomID);
                        pthread_mutex_unlock(&targetedRoomMutex);
                        sem_post(&roomSemaphore[roomID]);
                        break;
                    }
                    sem_post(&roomSemaphore[roomID]);
                }
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

        // if the guest gets a room then he stays until stay_timer expires or he is kicked out
        if (acquired)
        {
            timespec timeout;
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += stayTime;

            // Wait for the semaphore to be incremented, or for the timeout to expire
            cout<<"Guest "<<ID<<" Waiting for "<<stayTime<<" seconds"<<endl;
            int result = sem_timedwait(&controlSemaphore[room->RoomID], &timeout);
            if (result == -1)
            {
                if (errno == ETIMEDOUT)
                {
                    cout << "Guest " << ID << " is leaving Room " << room->RoomID << " at time " << time(NULL) << endl;
                }
                else
                {
                    perror("sem_timedwait");
                    exit(0);
                }
            }
            else
            {
                cout << "Guest " << ID << " is kicked out of Room " << room->RoomID << " at time " << time(NULL) << endl;
            }
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
            pthread_mutex_lock(&targetedRoomMutex);
            if (targetedRooms.find(room->RoomID) == targetedRooms.end() && room->secondGuestTime == -1)
            {
                cout << "Pushing Room " << room->RoomID << " into available Queue" << endl;
                pthread_mutex_lock(&availableRoomMutex);
                availableRooms.push(room);
                pthread_cond_broadcast(&availableRoomCond);
                pthread_mutex_unlock(&availableRoomMutex);
            }
            pthread_mutex_unlock(&targetedRoomMutex);
            sem_post(&roomSemaphore[room->RoomID]);

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
        // End acquire room part
    }
    exit(0);
}