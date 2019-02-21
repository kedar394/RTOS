#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <time.h>

struct msg_buffer 
{  
    int qid;
    long msg_type; 
    char msg_text[1024]; 
}; 

int main() 
{ 
    struct msg_buffer msg_rcv, msg_snd;
    struct timeval start, finish;
    msg_rcv.msg_type=1;
    msg_snd.msg_type=1;

    key_t snd,rcv; 

    snd = ftok("/home/kedar/iiitb/sem8/RTOS-master/RTOS-master/EchoEngine/server.c", 65); 
    
    rcv = ftok("client3",'3'); 

    int myid,sqid;

    sqid = msgget(snd, 0666 | IPC_CREAT);
    if(sqid == -1) 
    {
        perror("server:msgget");
        exit(1);
    }

    myid = msgget(rcv, 0666|IPC_CREAT);
    if(myid == -1) 
    {
        perror("msgget");
        exit(1);
    }
    msg_snd.qid=myid;

    while(fgets(msg_snd.msg_text, 198, stdin)) 
    {
        gettimeofday(&start, NULL);
        if (msgsnd (sqid, &msg_snd, sizeof (struct msg_buffer), 0) == -1) 
        {
            perror ("client: msgsnd");
            exit (1);
        }
        
        printf("Waiting for response..\n");
        
        if(msgrcv(myid, &msg_rcv, sizeof(struct msg_buffer), 0, 0) == -1)
        {
            perror("client:msgrcv");
            exit(1);
        }
        gettimeofday(&finish, NULL);

        long seconds = (finish.tv_sec - start.tv_sec); 
        long micros = ((seconds * 1000000) + finish.tv_usec) - (start.tv_usec);
        printf("File content: %s\n", msg_rcv.msg_text);
        
        printf("%ld seconds %ld microseconds\n", seconds, micros);
        
    }



    if (msgctl (myid, IPC_RMID, NULL) == -1) 
    {
        perror ("client: msgctl");
        exit (1);
    }

    return 0; 
}
