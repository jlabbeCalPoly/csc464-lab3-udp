// Client side - UDP Code				    
// By Hugh Smith	4/1/2017		

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "pduLib.h"
#include "cpe464.h"

#define MAXBUF 1400

void talkToServer(int socketNum, struct sockaddr_in6 * server);
int readFromStdin(uint8_t * buffer);
void checkArgs(int argc, char * argv[]);

int main (int argc, char *argv[])
 {
	int socketNum = 0;				
	struct sockaddr_in6 server;		// Supports 4 and 6 but requires IPv6 struct
	int portNumber = 0;
	float errorRate = 0;
	
	checkArgs(argc, argv);
	portNumber = atoi(argv[3]);
	errorRate = atof(argv[1]);

	// debug
	// printf("Error rate: %f\n", errorRate);

	sendErr_init(errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
	
	socketNum = setupUdpClientToServer(&server, argv[2], portNumber);
	
	talkToServer(socketNum, &server);
	
	close(socketNum);

	return 0;
}

void talkToServer(int socketNum, struct sockaddr_in6 * server)
{
	int serverAddrLen = sizeof(struct sockaddr_in6);
	// char * ipString = NULL;
	
	// A few counters/defaults for this lab
	uint32_t sequenceNumberCounter = 0;
	uint8_t defaultFlag = 1;
	int dataLen = 0; 
	uint8_t buffer[MAXBUF];
	uint8_t recvBuffer[MAXBUF + 7];
	
	buffer[0] = '\0';
	while (buffer[0] != '.')
	{
		dataLen = readFromStdin(buffer);
		// printf("Sending: %s with len: %d\n", buffer,dataLen);
		
		// Include space for the header to be added on
		uint8_t pduBuffer[7 + dataLen];
		dataLen = createPDU(pduBuffer, sequenceNumberCounter, defaultFlag, buffer, dataLen);
		sequenceNumberCounter += 1;
	
		safeSendto(socketNum, pduBuffer, dataLen, 0, (struct sockaddr *) server, serverAddrLen);
		
		dataLen = safeRecvfrom(socketNum, recvBuffer, MAXBUF + 7, 0, (struct sockaddr *) server, &serverAddrLen);
		printPDU(recvBuffer, dataLen);
		
		// print out bytes received
		// ipString = ipAddressToString(server);
		// printf("Server with ip: %s and port %d said it received %s\n", ipString, ntohs(server->sin6_port), buffer);
	}
}

int readFromStdin(uint8_t *buffer)
{
	int aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

void checkArgs(int argc, char * argv[])
{
    /* check command line arguments  */
	
	if (argc != 4)
	{
		printf("usage: %s error-rate server-host server-port-number \n", argv[0]);
		exit(1);
	}
}





