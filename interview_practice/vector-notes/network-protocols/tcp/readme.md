## api used in server
int server_fd, client_fd;
server_fd = socket(AF_INET, SOCK_STREAM, 0);
bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
listen(server_fd, 5);
client_fd = accept(server_fd, NULL, NULL);

recv(client_fd, buff, sizeof(buff), 0);
send(client_fd, buff, strlen(buff)+1, 0);

## api used in client
int sock_fd;
sock_fd = socket(AF_INET, SOCK_STREAM, 0);
connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

send(sock_fd, buff, strlen(buff)+1, 0);
recv(sock_fd, buff, sizeof(buff), 0);


## Explanation :
#server_addr.sin_family = AF_INET;
What it means: This sets the Address Family to IPv4.
Why it matters: It explicitly tells the operating system's networking stack that this socket will handle standard 32-bit IP addresses (like 192.168.1.1 or 127.0.0.1) rather than IPv6 (AF_INET6) or local file-based sockets (AF_UNIX).

#server_addr.sin_port = htons(9000);
What it means: This assigns port number 9000 to the server, converted to network byte order.
Why it matters: Computers store numbers in memory differently depending on their architecture (usually "Little Endian" on standard Intel/AMD/ARM CPUs). Networks strictly use "Big Endian" (Network Byte Order). The htons() function (host to network short) automatically rearranges the bytes of the number 9000 so routers and network cards can read it correctly.

#server_addr.sin_addr.s_addr = INADDR_ANY;
What it means: This binds the server to all available network interfaces on the host machine.
Why it matters: If your server machine has multiple network connections (e.g., an Ethernet port, a Wi-Fi card, and a local loopback interface), INADDR_ANY (which maps to 0.0.0.0) tells the kernel: "Accept incoming connections on port 9000 no matter which network interface they come through."


