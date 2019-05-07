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

#define MAX 800 
#define PORT 8080 
#define SA struct sockaddr 
char buff[MAX];
int connfd;


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
    return str;
}


char* toString()
{ 	
	int i=0,j=0;
	int n=0;
	char * test;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			char str[200];
			// printf("pawns: %d\n",pawns[i][j]);
			
			if(my_itoa(pawns[i][j], str) != NULL)
   			{
				strcat(str," ");
				printf("%s\n", str);
    		}
			strcat(buff, str);

		}
		strcat(buff,"\n");	
	}
	printf("%s\n",buff);
	
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
		if(pawns[crid][i] == 1000)
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
	for(int i=0;i<4;i++)
	{
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


	// printf("%d\n", cid);

	int prev = pawns[crid][p];


	if(pawns[crid][p] >= (crid+1)*100)
	{
		if(num <= (crid+1)*100 + 6 - pawns[crid][p])
		{
			pawns[crid][p] = (pawns[crid][p] + num);	
		}
	}	

	else if(state[crid][p] == 1 && cid*13 - pawns[crid][p] <= num)
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

void *players()
{
	srand(time(0));
	int crid = cnt - 1;

	char ch;
	int pmoved;

	int dice;

	while(1)
	{
		pthread_mutex_unlock(&main_lock);
		pthread_mutex_lock(&(thread_locks[crid]));	
		
    	// bzero(ch, sizeof(ch));



		printf("\nPlayer %d roll dice (press enter)\n", crid + 1);
		scanf("%lc", &ch);

		dice = rand() % 6 + 1;

		printf("Player %d rolled %d\n", crid+1, dice);

		int atHome = inHouse(crid);
		int play = inPlay(crid);

		if(play == 0)
		{
			if(dice == 6)
			{
				printf("Removing new pawn\n");
				
				printf("Player %d roll dice (press enter)\n", crid + 1);
				scanf("%c", &ch);

				dice = rand() % 6 + 1;

				printf("Player %d rolled %d\n", crid+1, dice);

				pmoved = firstAv(crid);
				pawns[crid][pmoved] = (pawns[crid][pmoved] + dice) % 52 + crid*13 + 1;
			}
			else
			{
				// cout<<"Nothing\n";
			}
		}

		else if((atHome >= 1 && atHome <= 3))
		{
			// bool pawn;
			if(dice == 6)
			{
				int np;
				printf("Want to remove new pawn?? (enter 0 for no, 1 for yes)\n");
				scanf("%d", &np);
		    	fflush(stdin);

				//cout<<"Roll";
				if(np == 1)
				{				
					// dice = rand() % 6 + 1;
					// pawns[inHouse()] += dice;

					//cout<<"Roll";
					// dice = ;

					printf("Player %d roll dice (press enter)\n", crid + 1);
					scanf("%c", &ch);

					dice = rand() % 6 + 1;
					printf("Player %d rolled %d\n", crid+1, dice);

					pmoved = firstAv(crid);

					pawns[crid][pmoved] = (pawns[crid][pmoved] + dice) % 52 + crid*13 + 1;

				}
				if(np == 0)
				{
					if(play == 1)
					{
						// pawns[crid][0] = (pawns[crid][0] + dice) % 52;
						pmoved = firstPlay(crid);
						move(crid, pmoved, dice);
					}
					else if(play > 1)
					{
						int p;

						printf("Which pawn to move\n");
						scanf("%d", &p);
				    	fflush(stdin);

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
					pmoved = firstPlay(crid);
					move(crid, pmoved, dice);
				}
				else if(play > 1)
				{
					int p;

					printf("Which pawn to move\n");
					scanf("%d", &p);

					// pawns[crid][p-1] = (pawns[crid][p-1] + dice) % 52 ;
					move(crid, p-1, dice);

					pmoved = p-1;
				}
			}
			
		}

		else if(play > 1)
		{
			int p;
			printf("Which pawn to move\n");

			scanf("%d", &p);
			// pawns[crid][p-1] = (pawns[crid][p-1] + dice) % 52 ;
			move(crid, p-1, dice);

			pmoved = p-1;
		}

		displace(crid, pmoved);

		finish();

		showPawns();
		

	}


}





int main() 
{ 	
	// Initialize
	int i = 0; 
	for(i = 0; i < 4; i++)
	{
		pthread_mutex_init(&(thread_locks[i]), NULL);
		pthread_mutex_lock(&(thread_locks[i]));
	}

	pthread_mutex_lock(&main_lock);

	// struct timeval start, finish;


	// Create some co routines 
	int cr1 = create(&players);
	int cr2 = create(&players);
	int cr3 = create(&players);
	int cr4 = create(&players);

	// gettimeofday(&start, NULL);


	while(1)
	{
		runPlayer(cr1, "W");
		if(winner > -1)
			break;
		
		runPlayer(cr2, "W");
		if(winner > -1)
			break;
		
		runPlayer(cr3, "W");
		if(winner > -1)
			break;
		
		runPlayer(cr4, "W");
		if(winner > -1)
			break;

	}

	printf("Winner is player %d", winner+1);

	// Kill the routines
	kill(cr1);
	kill(cr2);
	kill(cr3);
	kill(cr4);

	return 0; 
} 