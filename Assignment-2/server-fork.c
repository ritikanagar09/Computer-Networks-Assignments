#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include<pthread.h> 

// PORT number
#define PORT 4444
uint32_t recvBuf = 0;
uint64_t sendBuf = 0;
FILE *fptr;
char *filename = "send.txt";
unsigned long long result;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


unsigned long long factorial(int n)
{
  if (n <= 1)
    return 1;
  return n * factorial(n - 1);
}


int main()
{
	
	int sockfd, ret;
	struct sockaddr_in serverAddr;
	int clientSocket;
	struct sockaddr_in cliAddr;
	socklen_t addr_size;
	pid_t childpid;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Error handling if socket id is not valid
	if (sockfd < 0) {
		printf("Error in connection.\n");
		exit(1);
	}

	printf("Server Socket is created.\n");

	
	memset(&serverAddr, '\0',
		sizeof(serverAddr));

	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);

	
	serverAddr.sin_addr.s_addr
		= INADDR_ANY;

	
	ret = bind(sockfd,
			(struct sockaddr*)&serverAddr,
			sizeof(serverAddr));

	
	if (ret < 0) {
		printf("Error in binding.\n");
		exit(1);
	}

	
	if (listen(sockfd, 10) == 0) {
		printf("Listening...\n\n");
	}

	int cnt = 0;
	while (1) {

		
		clientSocket = accept(
			sockfd, (struct sockaddr*)&cliAddr,
			&addr_size);

		
		if (clientSocket < 0) {
			exit(1);
		}

		
		printf("Connection accepted from %s:%d\n",
			inet_ntoa(cliAddr.sin_addr),
			ntohs(cliAddr.sin_port));

		
		printf("Clients connected: %d\n\n",
			++cnt);

		
		if ((childpid = fork()) == 0) {

			// Closing the server socket id
			close(sockfd);

			
			send(clientSocket, "hi client",
				strlen("hi client"), 0);

			fptr = fopen(filename, "a");
			pthread_mutex_lock( &mutex1 );
			if(fptr==NULL){
            	perror("Erro in writting to file");

        	}else{
				fprintf(fptr, "hello");
				for (int i = 1; i <= 20; ++i)
					{
						if (recv(clientSocket, &recvBuf, sizeof(recvBuf), 0) == -1)
						{
							perror("server: recv()");
						}
						int n = be32toh(recvBuf);
						if(n>=1 && n<=20)
							result = factorial(n);
							sendBuf = htobe64(result);
							
							fprintf(fptr, "fact(%d) = %llu\n", n, result);
							if (send(clientSocket, &sendBuf, sizeof(sendBuf), 0) == -1)
							{
								perror("server: send()");
							}
					}
			}
			pthread_mutex_lock( &mutex1 );

			fclose(fptr);

			
		}
	}

	
	close(clientSocket);
	return 0;
}
