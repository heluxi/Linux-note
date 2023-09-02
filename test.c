#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<string.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/epoll.h>

int main()
{
    int lfd;
    lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd==-1)
    {
        perror("socket() error");
        exit(-1);
    }
    struct sockaddr_in ser_addr;
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_addr.s_addr=INADDR_ANY;
    ser_addr.sin_port=htons(8888);

    if(bind(lfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr))==-1)
    {
        perror("bind() error");
        exit(-1);
    }
    listen(lfd,10);
    //创建epoll实例
    int epfd=epoll_create(100);
    if(epfd==-1)
    {
        perror("epoll_create()\n");
        exit(-1);
    }


    struct epoll_event epev;
    epev.events=EPOLLIN;
    epev.data.fd=lfd;
    //将监听的文件描述符添加进epoll实例中
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&epev);
    struct epoll_event epevs[1024];//存放检测后的信息
    while(1)
    {
        //检测函数
        int ret=epoll_wait(epfd,epevs,1024,-1);
        if(ret==-1)
        {
            perror("epool_wait()");
            exit(-1);
        }
        else {

            for(int i=0;i<ret;i++)
            {
                if(epevs[i].data.fd==lfd)
                {
                    struct sockaddr_in cln_addr;
                    socklen_t cln_addr_size=sizeof(cln_addr);
                    int cfd=accept(lfd,(struct sockaddr*)&cln_addr,&cln_addr_size);
                    if(cfd!=-1)
                    {
                        epev.events =EPOLLIN;
                        epev.data.fd=cfd;
                        epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&epev);
                    }
                }
                else
                {
                    char buf[1024];
                    int fd=epevs[i].data.fd;
                    int ret=read(fd,&buf,sizeof(buf));
                    if(ret==0)
                    {
                        printf("the client closed\n");
                        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
                        close(fd);

                    }else if(ret<0)
                    {
                        perror("read()");
                        exit(-1);
                    }else
                    {
                        write(fd,&buf,ret);
                    }

                }
            }
        }

    }



    close(lfd);
    return 0;

}

