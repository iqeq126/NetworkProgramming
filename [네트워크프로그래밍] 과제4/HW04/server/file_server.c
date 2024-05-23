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

int main(int argc, char *argv[]){
	int serv_sd, clnt_sd;
	FILE *fp;//, file_name;
	char buf[BUF_SIZE];
	char message[BUF_SIZE];
	int read_cnt;

	size_t fsize, nsize = 0;
	size_t fsize2;

	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;

	if(argc != 3){
		printf("Usage : %s <port> <file_name>\n", argv[0]);
		exit(1);
	}
	
	serv_sd=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sd==-1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sd, 5) == -1)
		error_handling("listen() error");

	clnt_adr_sz = sizeof(clnt_adr);
	clnt_sd = accept(serv_sd, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	if(clnt_sd == -1)
		error_handling("accept() error");


	write(clnt_sd, argv[2], BUF_SIZE);

	// print message	
	read(clnt_sd, message, BUF_SIZE);
	printf("Message from client : %s \n", message);

	// send file ( server to client) is argv[2]
	fp=fopen(argv[2], "rb");
	// move file pointer to file end
	fseek(fp, 0, SEEK_END);
	// calculate file size
	fsize = ftell(fp);
	// move file pointer to file start
	fseek(fp, 0, SEEK_SET);
	
	// send file contents
	while ( nsize != fsize){
		// read from file to buf

		int fpsize = fread(buf, 1, BUF_SIZE, fp);
		nsize += fpsize;
		send(clnt_sd, buf, fpsize, 0);
	}	
	/*while(1){
		read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
		if(read_cnt < BUF_SIZE){
			write(clnt_sd, buf, read_cnt);
			break;
		}		
			write(clnt_sd, buf, BUF_SIZE);
	}*/
	//read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
	//fwrite((void*)buf, 1, read_cnt, fp); //파일에 읽은 내용 계속 쓰기 
	shutdown(clnt_sd, SHUT_WR);

	fclose(fp); //fclose(file_name);
	close(clnt_sd); close(serv_sd);
	return 0;
}
void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

