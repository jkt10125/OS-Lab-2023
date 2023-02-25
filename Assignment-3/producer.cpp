#include <iostream>
#include "definitions.h"
#include <vector>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>

using namespace std;

int main()
{
    int node_shmid = shmget(key, max_node_size, IPC_CREAT | 0666);

    
    // Get the shared memory segment identifier
    int shm_id = shmget(IPC_PRIVATE, sizeof(vector<vector<int>>), S_IRUSR | S_IWUSR);
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

    // Access the shared memory segment
    vector<vector<int>> &graph = *reinterpret_cast<vector<vector<int>> *>(ptr);
    for (const auto &row : graph) {
        for (int i : row) {
            cout << i << ' ';
        }
        cout << endl;
    }

    // Detach the shared memory segment from the process' address space
    if (shmdt(ptr) == -1) {
        perror("shmdt");
        return 1;
    }

    return 0;
}
