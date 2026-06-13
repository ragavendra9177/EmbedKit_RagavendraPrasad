#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define SOF 0xAA
#define MAX_PAYLOAD 16


// Parser state machine states
typedef enum
{
    WAIT_SOF,       // Waiting for Start Of Frame (0xAA)
    WAIT_CMD,       // Waiting for command byte
    WAIT_LEN,       // Waiting for payload length
    WAIT_PAYLOAD,   // Receiving payload bytes
    WAIT_CHECKSUM   // Waiting for checksum byte
} ParserState_t;


// UART parser context structure
// This holds all runtime information required to parse a frame byte-by-byte
typedef struct
{
    ParserState_t state;          // Current state of the parser FSM
    uint8_t cmd;                  // Command byte of the frame
    uint8_t len;                  // Payload length
    uint8_t payload[MAX_PAYLOAD]; // Buffer to store payload bytes
    uint8_t payload_index;        // Current index while filling payload
    uint8_t checksum;             // Computed XOR checksum during parsing
    uint32_t last_time;           // Timestamp of last received byte (for timeout)
    uint32_t timeout;             // Inter-byte timeout threshold (ms)

} Parser_t;



void parser_reset(Parser_t *parser)
{
    parser->state = WAIT_SOF;
    parser->cmd = 0;
    parser->len = 0;
    parser->checksum = 0;
    parser->payload_index = 0;
    memset(parser->payload, 0, sizeof(parser->payload));
}


void parser_init(Parser_t *parser, uint32_t timeout)
{
    parser_reset(parser);
    parser->last_time=0;
    parser->timeout=timeout;
}


int parser_feed(Parser_t *parser, uint8_t byte,uint32_t timestamp)
{
    
 if(parser->state != WAIT_SOF && parser->timeout != 0)
{
    uint32_t gap;

    gap = timestamp - parser->last_time;

   if(gap > parser->timeout)
{
    
    parser_reset(parser);

    /* report timeout */
    parser->last_time = timestamp;

    /* if current byte is SOF, immediately start new frame */
    if(byte == SOF)
    {
        parser->state = WAIT_CMD;
    }

    return -2;
}
    
}

   switch(parser->state)
 {
     
    case WAIT_SOF:
    if(byte == 0xAA)
    {
        parser->state = WAIT_CMD;
    }
    break;
    
    

   case WAIT_CMD:

    parser->cmd = byte;
    parser->checksum = byte;
    parser->state = WAIT_LEN;
    break;
    

    case WAIT_LEN:

    parser->len = byte;
    if(parser->len > MAX_PAYLOAD)
    {
        parser_reset(parser);
        return -1;
    }
    parser->checksum ^= byte;
    parser->payload_index = 0;

    if(parser->len == 0)
        parser->state = WAIT_CHECKSUM;
    else
        parser->state = WAIT_PAYLOAD;

    break;
    

  case WAIT_PAYLOAD:
  
    parser->payload[parser->payload_index] = byte;
    parser->payload_index++;
    parser->checksum ^= byte;
    if(parser->payload_index == parser->len)
    {
        parser->state = WAIT_CHECKSUM;
    }
    break;
    

    case WAIT_CHECKSUM:

    if(byte == parser->checksum)
    {
        parser->last_time = timestamp;
        return 1;
    }
    else
    {
        parser_reset(parser);
        return -1;
    }
 }
 
 parser->last_time = timestamp;
    return 0;
}


void feed_stream(Parser_t *parser, uint8_t bytes[], uint32_t times[], int count)
{
    int result;

    for(int i = 0; i < count; i++)
    {
        result = parser_feed(parser, bytes[i],times[i]);

        if(result == 0)
        {
            printf("t=%ums byte=0x%02X -> receiving...\n",times[i],bytes[i]);
        }
        
        
        
        else if(result == 1)
        {
           printf("FRAME OK CMD=0x%02X LEN=%d PAYLOAD=[",parser->cmd,parser->len);

           for(int i=0; i<parser->len; i++)
            {
             printf("%02X ", parser->payload[i]);
            }
             printf("]\n");
  
             parser_reset(parser);
        }
        
        else if(result == -1)
        {
            printf("t=%ums byte=0x%02X -> CHECKSUM ERROR\n", times[i],bytes[i]);
        }
       
        
        else if(result == -2)
           {
    printf("t=%ums byte=0x%02X -> TIMEOUT\n", times[i], bytes[i]);
           
           }
    }
}




int main()
{
    
Parser_t parser;
printf("......Test 1......\n");
    
uint8_t bytes1[] ={0xAA, 0x01, 0x03,0x10, 0x20, 0x30,0x02};
uint32_t times1[] ={0,5,10,15,20,25,30};

parser_init(&parser, 50);
feed_stream(&parser, bytes1, times1, 7);

printf("\n\n");
printf("......Test 2.....\n");

uint8_t bytes2[] ={0xAA,0x01,0x03,0x10,0xAA,0x05,0x01,0x7F,0x7B};
uint32_t times2[] ={0,5,10,15,200,205,210,215,220};

parser_init(&parser, 50);
feed_stream(&parser, bytes2, times2, 9);



printf("\n\n");
printf("......Test 3.....\n");


uint8_t bytes3[] ={0xAA,0x03,0x01,0x55,0x57,0xAA,0x04,0x02,0xAA,0xBB,0x17};
uint32_t times3[] ={0,5,10,15,20,25,30,35,40,45,50};

parser_init(&parser, 50);
feed_stream(&parser, bytes3, times3, 11);



printf("\n\n");
printf("......Test 4......\n");

parser_init(&parser, 0);
feed_stream(&parser, bytes2, times2, 9);


return 0;
}





