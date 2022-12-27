#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include<pthread.h> 
#define SIZE 1024
#define NUM_CLIENT 10

void* connection_handler(void* ptr);
int main()
{
    int socket_desc , new_socket , c , *new_sock, i;
    pthread_t sniffer_thread;
    for (i=1; i<=NUM_CLIENT; i++) {
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , &i) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        
    }
    pthread_exit(NULL);
    return 0;
}

void *connection_handler(void *threadid)
{
    int threadnum = *((int*)threadid) ;
    int network_sock;
    struct sockaddr_in serv_addr;
    uint32_t sendBuf = 0;
    uint64_t recvBuf = 0;
    unsigned long long result;
    
    network_sock= socket(AF_INET, SOCK_STREAM, 0);
    if(network_sock< 0)
        printf("Failed creating socket\n");

    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

    
    int connection_status= connect(network_sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr));
    // Check for connection error
    if ( connection_status < 0) {
        printf("Connection Failed\n");
    }

    printf("Client with id:%d gets succesfully connected\n", threadnum);
    
    
    printf("For thread : %d\n", threadnum);
        
    for(int i=1;i<=20;i++){

        sendBuf=htobe32(i);
        if (send(network_sock, &sendBuf, sizeof(sendBuf), 0) == -1)
        {
            perror("client: send()");
        }
        if (recv(network_sock, &recvBuf, sizeof(recvBuf), 0) == -1)
        {
            perror("client: recv()");
        }
        result = be64toh(recvBuf);
        printf("client: send: %d, recv: %llu\n", i, result);
    }

    sleep(1);
        
        
        close(network_sock);

           
            
}
        
