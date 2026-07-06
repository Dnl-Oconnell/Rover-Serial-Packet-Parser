#include "PacketParser.h"

bool PacketParser::processByte(std::uint8_t incomingByte) {
    switch(m_currentState) {
        case ParseState::WAIT_FOR_START:
            if (incomingByte == 0x02) {
                m_currentState = ParseState::READ_CMD;
                m_checkSum = 0;
            }
            break;

        case ParseState::READ_CMD:
            m_commandID = incomingByte;
            m_currentState = ParseState::READ_PAYLOAD_HIGH;
            m_checkSum += incomingByte;
            break;

        case ParseState::READ_PAYLOAD_HIGH:
            m_combinedSpeed = incomingByte;
            m_combinedSpeed = m_combinedSpeed << 8;
            m_currentState = ParseState::READ_PAYLOAD_LOW;
            m_checkSum += incomingByte;
            break;

        case ParseState::READ_PAYLOAD_LOW:
            m_combinedSpeed = m_combinedSpeed | incomingByte;
            m_currentState = ParseState::READ_CHECKSUM;
            m_checkSum += incomingByte;
            break;

        case ParseState::READ_CHECKSUM:
            if (incomingByte == m_checkSum) {
                m_currentState = ParseState::WAIT_FOR_END;
            } else {
                m_currentState = ParseState::WAIT_FOR_START;
            }
            break;

        case ParseState::WAIT_FOR_END:
            if (incomingByte == 0x03) {
                m_currentState = ParseState::WAIT_FOR_START;
                return true;
            } else {
                m_currentState = ParseState::WAIT_FOR_START;
                return false;
            }
            break;
    }
    return false;
}
