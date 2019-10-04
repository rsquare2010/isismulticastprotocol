/*
        demo-udp-03: udp-send: a simple udp client
	send udp messages
	This sends a sequence of messages (the # of messages is defined in MSGS)
	The messages are sent to a port defined in SERVICE_PORT 

        usage:  udp-send

        Paul Krzyzanowski
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <getopt.h>
#include "port.h"
#include "messages.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <netinet/in.h>

#define BUFLEN 2048
#define MSGS 5	/* number of messages to send */
void serializeDM(DataMessage* sendMessage, char* messageSerialized) {
    uint32_t temp;
    temp = htonl(sendMessage->type);
    memcpy(&messageSerialized[0], &temp, sizeof(temp));
    temp = htonl(sendMessage->sender);
    memcpy(&messageSerialized[4], &temp, sizeof(temp));
    temp = htonl(sendMessage->message_id);
    memcpy(&messageSerialized[8], &temp, sizeof(temp));
    temp = htonl(sendMessage->data);
    memcpy(&messageSerialized[12], &temp, sizeof(temp));
}
//void printMsg(DataMessage* msgPacket)
//{
//    printf("message Type %d",msgPacket->sender);
//    cout << msgPacket->sender << endl;
//    cout << msgPacket->message_id << endl;
//    cout << msgPacket->data << endl;
//}

int main(int argc, char **argv)
{
    int option = 0;
    int counter;
    char* path;
    
    struct sockaddr_in myaddr, servaddr;
    int fd, i, slen=sizeof(servaddr);
    char buf[BUFLEN];    /* message buffer */
    int recvlen;        /* # bytes in acknowledgement message */
    //    char *server = "127.0.0.1";    /* change this to use a different server */
    char *host = "udptestrecv";
    struct hostent *hp;
    
    printf("entered \n");
    AckMessage ackMessages;
    SeqMessage seqMessage;
    ackMessages.type = 1;
    ackMessages.sender = 1001;
    ackMessages.msg_id = 9991;
    ackMessages.proposed_seq = 1234;
    ackMessages.proposer = 9191919;
    DataMessage sendMessage = {1, 1002, 9991, 123};
    

    while( (option = getopt(argc, argv, "h:c:")) != -1) {
        switch(option) {
            case 'h' :
                path = optarg;
                break;
            case 'c' :
                counter =  atoi(optarg);
                break;
            default: //print_usage();
                exit(EXIT_FAILURE);
        }
    }
    
	/* create a socket */

	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");


	/* bind it to all local addresses and pick any port number */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);


	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}       

	/* now define servaddr, the address to whom we want to send messages */
	/* For convenience, the host address is expressed as a numeric IP address */
	/* that we will convert to a binary format via inet_aton */

	memset((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVICE_PORT);

    //    if (inet_aton(server, &servaddr.sin_addr)==0) {
//        fprintf(stderr, "inet_aton() failed\n");
//        exit(1);
//    }

    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "could not obtain address of %s\n", host);
        return 0;
    }
    
    /* put the host's address into the server address structure */
    memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
    
    
    char* messageSerialized;
    serializeDM(&sendMessage, messageSerialized);
    printf("done8 \n");
    
    
	/* now let's send the messages */

	for (i=0; i < MSGS; i++) {
//        printf("Sending packet %d to %s port %d\n", i, server, SERVICE_PORT);
        sprintf(buf, "This is packet %d", i);
        printf("The message is");
		if (sendto(fd, messageSerialized, sizeof(messageSerialized), 0, (struct sockaddr *)&servaddr, slen)==-1) {
			perror("sendto");
			exit(1);
		}
		/* now receive an acknowledgement from the server */
		recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&servaddr, &slen);
                if (recvlen >= 0) {
                        buf[recvlen] = 0;	/* expect a printable string - terminate it */
                        printf("received message: \"%s\"\n", buf);
                }
	}
	return 0;
}


