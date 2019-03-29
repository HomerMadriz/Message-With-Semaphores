#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "semaphores.h"

#define TAMBUFFER 50

void emisor();
void receptor();
char * buff;
SEM_ID send, receive;

int main()
{
    int pid;
    int status; 
    int i; 
    int shm_id; 

    shm_id = shmget(IPC_PRIVATE, sizeof(char) * TAMBUFFER, IPC_CREAT | 0666);
    buff = shmat(shm_id, NULL, 0);

    send = createsem((key_t) 2003, 0);
    receive = createsem((key_t) 2203, 1);

    pid = fork();
    if(pid == 0)
        emisor();

    pid = fork();
    if(pid == 0)
        receptor();

    for (i = 0; i<2; i++)
        wait(&status);
    
    shmdt(buff);
    shmctl(shm_id, IPC_RMID,NULL);
    erasesem(send);
    erasesem(receive);

    return 0;
}


void emisor()
{
    int i;
    
    for(i = 0; i < 5; i++)
    {
        semwait(receive);
        sprintf(buff, "Holi boli por %d vez\n", i);
        semsignal(send);
        printf("Mensaje enviado\n");    
    }
    exit(0);
}

void receptor()
{  
    int i;

    for(i = 0; i < 5; i++)
    {
        semwait(send);
        printf("%s",buff);
        semsignal(receive);
        printf("Mensaje Recibido\n");  
    }
    exit(0);
}