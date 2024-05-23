// 다음의 조건으로 프로그램을 작성하시오.
// 1. 멀티프렉싱 기법(select함수)을 이용하여 윈도우 기반의 채팅 서버 프로그램을 작성하시오.
//  (아래의 echo 멀티플렉싱 서버 구현 코드 참조)
//  명령행 규칙 : chat  127.0.0.1  9190  nickName

//==echo_selectserv.c ==
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1044
#define TIME_OUT 64
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAdr, clntAdr;
    TIMEVAL timeout;
    fd_set reads, cpyReads;
 
    int adrSz;
    int strLen, fdNum, i, j;
    char buf[BUF_SIZE];

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling((char*)"WSAStartup() error!");

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling((char*)"bind() error");
    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling((char*)"listen() error");

    FD_ZERO(&reads);
    FD_SET(hServSock, &reads);
    while (1)
    {
        cpyReads = reads;
        timeout.tv_sec = 5; 
        timeout.tv_usec = 5000;

        if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
            break;

        if (fdNum == 0)   continue;

        for (i = 0; i < reads.fd_count; i++)
        {
            if (FD_ISSET(reads.fd_array[i], &cpyReads))

            {

                if (reads.fd_array[i] == hServSock) // connection request!
                {
                    adrSz = sizeof(clntAdr);
                    hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
                    FD_SET(hClntSock, &reads);
                    printf("connected client: %d \n", hClntSock);
                }

                else // read message!
                {
                    strLen = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0);
                    if (strLen == 0) // close request!
                    {
                        FD_CLR(reads.fd_array[i], &reads);
                        closesocket(cpyReads.fd_array[i]);
                        printf("closed client: %d \n", cpyReads.fd_array[i]);
                    }
                    else
                    {
                        for (j = 0; j < reads.fd_count; j++) {
                            if(j != i)
                                send(reads.fd_array[j], buf, strLen, 0); // echo!
                        }
                    }
                }
            }
        }
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}




//==echo_client_win.c=====================
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
char name[20];
char nameMessage[BUF_SIZE + 20];
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hSocket;
    char message[BUF_SIZE];
    int strLen;
    SOCKADDR_IN servAdr;

    if (argc != 4) {
        printf("Usage : %s <IP> <port> <nickname>\n", argv[0]);
        exit(1);
    }
    strcpy(name, argv[3]); // 닉네임 지정
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling((char*)"WSAStartup() error!");

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling((char*)"socket() error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling((char*)"connect() error!");
    else
        puts("Connected...........");

    while (1)
    {
        // fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))  break;
        sprintf(nameMessage, "[%s] : %s", name, message); // 메시지 형식 지정
        send(hSocket, nameMessage, strlen(nameMessage), 0);
        strLen = recv(hSocket, nameMessage, BUF_SIZE + 20 - 1, 0);
        nameMessage[strLen] = 0;
        printf("%s", nameMessage);
    }

    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
