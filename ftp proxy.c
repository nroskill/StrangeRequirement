/*
	By Nroskill 年代久远，不可考
	帮妹子写的大作业，写一个ftp的代理，可以缓存文件和转发命令 
*/ 
#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#define BUFFSIZE 1460

void monitorSocket(int *maxfd, int sockfd, fd_set *master_set)
{
    if(sockfd > *maxfd)
        *maxfd = sockfd;
    FD_SET(sockfd, master_set);
}

void closeSocket(int *sockfd, fd_set *master_set)
{
    if(!*sockfd)
        return;
    close(*sockfd);
    FD_CLR(*sockfd, master_set);
    *sockfd = 0;
}

void translateStrToAddress(char *str, char *ip, int *port)
{
    int i;
    int count = 0;

    *port = 0;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] == ',')
        {
            if (++count == 4)
                break;
            ip[i] = '.';
        }
        else
            ip[i] = str[i];
    }
    ip[i] = '\0';
    *port += 256 * getReturnNum(str + i + 1);
    *port += getReturnNum(strchr(str + i + 1, ',') + 1);
}

char *itoa(int value, char *string, int radix)
{
    int i = 0;
    char temp[20];

    if (radix > 10)
    {
        printf("itoa error\n");
        exit(1);
    }

    while (value > 0)
    {
        temp[i++] = '0' + value % radix;
        value /= radix;
    }
    temp[i] = 0;

    for(i = 0; i < strlen(temp); i++)
        string[strlen(temp) - i - 1] = temp[i];
    string[strlen(temp)] = 0;

    return string;
}

void getHostAddress(char* result)
{
    int inet_sock;
    struct ifreq ifr;
    inet_sock = socket(AF_INET, SOCK_DGRAM,0);
    strcpy(ifr.ifr_name, "eth0");

    if (ioctl(inet_sock, SIOCGIFADDR, &ifr) <  0)
        perror("ioctl");
    strcpy(result, inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));
}

int bindAndListenSocket(int listen_port)
{
    int on = 1;
    struct sockaddr_in addr;
    int sockfd;
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(listen_port);
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() failed.\n");
        exit(1);
    }

    if ((bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)))<0)
    {
        perror("bind() failed\n");
        exit(1);
    }
        
    if (listen(sockfd, 128) < 0) 
    {
        perror("listen() error\n");
        exit(1);
    }
    printf("listen on port %d\n", listen_port);
    return sockfd;
}

int acceptSocket(int sockfd)
{
    int new_socket;

    new_socket = accept(sockfd, NULL, NULL);
    if (new_socket < 0)
    {
        perror("acceptance error. \n");
        exit(1);
    }

    printf("listen success on socket %d, new sockefd = %d\n", sockfd, new_socket);
    return new_socket;
}

int connectTo(char *ip, int port, int bindNum)
{
    struct sockaddr_in ServAddr;
    int sockfd;
    struct sockaddr_in addr;
    int tmp = 1;

    printf("connect to %s:%d...\n", ip, port);

    memset(&ServAddr, 0, sizeof(ServAddr));
    ServAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &ServAddr.sin_addr);
    ServAddr.sin_port = htons(port);
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() failed.\n");
        exit(1);
    }

    if(bindNum != 0)
    {
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int));
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(bindNum);
        if ((bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)))<0)
        {
            perror("bind() failed\n");
            exit(1);
        }
    }
    
    if (connect(sockfd, (struct sockaddr *)&ServAddr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("connect failed.\n");
        exit(1);
    }

    printf("connect succeed, sockfd = %d.\n", sockfd);

    return sockfd;
}

int getReturnNum(char *str)
{
    int i = 0, s = 0;
    while (str[i] >= '0' && str[i] <= '9')
    {
        s *= 10;
        s += str[i] - '0';
        i++;
    }
    return s;
}

void getCurrentPath(char* current_path)
{
    getcwd(current_path, 128);
    strcat(current_path, "/");
}

int isExists(char* filename)
{
    char current_path[128];

    getCurrentPath(current_path);
    strcat(current_path, filename);

    if(access(current_path, F_OK) == -1)
        return 0;
    else
        return 1;
}

void send_file(int fd, int data_socket)
{
    char fileCont[BUFFSIZE];
    int msgSize;
    
    msgSize = read(fd, fileCont, BUFFSIZE);
    while (msgSize > 0)
    {
        printf("ftpdata length = %d, state = %d\n", msgSize, (int)write(data_socket, fileCont, msgSize));
        msgSize = read(fd, fileCont, BUFFSIZE);
    }

    printf("The file transfer completed.\n");
}


void nextDataPort(int *port)
{
    if(!*port)
        *port = 2333;
    else
       (*port)++;
}

int main(int argc, char *argv[])
{
    fd_set master_set, working_set;  
    struct timeval timeout;   
    int proxy_cmd_socket    = 0;
    int accept_cmd_socket   = 0;
    int connect_cmd_socket  = 0;
    int proxy_data_socket   = 0;
    int accept_data_socket  = 0;
    int connect_data_socket = 0;
    int selectResult = 0;
    int maxfd = 0;

    int read_filefd = 0, write_filefd = 0;
    int proxy_data_port = 0;
    struct sockaddr_in ClientAddr;
    char current_path[128];
    int isActiveMode = 0;
    int isUploading = 0;
    char ip[32] = {0};
    int port = 0;
    int i;
    int accept_bug_socket = 0;
    int connect_bug_socket = 0;

    if (argc != 3)
    {
        printf("Usage: %s <FTP SERVER IP_Address> <FTP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    getCurrentPath(current_path);

    FD_ZERO(&master_set);
    bzero(&timeout, sizeof(timeout));

    proxy_cmd_socket = bindAndListenSocket(21);
    monitorSocket(&maxfd, proxy_cmd_socket, &master_set);
    
    timeout.tv_sec = 600;
    timeout.tv_usec = 0;

    nextDataPort(&proxy_data_port);

    while (1)
    {
        FD_ZERO(&working_set);
        memcpy(&working_set, &master_set, sizeof(master_set));
        
        selectResult = select(maxfd + 1, &working_set, NULL, NULL, &timeout);
        
        if (selectResult < 0)
        {
            perror("select() failed\n");
            exit(1);
        }
        
        
        if (selectResult == 0)
        {
            printf("select() timed out.\n");
            exit(1);
        }
        
        for (i = 1; i <= maxfd; i++)
        {
            if (FD_ISSET(i, &working_set))
            {
                int ret = 0, j = 0, t = 0;
                char buff[BUFFSIZE] = {0};

                if (i != proxy_cmd_socket && i != proxy_data_socket)
                    ret = read(i, buff, BUFFSIZE);

                if (i == proxy_cmd_socket)
                {
                    printf("proxy_cmd_socket:\n");
                    if ( !accept_bug_socket && !connect_bug_socket)
                    {
                        accept_bug_socket = acceptSocket(proxy_cmd_socket);
                        connect_bug_socket = connectTo(argv[1], getReturnNum(argv[2]), 0);
                        monitorSocket(&maxfd, accept_bug_socket, &master_set);
                        monitorSocket(&maxfd, connect_bug_socket, &master_set);
                    }
                    else
                    {
                        accept_cmd_socket = acceptSocket(proxy_cmd_socket);
                        connect_cmd_socket = connectTo(argv[1], getReturnNum(argv[2]), 0);
                        monitorSocket(&maxfd, accept_cmd_socket, &master_set);
                        monitorSocket(&maxfd, connect_cmd_socket, &master_set);
                    }
                }

                if (i == accept_cmd_socket || i == accept_bug_socket)
                {
                    char cmd[128] = {0};
                    char addr[128] = {0};

                    printf("accept_cmd_socket:\n");

                    int server_socket = i == accept_cmd_socket ? connect_cmd_socket : connect_bug_socket;
                    int client_socket = i == accept_cmd_socket ? accept_cmd_socket : accept_bug_socket;

                    if (i == accept_cmd_socket && ret <= 0)
                    {
                        closeSocket(&accept_cmd_socket, &master_set);
                        closeSocket(&connect_cmd_socket, &master_set);
                        continue;
                    }
                    
                    if (i == accept_bug_socket && ret <= 0)
                    {
                        closeSocket(&accept_bug_socket, &master_set);
                        closeSocket(&connect_bug_socket, &master_set);
                        continue;
                    }
                    
                    write(1, "cmd  client->proxy : ", 21);
                    write(1, buff, ret);
                    getHostAddress(addr);
                    for(j = 0; j < strlen(addr); j++)
                        if(addr[j] == '.')
                            addr[j] = ',';

                    if (!strncmp(buff, "PORT", 4))
                    {
                        isActiveMode = 1;
                        closeSocket(&proxy_data_socket, &master_set);
                        proxy_data_socket = bindAndListenSocket(proxy_data_port);//寮�鍚痯roxy_data_socket銆乥ind锛堬級銆乴isten鎿嶄綔
                        monitorSocket(&maxfd, proxy_data_socket, &master_set);
                        sprintf(cmd, "PORT %s,%d,%d\r\n", addr, proxy_data_port / 256, proxy_data_port % 256);
                        
                        nextDataPort(&proxy_data_port);
                        write(1, "cmd  proxy->server : ", 21);
                        write(1, cmd, strlen(cmd));
                        write(server_socket, cmd, strlen(cmd));
                        
                        strncpy(addr, buff + 5, ret - 7);
                        addr[ret - 7] = 0;
                        translateStrToAddress(addr, ip, &port);

                        continue;
                    }


                    if (!strncmp(buff, "PASV", 4))
                    {
                        isActiveMode = 0;
                        
                        write(1, "cmd  proxy->server : ", 21);
                        write(1, buff, ret);
                        write(server_socket, buff, ret);
                        
                        closeSocket(&proxy_data_socket, &master_set);
                        proxy_data_socket = bindAndListenSocket(proxy_data_port);//寮�鍚痯roxy_data_socket銆乥ind锛堬級銆乴isten鎿嶄綔
                        monitorSocket(&maxfd, proxy_data_socket, &master_set);
                        continue;
                    }


                    if (!strncmp(buff, "RETR", 4))
                    {
                        char filename[128];
                        char filepath[128];
                        strncpy(filename, buff + 5, ret - 7);
                        filename[ret - 7] = 0;
                        sprintf(filepath, "%s%s", current_path, filename);

                        if(!isExists(filename))
                        {
                            write_filefd = open(filepath, O_CREAT | O_WRONLY | O_APPEND);
                            if (write_filefd < 0)
                            {
                                printf("open failed");
                                exit(1);
                            }
                            write(1, "cmd  proxy->server : ", 21);
                            write(1, buff, ret);
                            write(server_socket, buff, ret);
                        }

                        else
                        {
                            read_filefd = open(filepath, O_RDONLY);
                            if (read_filefd < 0)
                            {
                                printf("open failed");
                                exit(1);
                            }

                            if (isActiveMode)
                            {
                                connect_data_socket = connectTo(ip, port, 20);
                                monitorSocket(&maxfd, connect_data_socket, &master_set);
    
                                write(1, "cmd  proxy->client : ", 21);
                                write(1, "150 Opening data channel for file download\r\n", 44);
                                write(client_socket, "150 Opening data channel for file download\r\n", 44);
    
                                send_file(read_filefd, connect_data_socket);
                                write(1, "cmd  proxy->client : ", 21);
                                write(1, "226 Successfully transfered\r\n", 29);
                                write(client_socket, "226 Successfully transfered\r\n", 29);
                                close(read_filefd);
                                read_filefd = 0;

                                closeSocket(&connect_data_socket, &master_set);
                            }
                        }
                        continue;
                    }

                    if (!strncmp(buff, "STOR", 4))
                    {
                        isUploading = 1;
                    }

                    write(1, "cmd  proxy->server : ", 21);
                    write(1, buff, ret);
                    write(server_socket, buff, ret);
                }

                if (i == connect_cmd_socket || i == connect_bug_socket)
                {
                    printf("connect_cmd_socket:\n");
                    
                    int server_socket = i == connect_cmd_socket ? connect_cmd_socket : connect_bug_socket;
                    int client_socket = i == connect_cmd_socket ? accept_cmd_socket : accept_bug_socket;

                    if (i == connect_cmd_socket && ret <= 0)
                    {
                        closeSocket(&accept_cmd_socket, &master_set);
                        closeSocket(&connect_cmd_socket, &master_set);
                        continue;
                    }
                    
                    if (i == connect_bug_socket && ret <= 0)
                    {
                        closeSocket(&accept_bug_socket, &master_set);
                        closeSocket(&connect_bug_socket, &master_set);
                        continue;
                    }

                    write(1, "cmd  server->proxy : ", 21);
                    write(1, buff, ret);

                    if (!strncmp(buff, "227", 3))
                    {
                        char addr[64] = {0};
                        char cmd[128] = {0};

                        strncpy(addr, strchr(buff, '(') + 1, strchr(buff, ')') - strchr(buff, '(') - 1);
                        translateStrToAddress(addr, ip, &port);
                        
                        getHostAddress(addr);
                        for(j = 0; j < strlen(addr); j++)
                            if(addr[j] == '.')
                                addr[j] = ',';
                        sprintf(cmd, "227 Entering Passive Mode (%s,%d,%d)\r\n", addr, proxy_data_port / 256, proxy_data_port % 256);

                        nextDataPort(&proxy_data_port);
                        write(1, "cmd  proxy->client : ", 21);
                        write(1, cmd, strlen(cmd));
                        write(client_socket, cmd, strlen(cmd));
                        
                        continue;
                    }
                    
                    write(1, "cmd  proxy->client : ", 21);
                    write(1, buff, ret);
                    write(client_socket, buff, ret);
                }
                
                if (i == proxy_data_socket)
                {
                    printf("proxy_data_socket:\n");
                    closeSocket(&accept_data_socket, &master_set);
                    closeSocket(&connect_data_socket, &master_set);

                    if (isActiveMode)
                    {
                        accept_data_socket = acceptSocket(proxy_data_socket);
                        monitorSocket(&maxfd, accept_data_socket, &master_set);

                        connect_data_socket = connectTo(ip, port, 20);
                        monitorSocket(&maxfd, connect_data_socket, &master_set);
                    }

                    else
                    {
                        connect_data_socket = acceptSocket(proxy_data_socket);
                        monitorSocket(&maxfd, connect_data_socket, &master_set);

                        if (read_filefd)
                        {
                            send_file(read_filefd, connect_data_socket);
                            write(1, "cmd  proxy->client : ", 21);
                            write(1, "150 Opening data channel for file download\r\n", 44);
                            write(accept_cmd_socket, "150 Opening data channel for file download\r\n", 44);
                            write(1, "cmd  proxy->client : ", 21);
                            write(1, "226 Successfully transfered\r\n", 29);
                            write(accept_cmd_socket, "226 Successfully transfered\r\n", 29);
                            close(read_filefd);
                            read_filefd = 0;

                            closeSocket(&connect_data_socket, &master_set);
                        }
                        else
                        {
                            accept_data_socket = connectTo(ip, port, 0);
                            monitorSocket(&maxfd, accept_data_socket, &master_set);
                        }
                    }
                }

                if (i == accept_data_socket)
                {
                    printf("accept_data_socket:\n");

                    if (ret <= 0)
                    {

                        if(write_filefd)
                            close(write_filefd);
                        write_filefd = 0;

                        closeSocket(&accept_data_socket, &master_set);
                        closeSocket(&connect_data_socket, &master_set);
                        continue;
                    }

                    else
                    {

                        if (write_filefd != 0)
                        {
                            printf("ftpdata server->proxy length = %d, state = %d\n", ret, (int)write(write_filefd, buff, ret));
                            printf("ftpdata proxy->client length = %d, state = %d\n", ret, (int)write(connect_data_socket, buff, ret));
                        }

                        else
                        {
                            write(1, "data server->proxy : ", 21);
                            write(1, buff, ret);
                            write(1, "data proxy->client : ", 21);
                            write(1, buff, ret);
                            write(connect_data_socket, buff, ret);
                        }
                    }
                }

                if (i == connect_data_socket)
                {
                    printf("connect_data_socket:\n");

                    if (ret <= 0)
                    {
                        isUploading = 0;

                        closeSocket(&accept_data_socket, &master_set);
                        closeSocket(&connect_data_socket, &master_set);
                        continue;
                    }

                    else
                    {

                        if (isUploading)
                        {
                            printf("ftpdata client->proxy length = %d, state = %d\n", ret, ret);
                            printf("ftpdata proxy->server length = %d, state = %d\n", ret, (int)write(accept_data_socket, buff, ret));
                        }
                        
                        else
                        {
                            write(1, "data client->proxy : ", 21);
                            write(1, buff, ret);
                            write(1, "data proxy->server : ", 21);
                            write(1, buff, ret);
                            write(accept_data_socket, buff, ret);
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}
