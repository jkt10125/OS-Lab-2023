#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>

using namespace std;

int main()
{
    // Create a shared memory segment
    int shm_id = shmget(IPC_PRIVATE, sizeof(vector<vector<int>>), IPC_CREAT | 0777);
    if (shm_id == -1) {
        perror("shmget");
        return 1;
    }

    // Attach the shared memory segment to the process' address space
    void *ptr = shmat(shm_id, nullptr, 0);
    if (ptr == (void *)-1) {
        perror("shmat");
        return 1;
    }

    // Create a vector of vectors in shared memory
    new (ptr) vector<vector<int>>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    while (1);

    // Detach the shared memory segment from the process' address space
    if (shmdt(ptr) == -1) {
        perror("shmdt");
        return 1;
    }

    // Remove the shared memory segment
    if (shmctl(shm_id, IPC_RMID, nullptr) == -1) {
        perror("shmctl");
        return 1;
    }

    return 0;
}
