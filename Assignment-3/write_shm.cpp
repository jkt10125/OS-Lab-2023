#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <bits/stdc++.h>

using namespace std;


// // Function to print a matrix of size r x c
// void print_matrix(vector<vector<int> > mat) {
//     int r = mat.size();
//     int c;
//     cout << "[";
//     for (int i = 0; i < r; i++) {
//         cout << "[";
//         c = mat[i].size();
//         for (int j = 0; j < c; j++) {
//             cout << mat[i][j];
//             if (j != c - 1) {
//                 cout << ", ";
//             }
//         }
//         cout << (i != r - 1 ? "],\n" : "]");
//     }
//     cout << "]" << endl;
//     return;
// }

// int** get_matrix(int& shmid, int shmids[], vector<vector<int> > mat){

//     int r = mat.size();
//     int c;

//     // Create shared memory segment of size r * sizeof(int*)
//     shmid = shmget(IPC_PRIVATE, r * sizeof(int*), IPC_CREAT | 0666);

//     // Attach the memory segment mat to the address space of this process
//     int** ptr = (int**)shmat(shmid, NULL, 0);

//     for (int i = 0; i < r; i++) {
//         // Create shared memory segment of size c * sizeof(int)
//         c = mat[i].size();
//         shmids[i] = shmget(IPC_PRIVATE, c * sizeof(int), IPC_CREAT | 0666);
//         // Attach the memory segment ptr[i] to the address space of this process
//         ptr[i] = (int*)shmat(shmids[i], NULL, 0);
//         for (int j = 0; j < c; j++) {
//             // Fill ptr[i][j] with a random int value between 0 and 10
//             ptr[i][j] = mat[i][j];
//         }
//     }
//     return ptr;
// }

void print_array(int *a, int sz){
    for(int i = 0; i < sz; i++){
        cout << a[i] << " ";
    }
    cout << endl;
}

int *vector_from_shm(int& shmid, key_t keyid, int size){
    shmid = shmget(keyid, size*sizeof(int), IPC_CREAT | 0666);
    int *output = (int *)shmat(shmid, NULL, 0);
    return output;
}

int *vector_to_shm(int *input, int size, int& shmid, key_t keyid){
    shmid = shmget(keyid, size*sizeof(int), IPC_CREAT | 0666);
    int *output = (int *)shmat(shmid, NULL, 0);

    for(int i = 0; i < size; i++){
        output[i] = input[i];
    }

    return output;
    
}

void destroy_shmid(int shmid, int* &ptr) {
    // Detach the memory segment mat from the address space of this process
    shmdt(ptr);
    // Mark the segment identified by shmid to be destroyed
    shmctl(shmid, IPC_RMID, NULL);
}


int main(){


    key_t key = 0x1212;
    int *vec = (int *)malloc(10 * sizeof(int));

    for(int i = 0; i < 10; i++){
        vec[i] = i;
    }


    print_array(vec, 10);
    int shmid;
    int *ptr = vector_to_shm(vec, 10, shmid, key);
    print_array(ptr, 10);

    int *atr = vector_from_shm(shmid, key, 10);
    print_array(atr, 10);

    destroy_shmid(shmid, ptr);


	return 0;

}


// Adjascency List representation in C++

