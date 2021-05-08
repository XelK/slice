 #include <stdio.h> 			// Per printf() e fprintf()
 #include <sys/socket.h> 	// Per socket(),bind() e connect()
 #include <arpa/inet.h>		// Per sockaddr_in e inet_ntoa()
 #include <string.h>			// Per memset()
 #include <unistd.h>			// Per close()
 #include <stdlib.h>			// Per atoi() e exit()
// #include <sys/time.h>		// Per struct timeval

#define PORT 8080
#define MAXPENDING 5
#define BUFFERSIZE 1000


void handleClient(int client);

char * search(char from[]);
void prepMessage(int code, char *from, char *to, char * message);
char * updateDB(char *to[]);

int main(){
    
    int sock;
    int client;
    struct sockaddr_in addr;
    int pid;


   if ((sock = socket(AF_INET,SOCK_STREAM,0))<0){
        fprintf(stderr,"Error in socket creation!\n");
        exit(1);
    }

    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(PORT);

    if(bind(sock,(struct sockaddr *)&addr,sizeof(addr))<0){
        fprintf(stderr,"Error in bind()!\n");
        exit(1);
    }

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

}


void handleClient(int client){
    char message[BUFFERSIZE];
    int msg;

    if((msg=recv(client,message,BUFFERSIZE,0))<0){
        fprintf(stderr,"Errore in ricezione!\n");
        exit(1);
    }

    printf("Messaggio ricevuto:\n%s\n",message);

    char from[]=" "; 
    char to[50];

    char * temp;

    if(strstr(message,"GET") || strstr(message,"HEAD")){
        printf("GET/HEAD find!\n");
  
        int i;
        int c=0;
        if (strstr(message,"GET"))
            i=5;
        else
            i=6;
        while(message[i]!=' ' && message[i]!='\n'){
            from[c++]=message[i];
            if (message[++i]==' ')
                from[c]='\0';
        }
        printf("from: %s\n",from);

        if (strcmp(from," ")==0)
            prepMessage(200,from," ",message);
        else if (strcmp(temp=search(from)," ")!=0){
            prepMessage(302,from,temp,message);
        }
        else
            prepMessage(404,from," ",message);
    }
    else if(strstr(message,"POST")){
        printf("POST find!\n");
        temp=strstr(message,"url:");
        for(int i=4;i<strlen(temp);i++){
            to[i-4]=temp[i];
        }
        printf("to: %s\n",to);

        if(strcmp(search(to)," ")==0){
            prepMessage(200,from,updateDB(to),message);
        }        

    }
    else{
        printf("ERRORE! Methodo non supportato!\n");
        prepMessage(405,from,"",message); //405 Method Not Allowed
    }

    printf("Message to reply:\n%s\nfine messaggio!\n",message);
 //   if (send(client,message,sizeof(message),0)!=sizeof(message)){
    if (send(client,message,strlen(message),0)!=sizeof(message)){
        printf("errore in send\n");
        exit(1);
    }

}

char * search(char from[]){
    char word[]="aaa";
    printf("Searching...from: %s to: %s\n",from, word);

    if(strcmp(from,word)==0){
        printf("trovato %s\n",word);
        return "https://xelk.me";
    }
    else{
        printf("NON TROVATO!\n");
        return " ";
    }
}

void prepMessage(int code, char *from, char *to, char * message){
    printf("Responding...\n");
    printf("- from: %s\n- to:%s\n",from,to);

    if (code==200){
        printf("PREP-200\n");
        strcpy(message,"HTTP/1.1 200 OK\nDate: Mon, 23 May 2005 22:38:34 GMT\nContent-Type: text/html; charset=UTF-8\nConnection: close\r\n\r\n");
        strcat(message,"<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n<title> Hello from slice server!</title>\n</head>\n<body>\n<center><h1>Slice server!</h1></center>\n</body>\n</html>");
    }
    if(code == 302){
        printf("PREP-301\n");
        strcpy(message,"HTTP/1.1 301 Moved Permanently\nServer: slice\nDate: Mon, 03 May 2021 17:44:08 GMT\nContent-Type: text/html; charset=UTF-8\n");
        strcat(message,"Location: ");
        strcat(message,to);
        strcat(message,"\nAccept-Ranges: bytes\nConnection: close\r\n\r\n");
        strcat(message,"<!DOCTYPE html>\n<html>\n<head>\n<title>301 Moved Permanently</title>\n</head>\n<body>\n<center><h1>301 Moved Permanently</h1></center>\n</body>\n</html>");
    }
    if(code==404){
        printf("PREP-404\n");
        strcpy(message,"HTTP/1.1 404\ncontent-type: text/html\r\n\r\n");
        strcat(message,"<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n<title>404 Not found!</title>\n</head>\n<body>\n<center><h1>404 Page Not found!</h1></center>\n</body>\n</html>");
    }
        printf("Message to send: %s\n\n",message);
        printf("FINE!\n");
}

char * updateDB(char *to[]){
    printf("Updating DB...\n");
}