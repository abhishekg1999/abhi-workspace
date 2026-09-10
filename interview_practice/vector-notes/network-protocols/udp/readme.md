## api on server side

sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

recvfrom(sock_fd, buff, sizeof(buff), 0, (struct sockaddr*)&client_addr, &client_len);
sendto(sock_fd, buff, strlen(buff)+1, 0, (struct sockaddr*)&client_addr, client_len);

## api on client side

sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
sendto(sock_fd, buff, strlen(buff)+1, 0, (struct sockaddr*)&server_addr, server_len);
recvfrom(sock_fd, buff, sizeof(buff), 0, NULL, NULL);

