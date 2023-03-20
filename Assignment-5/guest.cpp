#include "definitions.h"
#include <ctime>

using namespace std;

void initGuests(int Y)
{
    srand(time(NULL));
    Guests.resize(Y);
    for (int i = 0; i < Guests.size(); i++)
    {
        Guests[i].Priority = rand() % Y + 1;
    }
}

void *simulateGuests(void *params)
{
    srand(time(NULL));
    int ID = *(int *)params;
    while (true)
    {
        sleep(rand() % 11 + 10);
        int stayTime = rand() % 21 + 10;
        time_t exitTime = time(nullptr) + stayTime;

        // acquire room part
        int i = 0;
        bool ok = false;
        for (i; i < RoomSize; i++)
        {
            sem_wait(&roomSemaphore[i]);
            if (Rooms[i].currentGuest == -1 && (Rooms[i].firstGuestTime == -1 || Rooms[i].secondGuestTime == -1))
            {
                Rooms[i].currentGuest = ID;
                ok = true;
            }
            sem_post(&roomSemaphore[i]);
        }

        // if the guest gets a room then he stays until stay_timer expires or he is kicked out
        if (ok)
        {
            time_t currentTime;

            while (ID != kickedGuest && (currentTime = time(nullptr)) < exitTime) ;

            // if he is kicked out then he updates his kickedGuest variable
            if (ID == kickedGuest)
            {
                pthread_mutex_lock(&kickedGuestMutex);
                kickedGuest = -1;
                pthread_mutex_unlock(&kickedGuestMutex);
            }

            stayTime -= (exitTime > currentTime) ? (exitTime - currentTime) : 0;
            
            // updating the room info
            sem_wait(&roomSemaphore[i]);
            if (Rooms[i].firstGuestTime == -1)
            {
                Rooms[i].firstGuestTime = stayTime;
            }
            else
            {
                Rooms[i].secondGuestTime = stayTime;
            }
            sem_post(&roomSemaphore[i]);
        }

        // else he seeks a lower priority guest occupying a room and kicks him
        
        else {

            

        }

        // End acquire room part
    }
}