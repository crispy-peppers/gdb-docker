/* 
   A simple server listening on TCP port 8080
   Compile with:
   gcc vuln-echo-server.c -g -o vuln-echo-server -m32 -fno-stack-protector -z execstack -no-pie -Wl,-z,norelro
   If you get compile time errors, make sure you've installed gcc-multilib first!
   Hint: You can use tools such as peda(https://github.com/longld/peda) and pwndbg(https://github.com/pwndbg/pwndbg)
   to get more output from gdb!
   Note: This code uses various tricks to deal with ASLR inside container!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

void error(const char *msg)
{
    perror(msg);
    fflush(stdout);
    //exit(1);
}

int copier(int fd, char *reply, int size_of_reply_buffer, char *str)
{
    char buffer[1024];
    strcpy(buffer, str);
    if (strlen(buffer) > sizeof(buffer))
        return -1;
    if (write(fd, reply, strlen(reply)) < 0) {
        error("ERROR writing to socket");
        return -1;
    }
    bzero(reply, size_of_reply_buffer);
    snprintf(reply, size_of_reply_buffer, "I got message: %sI copied this message at %8p\n", buffer, buffer);
    if (write(fd, reply, strlen(reply)) < 0)
    {
        error("ERROR writing to socket");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN); // Don't let process exit due to SIGPIPE!
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[4096], reply[5100];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    //portno = 8080;
    if (getenv("PORT"))
        portno = atoi(getenv("PORT"));
    else 
        portno = 8080;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (1)
    {
        newsockfd = accept(sockfd,
                           (struct sockaddr *)&cli_addr,
                           &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        char *msg = "Welcome to my server! If you crash me, you need to start the lab again!\n";
        write(newsockfd, msg, strlen(msg));
        while (1)
        {
            char *msg = "Type in a message!\n";
            n = write(newsockfd, msg, strlen(msg));
            bzero(buffer, sizeof(buffer));
            bzero(reply, sizeof(reply));
            n = read(newsockfd, buffer, 4095);
            if (n < 0)
                error("ERROR reading from socket");

            // CALL A FUNCTION WITH A BUFFER OVERFLOW VULNERABILITY
            n = copier(newsockfd, reply, sizeof(reply), buffer);
            if (n < 0)
            {
                error("");
                break;
            }
        }
        close(newsockfd);
    }
    close(sockfd);
    fflush(stdout);
    return 0;
}