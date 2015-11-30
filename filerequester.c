#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFSIZE 1280000
#define FILESIZE 256

int SetupTCPClientSocket(const char *host, const char *service);		/* function prototype to set up tcp socket */
//void writeFile(char *fileName, char *fileContents, size_t fileSize);	/* function prototype to write file to system */

int main(int argc, char *argv[]) {

	if(argc != 4) {
		fprintf(stderr, "Sorry, you did not input the proper number of arguments\n");
		exit(1);
	}

	char *server = argv[1];
	char *filename = argv[2];
	char *port = argv[3];
		
	int sock = SetupTCPClientSocket(server, port);

	if(sock < 0) {
		fprintf(stderr, "Sorry, it couldn't connect\n");
		exit(1);
	}


	//Send the string to the server
	ssize_t numBytes = send(sock, filename, strlen(filename), 0);
	if(numBytes < 0) {
		printf("Number of bytes less than 0\n");
		exit(1);
	}
	else if (numBytes != strlen(filename)) {
		printf("Number of bytes does not equal starting guess length\n");
		exit(1);
	}
	
	// Receive the string back from the server
	uint64_t totalBytes = 0;
	char buffer[BUFSIZE]; // I/O Buffer
	//fputs("Received: ", stdout);
	
	uint64_t fileSize;
	recv(sock, &fileSize, sizeof(uint64_t), 0);
	fileSize = ntohl(fileSize);
	
	FILE *fp = fopen(filename, "w+b");
	
	if(fp == NULL) {
		fprintf(stderr, "Cannot save file because it won't open\n");
	}
	
	while(totalBytes < fileSize) {
		size_t readSize = fileSize;
		
		if (readSize > BUFSIZE)
			readSize = BUFSIZE;
		
		numBytes = recv(sock, buffer, readSize, 0);
		if(numBytes <= 0) {
			printf("Number of bytes in response is less than or equal to zero\n");
			exit(1);
		}
		
		fwrite(buffer, sizeof(char), numBytes, fp);
		totalBytes += numBytes;
		//buffer[numBytes] = '\0'; // Terminate the string
		//fputs(buffer, stdout); // Print the buffer
	}	
	
	fclose(fp);
	fprintf(stderr, "The total number of bytes is %zd\n", totalBytes);
	//writeFile(filename, buffer, numBytes);
	fprintf(stderr, "The file %s was successfully written\n", filename);
	close(sock);
	//exit(0);
	return 0;
}

int SetupTCPClientSocket(const char *host, const char *service) {
  // Tell the system what kind(s) of address info we want
  struct addrinfo addrCriteria;                   // Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             // v4 or v6 is OK
  addrCriteria.ai_socktype = SOCK_STREAM;         // Only streaming sockets
  addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol

  // Get address(es)
  struct addrinfo *servAddr; // Holder for returned list of server addrs
  int rtnVal = getaddrinfo(host, service, &addrCriteria, &servAddr);

  if(rtnVal < 0)
	 exit(1);

  int sock = -1;
  struct addrinfo *addr = servAddr;
  for (; addr != NULL; addr = addr->ai_next) {
    // Create a reliable, stream socket using TCP
    sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (sock < 0)
      continue;  // Socket creation failed; try next address

    // Establish the connection to the echo server
    if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0)
      break;     // Socket connection succeeded; break and return socket

    close(sock); // Socket connection failed; try next address
    sock = -1;
  }

  freeaddrinfo(servAddr); // Free addrinfo allocated in getaddrinfo()
  return sock;
}
