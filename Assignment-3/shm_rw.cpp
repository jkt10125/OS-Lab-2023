#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <bits/stdc++.h>

using namespace std;

#define BUF_SIZE 1024

int write_shm(key_t key, char *input, int size){

	int shmid = shmget(key, size, 0666|IPC_CREAT);

    // typecast into graph structure
	char *str = (char*) shmat(shmid,(void*)0,0);

    strcpy(str, input);
    // just insert into the address pointer
    // depends on the structure used

	printf("Data written in memory: %s\n",str);
	
	shmdt(str);

    return 1;
}

int main(){

    key_t key = 0x1212;
    char input[BUF_SIZE];
    cout << "Write Data : ";

	fgets(input, BUF_SIZE, stdin);

    write_shm(key, input, BUF_SIZE);
    

    //reading part
	int shmid = shmget(key, BUF_SIZE, 0666|IPC_CREAT);
    // typecast into graph struct
	char *read_str = (char*) shmat(shmid, (void*)0, 0);

	printf("Data read from memory: %s\n",read_str);
	shmdt(read_str);

    // Remember to remove this from here and add in correct location
    // else data would be deleted on execution
	shmctl(shmid, IPC_RMID,NULL);

	return 0;

}
