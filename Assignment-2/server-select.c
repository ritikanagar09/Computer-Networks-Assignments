#include <stdio.h>
 #include <netinet/in.h> 
 #include <unistd.h> 
 #include <string.h>
 #include <errno.h>
 #include<pthread.h>

#define DATA_BUFFER 1
#define MAX_CONNECTIONS 10
FILE *fptr;
char *filename = "send.txt";
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
uint32_t recvBuf = 0;
uint64_t sendBuf = 0;
unsigned long long result;
 

 int create_tcp_server_socket() {
     struct sockaddr_in saddr;
     int fd, ret_val;

     /* Step1: create a TCP socket */
     fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
     if (fd == -1) {
         fprintf(stderr, "socket failed [%s]\n", strerror(errno));
         return -1;
     }
     printf("Created a socket with fd: %d\n", fd);

     
     saddr.sin_family = AF_INET;         
     saddr.sin_port = htons(7000);     
     saddr.sin_addr.s_addr = INADDR_ANY; 

     
     ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
     if (ret_val != 0) {
         fprintf(stderr, "bind failed [%s]\n", strerror(errno));
         close(fd);
         return -1;
     }

     
     ret_val = listen(fd, 5);
     if (ret_val != 0) {
         fprintf(stderr, "listen failed [%s]\n", strerror(errno));
         close(fd);
         return -1;
     }
     return fd;
 }


 unsigned long long factorial(int n)
{
  if (n <= 1)
    return 1;
  return n * factorial(n - 1);
}

 int main () {
     fd_set read_fd_set;
     struct sockaddr_in new_addr;
     int server_fd, new_fd, ret_val, i,n;
     socklen_t addrlen;
     char buf[DATA_BUFFER];
     int all_connections[MAX_CONNECTIONS];

     /* Get the socket server fd */
     server_fd = create_tcp_server_socket(); 
     if (server_fd == -1) {
       fprintf(stderr, "Failed to create a server\n");
       return -1; 
     }   

    
     for (i=0;i < MAX_CONNECTIONS;i++) {
         all_connections[i] = -1;
     }
     all_connections[0] = server_fd;

     while (1) {
         FD_ZERO(&read_fd_set);
         /* Set the fd_set before passing it to the select call */
         for (i=0;i < MAX_CONNECTIONS;i++) {
             if (all_connections[i] >= 0) {
                 FD_SET(all_connections[i], &read_fd_set);
             }
         }

         /* Invoke select() and then wait! */
         printf("\nUsing select() to listen for incoming events\n");
         ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

         /* select() woke up. Identify the fd that has events */
         if (ret_val >= 0 ) {
             printf("Select returned with %d\n", ret_val);
             /* Check if the fd with event is the server fd */
             if (FD_ISSET(server_fd, &read_fd_set)) { 
                 /* accept the new connection */
                 printf("Returned fd is %d (server's fd)\n", server_fd);
                 new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                 if (new_fd >= 0) {
                     printf("Accepted a new connection with fd: %d\n", new_fd);
                     for (i=0;i < MAX_CONNECTIONS;i++) {
                         if (all_connections[i] < 0) {
                             all_connections[i] = new_fd; 
                             break;
                         }
                     }
                 } else {
                     fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                 }
                 ret_val--;
                 if (!ret_val) continue;
             } 




             /* Check if the fd with event is a non-server fd */
             for (i=1;i < MAX_CONNECTIONS;i++) {
                 if ((all_connections[i] > 0) &&
                     (FD_ISSET(all_connections[i], &read_fd_set))) {
                     /* read incoming data */   
                     printf("Returned fd is %d [index, i: %d]\n", all_connections[i], i);
                     ret_val = recv(all_connections[i], &recvBuf,sizeof(recvBuf),0);

                     if (ret_val == 0) {
                         printf("Closing connection for fd:%d\n", all_connections[i]);
                         close(all_connections[i]);
                         all_connections[i] = -1; /* Connection is now closed */
                     } 
                      
                     if (ret_val == -1) {
                         printf("recv() failed for fd: %d [%s]\n", 
                             all_connections[i], strerror(errno));
                         break;
                     }

                    if (ret_val > 0) { 
                        fptr = fopen(filename, "a");
                        
    
                        if(fptr==NULL){
                            perror("Error in writting to file");
                        }else{
                                int k=be32toh(recvBuf);// integer recieved from client
                                if(k>=1 && k<=20)
                                    result=factorial(k);
                                    sendBuf=htobe64(result);
                                    fprintf(fptr,"fact(%d)= %llu\n", k ,result);

                                
                                

                                
                                if(send(all_connections[i], &sendBuf,sizeof(sendBuf),0)==-1)
                                {
                                    perror("server: send()");
                                }

                            
                            
                        }

                    
                        fclose(fptr);
                        
                    }
                }       

                ret_val--;
                if (!ret_val) continue;
             } /* for-loop */
         } /* (ret_val >= 0) */
     } /* while(1) */

     /* Last step: Close all the sockets */
     for (i=0;i < MAX_CONNECTIONS;i++) {
         if (all_connections[i] > 0) {
             close(all_connections[i]);
         }
     }
     return 0;
 }