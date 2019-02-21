#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 

struct msg_buffer 
{ 
    int qid;
    long msg_type; 
    char msg_text[1024]; 
}; 

int main(void)
{
    struct msg_buffer msg_rcv, msg_snd;
    key_t key;
    int myQid;
    char dataToBeRead[1024];
    
    if((key= ftok("/home/kedar/iiitb/sem8/RTOS-master/RTOS-master/EchoEngine/server.c", 65))==-1)
    {
        perror("ftok");
        exit(1);
    }
    
    if((myQid = msgget(key, 0666 | IPC_CREAT))==-1) 
    {
        perror("msgget");
        exit(1);
    }
    
    printf("SERVER ID %d\n", myQid);
    
    while(1) 
    {
        msgrcv(myQid, &msg_rcv, sizeof(struct msg_buffer), 0, 0); 

        printf("Got: %s\n", msg_rcv.msg_text);
        char * pch = strtok (msg_rcv.msg_text," "); 
        char fname[1024];
        
        while(pch != NULL)
        {
        
            strcpy(fname,pch);
            pch = strtok (NULL, " ");
        }

        size_t ln = strlen(fname) - 1;
        
        if (*fname && fname[ln] == '\n')  
            fname[ln] = '\0';
        
        printf("FILE NAME: %s\n",fname);
        FILE *fp;
        fp = fopen(fname,"r");
        
        if ( fp == NULL )  //file not found
        { 
            printf( "%s file failed to open.\n",fname ) ; 
            perror("Couldn't open file");
        } 
        else
        { 
            printf("The file is now opened.\n") ; 
            fgets( dataToBeRead, 50, fp ); 
        } 
        
        fclose(fp);

        msg_snd=msg_rcv; 
        strcpy(msg_snd.msg_text,dataToBeRead); 
        //msg_snd.msg_type=1;
        
        printf("Sending: %s\n", msg_snd.msg_text); 
        int client_id=msg_rcv.qid;
        //printf("%d\n", client_id);
        
        if(msgsnd(client_id, &msg_snd, sizeof(struct msg_buffer),0)==-1)
        {
            perror("msgsnd");
            exit(1);
        }

        printf("Sent\n");
    }



}
