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
#define MAX_STRING_SIZE 40
#define MAX_PEERS 20

char pnames[MAX_PEERS][MAX_STRING_SIZE];
int psize;

void readFromHostFile( char* myIP) {
    FILE *fp;
    long lSize;
    char* contents;
    char myIPAddress[40];
    strcpy(myIPAddress, myIP);

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
    
    char *temp;
    temp = strtok(contents,"\n");
    char *IPbuffer1;
    struct hostent *host_entry;

    host_entry = gethostbyname(temp);
    if(host_entry != NULL) {
        IPbuffer1 = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
    }
//    printf("IPB:%s",IPbuffer1);
    int index = 0;
    while (temp != NULL)
    {
        printf("IPB:%s",IPbuffer1);
        if((IPbuffer1 != NULL) && strcmp(IPbuffer1, myIPAddress)!=0) {
            strcpy(pnames[index], IPbuffer1);
            index++;
        }
        temp = strtok (NULL, "\n");
        IPbuffer1 = NULL;
        if( temp != NULL) {
            host_entry = gethostbyname(temp);
            if(host_entry != NULL) {
                    IPbuffer1 = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
            }
        }
    }
    for(int i = 0; i< index; i++) {
        printf("The tokens are: %s\n",pnames[i]);
    }
    psize = index;

}

int main(int argc, char **argv)
{
    int option = 0;
    int MSG_COUNT = 0;
    int messageCounter = 0;
    int sequnceCounter = 0;
    
    char* path;
    int myId;
    
    struct sockaddr_in myaddr, servaddr[MAX_PEERS];
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    int fd, i, slen=sizeof(servaddr);
    unsigned char buf[BUFLEN];    /* message buffer */
    int recvlen;        /* # bytes in acknowledgement message */
    struct hostent *hp;
    
    char *filecontents;
//    readFromHostFile(filecontents);
    
    AckMessage ackMessages;
    SeqMessage seqMessage;
    DataMessage sendMessage = {1, 1002, 9991, 123};
    DataMessage rcvdDMMessage;
    AckMessage rcvdAMMdessage;
    SeqMessage rcvdSeqMessage;
    
    
    while( (option = getopt(argc, argv, "h:c:")) != -1) {
        switch(option) {
            case 'h' :
                path = optarg;
                break;
            case 'c' :
                MSG_COUNT =  atoi(optarg);
                break;
            default: //print_usage();
                exit(EXIT_FAILURE);
        }
    }
    ProposalCounter propCounter = {0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},0,0};
    ProposalCounter proposalCounter[MSG_COUNT];
    for (int i =0; i < MSG_COUNT; i++) {
        proposalCounter[i].sequence = 0;
        proposalCounter[i].seqPropId = 0;
        for(int j = 0; j < 20; j++) {
            proposalCounter[i].proposed_id[j] = 0;
        }
        proposalCounter[i].ackCount = 0;
    }
    
//    for(int i =0; i< MSG_COUNT; i++) {
//        proposalCounter[i].ackCount = 0;
//    }
    
    /* create a socket */
    
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");
    
    int broadcastEnable=1;
    int ret=setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    
    /* bind it to all local addresses and pick any port number */
    
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(SERVICE_PORT);
    
    
    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        return 0;
    }
    /* now define servaddr, the address to whom we want to send messages */
    /* For convenience, the host address is expressed as a numeric IP address */
    /* that we will convert to a binary format via inet_aton */
    
    sleep(5);

    /* put the host's address into the server address structure */
    
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    myId = hostbuffer[0] + hostbuffer [1] + hostbuffer[3] + hostbuffer[4];
    
    host_entry = gethostbyname(hostbuffer);
    IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));

    printf("my ip is: %s\n", IPbuffer);
    readFromHostFile(IPbuffer);

    for (int i = 0; i < psize; i++) {
        memset((char *) &servaddr[i], 0, sizeof(servaddr[i]));
        servaddr[i].sin_family = AF_INET;
        servaddr[i].sin_port = htons(SERVICE_PORT);
        servaddr[i].sin_addr.s_addr = inet_addr(pnames[i]);
    }
    
    
    

    /* now let's send the messages */

    
    for (i=0; i < MSGS; i++) {
        for (int pid = 0; pid < psize; pid ++) {
            sendMessage.message_id = messageCounter;
            sendMessage.sender = myId;
            serializeDM(&sendMessage, &buf[0]);
            if (sendto(fd, buf, sizeof(sendMessage), 0, (struct sockaddr *)&servaddr[pid], sizeof(struct sockaddr_in))==-1) {
                perror("sendto failed");
            }
        }
        messageCounter++;
    }
        
        for (;;) {
            
            recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &addrlen);
            if (recvlen > 0) {
                            int temp;
                            memcpy(&temp, &buf[0], 4);
                if(ntohl(temp) == 1) {
                    deserializeDM(buf, &rcvdDMMessage);
                    ackMessages.type = 2;
                    ackMessages.sender = rcvdDMMessage.sender;
                    ackMessages.msg_id = rcvdDMMessage.message_id;
                    ackMessages.proposed_seq = sequnceCounter;
                    ackMessages.proposer = myId;
                    sequnceCounter++;
                    
                    serializeAM(&ackMessages, &buf[0]);
                    printf("id  of received message: %d\n",rcvdDMMessage.message_id);
                    printf("received message sender:%d\n", rcvdDMMessage.sender);
                    printf("ack from %d sender:%d\n", myId,ackMessages.proposed_seq);
//                    printf("received message sender:%d\n", rcvdDMMessage.sender);
                    
                    int len=50;
                    char ipbuffer[len];
                    if (sendto(fd, buf, sizeof(ackMessages), 0, (struct sockaddr *)&remaddr, sizeof(struct sockaddr_in))==-1) {
                        perror("sendto failed");
                    }
                } else if (ntohl(temp) == 2) {
                    deserializeAM(buf, &rcvdAMMdessage);
                    int messageId = rcvdAMMdessage.msg_id;
                    int ackCount = proposalCounter[messageId].ackCount;
//                    if(proposalCounter[messageId].ackCount == 0) {
//                        proposalCounter[messageId].sequence = rcvdAMMdessage.proposed_seq;
//                        proposalCounter[messageId].proposed_id[ackCount] = rcvdAMMdessage.proposer;
//                        proposalCounter[messageId].seqPropId = rcvdAMMdessage.proposer;
//                        proposalCounter[messageId].ackCount = 1;
//                    } else {
//                        int isDuplicate = 0;
//                        printf("ack count is:%d",ackCount);
//                        for (int i = 0; i < ackCount; i++) {
//                            if (proposalCounter[messageId].proposed_id[i] == rcvdAMMdessage.proposer) {
//                                isDuplicate = 1;
//                            }
//                        }
//                        if(isDuplicate == 1) {
//                            if (rcvdAMMdessage.proposed_seq == proposalCounter[messageId].sequence) {
//                                if(rcvdAMMdessage.proposer < proposalCounter[messageId].seqPropId) {
//                                    proposalCounter[messageId].sequence = rcvdAMMdessage.proposed_seq;
//                                    proposalCounter[messageId].proposed_id[ackCount] = rcvdAMMdessage.proposer;
//                                    proposalCounter[messageId].seqPropId = rcvdAMMdessage.proposer;
//                                    proposalCounter[messageId].ackCount++;
//                                }
//                            } else if (rcvdAMMdessage.proposed_seq > proposalCounter[messageId].sequence) {
////                                proposalCounter[messageId].seqPropId)
//                                    proposalCounter[messageId].sequence = rcvdAMMdessage.proposed_seq;
//                                    proposalCounter[messageId].proposed_id[ackCount] = rcvdAMMdessage.proposer;
//                                    proposalCounter[messageId].seqPropId = rcvdAMMdessage.proposer;
//                                    proposalCounter[messageId].ackCount++;
//                                } else {
//                                    proposalCounter[messageId].proposed_id[ackCount] = rcvdAMMdessage.proposer;
//                                    proposalCounter[messageId].ackCount++;
//                                }
//                            }
//                        }
//                        if(ackCount == MSG_COUNT - 1) {
//                            seqMessage.type = 3;
//                            seqMessage.sender = myId;
//                            seqMessage.msg_id = messageId;
//                            seqMessage.final_seq = proposalCounter[messageId].sequence;
//                            seqMessage.final_seq_proposer = proposalCounter[messageId].seqPropId;
//                            serializeSM(&seqMessage, &buf[0]);
//                            printf("send proposal for %d with sid: %d", messageId, seqMessage.final_seq);
////                            for(int i = 0; i < psize; i++) {
////                                if (sendto(fd, buf, sizeof(seqMessage), 0, (struct sockaddr *)&servaddr[i], sizeof(struct sockaddr_in))==-1) {
////                                    perror("sendto failed");
////                                }
////                            }
//                        }
                    printf("Ack received%d\n",messageId);
                printf("Ack count%d\n",ackCount);
                } else if (ntohl(temp) == 3) {
                    printf("SM received");
//                    deserializeSM(buf, &rcvdSeqMessage);
//                    printf("SM mID: %d\n",rcvdSeqMessage.msg_id);
//                    printf("SM sender:%d\n", rcvdSeqMessage.sender);
//                    printf("SM final seq: %d\n",rcvdSeqMessage.final_seq);
//                    printf("SM final seq prop:%d\n", rcvdSeqMessage.final_seq_proposer);
                }
            }
            else
                printf("uh oh - something went wrong!\n");
        }
    return 0;
}


