# Rover-Serial-Packet-Parser
A lightweight, robust, and deterministic Finite State Machine (FSM) implemented in C++ for parsing serial data packets on a mechatronics rover system.

This parser processes incoming data byte-by-byte asynchronously, validating frame boundaries, command IDs, multi-byte payloads, and data integrity using a rolling 8-bit checksum before marking a packet as ready for execution.

🛠️ Packet Architecture
The communication protocol utilizes fixed-frame packets designed to maintain data integrity across noisy serial lines (e.g., electromagnetic interference from drivetrain motors):

Bit 0  START - STX (Start of Text) byte signaling a new data frame - 0x02

Bit 1  CMD - Command ID identifying the target subsystem or action - 0x00 -> 0xFF

Bit 2  PAYLOAD_HIGH - Upper 8 bits of a 16-bit combined integer (e.g., Speed) - 0x00 -> 0xFF

Bit 3  PAYLOAD_LOW - Lower 8 bits of a 16-bit combined integer (e.g., Speed) - 0x00 -> 0xFF

Bit 4  CHECKSUM - 8-bit rolling modulo-256 sum of CMD + PAYLOAD_HIGH + PAYLOAD_LOWCalculated

Bit 5  END - ETX (End of Text) byte finalizing the frame - 0x03



🧠 State Machine Logic

To maintain a zero-blocking footprint ideal for real-time operating environments, the parser implements a strict state transition sequence:


WAIT_FOR_START: Discards stream junk until a valid 0x02 header byte arrives, resetting the internal checksum tally.

READ_CMD: Captures the operational command and adds it to the running checksum.

READ_PAYLOAD_HIGH: Stores the incoming byte as the high half of the 16-bit speed word and shifts it (<< 8).

READ_PAYLOAD_LOW: Blends the lower half of the speed word into place using a bitwise OR (\|) operation.

READ_CHECKSUM: Direct-compares the locally calculated running sum against the transceiver's incoming validation byte. If a mismatch is caught, the frame is instantly dropped.

WAIT_FOR_END: Verifies the final 0x03 terminator frame, transitions back to the start state, and signals a successful parse event.


🚀 Key Features

Non-Blocking Execution: Uses a single-byte ingestion model (processByte(uint8_t)), preventing loop lag on microcontrollers.

Bitwise Manipulation: Efficiently stitches multi-byte integers over 8-bit pipelines without using heavy deserialization libraries.

Error Mitigation: The rolling checksum prevents execution of corrupted instructions caused by signal dropouts or line noise.

Memory Constrained: Avoids dynamic allocation (new/delete) and large static ring buffers, preserving SRAM.


💻 Project StructurePlaintext

├── PacketParser.h    # Class definition, state enumerations, and member variables

├── PacketParser.cpp  # Finite State Machine implementation logic

└── main.cpp          # Automated hardware simulation and testing bench


📈 Verification & Testing

The project includes an automated test bench (main.cpp) that simulates a hardware transceiver stream to verify parsing boundaries and validation logic.

Simulated Test ResultsPlaintext

====================================================
        ROVER PACKET PARSER TEST BENCH              
====================================================

--- TEST 1: Feeding Valid Packet ---
Packet Data: 0x02 0x10 0x01 0xf4 0x05 0x03 

Streaming bytes sequentially into parser...
  Fed byte [0]: 0x02 -> Packet Complete? No
  Fed byte [1]: 0x10 -> Packet Complete? No
  Fed byte [2]: 0x01 -> Packet Complete? No
  Fed byte [3]: 0xf4 -> Packet Complete? No
  Fed byte [4]: 0x05 -> Packet Complete? No
  Fed byte [5]: 0x03 -> Packet Complete? YES!

  >>> SUCCESS! Valid packet was completely processed and verified! <<<

----------------------------------------------------

--- TEST 2: Feeding Corrupted Packet (Noise Simulation) ---
Packet Data: 0x02 0x10 0x01 0xee 0x05 0x03 

Streaming bytes sequentially into parser...
  Fed byte [0]: 0x02 -> Packet Complete? No
  Fed byte [1]: 0x10 -> Packet Complete? No
  Fed byte [2]: 0x01 -> Packet Complete? No
  Fed byte [3]: 0xee -> Packet Complete? No
  Fed byte [4]: 0x05 -> Packet Complete? No
  Fed byte [5]: 0x03 -> Packet Complete? No

  >>> SUCCESS! Parser detected the corrupted payload and rejected it safely! <<<
====================================================
