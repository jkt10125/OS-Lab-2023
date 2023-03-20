#include "definitions.h"

using namespace std;

void initRooms(int N){
    Rooms.resize(N);
    for (int i = 0; i < Rooms.size(); i++)
    {
        Rooms[i].firstGuestTime = -1;
        Rooms[i].secondGuestTime = -1;
        Rooms[i].currentGuest = -1;
    }
}