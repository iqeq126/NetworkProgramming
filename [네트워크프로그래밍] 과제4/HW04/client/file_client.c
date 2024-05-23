#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sd, i = 0; //소켓 디스크립터 
	int nbyte = BUF_SIZE;
	size_t buf_size = BUF_SIZE;
	FILE *fp; //파일 포인터 
	char buf[BUF_SIZE]; //버퍼 크기
	int read_cnt; //읽은 개수 
	struct sockaddr_in serv_adr; //서버 주소 소켓
	if(argc!=3) { //사용방법 표시  ex) ./fclient 127.0.0.1 9190
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	char file_name[BUF_SIZE];
	char your_message[BUF_SIZE];

	sd=socket(PF_INET, SOCK_STREAM, 0);    //소켓 디스크립터 생성

	memset(&serv_adr, 0, sizeof(serv_adr)); //서버의 주소 정보 메모리 할당
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

	connect(sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));  //클라이언트 소켓을 서버 소켓에 연결
	read( sd, file_name, BUF_SIZE );	
	printf("Received File Name : %s\n", file_name);
	fp=fopen(file_name, "wb");


	printf("your message?");
	fgets(your_message, BUF_SIZE, stdin);
	while(your_message[i++]){} //read_cnt가 0이 아니면 계속 읽기
	write(sd, your_message, sizeof(your_message)+1);  //sd를 통해 서버쪽에 메세지 전달.

	while(nbyte != 0){
		nbyte = recv(sd, buf, buf_size, 0);
		fwrite(buf, sizeof(char), nbyte, fp);
	}
	//while(your_message[i++]){} //read_cnt가 0이 아니면 계속 읽기
	//	fwrite((void*)buf, 1, read_cnt, fp); //파일에 읽은 내용 계속 쓰기 
	//	fgets(your_message, i-1,fp);
	//	fwrite(your_message, 1, i-1, fp);
	puts("Received file data\n");
	// write(sd, your_message, sizeof(your_message)+1);  //sd를 통해 서버쪽에 메세지 전달.
	fclose(fp); //파일 닫기 
	close(sd);  //클라이언트 소켓 닫기 
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
