#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#define PORT 8080

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0;
    char struid[10];
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0))<0) {
        printf("\n Error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nFailed \n");
        return -1;
    }
    int userId = getuid();    
    sprintf(struid, "%d", userId);
    send(sock, struid, strlen(struid), 0);
 
    if (userId != 0) {
        read(sock, buffer, 1024); 
        bzero(buffer, 1024);
        sprintf(buffer, "%s:%s\n", argv[2], argv[4]);
        send(sock, buffer, strlen(buffer), 0);
    }
    bzero(buffer, 1024);
    read(sock, buffer, 1024);
    printf("%s\n", buffer);
 
    if (strcmp(buffer, "Invalid username or password") == 0) {
        return 0;
    }
    char qry[255];
    for(;;) {
        printf("-# ");
        scanf(" %[^\n]", qry);
        send(sock, qry, strlen(qry), 0);
        bzero(buffer, 1024);
        read(sock, buffer, 1024);
        printf("%s\n", buffer);
    }
    return 0;
}
