#include <iostream>
#include "definitions.h"

std::vector<Guest> Guests;
std::vector<Room> Rooms;
int RoomSize;
std::vector<sem_t> roomSemaphore;

using namespace std;

void initSemaphores(){
    roomSemaphore.resize(RoomSize);
    for(int i = 0 ; i < RoomSize ; i++){
        sem_init(&roomSemaphore[i], 0, 1);
    }
}

int main()
{
    int X, Y, N;
    cout << "Please Enter <Number of Cleaning Staff> <Number of Guests> <Number of Rooms>" << endl;
    cin >> X >> Y >> N;
    RoomSize = N;
    initGuests(Y);
    initRooms(N);
    initSemaphores();
}