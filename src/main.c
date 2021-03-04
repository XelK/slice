#include <stdio.h> 			// Per printf() e fprintf()
#include <sys/socket.h> 	// Per socket(),bind() e connect()
#include <arpa/inet.h>		// Per sockaddr_in e inet_ntoa()
#include <string.h>			// Per memset()
#include <unistd.h>			// Per close()
#include <stdlib.h>			// Per atoi() e exit()
#include <sys/time.h>		// Per struct timeval

#include <signal.h>



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define PORT 8080
#define MAXPENDING 5
#define BUFFERSIZE 1000
#define T200        "template/200.html"
#define T301        "template/301.template"

void cleanExit(){ exit(0);}

void handleClient(int client){
    char buffer[BUFFERSIZE];
    char buffer2[BUFFERSIZE];
    int msg;
    FILE *fp;
    printf("1\n");
    if((msg=recv(client,buffer,BUFFERSIZE,0))<0){
        fprintf(stderr,"Error in recv()!\n");
    }
    if(msg>0){
        memset(buffer,0,BUFFERSIZE); //pulisco il buffer

        char con[BUFFERSIZE];
        fp=fopen(T200, "r" );
        while(fgets(con,BUFFERSIZE,fp)!=NULL)
            strcat(buffer,con);

        fclose(fp);
        printf("this: %s\n",buffer);
        int dim=sizeof(buffer);

        memset(buffer2,0,BUFFERSIZE);
        sprintf(buffer2,"HTTP/1.1 200\ncontent-type: text/html\ncontent-length: %d\n\n",strlen(buffer));
        strcat(buffer2,buffer);
        if (send(client,buffer2,sizeof(buffer2),0)!=msg){
            printf(stderr,"Error in send()!\n");
            exit(1);
        }
    }

}

int main(int argc, char const *argv[])
{

    int sock;
    int client;
    struct sockaddr_in addr;
    unsigned short port;
    int pid;
    if ((sock = socket(AF_INET,SOCK_STREAM,0))<0){
        fprintf(stderr,"Error in socket creation!\n");
        exit(1);
    }
    printf("3\n");

    signal(SIGTERM,cleanExit);
    signal(SIGINT,cleanExit);

    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(PORT);

    if(bind(sock,(struct sockaddr *)&addr,sizeof(addr))<0){
        fprintf(stderr,"Error in bind()!\n");
        exit(1);
    }
    printf("4\n");

    if(listen(sock,MAXPENDING)<0){
        fprintf(stderr,"Error in listen()!\n");
        exit(1);
    }
    while(1){
        int cl = sizeof(client);
        if((client=accept(sock,(struct sockaddr *)&addr,&cl))<0){
            fprintf(stderr,"Error in accept()!\n");
            exit(1);
        }
        pid=fork();
        if(pid == 0){            
            printf("Client connected!\n");
            handleClient(client);
            close(client);
            exit(0);
        }
        else if(pid != 0){
            close(client);
        }

    }

        printf("6\n");

    // close(sock);
    // exit(0);
}




