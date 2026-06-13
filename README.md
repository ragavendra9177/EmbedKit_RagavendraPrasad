# EmbedKit_RagavendraPrasad
UART Frame Parser in C using FSM with timeout and checksum validation

𝐍𝐨𝐭𝐞𝐬 𝐨𝐧 𝐓𝐞𝐬𝐭 𝐂𝐚𝐬𝐞 𝐂𝐥𝐚𝐫𝐢𝐟𝐢𝐜𝐚𝐭𝐢𝐨𝐧

During implementation, a discrepancy was observed in the sample checksum provided in Test Case 1 of the assignment document.
For the given frame:

CMD = 0x01
LEN = 0x03
PAYLOAD = [0x10, 0x20, 0x30]

𝐓𝐡𝐞 𝐜𝐨𝐫𝐫𝐞𝐜𝐭 𝐗𝐎𝐑 𝐜𝐡𝐞𝐜𝐤𝐬𝐮𝐦 𝐜𝐚𝐥𝐜𝐮𝐥𝐚𝐭𝐢𝐨𝐧 𝐢𝐬:
0𝐱01 𝐗𝐎𝐑 0𝐱03 𝐗𝐎𝐑 0𝐱10 𝐗𝐎𝐑 0𝐱20 𝐗𝐎𝐑 0𝐱30 = 0𝐱02

𝐇𝐨𝐰𝐞𝐯𝐞𝐫, 𝐭𝐡𝐞 𝐚𝐬𝐬𝐢𝐠𝐧𝐦𝐞𝐧𝐭 𝐝𝐨𝐜𝐮𝐦𝐞𝐧𝐭 𝐦𝐞𝐧𝐭𝐢𝐨𝐧𝐬 𝐭𝐡𝐞 𝐜𝐡𝐞𝐜𝐤𝐬𝐮𝐦 𝐚𝐬 0𝐱22.
This implementation follows the correct XOR-based checksum logic as defined in the protocol specification, and therefore uses 0x02 as the valid checksum.

𝐏𝐫𝐨𝐣𝐞𝐜𝐭 𝐎𝐯𝐞𝐫𝐯𝐢𝐞𝐰
This project implements a UART Frame Parser in standard C using a finite state machine approach. The parser processes incoming serial data byte by byte and reconstructs structured frames according to a defined protocol.
The implementation simulates real UART behavior where data arrives as a continuous stream, and the parser must handle incomplete frames, noise, and timing issues using inter byte timeout logic.

𝐊𝐞𝐲 𝐅𝐞𝐚𝐭𝐮𝐫𝐞𝐬
Byte level UART frame parsing
Finite State Machine design
SOF based frame synchronization
CMD LEN PAYLOAD CHECKSUM protocol support
XOR based checksum validation
Inter byte timeout handling
Automatic recovery and re synchronization
Deterministic testing using timestamps

𝐏𝐚𝐫𝐬𝐞𝐫 𝐃𝐚𝐭𝐚 𝐒𝐭𝐫𝐮𝐜𝐭𝐮𝐫𝐞
The UART parser uses a structured object to maintain the complete state of frame processing. This allows the parser to handle byte-by-byte input in a controlled and deterministic way.

Parser Structure
typedef struct
{
    ParserState_t state;
    uint8_t cmd;
    uint8_t len;
    uint8_t payload[MAX_PAYLOAD];
    uint8_t payload_index;
    uint8_t checksum;
    uint32_t last_time;
    uint32_t timeout;

} Parser_t;

𝐒𝐭𝐫𝐮𝐜𝐭𝐮𝐫𝐞 𝐅𝐢𝐞𝐥𝐝 𝐄𝐱𝐩𝐥𝐚𝐧𝐚𝐭𝐢𝐨𝐧
𝐬𝐭𝐚𝐭𝐞
Represents the current state of the finite state machine.
It decides which part of the frame is currently being processed such as SOF, CMD, LEN, PAYLOAD, or CHECKSUM.

𝐜𝐦𝐝
Stores the command byte of the UART frame.
This identifies the type of message being received.

𝐥𝐞𝐧
Indicates the length of the payload in bytes.
It controls how many payload bytes must be collected.

𝐩𝐚𝐲𝐥𝐨𝐚𝐝
Array used to store incoming payload data bytes.
The maximum allowed size is defined by MAX_PAYLOAD.

𝐩𝐚𝐲𝐥𝐨𝐚𝐝_𝐢𝐧𝐝𝐞𝐱
Tracks the current position while filling the payload array.
It ensures correct sequencing of incoming bytes.

𝐜𝐡𝐞𝐜𝐤𝐬𝐮𝐦
Stores the computed XOR checksum while parsing the frame.
It is compared with the received checksum byte for validation.

𝐥𝐚𝐬𝐭_𝐭𝐢𝐦𝐞
Stores the timestamp of the last received byte.
It is used for inter byte timeout detection.

𝐭𝐢𝐦𝐞𝐨𝐮𝐭
Defines the maximum allowed time gap (in milliseconds) between two consecutive bytes.
If this value is exceeded, the parser resets to prevent invalid frame processing.

𝐒𝐮𝐦𝐦𝐚𝐫𝐲
This structure acts as the core context of the UART parser, enabling:

Stateful byte-by-byte parsing
Frame reconstruction
Error detection using checksum
Robust recovery using timeout logic



𝐏𝐫𝐨𝐭𝐨𝐜𝐨𝐥 𝐅𝐨𝐫𝐦𝐚𝐭

+--------+--------+--------+-----------+----------+
| SOF | CMD | LEN | PAYLOAD | CHECKSUM |
| 0xAA | 1 byte | 1 byte | N bytes | 1 byte |
+--------+--------+--------+-----------+----------+

𝐂𝐡𝐞𝐜𝐤𝐬𝐮𝐦 𝐜𝐚𝐥𝐜𝐮𝐥𝐚𝐭𝐢𝐨𝐧

CHECKSUM = CMD XOR LEN XOR all PAYLOAD bytes

Example
CMD = 0x01
LEN = 0x03
PAYLOAD = 10 20 30

CHECKSUM = 0x01 XOR 0x03 XOR 0x10 XOR 0x20 XOR 0x30 = 0x22

𝐒𝐭𝐚𝐭𝐞 𝐌𝐚𝐜𝐡𝐢𝐧𝐞 𝐃𝐞𝐬𝐢𝐠𝐧

The parser operates using the following states
WAIT SOF
WAIT CMD
WAIT LEN
WAIT PAYLOAD
WAIT CHECKSUM

Each incoming byte transitions the parser from one state to the next until a full frame is reconstructed.

𝐈𝐧𝐭𝐞𝐫 𝐁𝐲𝐭𝐞 𝐓𝐢𝐦𝐞𝐨𝐮𝐭 𝐋𝐨𝐠𝐢𝐜
Each byte is associated with a timestamp in milliseconds.
If the time difference between consecutive bytes exceeds the configured timeout value, the parser resets automatically.
This prevents the parser from getting stuck on incomplete frames caused by missing or delayed bytes.
If timeout is set to zero, timeout handling is disabled.

𝐑𝐞𝐭𝐮𝐫𝐧 𝐂𝐨𝐝𝐞𝐬

1 Frame successfully received
0 Frame in progress
-1 Checksum mismatch or invalid frame
-2 Inter byte timeout occurred

𝐓𝐞𝐬𝐭 𝐂𝐚𝐬𝐞𝐬
𝐓𝐞𝐬𝐭 1 𝐂𝐥𝐞𝐚𝐧 𝐕𝐚𝐥𝐢𝐝 𝐅𝐫𝐚𝐦𝐞
A complete and correct UART frame is provided without any delay. The parser successfully reconstructs the frame and validates the checksum.

𝐓𝐞𝐬𝐭 2 𝐓𝐢𝐦𝐞𝐨𝐮𝐭 𝐚𝐧𝐝 𝐑𝐞𝐜𝐨𝐯𝐞𝐫𝐲
This test introduces a large delay between bytes causing a timeout condition. When the gap exceeds the configured timeout, the parser resets immediately.

After reset, a new frame starts with SOF 0xAA and the parser re synchronizes correctly. The second frame is parsed successfully and checksum is validated.

This test verifies timeout detection and automatic recovery behavior.

𝐓𝐞𝐬𝐭 3 𝐁𝐚𝐜𝐤 𝐭𝐨 𝐁𝐚𝐜𝐤 𝐅𝐫𝐚𝐦𝐞𝐬
Two valid frames are sent continuously without any delay. The parser processes both frames sequentially using the same state machine.

After completing the first frame, the parser resets to WAIT SOF and immediately begins processing the next frame.
This test verifies continuous stream processing capability.

𝐓𝐞𝐬𝐭 4 𝐓𝐢𝐦𝐞𝐨𝐮𝐭 𝐃𝐢𝐬𝐚𝐛𝐥𝐞𝐝
Timeout is disabled by setting it to zero. The parser does not reset on timing gaps and continues processing the stream.
If corruption occurs due to missing bytes, the checksum validation detects the error instead of timeout logic.
This test verifies behavior when timeout protection is disabled.


𝐂𝐨𝐧𝐜𝐞𝐩𝐭𝐬 𝐔𝐬𝐞𝐝
Finite State Machine design
UART communication protocol parsing
Stream based byte processing
Checksum validation using XOR
Timeout based error recovery
Embedded systems firmware logic
