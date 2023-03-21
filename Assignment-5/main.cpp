#include <iostream>
#include <cassert>
#include "definitions.h"

std::vector<Guest> Guests;
std::vector<Room> Rooms;
std::queue<Room *> availableRooms;
std::set<int> targetedRooms;
int RoomSize, Cleaners;
int totalOccupancy = 0;
int isCleaning = 0;
int CleanerDone = 0;
std::vector<sem_t> roomSemaphore;
std::vector<sem_t> controlSemaphore;
sem_t cleanerSemaphore;
pthread_mutex_t targetedRoomMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t availableRoomMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CleanerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t availableRoomCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t CleanerCond = PTHREAD_COND_INITIALIZER;

using namespace std;

void initSemaphores()
{
    roomSemaphore.resize(RoomSize);
    controlSemaphore.resize(RoomSize);
    for (int i = 0; i < RoomSize; i++)
    {
        sem_init(&roomSemaphore[i], 0, 1);
        sem_init(&controlSemaphore[i], 0, 0);
    }
    sem_init(&cleanerSemaphore, 0, 0);
}

int main()
{
    int X, Y, N;
    cout << "Please Enter <Number of Cleaning Staff> <Number of Guests> <Number of Rooms>" << endl;
    cin >> X >> Y >> N;
    assert(X > 0 && Y > 0 && N > 0 && Y > N && N > X);
    RoomSize = N;
    Cleaners = X;
    initGuests(Y);
    initRooms(N);
    cout<<"Initialized "<<RoomSize <<" Rooms, "<< Y <<" Guests, and "<<Cleaners<< " Cleaners Successfully"<<endl;
    initSemaphores();
    //initialize threads
    pthread_t cleanerThreads[X];
    pthread_t guestThreads[Y];

    //create guest threads
    for (int i = 0; i < Y; i++)
    {
        int *ID = new int;
        *ID = i;
        pthread_create(&guestThreads[i], NULL, simulateGuests, (void *)ID);
    }
    //create cleaner threads
    for (int i = 0; i < X; i++)
    {
        int *ID = new int;
        *ID = i;
        pthread_create(&cleanerThreads[i], NULL, simulateCleaners, (void *)ID);
    }
    //join guest threads
    for (int i = 0; i < Y; i++)
    {
        pthread_join(guestThreads[i], NULL);
    }
    //join cleaner threads
    for (int i = 0; i < X; i++)
    {
        pthread_join(cleanerThreads[i], NULL);
    }
    return 0;
}