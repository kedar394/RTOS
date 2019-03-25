#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <time.h>   
#include <arpa/inet.h>

#define PORT 8080 

void append_to_file(char* file, char* data) 
{
	FILE* fp = fopen(file, "a");

	if(fp == NULL) 
    {
		perror("File open failed.");
	}

	fprintf(fp, "%lu,%s\n", (unsigned long)time(NULL), data);

	fclose(fp);
}


void receive(int sid) 
{ 
    char buffer[10]="request"; 

    char msg[100];	
    int n; 
    while(1)
    { 
        write(sid, buffer, sizeof(buffer)); 
        
        memset(msg, 0, sizeof(msg)); 
        
        read(sid, msg, sizeof(msg)); 
        printf("From Server : %s\n", msg); 
        append_to_file("./results", msg);

        sleep(2); 
    } 
} 

int main() 
{ 
    int sid, connfd; 
    struct sockaddr_in serv, cli; 
  
    // socket create
    sid = socket(AF_INET, SOCK_STREAM, 0); 
    if (sid == -1) 
    { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
    {        
        printf("Socket successfully created..\n"); 
    }    

    memset(&serv, 0, sizeof(serv)); 
  
    // assign, bind and connect 
    serv.sin_family = AF_INET; 
    serv.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serv.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sid, (struct sockaddr*)&serv, sizeof(serv)) != 0) 
    { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
    {        
        printf("connected to the server..\n"); 
    }  
    
    receive(sid); 
  
    // close the socket 
    close(sid); 
} 
