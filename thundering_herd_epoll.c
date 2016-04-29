
//测试epoll 惊群
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>


int main(int argc, char const *argv[])
{
    int listenfd,epoll_fd,num;
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

    int i,j;
    
    epoll_fd = epoll_create(24);
    struct epoll_event ev,events[64];
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listenfd,&ev);
    for (j = 0; j < 10; j++) {
        printf("-------%d------\n",j);
        int pid;
        if((pid = fork()) == -1){
           perror("fork  error");
           exit(1);
        }
        if(pid == 0){
            for(;;){
                num = epoll_wait(epoll_fd,events,64,-1);
                for (i = 0; i < num; i++) {
                    if ((events[i].data.fd == listenfd) &&(events[i].events & EPOLLIN)) {
                        struct epoll_event client_ev;
                        int client_fd = accept(listenfd,(struct sockaddr*)&client_addr,&client_len);
                        client_ev.data.fd = client_fd;
                        client_ev.events = EPOLLIN | EPOLLET;
                        snprintf(sendbuff,sizeof(sendbuff),"accetp pid : %d \n",getpid()) ;
                        epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&client_ev);

                    }else if(events[i].events & EPOLLIN){
                        sleep(2);
                        send(events[i].data.fd,sendbuff,strlen(sendbuff)+1,0);
                        printf("process %d accept success!  fd:%d\n", getpid(),events[i].data.fd);
                        close(events[i].data.fd);                        
                        epoll_ctl(epoll_fd,EPOLL_CTL_DEL,events[i].data.fd,NULL);
                    }else{
                        printf("events[i].data.fd  %d\n",events[i].data.fd );
                    }
                }
            }
        }
    }

    for(;;){}
    return 0;
}