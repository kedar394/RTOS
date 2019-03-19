#include <errno.h> 
#include <arpa/inet.h>    //close 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>    
#include <fcntl.h>       

    
#define TRUE   1 
#define FALSE  0 
#define PORT 8888 

    
int main(int argc , char *argv[])  
{  
    int opt = TRUE;  
    int master_socket , addrlen , new_socket , client_socket[30] , 
          max_clients = 30 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
        
    char buffer[1025];  //data buffer of 1K 
        
    //set of socket descriptors 
    fd_set readfds;  
        
    //a message 
    char *message = "ECHO Daemon v1.0 \r\n";  
    
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
        
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
        
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
        
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
        
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
        
    //clear the socket set 
        
    //add child sockets to set 
    
    //wait for an activity on one of the sockets , timeout is NULL , 
    //so wait indefinitely 
   
    int fdj;
    fdj = open("joint.txt", O_RDWR);
    
    int fd;
    fd = open("project.txt", O_RDWR);
    // printf("%d\n", fd);
    int ls;


    char *hello;// = "Send request";
    char fileData[1024];

        
    while(TRUE)  
    {  
        FD_ZERO(&readfds);  

        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket; 

        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }

        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
        
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        } 
            
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
            
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
          
            //send new connection greeting message 
            // if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            // {  
            //     perror("send");  
            // }  
                
            puts("Welcome message sent successfully");  
                
            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                        
                    hello = "Send request";
                    // send(client_socket[i], hello, strlen(hello), 0);
                    break;  
                }  
            }  
        }  
            
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                
            // printf("here\n");
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                memset(buffer, 0, 1024);
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                        
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                    
                }  
                    
                //Echo back the message that came in 
                else
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    // buffer[valread] = '\0';  
                    // send(sd , buffer , strlen(buffer) , 0 );
                    // printf("%s\n", buffer);
                    // send(sd, "")
                    // while(1)


                    // msgrcv(id, &msg_rcv, sizeof(struct msg_buffer), 0, 0); 

                    printf("Got: %s\n", buffer);

                    char *rctext = strtok (buffer," "); 
        
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
                        strcpy(fileData, "Data not found");
                    } 
                    else
                    { 
                        fgets( fileData, 50, fp ); 
                    } 
                    
                    fclose(fp);

                    // msg_snd = msg_rcv; 
                    // strcpy(msg_snd.msg_text, fileData); 

                    // int client_id=msg_rcv.qid;
                    

                    // if(msgsnd(client_id, &msg_snd, sizeof(struct msg_buffer), 0) == -1)
                    // {
                    //     perror("msgsnd");
                    //     exit(1);
                    // }  
                    // msg_snd=msg_rcv;

                    // printf("check send::: %s\n", msg_snd.msg_text); 
                    // int client_id=msg_rcv.qid;

                    // if(msgsnd(client_id, &msg_snd, sizeof(struct msg_buffer), 0) == -1) 
                    send(sd , fileData , strlen(fileData) , 0 );
                    // {
                    //     perror("msgsnd");
                    //     exit(1);
                    // }

                    memset(buffer, 0, 1024);   
                    memset(fileData, 0, 1024);   


                    printf("Sent\n");

                }  
            }  
        }  
    }  
        
    return 0;  
}