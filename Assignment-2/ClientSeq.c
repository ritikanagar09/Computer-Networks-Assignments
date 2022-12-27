#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <arpa/inet.h>

void error(char *mssg){
    perror(mssg);
    exit(0);
}

int main(int argc, char const* argv[])
{   int n;
    int portno=8080;
    char buffer[256];
  
    struct sockaddr_in servAddr;

    int sockD = socket(AF_INET, SOCK_STREAM, 0);
    if(sockD <0){
        error("Error Opening socket");
    }

    bzero((char*)& servAddr, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_port= htons(portno); // use some unused port number
    servAddr.sin_addr.s_addr = INADDR_ANY;
  

    
    int connectStatus
        = connect(sockD, (struct sockaddr*)&servAddr,
                  sizeof(servAddr));
    
  
    if (connectStatus == -1) {
        printf("Error...\n");
    }else{
        printf("Connected to Server");
    }
    
    printf("\n");

    

    
    for (int i=1;i<=20;i++){
       


        int number_to_send; // Put your value
        int converted_number = htonl(i);
        
        write(sockD, &converted_number, sizeof(converted_number));
        unsigned long long l=0;
        int m=recv(sockD, &l, sizeof(l),0);
        printf("fact: %llu \n", l);
        
        

    }


    

    close(sockD);

   

  
    return 0;
}







