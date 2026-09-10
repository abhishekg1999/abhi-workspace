#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){

	int sock_fd;
	struct sockaddr_in server_addr;

	char buff[50];
	socklen_t server_len = sizeof(server_addr);

	//create udp socket
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	//configure server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	printf("enter the data:\n");
	scanf("%s", buff);
	sendto(sock_fd, buff, strlen(buff)+1, 0, (struct sockaddr*)&server_addr, server_len);

	recvfrom(sock_fd, buff, sizeof(buff), 0, NULL, NULL);
        printf("recived : %s\n", buff);

	close(sock_fd);
	return 0;
}
