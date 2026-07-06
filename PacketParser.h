#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <iostream>
#include <cstdint>

class PacketParser {
public:
    enum class ParseState : std::uint8_t {
        WAIT_FOR_START,
        READ_CMD,
        READ_PAYLOAD_HIGH,
        READ_PAYLOAD_LOW,
        READ_CHECKSUM,
        WAIT_FOR_END
    };

    bool processByte(uint8_t incomingByte);

private:
    ParseState m_currentState = ParseState::WAIT_FOR_START;
    std::uint8_t m_commandID = 0;
    std::uint8_t m_checkSum = 0;
    std::uint16_t m_combinedSpeed = 0;
};

#endif
