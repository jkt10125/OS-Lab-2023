#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP
#define ALPHA 0.2

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
    int GuestID;
};

extern pthread_mutex_t targetedRoomMutex;
extern pthread_mutex_t availableRoomMutex;
extern pthread_mutex_t CleanerMutex;
extern pthread_mutex_t omutex;
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
void updateOccupancy();
void updateRoom(Room *&room, int stayTime);
void checkOccupancy(int ID);
int checkIn(Room *&room, int ID);
int evict(Room *&room, int ID, int roomID);
int identifyGuestToEvict(int ID);
void *simulateGuests(void *params);
void *simulateCleaners(void *params);
void clean(int T);

#endif