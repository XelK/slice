# slice
url shortener


Server in ascolto sulla porta 80 tcp. 
Può ricevere metodi:
- GET/HEAD
- POST
- ALTRO: risponde con 405 errore! (405: method not allowed)


se riceve un GET:
- controlla il path
- path esiste:
    - risponde con 302 indicando nuovo url
- path non esiste:
    - risponde con 404

se riceve un POST:
- controlla se url inserito:
    - corretto:
        - produce un url accorciato
        - risponde con l'url accorciato
    - sbagliato:
        - risponde con 400 (400: bad request)


main(): 
- rimane in ascolto sulla porta 80
- riceve la richiesta crea un socket e passa alla funzione handle()

handle(): legge la richiesta e decide cosa fare con la richiesta

extractMethod(): estrae il metodo usato nella richiesta
extractPath(): estrae il path chiamato nella richiesta
extractParam(): estrae il parametro passato

formatResponse(): crea la risposta da dare al client


updateDB(): aggiunge il path al db
searchDB(): cerca il path chiamato nel DB

handleGet(): gestisce il metodo get()

handlePost(): gestisce il metodo post()





## Usage

```
POST https://example.com/comments/1

```




POST https://example.com/comments HTTP/1.1
content-type: application/json

{
    "name": "sample",
    "time": "Wed, 21 Oct 2015 18:27:50 GMT"
}
"""








## 302 respond:
HTTP/1.1 302 Found
Date: Mon, 23 May 2005 22:38:34 GMT
Status: 302 Found
Content-Type: text/html; charset=UTF-8
Location: https://raw.githubusercontent.com/XelK/arch/master/arch_install.sh
Content-Length: 0







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

typedef struct _tmessage{
        char method[10];
        char path[100];
        char version[10];
} tmessage;

void cleanExit(){ exit(0);}

void getMessage(char buffer[BUFFERSIZE]);
void postMessage(char buffer[BUFFERSIZE]);
void handeClient(int client,tmessage message);


void extract(char buffer[BUFFERSIZE],tmessage message) {
    for(int i=0;i<strlen(buffer);i++){
        if( buffer[i] != (int)' ')
            printf("%c",buffer[i]);
        else
            printf("\n");
    }
    printf("\n");

    printf("\n\n\n");

    int i=0;
    int x=0;
    for(i;buffer[i]!=(int)' ';i++){
        message.method[x]=buffer[i];
        x++;
        //strcat(message.method,buffer[i]);
    }
    x=0;
    for(i++;buffer[i]!=(int)' ';i++){
        message.path[x]=buffer[i];
        x++;
        //strcat(message.path,buffer[i]);
    }
    x=0;
    for(i++;i<strlen(buffer);i++){
        message.version[x]=buffer[i];
        x++;
        //strcat(message.version,buffer[i]);
    }

    printf("Extracted Method: %s\nPath: %s\nVersion: %s\n",message.method,message.path,message.version);

}

void getMessage(char buffer[BUFFERSIZE]){
    FILE *fp;
    char con[BUFFERSIZE];

    printf("\n Ricevuto GET:\n%s\n-------\n",buffer);
    

    // string manipulation
    
    memset(buffer,0,BUFFERSIZE); //pulisco il buffer        
    fp=fopen(T200, "r" );
    while(fgets(con,BUFFERSIZE,fp)!=NULL)
        strcat(buffer,con);
    fclose(fp);
    printf("getMessage: %s\n",buffer);
}

void postMessage(char buffer[BUFFERSIZE]){
    FILE *fp;
    char con[BUFFERSIZE];

    printf("\nRicevuto POST:\n%s\n-------\n",buffer);
    memset(buffer,0,BUFFERSIZE); //pulisco il buffer        
    fp=fopen(T200, "r" );
    while(fgets(con,BUFFERSIZE,fp)!=NULL)
        strcat(buffer,con);
    fclose(fp);
}

void handleClient(int client,tmessage message){
    char buffer[BUFFERSIZE];
    int msg;
    if((msg=recv(client,buffer,BUFFERSIZE,0))<0){
        fprintf(stderr,"Error in recv()!\n");
    }
    printf("\nMessage received:\n%s\n---------\n",buffer);
    
    // if(msg>0){
    //     extract(buffer,message);
    //     //if(message.method=="GET") // || message.method=="HEAD")
    //     if (strcmp(message.method,"GET"))
    //         getMessage(buffer);
    //     else if(strcmp(message.method,"POST"))
    //         postMessage(buffer);
        
    //     printf("Method: %s\nPath: %s\nVersion: %s\n",message.method,message.path,message.version);

    //     printf("\nMessage to send:\n%s\n---------\n",buffer);
    //     if (send(client,buffer,sizeof(buffer),0)!=msg){
    //         printf(stderr,"Error in send()!\n");
    //         exit(1);
    //     }
    // }
        
    extract(buffer,message);

    if(msg>0){
        
        if(strcmp(message.method,"GET")){
            getMessage(buffer);
        }
        else if(strstr(buffer,"HEAD")){
            getMessage(buffer);
        }       
        else if(strstr(buffer,"POST"))
            postMessage(buffer);


        printf("\nMessage to send:\n%s\n---------\n",buffer);
        if (send(client,buffer,sizeof(buffer),0)!=msg){
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
    tmessage message;

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
            handleClient(client,message);
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





