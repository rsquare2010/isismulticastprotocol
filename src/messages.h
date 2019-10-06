typedef struct {
    uint32_t type;
    uint32_t sender;
    uint32_t message_id;
    uint32_t data;
} DataMessage;

typedef struct {
    uint32_t type;
    uint32_t sender;
    uint32_t msg_id;
    uint32_t proposed_seq;
    uint32_t proposer;
} AckMessage;

typedef struct {
    uint32_t type;
    uint32_t sender;
    uint32_t msg_id;
    uint32_t final_seq;
    uint32_t final_seq_proposer;
} SeqMessage;

//struct Comparator {
//    bool operator()(SeqMessage const& s1, SeqMessage const& p2)
//    {
//        // return "true" if "s1" is ordered
//        // before "s2", for example:
//        if(s1.final_seq == s2.final_seq) {
//            return s1.final_seq_proposer > s2.final_seq_proposer;
//        }
//        return s1.final_seq < s2.final_seq;
//    }
//};

//typedef struct {
//    int sequence;
//    int proposed_id[20]; ///max process is 20?
//    int seqPropId;
//    int ackCount;
//} ProposalCounter;

void serializeDM(DataMessage* sendMessage, char* messageSerialized) {
    uint32_t temp;
    temp = htonl(sendMessage->type);
    memcpy(&messageSerialized[0], &temp, 4);
    temp = htonl(sendMessage->sender);
    memcpy(&messageSerialized[4], &temp, 4);
    temp = htonl(sendMessage->message_id);
    memcpy(&messageSerialized[8], &temp, 4);
    temp = htonl(sendMessage->data);
    memcpy(&messageSerialized[12], &temp, 4);
}

void serializeAM(AckMessage* ackMessage, char* messageSerialized) {
    uint32_t temp;
    temp = htonl(ackMessage->type);
    memcpy(&messageSerialized[0], &temp, 4);
    temp = htonl(ackMessage->sender);
    memcpy(&messageSerialized[4], &temp, 4);
    temp = htonl(ackMessage->msg_id);
    memcpy(&messageSerialized[8], &temp, 4);
    temp = htonl(ackMessage->proposed_seq);
    memcpy(&messageSerialized[12], &temp, 4);
    temp = htonl(ackMessage->proposer);
    memcpy(&messageSerialized[16], &temp, 4);
}

void serializeSM(SeqMessage* seqMessage, char* messageSerialized) {
    uint32_t temp;
    temp = htonl(seqMessage->type);
    memcpy(&messageSerialized[0], &temp, 4);
    temp = htonl(seqMessage->sender);
    memcpy(&messageSerialized[4], &temp, 4);
    temp = htonl(seqMessage->msg_id);
    memcpy(&messageSerialized[8], &temp, 4);
    temp = htonl(seqMessage->final_seq);
    memcpy(&messageSerialized[12], &temp, 4);
    temp = htonl(seqMessage->final_seq_proposer);
    memcpy(&messageSerialized[16], &temp, 4);
}

void deserializeDM(char* buffer, DataMessage* dataMessage)
{
    uint32_t tempo;
    memcpy(&tempo, &buffer[0], 4);
    dataMessage->type = ntohl(tempo);
    memcpy(&tempo, &buffer[4], 4);
    dataMessage->sender = ntohl(tempo);
    memcpy(&tempo, &buffer[8], 4);
    dataMessage->message_id = ntohl(tempo);
    memcpy(&tempo, &buffer[12], 4);
    dataMessage->data = ntohl(tempo);
}

void deserializeAM(char* buffer, AckMessage* ackMessage)
{
    uint32_t temp;
    memcpy(&temp, &buffer[0], 4);
    ackMessage->type = ntohl(temp);
    memcpy(&temp, &buffer[4], 4);
    ackMessage->sender = ntohl(temp);
    memcpy(&temp, &buffer[8], 4);
    ackMessage->msg_id = ntohl(temp);
    memcpy(&temp, &buffer[12], 4);
    ackMessage->proposed_seq = ntohl(temp);
    memcpy(&temp, &buffer[16], 4);
    ackMessage->proposer = ntohl(temp);
}

void deserializeSM(char* buffer, SeqMessage* seqMessage)
{
    uint32_t temp;
    memcpy(&temp, &buffer[0], 4);
    seqMessage->type = ntohl(temp);
    memcpy(&temp, &buffer[4], 4);
    seqMessage->sender = ntohl(temp);
    memcpy(&temp, &buffer[8], 4);
    seqMessage->msg_id = ntohl(temp);
    memcpy(&temp, &buffer[12], 4);
    seqMessage->final_seq = ntohl(temp);
    memcpy(&temp, &buffer[16], 4);
    seqMessage->final_seq_proposer = ntohl(temp);
}
