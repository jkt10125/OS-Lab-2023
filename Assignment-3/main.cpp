// Ass-4
// Group 18
// 20CS30024
// 20CS10083
// 20CS10089
// 20CS10001

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/shm.h>
#include <algorithm>

using namespace std;

int marker = (1 << 30);

// Flattening our 2D vector into integer array
int *Encode(std::vector<std::vector<int>> &adj, int &len) {
    int *a = (int *) malloc(sizeof(int));
    a[0] = '\0';

    len = 0;

    for (int i = 0; i < adj.size(); i++) {
        a = (int *) realloc(a, sizeof(int) * (adj[i].size() + len + 2));
        a[len++] = (i | marker);
        for (int it : adj[i]) {
            a[len++] = it;
        }
        a[len] = '\0';
    }
    return a;
}

std::vector<std::vector<int>> Decode(int *a, int len) {
    int mmn = 0; // Maximum Marked Node
    for (int i = 0; i < len; i++) {
        if (a[i] & marker) {
            mmn = std::max(mmn, (a[i] ^ marker));
        }
    }
    
    std::vector<std::vector<int>> adj(mmn + 1);
    int curr = 0;
    for (int i = 0; i < len; i++) {
        if (a[i] & marker) {
            curr = (a[i] ^ marker);
        }
        else {
            adj[curr].push_back(a[i]);
        }
    }

    return adj;
}


void vector_to_shm(int &shmid, key_t keyid, int *input, int size) {
    shmid = shmget(keyid, (size + 1) * sizeof(int), IPC_CREAT | 0666);

    int *output = (int *) shmat(shmid, NULL, 0);

    output[0] = size;
    for(int i = 0; i < size; i++) {
        output[i + 1] = input[i];
    }
}

int *vector_from_shm(int &shmid, key_t keyid, int &size){
    shmid = shmget(keyid, sizeof(int), IPC_CREAT | 0666);
    int *output = (int *) shmat(shmid, NULL, 0);
    size = output[0];

    shmid = shmget(keyid, (size + 1) * sizeof(int), IPC_CREAT | 0666);
    output = (int *) shmat(shmid, NULL, 0);

    // std::cout << size;
    return &output[1];
}

void destroy_shmid(int shmid, int* &ptr) {
    // Detach the memory segment mat from the address space of this process
    shmdt(ptr);
    // Mark the segment identified by shmid to be destroyed
    shmctl(shmid, IPC_RMID, NULL);
}

int main(int argc, char *argv[])
{
    // The result of the read is placed in here
    // In C++, we use a vector like an array but vectors can dynamically grow
    // as required when we get more edges.
    std::vector<std::vector<int>> edges;

    // Giving the command line arguments to filepath
    std::ifstream file(argv[1]);

    std::string line;
    // Read one line at a time into the variable line:
    while(std::getline(file, line))
    {
        std::vector<int> lineData;
        std::stringstream lineStream(line);

        int value;
        // Read an integer at a time from the line
        while(lineStream >> value)
        {
            // Add the integers from a line to a 1D array (vector)
            lineData.push_back(value);
        }
        // When all the integers have been read, add the 1D array
        // into a 2D array (as one line in the 2D array)
        edges.push_back(lineData);
    }

    int max_node = 0;

    // 
    for (std::vector<int> it : edges) {
        max_node = std::max({max_node, it[0], it[1]});
    }

    std::vector<std::vector<int>> adj(max_node + 1);
    
    for (std::vector<int> it : edges) {
        adj[it[0]].push_back(it[1]);
        adj[it[1]].push_back(it[0]);
    }

    int len;
    int *a = Encode(adj, len);
    adj.clear();

    key_t key = ftok("main.cpp", 0);

    int shmid;

    vector_to_shm(shmid, key, a, len);
    

    int size;

    int *atr = vector_from_shm(shmid, key, size);

    for (int i = 0; i < size; i++) cout << atr[i] << " ";
    
    adj = Decode(atr, size);

    cout << "Rsultant Graph after decode is : \n";

    for (int i = 0; i < adj.size(); i++) {
        cout << i << " --> ";
        for (int node : adj[i]) {
            cout << node << ' ';
        }
        cout << endl;
    }

    cout << "\nSize of the Flattened Graph: " << size << endl;

    destroy_shmid(shmid, atr);

    return 0;
}