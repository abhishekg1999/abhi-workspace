#######  Deploy the API using bellow steps #########

## prerequisites 
Install "websocat" on the client machine.

for ubuntu)
	cd /usr/local/bin
	sudo wget https://github.com/vi/websocat/releases/download/v1.12.0/websocat.x86_64-unknown-linux-musl
	sudo chmod +x websocat.x86_64-unknown-linux-musl
	sudo mv websocat.x86_64-unknown-linux-musl websocat
	
for windows)
	download and intall nodejs from official website 
	install websocat using npm
	npm npm install -g wscat

## Deploy API on Server (Target Board)

1. Download the source ZIP file and transfer it to the target board.
2. Extract the ZIP file and navigate to the extracted directory:
3. Build and install:
make clean
make install


## Call API from Client (Windows/Linux PC)

Linux:
websocat ws://172.24.111.211:9000 | jq

Windows:
wscat -c ws://172.24.111.211:9000


## Server Control (Optional)

Start server:
service ws_server start

Stop server:
service ws_server stop

Check server status:
service ws_server status

