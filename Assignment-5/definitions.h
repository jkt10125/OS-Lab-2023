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

extern std::vector<Guest> Guests;
extern std::vector<Room> Rooms;
extern int RoomSize;
extern std::vector<sem_t> roomSemaphore;

void initGuests(int Y);
void initRooms(int N);
void initSemaphores();
void *simulateGuests(void *params);

#endif