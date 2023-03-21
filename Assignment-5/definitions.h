#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <unistd.h>
#include <semaphore.h>


class Room
{
public:
    int firstGuestTime;
    int secondGuestTime;
    int currentGuest;
    int RoomID;
};

class Guest
{
public:
    int Priority;
};

extern pthread_mutex_t targetedRoomMutex;
extern pthread_mutex_t availableRoomMutex;
extern pthread_mutex_t CleanerMutex;
extern pthread_cond_t availableRoomCond;
extern pthread_cond_t CleanerCond;
extern sem_t cleanerSemaphore;
extern std::vector<Guest> Guests;
extern std::vector<Room> Rooms;
extern int RoomSize, totalOccupancy, Cleaners, CleanerDone;
extern std::vector<sem_t> roomSemaphore;
extern std::vector<sem_t> controlSemaphore;
extern std::queue<Room *> availableRooms;
extern std::set<int> targetedRooms;


void initGuests(int Y);
void initRooms(int N);
void initSemaphores();
void *simulateGuests(void *params);
void *simulateCleaners(void *params);
void clean(int T);

#endif