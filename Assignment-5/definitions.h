#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <iostream>
#include <vector>
#include <unistd.h>
#include <semaphore.h>


class Room
{
public:
    int firstGuestTime;
    int secondGuestTime;
    int currentGuest;
};

class Guest
{
public:
    int Priority;
};

pthread_mutex_t kickedGuestMutex = PTHREAD_MUTEX_INITIALIZER;

extern std::vector<Guest> Guests;
extern std::vector<Room> Rooms;
extern int RoomSize, kickedGuest;
extern std::vector<sem_t> roomSemaphore;

void initGuests(int Y);
void initRooms(int N);
void initSemaphores();
void *simulateGuests(void *params);

#endif