#include <stdio.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <poll.h>

#define DATA_BUFFER 5000
#define MAX_CONNECTIONS 10 
uint32_t recvBuf = 0;
uint64_t sendBuf = 0;
unsigned long long result;
FILE *fptr;
char *filename = "sendpoll.txt";

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

     /* Initialize the socket address structure */
     saddr.sin_family = AF_INET;         
     saddr.sin_port = htons(7000);     
     saddr.sin_addr.s_addr = INADDR_ANY; 

     /* Step2: bind the socket to port 7000 on the local host */
     ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
     if (ret_val != 0) {
         fprintf(stderr, "bind failed [%s]\n", strerror(errno));
         close(fd);
         return -1;
     }

     /* Step3: listen for incoming connections */
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
     
     struct sockaddr_in new_addr;
     int server_fd, ret_val, i;
     socklen_t addrlen;
     char buf[DATA_BUFFER];
     int all_connections[MAX_CONNECTIONS];
     struct pollfd *pfds;
     int nfds = MAX_CONNECTIONS-1, num_open_fds=nfds;
     struct pollfd pollfds[MAX_CONNECTIONS + 1];

     /* Get the socket server fd */
     server_fd = create_tcp_server_socket(); 
     if (server_fd == -1) {
       fprintf(stderr, "Failed to create a server\n");
       return -1; 
     }   
     
     pollfds[0].fd=server_fd;
     pollfds[0].events=POLLIN;
     int useClient =0;
     for(int i=1; i<MAX_CONNECTIONS;i++){
        pollfds[i].fd=0;
        pollfds[i].events=POLLIN;
     }



     while (1) {
         ret_val= poll(pollfds, useClient +1, 5000);
         /* Set the fd_set before passing it to the select call */
         
         /* select() woke up. Identify the fd that has events */
         if (ret_val >= 0 ) {
             printf("Poll returned with %d\n", ret_val);
             /* Check if the fd with event is the server fd */
             if (pollfds[0].revents && POLLIN) { 
                 /* accept the new connection */
                 printf("Returned fd is %d (server's fd)\n", server_fd);
                 int new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                 if (new_fd >= 0) {
                     printf("Accepted a new connection with fd: %d\n", new_fd);
                     for (i=0;i < MAX_CONNECTIONS;i++) {
                         if (pollfds[i].fd==0) {
                             pollfds[i].fd = new_fd; 
                             pollfds[i].events=POLLIN;
                             useClient++;
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
                 if ( pollfds[i].fd > 0 && pollfds[i].revents &POLLIN) {
                     /* read incoming data */
                     memset(buf, 0, sizeof(buf));
                     //read(pollfds[i].fd, buf, DATA_BUFFER-1);
                     int bufsize= recv(pollfds[i].fd, &recvBuf,sizeof(recvBuf),0);
                     

                     if(bufsize==-1){
                        pollfds[i].fd=0;
                        pollfds[i].revents=0;
                        pollfds[i].events=0;
                        useClient--;
                     }else if(bufsize==0){
                        pollfds[i].fd=0;
                        pollfds[i].revents=0;
                        pollfds[i].events=0;
                        useClient--;
                     }else{
                        fptr = fopen(filename, "a");
                        if(fptr==NULL){
                            perror("Error in writting to file");
                        }else{
                                int k=be32toh(recvBuf);// integer recieved from client
                                if(k>=1 && k<=20)
                                    result=factorial(k);
                                    sendBuf=htobe64(result);
                                    fprintf(fptr,"fact(%d)= %llu\n", k ,result);
                                if(send(pollfds[i].fd, &sendBuf,sizeof(sendBuf),0)==-1)
                                {
                                    perror("server: send()");
                                }
                        
                        }
                        fclose(fptr);
                    }
                 }
             }
                
                 
              
         } 
         else{
            printf("Failed to connect.\n");
         }
     } 

     
     
     return 0;
 }