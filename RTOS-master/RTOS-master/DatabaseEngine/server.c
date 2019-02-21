#include <unistd.h>
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


int main()
{
    struct msg_buffer msg_rcv, msg_snd;
    key_t key;
    // key_t id;

    int id;

    char fileData[1024];
    
    key = ftok("../EchoEngine/server.c", 65);

    if(key == -1)
    {
        perror("ftok");
        exit(1);
    }
    
    id = msgget(key, 0666 | IPC_CREAT);
    if(id == -1) 
    {
        perror("msgget");
        exit(1);
    }
    
    
    while(1) 
    {
        msgrcv(id, &msg_rcv, sizeof(struct msg_buffer), 0, 0); 

        // printf("File name reveived: %s\n", msg_rcv.msg_text);
        char *rctext = strtok (msg_rcv.msg_text," "); 
        
        char filename[1024];
        
        while(rctext != NULL)
        {
            strcpy(filename,rctext);
            rctext = strtok (NULL, " ");
        }

        size_t len = strlen(filename) - 1;
        
        if (*filename && filename[len] == '\n')  
        {
            filename[len] = '\0';
        }
        
        FILE *fp;
        fp = fopen(filename, "r");
        
        if (fp == NULL) 
        { 
            perror("File cnt be opened");
        } 
        else
        { 
            fgets( fileData, 50, fp ); 
        } 
        
        fclose(fp);

        msg_snd = msg_rcv; 
        strcpy(msg_snd.msg_text, fileData); 

        int client_id=msg_rcv.qid;
        
        if(msgsnd(client_id, &msg_snd, sizeof(struct msg_buffer), 0) == -1)
        {
            perror("msgsnd");
            exit(1);
        }
        // printf("Sent fine data");
    }

}
