#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8080
 
int server_fd, new_socket, val;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
 
char fdDatabase[] = "DB";
char cwDB[255] = {0};
int root = 0;
int acc = 0;
 
struct logged{
    char id[255], password[255];
};
struct logged login;
 
char* grantedPermission(char str[]) {
    char* tra;
    char database[50], id[50], msg[255];
    bzero(database, 50); 
    bzero(id, 50); 
    bzero(msg, 255);
    char tmp[512]; 
    bzero(tmp, 255); 
    strcpy(tmp, str);
    char* tkn;
    char* rest = tmp;
    int z = 0;
 
    while ((tkn = strtok_r(rest, " ", &rest))) {
        if (z == 4) {
            strncpy(database, tkn, strlen(tkn) - 1);
        }
        else if (z == 2) {
            strcpy(id, tkn);
        }
        else if ((z == 3 && strcmp(tkn, "INTO")) || z > 4) {
            strcpy(msg, "Syntax error.");
            tra = msg;
            return tra;
        }
        z++;
    }
    printf("id:%s\n", id);
    printf("database:%s\n", database);
    FILE* opfile = fopen("DB/user/user.txt", "r");
    char baris[80]; int isUser = 0;
    while(fgets(baris, 80, opfile)) {
        if (!strncmp(baris, id, strlen(id))) {
            isUser = 1;
            break;
        }
    }
    fclose(opfile);
 
    if (!isUser) {
        strcpy(msg, "No user found");
        tra = msg;
        return tra;
    }
 
    char fileName[255];
    sprintf(fileName, "%s/%s/granted_user.txt", fdDatabase, database);
    printf("%s\n", fileName);
    opfile = fopen(fileName, "a");
    if (!opfile) {
        strcpy(msg, "No database found");
        tra = msg;
        return tra;
    }
    fprintf(opfile, "%s\n", id);
    fclose(opfile);
 
    strcpy(msg, "Access Granted");
    tra = msg; return tra;
}
 
char* useDb(char str[]) {
    char msg[255];
    bzero(msg, 255);
    char* tmp = str + 4;
    char database[50]; 
    bzero(database, 50); 
    strncpy(database, tmp, strlen(tmp) - 1);
    char fileName[100];
    bzero(fileName, 100);
    sprintf(fileName, "%s/%s/granted_user.txt", fdDatabase, database);
 
    FILE* opfile = fopen(fileName, "r");
    if (!opfile) {
        strcpy(msg, "No such database/");
        tmp = msg;
        return tmp;
    }
    char baris[80];
    while(fgets(baris, 80, opfile)) {
        if (strncmp(baris, login.id, strlen(login.id)==0)) {
            fclose(opfile);
            strcpy(msg, "Current Working Database changed");
            tmp = msg;
            strcpy(cwDB, database);
            return tmp;
        }
    }
    fclose(opfile);
    strcpy(msg, "Permission not granted.");
    tmp = msg;
    return tmp;
}
char* createUser(char str[]) {
    char* tra;
    char msg[255]; 
    bzero(msg, 255);
    char id[255], 
    password[255];
    bzero(id, 255); 
    bzero(password, 255);
    char tmp[512]; 
    bzero(tmp, 512); 
    strcpy(tmp, str);
    char* tkn;
    char* rest = tmp;
    int z = 0;
 
    while ((tkn = strtok_r(rest, " ", &rest))) {
        if (z == 2) {
            strcpy(id, tkn);
        }
        else if ((z == 3 && strcmp(tkn, "IDENTIFIED")) || (z == 4 && strcmp(tkn, "BY")) || z > 5) {
            strcpy(msg, "Syntax Error");
            tra = msg;
            return tra;
        }
        else if (z == 5) {
            strncpy(password, tkn, strlen(tkn) - 1);
        }
        z++;
    }
 
    bzero(tmp, 512);
    sprintf(tmp, "%s:%s", id, password);
    writeFile("DB/user/user.txt", tmp, "a");
    strcpy(msg, "USER CREATED SUCCESSFULLY");
    tra = msg;
    return tra;
}
 
void writeFile(char fileName[], char str[], char mode[]) {
    FILE* opfile = fopen(fileName, mode);
    fprintf(opfile, "%s\n", str);
    fclose(opfile);
}
 
void connectt() {
    char buffer[1024] = {0}, msg[1024] = {0};
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    val = read(new_socket, buffer, 1024);
 
    if (!strcmp(buffer, "0")) {
        strcpy(msg, "Welcome, root");
        acc = 1;
        root = 1;
        strcpy(login.id, "root");
    }
 
    if (!acc) {
        send(new_socket, "tes", 10, 0);
        bzero(buffer, 1024);
 
        val = read(new_socket, buffer, 1024);
 
        if (check(buffer)) {
            acc = 1;
            strcpy(msg, "Welcome User!");
            char* tra = buffer;
            char* separator = strchr(buffer, ':') + 1;
            strcpy(login.password, separator);
            strncpy(login.id, tra, strlen(tra) - strlen(separator) - 1);
        }
        else {
            strcpy(msg, "Invalid username or password");
        }
    }
    send(new_socket, msg, strlen(msg), 0);
}
 
int check(char str[]) {
    char fileName[512];
    sprintf(fileName, "%s/user/user.txt", fdDatabase);
    FILE* opfile = fopen(fileName, "r");
    char baris[80];
    while(fgets(baris, 80, opfile)) {
        if (!strcmp(baris, str)) {
            fclose(opfile);
            return 1;
        }
    }
    fclose(opfile);
    return 0;
}

char* createDb(char str[]){
    char* tra;
	char msg[1024];

	char dbName[1024];
	bzero(dbName, 1024);

	int z;
	char prs[1024];
	strcpy(prs, str);
	char* prsptr = prs;
	char* tkn;

	for (z = 0; tkn = strtok_r(prsptr, " ", &prsptr); z++)
	{
		if (z == 2)
		{
			strncpy(dbName, tkn, strlen(tkn) - 1);
		}
	}

	char dbPath[1024];
	sprintf(dbPath, "%s/%s", fdDatabase, dbName);
	char* path = dbPath;

	if (mkdir(path, 0777) != 0)
	{
		strcpy(msg, "Failed creating database!");
		tra = msg;
		return tra;
	}

	char granted[1024];
	strcpy(granted, dbPath);
	strcat(granted, "/granted_user.txt");

    FILE* opfile = fopen(granted, "a");
    fprintf(opfile, "%s\n", login.id);
    fclose(opfile);

	strcpy(msg, "Create Database succeed!");
	tra = msg;
	return tra;
}

int main() {
    // pid_t pid, sid;
    // pid = fork();
 
    // if (pid < 0) {
    //     exit(EXIT_FAILURE);
    // }
 
    // if (pid > 0) {
    //     exit(EXIT_SUCCESS);
    // }
 
    // umask(0);
 
    // sid = setsid();
    // if (sid < 0) {
    //     exit(EXIT_FAILURE);
    // }
 
    // if ((chdir("/")) < 0) {
    //     exit(EXIT_FAILURE);
    // }
 
    // close(STDIN_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);
 
    char buffer[1024] = {0}, msg[1024] = {};
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed.");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
 
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    connectt();
 
    mkdir("DB", 0777);
    mkdir("DB/user", 0777);
    FILE* opfile = fopen("DB/user/user.txt", "a");
    if(opfile)
        fclose(opfile);
 
    for(;;) {
        bzero(buffer, 1024);
        bzero(msg, 1024);
        val = read(new_socket, buffer, 1024);
        if (!val) {
            acc = 0;
            root = 0;
            connectt();
            continue;
        }
        printf("- %s\n", buffer);
        if (buffer[strlen(buffer) - 1] != ';') {
            strcpy(msg, "Syntax Error");
        }
        else if (strncmp(buffer, "CREATE USER", 11)==0) {
            if (root)
                strcpy(msg, createUser(buffer));
            else 
                strcpy(msg, "Permission Denied.");
        }
        else if (strncmp(buffer, "USE", 3)==0) {
            strcpy(msg, useDb(buffer));
        }
        else if (strncmp(buffer, "GRANT PERMISSION", 16)==0) {
            if (!root)
                strcpy(msg, "Permission Denied");
            else 
                strcpy(msg, grantedPermission(buffer));
        }
        else if(strncmp(buffer, "CREATE DATABASE", 15)==0){
            strcpy(msg, createDb(buffer));
        }
        else {
            strcpy(msg, "Wrong Query.");
        }
        send(new_socket, msg, strlen(msg), 0);
    }
}
