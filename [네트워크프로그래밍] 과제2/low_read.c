#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 100
void error_handling(char* message);

int main(void){
	int fd;
	char buf[BUF_SIZE];
	fd = open("data.txt", O_RDONLY);
	if(fd == -1)
		error_handling("open() error!");

	printf("file descriptor: %d \n", fd);
	int size = read(fd, buf, sizeof(buf));
		
	while(1){
		size = read(fd, buf, sizeof(buf));
		printf("%s", buf);
		if(size <= 0) {
			error_handling("read() error!");
			break;			
		}		
	}

	close(fd);
	return 0;
}


void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
