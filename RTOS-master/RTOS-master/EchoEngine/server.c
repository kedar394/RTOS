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
    int id;
    
    key = ftok("/home/kedar/iiitb/sem8/RTOS-master/RTOS-master/EchoEngine/server.c", 65);
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
    
    // printf("Sbrttgtebt %d\n", id);

    while(1)
    {
        msgrcv(id, &msg_rcv, sizeof(struct msg_buffer), 0, 0); 

        printf("Got: %s\n", msg_rcv.msg_text);

        if(msg_rcv.msg_text[0] >= 'a' && msg_rcv.msg_text[0] <= 'z')
        {
            msg_rcv.msg_text[0] = (char)(msg_rcv.msg_text[0] - 32); 
        }  
        msg_snd=msg_rcv;

        // printf("check send::: %s\n", msg_snd.msg_text); 
        int client_id=msg_rcv.qid;

        if(msgsnd(client_id, &msg_snd, sizeof(struct msg_buffer), 0) == -1) 
        {
            perror("msgsnd");
            exit(1);
        }

        printf("Sent\n");
    }



}
