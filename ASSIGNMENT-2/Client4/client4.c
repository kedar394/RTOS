// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


#define PORT 8888
  
int main(int argc, char const *argv[])
{
    struct timeval start, finish;
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    char user[20];

    // char *hello = "Hello from client";
    // send(sock , hello , strlen(hello) , 0 );
    // printf("Hello message sent\n");

    while(1) 
    {

        memset(buffer, 0, 1024);   
        
        scanf("%s", &buffer);
        gettimeofday(&start, NULL);

        send(sock , buffer , strlen(buffer) , 0 );
        // if (msgsnd (sqid, &msg_snd, sizeof (struct msg_buffer), 0) == -1) 
        // {
        //     perror ("client: msgsnd");
        //     exit (1);
        // }
        
        printf("Waiting for response..\n");

        memset(buffer, 0, 1024);   
        valread = read( sock , buffer, 1024);
        // if(msgrcv(myid, &msg_rcv, sizeof(struct msg_buffer), 0, 0) == -1)
        // {
        //     perror("client:msgrcv");
        //     exit(1);
        // }
        gettimeofday(&finish, NULL);

        long seconds = (finish.tv_sec - start.tv_sec); 
        long micros = ((seconds * 1000000) + finish.tv_usec) - (start.tv_usec);

        printf("File content: %s\n", buffer);

        printf("%ld seconds %ld microseconds\n", seconds, micros);
        
    }

    return 0;
}