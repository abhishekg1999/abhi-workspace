#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){

	int sock_fd;
	struct sockaddr_in server_addr;
	char buff[50];

	//create socket
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	//configure server address 
	server_addr.sin_family = AF_INET;  //for ipv4 protocol
	server_addr.sin_port = htons(9000); 
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// connect to server
	connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	printf("connected to server..\n");

	// data exchange 
	printf("enter data:\n");
	scanf("%s", buff);
	send(sock_fd, buff, strlen(buff)+1, 0);

	recv(sock_fd, buff, sizeof(buff), 0);
        printf("recieved data : %s\n", buff);

	close(sock_fd);
	return 0;
}
