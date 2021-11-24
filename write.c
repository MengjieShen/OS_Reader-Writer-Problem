#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "helper.h"
  
int main(int argc, char *argv[]) 
{ 
    int id;
    key_t key1, key2, key3;
    key1 = 9999;
    key2 = 41777;
    key3 = 23;
    int shmID;
    int infoID;
    int readCount;
    int* readptr;

    sem_t *sem1 = sem_open("/order", O_CREAT, 0666, 1);
    sem_t *sem2 = sem_open("/wrt", O_CREAT, 0666, 1);
    sem_t *sem3 = sem_open("/mutex", O_CREAT, 0666, 1);


    /* load the shared memory */
    shmID = shmget(key1, sizeof(struct logData) , 0666|IPC_CREAT);

    //check for faiure (no segment found with that key)
    if (shmID < 0)
    {
        perror("shmget failure");
        exit(1);
    }

    //attach the memory segment
    struct logData *p = (struct logData *) shmat(shmID, NULL, 0);
    if((int) p < 0){
        printf("shmat() failed \n");
        exit(1);
    }

    infoID = shmget(key2, sizeof(struct studentInfo)*50, 0666|IPC_CREAT);/* get shared memory to store data*/
    if (infoID < 0) {
        perror("create: shmget failed");
        exit(1);
    }

    readCount = shmget(key3, sizeof(int), 0666|IPC_CREAT);
    if (readCount < 0) {
        perror("create: shmget failed for read_count");
        exit(1);
    }

    // attach read_count memory
    readptr = (int*)shmat(readCount, 0, 0);
    if (readptr <= (int*)(0)) {
        perror("create: shmat failed for read_count");
        exit(2);
    }

    sem_wait(sem1);
    sem_wait(sem2);
    sem_post(sem1);
    printf("Write Data : "); 
    // gets(readptr);  //get input from user
    *readptr = 3;
    printf("Data written in memory: %d\n",*readptr); 
    
    sem_post(sem2);
    //detach from shared memory  
    shmdt(p); 
    shmdt(readptr); 
  
    return 0; 
} 
