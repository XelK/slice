 #include <stdio.h> 			// Per printf() e fprintf()
 #include <sys/socket.h> 	// Per socket(),bind() e connect()
 #include <arpa/inet.h>		// Per sockaddr_in e inet_ntoa()
 #include <string.h>			// Per memset()
 #include <unistd.h>			// Per close()
 #include <stdlib.h>			// Per atoi() e exit()
 #include <time.h>		

#define PORT 8080
#define MAXPENDING 5
#define BUFFERSIZE 1000
#define MAXDB 5
#define MAXURL 20

void handleClient(int client);

char * search(char from[]);
void prepMessage(int code, char from[], char to[], char * message);
int updateDB(char DB[MAXDB][MAXURL], char from[]);
char DB[MAXDB][MAXURL]={"","","","",""};

int main(){
    
    int sock;
    int client;
    struct sockaddr_in addr;

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
            handleClient(client);
            close(client);
    }

}

void handleClient(int client){
    char message[BUFFERSIZE];
    int msg;

    memset(&message,0,sizeof(message));
    
    if((msg=recv(client,message,BUFFERSIZE,0))<0){
        fprintf(stderr,"Error in recive!\n");
        exit(1);
    }

    char from[50]=" "; 
    char to[50];

    char * temp;

    if(strstr(message,"GET") || strstr(message,"HEAD")){

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

        if (strcmp(from," ")==0) // index
            prepMessage(200,from," ",message);
        else if (strcmp(temp=search(from),"")!=0){ // found in DB
            prepMessage(301,from,temp,message);
        }
        else
            prepMessage(404,from,"",message);
    }
    else if(strstr(message,"POST")){
        temp=strstr(message,"url:");
    
        // extract url passed with post
        int x=(strlen("url:"));
        int z=0;
        while(x <= strlen(temp)){
            if(x==strlen(temp))
                from[z++]='\0';
            else
                from[z++]=temp[x++];
        }

        int r=updateDB(DB,from);
        if(r<0){
            fprintf(stderr,"DB FULL!\n");
            exit(1);
        }

        to[0]=r+'0';

        prepMessage(201,from,to,message); //when DB updated!
    }
    else{
        prepMessage(405,from,"",message); //405 Method Not Allowed
    }

    if (send(client,message,strlen(message),0)!=strlen(message)){
        printf("Error in send\n");
        exit(1);
    }

}

char * search(char from[]){
    int i=atoi(from);
    if(i>=0 && i<10)
        return DB[i];
    else
        return "";
}

void prepMessage(int code, char from[], char to[], char * message){

    char t_date[1000];                                                     
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(t_date, sizeof(t_date), "%a, %d %b %Y %H:%M:%S %Z", &tm);

    if (code==200){
        strcpy(message,"HTTP/1.1 200 OK\nDate: "); 
        strcat(message,t_date);
        strcat(message,"\nContent-Type: text/html; charset=UTF-8\nConnection: close\r\n\r\n");
        strcat(message,"<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n<title> Hello from slice server!</title>\n</head>\n<body>\n<center><h1>Slice server!</h1></center>\n</body>\n</html>");
    }
    if (code==201){
        strcpy(message,"HTTP/1.1 201 Created\nDate: ");
        strcat(message,t_date);
        strcat(message,"\nContent-Type: text; charset=UTF-8\nConnection: close\r\n\r\n");
        strcat(message,from);
        strcat(message,"\n");
        strcat(message,to);
    }
    if(code == 301){
        strcpy(message,"HTTP/1.1 301 Moved Permanently\nServer: slice\nDate: ");
        strcat(message,t_date);
        strcat(message,"\nContent-Type: text/html; charset=UTF-8\n");
        strcat(message,"Location: ");
        strcat(message,to);
        strcat(message,"\nAccept-Ranges: bytes\nConnection: close\r\n\r\n");
        strcat(message,"<!DOCTYPE html>\n<html>\n<head>\n<title>301 Moved Permanently</title>\n</head>\n<body>\n<center><h1>301 Moved Permanently to: ");
        strcat(message,to);
        strcat(message,"</h1></center>\n</body>\n</html>");
    }
    if(code==404){
        strcpy(message,"HTTP/1.1 404\ncontent-type: text/html\r\n\r\n");
        strcat(message,"<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n<title>404 Not found!</title>\n</head>\n<body>\n<center><h1>404 Page Not found!</h1></center>\n</body>\n</html>");
    }
    if(code==405){
        strcpy(message,"HTTP/1.1 405\ncontent-type: text/html\r\n\r\n");
        strcat(message,"<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n<title>405 Method not allowed!</title>\n</head>\n<body>\n<center><h1>405 Method not allowed!</h1></center>\n</body>\n</html>");
    }
}

int updateDB(char DB[MAXDB][MAXURL], char from[]){
    for (int i=0; i<MAXDB; i++){
        if ( (int)strlen(DB[i]) == 0 ){
            memcpy(DB[i],from,strlen(from));
            return i;
        }
    }
    return -1;
}