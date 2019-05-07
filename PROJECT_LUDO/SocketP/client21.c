
// Write CPP code here 
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#define MAX 80 
#define PORT 9000 
#define SA struct sockaddr 


void func(int sockfd) 
{ 
	// printf("TEST\n");
    char buff[MAX]; 
    int n; 



    for (;;) 
    { 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
    	// printf("Here\n");
        //printf("From Server : %s", buff);
		if((strncmp(buff, "Roll", 4)) == 0)
		{
			printf("Roll the dice, enter any key!\n");
        		bzero(buff, sizeof(buff)); 
			char c[1];
			
			scanf("%s",c);
			// printf("Going to read the character:\n");
			send(sockfd,c,sizeof(c),0);
			//printf("buff%s\n", buff);
			//write(sockfd , buff , sizeof(buff) );

			char dice[2];
			read(sockfd,dice,sizeof(dice));

			printf("You have rolled %s\n", dice);
			
		}

		if((strncmp(buff, "Rem", 3)) == 0)
		{
			bzero(buff, sizeof(buff));
			printf("Removing first pawn\n");
			
			// read(sockfd,buff,sizeof(buff));
			char c[1];
			// send(sockfd,c,sizeof(c),0);
			
			scanf("%s",c);
			// printf("Going to read the character:\n");
			send(sockfd,c,sizeof(c),0);
			// printf("Roll the dice, enter any key!\n");
			// char c;
			// scanf("%c",&c);
			// write(sockfd,c,sizeof(c));
		}


		if((strncmp(buff, "New", 3)) == 0)
		{
			printf("Remove new pawn?[Y/N]\n");
			char c[1];
			scanf("%s", c);
			send(sockfd,c,sizeof(c), 0);
		}

		if((strncmp(buff, "Which", 5)) == 0)
		{
			printf("Which pawn to move?\n");
			char c[1];
			scanf("%s", c);
			// write(sockfd,c,sizeof(c));
		}
		
		if((strncmp(buff, "Board", 5)) == 0)
		{
			printf("Press any key for board\n");

			// char c2[1];
			// scanf("%s", c2);

			// send(sockfd,c2,sizeof(c2),0);

			char c[MAX];
			printf("This is the board\n");
			//scanf("%c",&c);
			read(sockfd,c,sizeof(c));

			
			printf("%s\n",c);
		}
		
        if ((strncmp(buff, "exit", 4)) == 0)
		{ 
			printf("Client Exit...\n"); 
			//break; 
		} 
    } 
} 
  
int main() 
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




    send(sock, "Hello", strlen("Hello") , 0 );

    char b[MAX];

    // while(1)
    {
    	
	    // bzero(b, sizeof(b)); 

    	printf("Waiting here\n");


	    // read(sock, b, sizeof(b)); 


		// if((strncmp(b, "Start", 5)) == 0)
		{


			bzero(b, sizeof(b)); 

	    	// printf("Waiting here\n");
		    read(sock, b, sizeof(b)); 

		    printf("You are player: %c\n\n", b[0]);

	    	// break;
			
			
		}
    }





    // function for chat 
    func(sock); 
  
    // close the socket 
    close(sock); 
} 

