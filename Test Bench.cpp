#include <iostream>
#include <vector>
#include <iomanip>
#include "PacketParser.h"

// Simple helper utility to print values out as clean hex digits (like 0x02)
void printHex(std::uint8_t byte) {
    std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << (int)byte;
}

int main() {
    PacketParser parser;

    std::cout << "====================================================\n";
    std::cout << "        ROVER PACKET PARSER TEST BENCH              \n";
    std::cout << "====================================================\n\n";

    // ----------------------------------------------------------------
    // TEST 1: THE HAPPY PATH (A Perfectly Clean Packet)
    // Target: Command ID = 0x10, Speed = +500 (Hex: 0x01F4)
    // Sender Checksum Math: 0x10 + 0x01 + 0xF4 = 0x05 (8-bit rollover)
    // Layout: [START, CMD, HIGH_BYTE, LOW_BYTE, CHECKSUM, END]
    // ----------------------------------------------------------------
    std::vector<std::uint8_t> validPacket = {0x02, 0x10, 0x01, 0xF4, 0x05, 0x03};

    std::cout << "--- TEST 1: Feeding Valid Packet ---\n";
    std::cout << "Packet Data: ";
    for (std::uint8_t b : validPacket) {
        printHex(b);
        std::cout << " ";
    }
    std::cout << "\n\nStreaming bytes sequentially into parser...\n";

    for (size_t i = 0; i < validPacket.size(); ++i) {
        bool packetComplete = parser.processByte(validPacket[i]);

        std::cout << "  Fed byte [" << i << "]: ";
        printHex(validPacket[i]);
        std::cout << " -> Packet Complete? " << (packetComplete ? "YES!" : "No") << "\n";

        if (packetComplete) {
            std::cout << "\n  >>> SUCCESS! Valid packet was completely processed and verified! <<<\n";
        }
    }

    std::cout << "\n----------------------------------------------------\n\n";

    // ----------------------------------------------------------------
    // TEST 2: THE CRASH PATH (Simulated Electromagnetic Motor Noise)
    // We send the same packet, but the low byte gets mangled from 0xF4 to 0xEE.
    // The checksum remains 0x05. Your logic should catch the mismatch.
    // ----------------------------------------------------------------
    std::vector<std::uint8_t> corruptedPacket = {0x02, 0x10, 0x01, 0xEE, 0x05, 0x03};

    std::cout << "--- TEST 2: Feeding Corrupted Packet (Noise Simulation) ---\n";
    std::cout << "Packet Data: ";
    for (std::uint8_t b : corruptedPacket) {
        printHex(b);
        std::cout << " ";
    }
    std::cout << "\n\nStreaming bytes sequentially into parser...\n";

    bool caughtError = true;
    for (size_t i = 0; i < corruptedPacket.size(); ++i) {
        bool packetComplete = parser.processByte(corruptedPacket[i]);

        std::cout << "  Fed byte [" << i << "]: ";
        printHex(corruptedPacket[i]);
        std::cout << " -> Packet Complete? " << (packetComplete ? "YES!" : "No") << "\n";

        if (packetComplete) {
            std::cout << "\n  >>> ERROR: The parser accepted a broken packet! <<<\n";
            caughtError = false;
        }
    }

    if (caughtError) {
        std::cout << "\n  >>> SUCCESS! Parser detected the corrupted payload and rejected it safely! <<<\n";
    }

    std::cout << "====================================================\n";
    return 0;
}
