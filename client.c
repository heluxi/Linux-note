#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main()
{

    int fd;
    char buf[1024];
    fd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in seraddr;
    seraddr.sin_family=PF_INET;
    seraddr.sin_port=htons(8888);
    seraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(connect(fd,(struct sockaddr*)&seraddr,sizeof(seraddr))!=-1)
    {
         printf("connect sucess\n");
        while(1)
        {

            scanf("%s",buf);
            write(fd,&buf,strlen(buf));
            read(fd,&buf,sizeof(buf));
            printf("%s\n",buf);
        }
    }
    close(fd);
    return 0;
}

