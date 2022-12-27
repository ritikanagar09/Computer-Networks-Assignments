#include <netinet/in.h> 
#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <unistd.h>
#include<pthread.h> 

FILE *fptr;
char *filename = "sendthead.txt";
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
void *connection_handler(void *ptr);
char ipstr[INET6_ADDRSTRLEN];
unsigned short int port;


unsigned long long factorial(int n)
{
  if (n <= 1)
    return 1;
  return n * factorial(n - 1);
}

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    struct sockaddr_storage clientAddr;
    struct addrinfo *ai;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8080);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 10);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

        c=sizeof(struct sockaddr_in);
       while(client_sock=accept(socket_desc,(struct sockaddr*)&client,(socklen_t*)&c))
       {
            puts("Connection accepted");

            
            pthread_t sniffer_thread;
            // new_sock = malloc(sizeof(client_sock));
            // *new_sock = client_sock;
            printf("Connection accepted from %s: => %d\n",
			inet_ntoa(client.sin_addr),
			ntohs(client.sin_port));
            
            if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) &client_sock) < 0)
            {
                perror("could not create thread");
                return 1;
            }

            puts("Handler assigned");

           

    }


     

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;

    
}
/*
  This will handle connection for each client
  */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int n;
    int ret=1;
    uint32_t recvBuf = 0;
    uint64_t sendBuf = 0;
    unsigned long long result;

    fptr = fopen(filename, "a");
    pthread_mutex_lock( &mutex1 );

    
    
        if(fptr==NULL){
            perror("Erro in writting to file");
        }else{


            for(int i=1;i<=20;i++){
                

                if(recv(sock, &recvBuf,sizeof(recvBuf),0)==-1){
                    perror("server: recv()");
                }


                n=be32toh(recvBuf);
                if(n>=1 && n<=20)
                    result=factorial(n);
                    sendBuf=htobe64(result);
                    
                    fprintf(fptr, "fact(%d) = %llu\n", n, result);
                    if(send(sock, &sendBuf,sizeof(sendBuf),0)==-1)
                    {
                        perror("server: send()");
                    }

            }

        }
        
        fclose(fptr);
        pthread_mutex_unlock( &mutex1 );
        
        

        fprintf(stderr, "Client dropped connection\n");
        pthread_exit(&ret);
}

















