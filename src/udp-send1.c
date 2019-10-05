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
#include <getopt.h>
#include <unistd.h>

#define BUFLEN 4096
#define MSGS 5    /* number of messages to send */
#define PORT 21233

void readFromHostFile( char* contents) {
    FILE *fp;
    long lSize;
    
    
    fp = fopen ( "hostfile.txt" , "r" );
    if( !fp ) perror("hostfile"),exit(1);
    
    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );
    
    contents = calloc( 1, lSize+1 );
    if( !contents ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);
    
    if( 1!=fread( contents , lSize, 1 , fp) )
        fclose(fp),free(contents),fputs("entire read fails",stderr),exit(1);
    
    fclose(fp);
    
    printf("file contents: %s", contents);
    //    free(buffer);
}

int main(int argc, char **argv)
{
    int option = 0;
    int counter;
    char* path;
    
    struct sockaddr_in myaddr, servaddr;
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    int fd, i, slen=sizeof(servaddr);
    unsigned char buf[BUFLEN];    /* message buffer */
    int recvlen;        /* # bytes in acknowledgement message */
    char *host = "udptestsend";
    struct hostent *hp;
    
    char *filecontents;
    readFromHostFile(filecontents);
    
    AckMessage ackMessages;
    SeqMessage seqMessage;
    ackMessages.type = 2;
    ackMessages.sender = 2001;
    ackMessages.msg_id = 9992;
    ackMessages.proposed_seq = 2234;
    ackMessages.proposer = 9191919;
    DataMessage sendMessage = {1, 1002, 9991, 123};
    DataMessage rcvdDMMessage;
    AckMessage rcvdAMMdessage;
    
    
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
    myaddr.sin_port = htons(SERVICE_PORT);
    
    
    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        return 0;
    }
    printf("bind succeded\n");
    /* now define servaddr, the address to whom we want to send messages */
    /* For convenience, the host address is expressed as a numeric IP address */
    /* that we will convert to a binary format via inet_aton */
    
    memset((char *) &servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVICE_PORT);
    sleep(5);
    do {
        printf("host is:%s", host);
        hp = gethostbyname(host);
        if (hp == NULL) {
            fprintf(stderr, "could not obtain address of %s\n", host);
            sleep(3);
            //            return 0;
        }
        else {
            printf("managed to connect");
            break;
        }
    } while(1 == 1);
    printf("usnure if host connected?\n");
    /* put the host's address into the server address structure */
    memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
    sleep(10);
//    char* messageSerialized;
    serializeDM(&sendMessage, &buf[0]);
//    sprintf(messageSerialized, "What is this?");
//    sleep(5);
    /* now let's send the messages */
    printf("before send\n");
//    sprintf(buf, "test message");
//    if (sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in))==-1) {
//        perror("sendto failed");
////        exit(1);
//    }
//    printf("after send\n");
//    recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&servaddr, &slen);
//    if(recvlen > 0) {
        for (i=0; i < MSGS; i++) {
//            printf("sending Messages %ld", sizeof(messageSerialized));
            
            if (sendto(fd, buf, sizeof(sendMessage), 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in))==-1) {
                perror("sendto failed");
//                exit(1);
            }
        }
        
        for (;;) {
            //        printf("waiting on port %d\n", SERVICE_PORT);
            
            recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&servaddr, &slen);
            if (recvlen > 0) {
                            int temp;
                            memcpy(&temp, &buf[0], 4);
//                            printf("the received int1 is: %d",ntohl(temp));
                if(ntohl(temp) == 1) {
                    deserializeDM(buf, &rcvdDMMessage);
                    printf("Data message received \n");
                    serializeAM(&ackMessages, &buf[0]);
                    if (sendto(fd, buf, sizeof(ackMessages), 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in))==-1) {
                        perror("sendto failed");
                    }
                } else if (ntohl(temp) == 2) {
                    deserializeAM(buf, &rcvdAMMdessage);
                    printf("Ack received\n");
                }
                

//                            printf("the received int is: %d \n",rcvdDMMessage.data);
                
//                if(rcvdMessage.type == 1) {
//
//                }
//                if(rcvdMessage.type == 2) {
//
//                }
            }
            else
                printf("uh oh - something went wrong!\n");
        }
//    } else {
//        printf("peer not alive\n");
//    }
    sleep(2);
    return 0;
}


