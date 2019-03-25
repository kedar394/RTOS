#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdio.h> 
#include <time.h>   

#define PORT 8080 


void toClient(int sid) 
{ 
    char buffer[80], msg[100], filename[100];		
    int n, sensorData[5]; 

    while(1)    
    { 
        memset(buffer, 0, 80); 

        read(sid, buffer, sizeof(buffer)); 
        for(int i = 0; i < 5; i++)
        {
    		sprintf(filename, "./sensor_data/sensor%d.txt", i+1);
    		FILE* fp = fopen(filename, "r");

    		if(fp == NULL) 
    		{
    			perror("Error while reading sensor data.");
    			printf("%s\n", filename);
    		}
    		fscanf(fp, "%d", &(sensorData[i]));

    		fclose(fp);
        }

        sprintf(msg, "%d,%d,%d,%d,%d", sensorData[0], sensorData[1], sensorData[2], sensorData[3], sensorData[4]);

        write(sid, msg, sizeof(msg)); 
        // printf("To client: %s\n",msg);

    } 
} 
  

int main() 
{ 
    int sid, cid, l; 
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

    // assign and bind 
    serv.sin_family = AF_INET; 
    serv.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv.sin_port = htons(PORT); 

    if ((bind(sid, (struct sockaddr*)&serv, sizeof(serv))) != 0) 
    { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
    {        
        printf("Socket successfully binded..\n"); 
    }  

    //listen for response
    if ((listen(sid, 5)) != 0) 
    { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
    {        
        printf("Server listening..\n"); 
    }    

    l = sizeof(cli); 

    // Accept the data packet from client and verification 
    cid = accept(sid, (struct sockaddr*)&cli, &l); 
    if (cid < 0) 
    { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
    {        
        printf("server acccept the client...\n"); 
    } 

    // Function for chatting between client and server 
    toClient(cid); 

    // After chatting close the socket 
    close(sid); 
}
