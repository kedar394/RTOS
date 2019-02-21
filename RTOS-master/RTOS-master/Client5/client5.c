#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/time.h>


struct msg_buffer 
{ 
  	int qid;
    long msg_type; 
    char msg_text[1024]; 
}; 
  
int main() 
{ 
    struct msg_buffer msg_rcv, msg_snd;
    msg_rcv.msg_type=1;
    msg_snd.msg_type=1;
    struct timeval start, end;
    key_t ks,kr; 


    ks = ftok("/home/kedar/iiitb/sem8/RTOS-master/RTOS-master/EchoEngine/server.c", 65); 
    kr= ftok("client5",'5');
    //printf("ID ks,kr %d %d\n",ks,kr);

    int myid,sqid;


    if((sqid= msgget(ks, 0666 | IPC_CREAT))==-1) 
    {
        perror("server:msgget");
        exit(1);
    }


    if((myid=msgget(kr, 0666|IPC_CREAT))==-1)
    {
        perror("msgget");
        exit(1);
    }
    msg_snd.qid=myid;
    printf("Input: ");
    while(fgets(msg_snd.msg_text, 198, stdin))
    {
        //msg_snd.msg_text[0]='b';

        gettimeofday(&start, NULL);
        if (msgsnd (sqid, &msg_snd, sizeof (struct msg_buffer), 0) == -1) 
        {
            perror ("client: msgsnd");
            exit (1);
        }
        printf("Recieving..\n");
        if(msgrcv(myid, &msg_rcv, sizeof(struct msg_buffer), 0, 0)==-1)
        {
            perror("client:msgrcv");
            exit(1);
        }

        printf("Op from server %s\n",msg_rcv.msg_text );
        gettimeofday(&end, NULL);

        long seconds = (end.tv_sec - start.tv_sec);
        long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

        printf("%d seconds %d microseconds\n", seconds, micros);


        printf ("Please type a message: ");
    }


    if (msgctl (myid, IPC_RMID, NULL) == -1) 
    {
        perror ("client: msgctl");
        exit (1);
    }

    printf ("Client: bye\n");






    return 0; 
}
