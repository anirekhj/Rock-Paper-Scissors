#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    // connection established
    printf("Enter Rock<R> Paper<P> or Scissors<S> to play or Quit<q>: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    if (strcmp(buffer,"q\n") == 0) {
      n = write(sockfd,buffer,strlen(buffer));
      close(sockfd);
      return 0;
    }
    else {
      do {
        if ((strcmp(buffer,"R\n") != 0) && (strcmp(buffer,"P\n") != 0) && (strcmp(buffer,"S\n") != 0))  {
          printf("Incorrect input. Enter Rock<R> Paper<P> or Scissors<S> or Quit<q>: ");
          bzero(buffer,256);
          fgets(buffer,255,stdin);
          continue;
        }
        else {
          n = write(sockfd,buffer,strlen(buffer));
          if (n < 0)  error("ERROR writing to socket");
          bzero(buffer,256);
          n = read(sockfd,buffer,255);
          if (n < 0)  error("ERROR reading from socket");
          printf("%s\n",buffer);
          printf("Enter Rock<R> Paper<P> or Scissors<S> to play or Quit<q>: ");
          bzero(buffer,256);
          fgets(buffer,255,stdin);
          // printf("%s\n", strcmp(buffer,"y\n");
        }
      } while(strcmp(buffer,"q\n") != 0);
    }
    close(sockfd);
    return 0;
}
