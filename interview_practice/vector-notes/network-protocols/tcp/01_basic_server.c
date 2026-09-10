#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){

	int server_fd, client_fd;
	struct sockaddr_in server_addr;
	char buff[50];

	//create socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	//configure server address 
	server_addr.sin_family = AF_INET;  //for ipv4 protocol
	server_addr.sin_port = htons(9000); 
	server_addr.sin_addr.s_addr = INADDR_ANY;

	//bind socket to IP + Port 
	bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));


	//listen to client 
	listen(server_fd, 5);  

	//accept client connection
	client_fd = accept(server_fd, NULL, NULL);
	printf("client connected..\n");

	// data exchange 
	recv(client_fd, buff, sizeof(buff), 0);
	printf("recieved data : %s\n", buff);

	printf("enter data:\n");
	scanf("%s", buff);
	send(client_fd, buff, strlen(buff)+1, 0);

	close(client_fd);
	close(server_fd);

	return 0;
}
