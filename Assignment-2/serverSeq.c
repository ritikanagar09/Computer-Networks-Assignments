#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <arpa/inet.h>


void error(char* mssg){
    perror(mssg);
    exit(1);
}

int main(int argc, char const* argv[])
{   int portno=8080;
    int newsockfd;
    socklen_t clilen;
    int n;
    char buffer[256];
    struct sockaddr_in servAddr,cli_addr;

    
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);
    if(servSockD<0){
        error("Error Opening the socket");
    }
    
    
    bzero((char *)&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(portno);
    servAddr.sin_addr.s_addr = INADDR_ANY;


    
    // bind socket to the specified IP and port
    if(bind(servSockD, (struct sockaddr*)&servAddr,sizeof(servAddr))){
        printf("Erorr on binding");
    }
    
    listen(servSockD, 4);
    clilen=sizeof(cli_addr);

    newsockfd=accept(servSockD, (struct sockaddr*)&cli_addr, &clilen );
    if(newsockfd<0){
        error("Error on Accept");
    }

    printf("Connected.....");
    printf("\n");

    while(1){
        //bzero(buffer,256);
        int received_int = 0;
        n = read(newsockfd, &received_int, sizeof(received_int));

        printf("%d", n);
        if(n<0){
            error("Error reading from socket");
        }else if(n==0){
            break;
        }else{
        
            fprintf(stdout, "Received int = %d\n", ntohl(received_int));
            int i;
            unsigned long long fact = 1;
            
            int number= ntohl(received_int);
            for(i=1;i<=number;i++){    
                fact=fact*i;    
            }

            printf("Factorial of num %d %llu",number, fact);
            write(newsockfd, &fact, sizeof(fact));
            
            printf("\n");
        }

        
    }


    
    close(servSockD);

  
    return 0;
}





























