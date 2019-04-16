#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <time.h>   
#include <arpa/inet.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>
#include <errno.h>

#define MAX 80 
#define PORT 8000


static ssize_t loop_write(int fd, const void*data, size_t size) 
{
    ssize_t ret = 0;
    while (size > 0) 
    {
        ssize_t r;
        if ((r = write(fd, data, size)) < 0)
            return r;
        if (r == 0)
            break;
        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }
    return ret;
}


void comm(int sid) 
{ 
    char buffer[1000]; 
    char msg[1000];	
    while(1) 
    { 

        printf("Type message to sensor...\n");
        scanf("%s",msg);
        write(sid, msg, sizeof(msg)); 
        printf("waiting for sensor...\n");
        read(sid, buffer, sizeof(buffer)); 
        printf("Message from server: %s\n",buffer);
    } 
} 
  

int main(int argc, char *argv[]) 
{ 
    int sid; 
    struct sockaddr_in servaddr, cli; 
    

    static const pa_sample_spec ss = 
    {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    pa_simple *s = NULL;

    int ret = 1;
    int error;
  
    // socket create and varification 
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
    memset(&servaddr, sizeof(servaddr), 0); 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
  
    // connect the client to server 
    if (connect(sid, (struct sockaddr_in*)&servaddr, sizeof(servaddr)) != 0) 
    { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
    {
        printf("connected to the server..\n"); 
    }


    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) 
    {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        exit(0);
    }

    
    while(1) 
    { 
        uint8_t buf[1024];
        ssize_t r;
        if(pa_simple_read(s, buf, sizeof(buf), &error) < 0) 
        {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            exit(0);
        }

        if (loop_write(sid, buf, sizeof(buf)) != sizeof(buf))
        {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            exit(0);
        }
    }
 
    close(sid); 
} 
