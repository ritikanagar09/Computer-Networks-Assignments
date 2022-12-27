#include <stdio.h>
#include<stdlib.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <poll.h>
#include<sys/epoll.h>


#define DATA_BUFFER 5000
#define MAX_CONNECTIONS 10 
uint32_t recvBuf = 0;
uint64_t sendBuf = 0;
unsigned long long result;
FILE *fptr;
char *filename = "sendepoll.txt";

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
     int server_fd, ret_val, i, efd,s;
     socklen_t addrlen;
     char buf[DATA_BUFFER];
     int nfds = MAX_CONNECTIONS-1, num_open_fds=nfds;
     struct pollfd *pfds;
     struct epoll_event pollfd;
     struct epoll_event *pollfds;
     

     /* Get the socket server fd */
     server_fd = create_tcp_server_socket(); 
     if (server_fd == -1) {
       fprintf(stderr, "Failed to create a server\n");
       return -1; 
     }   
     
     efd=epoll_create1(0);
     pollfd.data.fd=server_fd;
     pollfd.events=EPOLLIN;
     s=epoll_ctl(efd, EPOLL_CTL_ADD,server_fd, &pollfd);

     if(s==-1){
        perror("ERror epoll_ctl");
        abort();
     }

     int useClient=0;
     
     pollfds=(struct epoll_event*)calloc(MAX_CONNECTIONS, sizeof(pollfd));


     
     for(int i=0; i<MAX_CONNECTIONS;i++){
        pollfds[i].data.fd=0;
        pollfds[i].events=EPOLLIN;
     }



     while (1) {


         ret_val= epoll_wait(efd, pollfds, MAX_CONNECTIONS,-1);
         
         if(ret_val==0){
            break;
         }

         for(int j=0;j <ret_val;j++)
         {
            int fd=pollfds[i].data.fd;
            printf("Returned fd is %d (server' fd)\n",server_fd);
            
            int buffsize=recv(pollfds[i].data.fd,&recvBuf,sizeof(recvBuf),0 );

            if(buffsize==-1){
                pollfds[i].data.fd=0;
                pollfds[i].events=0;
                useClient--;
                printf("Error Reading");
            }else if(buffsize==0){
                pollfds[i].data.fd=0;
                pollfds[i].events=0;
                useClient--;
                break;
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
                        if(send(pollfds[i].data.fd, &sendBuf,sizeof(sendBuf),0)==-1)
                        {
                            perror("server: send()");
                        }
                        
                }
                fclose(fptr);
            }
         }
         
     }

     for(int i=0; i<MAX_CONNECTIONS;i++){
        close(pollfds[i].data.fd);
     } 

     free(pollfds);
     close(server_fd);

     return 0;
 }