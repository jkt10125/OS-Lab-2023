#include "definitions.h"

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
        int stayTime = (rand() % 21 + 10);

        // acquire room part
        int i = 0;
        for (i; i < RoomSize; i++)
        {
            sem_wait(&roomSemaphore[i]);
            if (Rooms[i].currentGuest == -1 && (Rooms[i].firstGuestTime == -1 || Rooms[i].secondGuestTime == -1))
            {
                Rooms[i].currentGuest = ID;
            }
            sem_post(&roomSemaphore[i]);
        }
        sleep(stayTime);

        sem_wait(&roomSemaphore[i]);
        if (Rooms[i].currentGuest == ID){
            Rooms[i].currentGuest = -1;
            if(Rooms[i].firstGuestTime == -1){
                Rooms[i].firstGuestTime = stayTime;
            }
            else{
                Rooms[i].secondGuestTime = stayTime;
            }
        }
        // End acquire room part
    }
}