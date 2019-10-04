/*
        demo-udp-03: udp-recv: a simple udp server
	receive udp messages

        usage:  udp-recv

        Paul Krzyzanowski
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "port.h"
#include "messages.h"
#include <inttypes.h>
#include <stdint.h>

#define BUFSIZE 2048
void deserialize(char* buffer, DataMessage* dataMessage)
{
//    int *q = (int*)data;
//    dataMessage->type = *q;       q++;
//    dataMessage->sender = *q;   q++;
//    dataMessage->message_id = *q;     q++;
//    dataMessage->data = *q;     q++;
    uint32_t temp;
    memcpy(&temp, &buffer[0], sizeof(buffer[0]));
    dataMessage->type = ntohl(temp);
    memcpy(&temp, &buffer[4], sizeof(buffer[4]));
    dataMessage->sender = ntohl(temp);
    memcpy(&temp, &buffer[8], sizeof(buffer[8]));
    dataMessage->message_id = ntohl(temp);
    memcpy(&temp, &buffer[12], sizeof(buffer[12]));
    dataMessage->data = ntohl(temp);
    
}

int
main(int argc, char **argv)
{
	struct sockaddr_in myaddr;	/* our address */
	struct sockaddr_in remaddr;	/* remote address */
	socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
	int recvlen;			/* # bytes received */
	int fd;				/* our socket */
	int msgcnt = 0;			/* count # of messages we received */
	unsigned char buf[BUFSIZE];	/* receive buffer */
    DataMessage* rcvdMessage;

//    char data[BUFSIZE];

	/* create a UDP socket */

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return 0;
	}

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(SERVICE_PORT);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}

	/* now loop, receiving data and printing what we received */
	for (;;) {
		printf("waiting on port %d\n", SERVICE_PORT);
//        recvlen = recvfrom(fd, (struct DataMessage *)&rcvdMessage, sizeof(rcvdMessage), 0, (struct sockaddr *)&remaddr, &addrlen);
//        if (recvlen > 0) {
////            buf[recvlen] = 0;
////            printf("received message: \"%d\" (%d bytes)\n", &rcvdMessage.sender, recvlen);
//            printf("%lu\n", (unsigned long) &rcvdMessage.sender);
//            printf("number of bytes received is (%d)\n", recvlen);
//        }
        recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
        if (recvlen > 0) {
//            buf[recvlen] = 0;
//            deserialize(buf, rcvdMessage);
//            printf("sender: %lu\n", (unsigned long) &rcvdMessage->sender);
            int temp;
            memcpy(&temp, &buf[4], 4);
            printf("the received int is: %d",ntohl(temp));
//            printf("received message: \"%s\" (%d bytes)\n", buf, recvlen);
//            printf("received sender: %ls", &rcvdMessage->sender);
        }
		else
			printf("uh oh - something went wrong!\n");
		sprintf(buf, "ack %d", msgcnt++);
		printf("sending response \"%s\"\n", buf);
		if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			perror("sendto");
	}
	/* never exits */
}


