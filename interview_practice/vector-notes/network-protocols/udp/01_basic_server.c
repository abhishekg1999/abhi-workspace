#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){

	int sock_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	char buff[50];
	socklen_t client_len = sizeof(client_addr);

	//create udp socket
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	//configure server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9000);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	//bind IP + Port to socket
	bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	printf("server waiting..\n");

	recvfrom(sock_fd, buff, sizeof(buff), 0, (struct sockaddr*)&client_addr, &client_len);
	printf("recived : %s\n", buff);


	printf("enter the data:\n");
	scanf("%s", buff);
	sendto(sock_fd, buff, strlen(buff)+1, 0, (struct sockaddr*)&client_addr, client_len);

	close(sock_fd);
	return 0;
}
