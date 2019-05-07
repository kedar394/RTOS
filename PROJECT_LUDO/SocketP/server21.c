#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <time.h>   
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <sys/types.h> 

#include <errno.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <sys/stat.h>    
#include <fcntl.h>


#define TRUE   1 
#define FALSE  0 
// #define PORT 8888 

#define MAX 800 
#define PORT 9000
#define SA struct sockaddr 
char buff[MAX];
int sd;


int client_socket[30];


pthread_t thread_id[4]; 
int cnt = 0;

pthread_mutex_t thread_locks[4]; 
pthread_mutex_t main_lock;

char str[100] = "\0";

	
int pawns[4][4] = {{-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}};

int state[4][4] = {{1,1,1,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};

int winstate[4] = {0};

int winner = -1;

int board[56] = {0};



void runPlayer(int crid, char *msg)
{
	strcpy(str, msg);
	pthread_mutex_unlock(&(thread_locks[crid]));
	pthread_mutex_lock(&main_lock);
}

// Kill a co routine
void kill(int crid)
{
	pthread_cancel(thread_id[crid]);
	pthread_mutex_destroy(&(thread_locks[crid])); 
}

int create(void *(*f)())
{
	pthread_create(&(thread_id[cnt++]), NULL, f, NULL);
	pthread_mutex_lock(&main_lock);
	return cnt - 1;
}



char *my_itoa(int num, char *str)
{
    if(str == NULL)
    {
        return NULL;
    }
    sprintf(str, "%d", num);
    // sprintf()
	// printf("\nOkY\n");
    return str;
}


void toString()
{ 	

	int i=0,j=0;
	int n=0;

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			char str[200];
			// printf("pawns: %d\n",pawns[i][j]);
			
			if(my_itoa(pawns[i][j], str) != NULL)
   			{
				strcat(str," ");
				// printf("%s\n", str);
    		}
			strcat(buff, str);

		}
		strcat(buff,"\n");	
	}
	
}






int inHouse(int crid)
{
	int c=0;
	for(int i=0;i<4;i++)
	{
		if(pawns[crid][i] == -1)
			c++;
	}
	return c;
}

int cleared(int crid)
{
	int c=0;
	for(int i=0;i<4;i++)
	{
		if(pawns[crid][i] == 100)
			c++;
	}
	return c;

}

int inPlay(int crid)
{
	return 4 - (inHouse(crid) + cleared(crid));
}


void showPawns()
{
	printf("\n");
	for(int i=0;i<4;i++)
	{
		printf("Player %d: ", i+1);
		for(int j=0;j<4;j++)
		{
			printf("%d ", pawns[i][j]);
		}
		printf("\n");
	}
}

void displace(crid, p)
{
	if(p < 0)
		return;
	//printf("before\n");
	for(int i=0;i<4;i++)
	{
	//printf("bef\n");		
		for(int j=0;j<4;j++)
		{
			if(crid == i)
			{
				break;
			}
			if(pawns[i][j] == pawns[crid][p])
			{
				pawns[i][j] = -1;
			}
		}
	}
}

void finish()
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(pawns[i][j] == (i+1)*100 + 6)
			{
				pawns[i][j] = 1000;
				winstate[i]++;
			}

		}

		if(winstate[i] == 4)
		{
			winner = i+1;
		}
	}
}


int firstAv(int crid)
{
	for(int i=0;i<4;i++)
	{
		if(pawns[crid][i] == -1)
			return i;
	}
}

int firstPlay(int crid)
{
	for(int i=0;i<4;i++)
	{
		if(pawns[crid][i] >= 0 && pawns[crid][i] != 1000)
			return i;
	}

}


void move(int crid, int p, int num)
{
	int cid;
	if(crid == 0)
		cid = 4;
	else
		cid = crid;


	printf("%d\n", cid);

	int prev = pawns[crid][p];


	if(pawns[crid][p] > (crid+1)*100)
	{
		if(num <= (crid+1)*100 + 6 - pawns[crid][p])
		{
			pawns[crid][p] = (pawns[crid][p] + num);	
		}
	}	

	else if(state[crid][p] == 1 && cid*13 - pawns[crid][p] < num)
	{
		pawns[crid][p] = (crid+1)*100 + (num - (cid*13 - pawns[crid][p]));
	}
	else
	{
		pawns[crid][p] = (pawns[crid][p] + num) % 52;
	}


	if(prev > pawns[crid][p])
	{
		state[crid][p] = 1;
	}

}


void sendBoard()
{

	char ch[2];
	bzero(buff, MAX);
	toString();

	printf("%s\n",buff);

	for(int i=0;i<2;i++)
	{
		send(client_socket[i], "Board", sizeof("Board"), 0);	


		// read(sd, ch, sizeof(ch));
		send(client_socket[i], buff, sizeof(buff), 0);	
		
	}
	
}



void players(int crid, int sd)
{
	char ch[1];
	srand(time(0));
	// int crid = cnt - 1;

	int pmoved = -1;

	int dice;

	// while(1)
	{
		// pthread_mutex_unlock(&main_lock);
		// pthread_mutex_lock(&(thread_locks[crid]));	
		
		// int atHome = inHouse(crid);
		// int play = inPlay(crid);

		// bzero(&commStr, sizeof(commStr)); 	
	
		
		// cout<<"Plaoll dice";
		// printf("\nPlayer %d roll dice (press enter)\n", crid + 1);
		

		// write(sd,"Player roll dice (press enter)\n", sizeof("Player roll dice (press enter)\n"));

		// write(sd,"Roll", sizeof("Roll"));

		// sleep(2);

    	send(sd , "Roll" , strlen("Roll") , 0 );

		
		// scanf("%lc", &ch);
		// read("%c", &ch);
		printf("inside roll\n");
		read(sd, ch, sizeof(ch));
		printf("read character\n");
		dice = rand() % 6 + 1;
		// dice = 6;
		printf("dice %d\n",dice);
		// printf("Player %d rolled %d\n", crid+1, dice);
		// commStr = my_itoa(dice, commStr);

		//printf("%d\n", dice);
		
		char dice_str[1];
		dice_str[0]=dice+'0';

		printf("dice_str%s\n", dice_str);

		// // char crid_str[1];
		// crid_str[0]= crid+'0';
		// strcat(dice_str,crid_str);
		// printf("dice_str: %s\n",dice_str);
		//writing result of dice throw

        write(sd , dice_str , sizeof(dice_str));

        int atHome = inHouse(crid);
		int play = inPlay(crid);
		// write(sd, dice_str, sizeof(dice_str));
		printf("after write\n");

		if(play == 0)
		{
			if(dice == 6)
			{
				// printf("Removing first pawn\n");
				
				// printf("Player %d roll dice (press enter)\n", crid + 1);
				// scanf("%c", &ch);

				// dice = rand() % 6 + 1;

				// printf("Player %d rolled %d\n", crid+1, dice);

				// pawns[crid][0] = (pawns[crid][0] + dice) % 52 + crid*13 + 1;
				// pmoved = 0;



				send(sd,"Rem",sizeof("Rem"), 0);
				// char * temp= my_itoa((crid+1),temp);
				
				read(sd, &ch, sizeof(1));
				
				// send(sd,"Player roll dice (press enter)\n", sizeof("Player roll dice (press enter)\n"));
				// read("%c", &ch);
				send(sd,"Roll", sizeof("Roll"), 0);
					
				read(sd, &ch, sizeof(1));

				dice = rand() % 6 + 1;

				char dice_str[1];
				dice_str[0]=dice+'0';

				send(sd, dice_str, sizeof(dice_str), 0);

				// printf("Player %d rolled %d\n", crid+1, dice);

				// write(sd,"Player roll dice (press enter)\n", sizeof("Player roll dice (press enter)\n"));

				pmoved = firstAv(crid);
				pawns[crid][pmoved] = (pawns[crid][pmoved] + dice) % 52 + crid*13 + 1;
			}
			else
			{
				// cout<<"Nothing\n";
			}
		}

		else if(atHome >= 1 && atHome <= 3)	
		{
			// bool pawn;
			if(dice == 6)
			{
				int np;
				// printf("Want to remove new pawn?? (enter 0 for no, 1 for yes)\n");
				write(sd,"New",sizeof("New"));

				char h[1];
				
				read(sd, h, sizeof(h));

				// scanf("%d", &np);

				printf("revrvravar    %s\n", h);

				//cout<<"Roll";
				if(h[0] == 'Y')
				{				
					// dice = rand() % 6 + 1;
					// pawns[inHouse()] += dice;

					//cout<<"Roll";
					// dice = ;

					// printf("Player %d roll dice (press enter)\n", crid + 1);

					write(sd,"Roll", sizeof("Roll"));
					
					// scanf("%lc", &ch);
					// read("%c", &ch);
					read(sd, &ch, sizeof(ch));

					dice = rand() % 6 + 1;

					char dice_str[1];
					dice_str[0]=dice+'0';


					write(sd, dice_str, sizeof(dice_str));
					

					pmoved = firstAv(crid);

					printf("%d\n", pmoved);

					pawns[crid][pmoved] = (pawns[crid][pmoved] + dice) % 52 + crid*13 + 1;

				}
				if(h[0] == 'N')
				{
					if(play == 1)
					{
						// pawns[crid][0] = (pawns[crid][0] + dice) % 52;
						// move(crid, 0, dice);
						pmoved = firstPlay(crid);
						move(crid, pmoved, dice);
					}
					else if(play > 1)
					{
						int p;

						// printf("Which pawn to move\n");
						write(sd,"Which",sizeof("Which"));
						
						// scanf("%d", &p);
						read(sd, &ch, sizeof(1));

						p = ch[0] - 48;


						// pawns[crid][p-1] = (pawns[crid][p-1] + dice) % 52 ;
						move(crid, p-1, dice);

						pmoved = p-1;
					}
				}

			}
			// else

			else if(dice < 6)
			{
				if(play == 1)
				{
					// pawns[crid][0] = (pawns[crid][0] + dice) % 52;
					// move(crid, 0, dice);
					pmoved = firstPlay(crid);
					move(crid, pmoved, dice);
				}
				else if(play > 1)
				{
					int p;

					write(sd,"Which",sizeof("Which"));
						
						// scanf("%d", &p);
					read(sd, &ch, sizeof(1));

					p = atoi(ch);

					// pawns[crid][p-1] = (pawns[crid][p-1] + dice) % 52 ;
					move(crid, p-1, dice);

					pmoved = p-1;
				}
			}
			
		}

		else if(play > 1)
		{
			int p;
			write(sd,"Which",sizeof("Which"));
						
			read(sd, &ch, sizeof(1));

			p = atoi(ch);
			// pawns[crid][p-1] = (pawns[crid][p-1] + dice) % 52 ;
			move(crid, p-1, dice);

			pmoved = p-1;
		}
		
		displace(crid, pmoved);
		printf("before\n");
		finish();

		showPawns();

		// toString();

		// write(sd, buff, sizeof(buff));

		sendBoard()	;

	}


}






int main(int argc , char *argv[])
{

	int clientCnt = 0;

	int opt = TRUE;  
    int master_socket , addrlen , new_socket , 
          max_clients = 30 , activity, i , valread ;  
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
                        
            		clientCnt++;
                    
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

                	// int cr1 = create(&players);
                	// int cr2 = create(&players);
                	// int cr3 = create(&players);
                	// int cr4 = create(&players);

                	// gettimeofday(&start, NULL);

                	// while(1)

                	if(clientCnt == 2)
                	{

                		int chSend[2];

                		// sprintf()

                		// send(client_socket[0],"S",sizeof("S"), 0);
                		// send(client_socket[2],"Start",sizeof("Start"), 0);
                		// send(client_socket[3],"Start",sizeof("Start"), 0);

                		for(int i=0;i<2;i++)
                		{
                			sprintf(chSend, "%d", i+1);
                			send(client_socket[i],chSend,sizeof(chSend), 0);

                		}


                		while(1)
	                	{
	                		// runPlayer(cr1, "W");
	                		players(0, client_socket[0]);
	                		players(1, client_socket[1]);
	                		// players(2, client_socket[2]);
	                		// players(3, client_socket[3]);
	                		
	                		// runPlayer(cr2, "W");
	                		
	                		// runPlayer(cr3, "W");
	                		
	                		// runPlayer(cr4, "W");

	                	}
	                }


                	// Kill the routines
                	// kill(cr1);
                	// kill(cr2);
                	// kill(cr3);
                	// kill(cr4);


                }  
            }  
        }  
    }  
}
