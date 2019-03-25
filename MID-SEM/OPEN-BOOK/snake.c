#include<stdio.h> 
#include<string.h> 
#include<pthread.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<time.h>

pthread_t thread_id[4]; 
int cnt = 0;

pthread_mutex_t thread_locks[4]; 
pthread_mutex_t main_lock;

char str[100] = "\0";
int winner = 0;



// Create a co-routine
int create(void *(*f)())
{
	pthread_create(&(thread_id[cnt++]), NULL, f, NULL);
	pthread_mutex_lock(&main_lock);
	return cnt - 1;
}

// Send data to global data
void copyString(int crid, char * msg)
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



void* players()
{
	srand(time(0)); 
	int crid = cnt - 1;
	int level = 0;
	int dice;
	while(1)
	{
		pthread_mutex_unlock(&main_lock);
		pthread_mutex_lock(&(thread_locks[crid]));	
		dice = rand() % 6 + 1;

		level += dice;
		if(level >= 100)
		{
			winner = crid + 1;
			printf("Winner: Player %d\n", crid+1);

		}
		// printf("%d %d\n", crid+1, level);

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

	struct timeval start, finish;


	// Create some co routines 
	int cr1 = create(&players);
	int cr2 = create(&players);
	int cr3 = create(&players);
	int cr4 = create(&players);

	// Execute the threads
	// copyString(cr2, "X");
	// copyString(cr2, "Y");
	// copyString(cr1, "C");    //Message doesn't matter for loop functions
	// copyString(cr1, "S");
	// copyString(cr1, "Q");

	gettimeofday(&start, NULL);


	while(1)
	{
		if(winner > 0)
			break;
		copyString(cr1, "W");
		if(winner > 0)
			break;
		copyString(cr2, "W");
		if(winner > 0)
			break;
		copyString(cr3, "W");
		if(winner > 0)
			break;
		copyString(cr4, "W");

	}

    gettimeofday(&finish, NULL);

	// printf("Time taken: %d\n", winner);
	long seconds = (finish.tv_sec - start.tv_sec); 
    long micros = ((seconds * 1000000) + finish.tv_usec) - (start.tv_usec);

    printf("%ld seconds %ld microseconds\n", seconds, micros);

	// Kill the routines
	kill(cr1);
	kill(cr2);
	kill(cr3);
	kill(cr4);

	return 0; 
} 