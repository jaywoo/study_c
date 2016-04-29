
//测试accept 惊群
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char const *argv[])
{
    int listenfd,client_fd;
    struct sockaddr_in serv_addr,client_addr;
    socklen_t client_len;
    char sendbuff[1024];

    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if (listenfd == -1 )
    {
        perror("create listenfd ");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8888);

    bind(listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    listen(listenfd,1024);

    int i;
    for (i = 0; i < 10; i++)
    {
        int pid;
        if((pid = fork()) == -1){
            perror("fork  error");
            exit(1);
        }
        for(;;){
            client_fd = accept(listenfd,(struct sockaddr*)&client_addr,&client_len);
            snprintf(sendbuff,sizeof(sendbuff),"accetp pid : %d \n",getpid()) ;
            send(client_fd,sendbuff,strlen(sendbuff)+1,0);
            printf("process %d accept success!\n", getpid());
            close(client_fd);
        }
    }
    return 0;
}