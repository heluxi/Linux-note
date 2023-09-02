#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void* request_handler(void* arg);
void send_data(FILE* fp,char* ct,char* file_name);
char* content_type(char* file);
void send_error(FILE* fp);
void error_handling(char* message);

int main(int argc,char *argv[])
{

    int serv_sock,clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr;
    int clnt_adr_size;
    char buf[BUF_SIZE];

    pthread_t t_tid;
    if(argc!=2)
    {
        printf("Usage:%s <port>\n",argv[0]);
        exit(1);
    }
    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htonl(atoi(argv[1]));
    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
    {
        error_handling("bind() error");
    }
    if(listen(serv_sock,20)==-1)
        error_handling("listen() error");

    while(1)
    {
        clnt_adr_size=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_size);
        printf("connection request:%s:%d\n",inet_ntoa(clnt_adr.sin_addr),ntohs(clnt_adr.sin_port));
        pthread_create(&t_tid,NULL,request_handler,&clnt_sock);
        pthread_detach(t_tid);
    }
    close(serv_sock);
    return 0;
}
void* request_handler(void* arg)
{
    int clnt_sock=*((int*)arg);
    char req_line[SMALL_BUF];
    FILE* clnt_read;
    FILE* clnt_write;
    char method[10];
    char ct[15];
    char file_name[30];
    clnt_read=fdopen(clnt_sock,'r');
    clnt_write=fdopen(dup(clnt_sock),'w');
    fgets(req_line,SMALL_BUF,clnt_read);
    if(strstr(req_line,"HTTP/")==NULL)
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return;
    }
    strcpy(method,strtok(req_line,"/"));
}
void send_data(FILE* fp,char* ct,char* file_name);
char* content_type(char* file);
void send_error(FILE* fp);
void error_handling(char* message);
