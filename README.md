# super-computing-at-home

Compilation instruction for client.c and server.c:
gcc filename.c -o filename.o

To run server:
./server.o <AVAILABLE_PORTNUMBER>

To run client:
./client.o <SERVER_HOSTNAME> <SERVER_PORTNUMBER>

NOTE: The client and server may be compiled on separate machines over a LAN. In this case, add server hostname and server IP to the client machine's DNS located at /etc/hosts file.
