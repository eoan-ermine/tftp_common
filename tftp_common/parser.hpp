#pragma once

#include "packets.hpp"

namespace tftp_common {

namespace packets {

/// The result of parsing a single packet
struct ParseResult {
    /// if the parsing was successful
    bool success;
    /// count of bytes read
    std::size_t bytes_read;
};

/// Parse read/write request packet from buffer converting all fields to host byte order
/// @param[buffer] Assumptions: \p buffer is not a nullptr, it's size is greater or equal than \p len
/// @param[len] Assumptions: \p len is greater than zero
/// @n If parsing wasn't successful, \p packet remains in valid but unspecified state
ParseResult parse(std::uint8_t *buffer, std::size_t len, Request &packet) {
    assert(buffer != nullptr);
    assert(len > 0);

    std::size_t step_ = 0, bytes_read = 0;
    for (std::size_t i = 0; i != len; ++i) {
        const auto byte = buffer[i];
        bytes_read++;

        switch (step_) {
        // Opcode (2 bytes)
        case 0:
            packet.type_ = std::uint16_t(byte) << 8;
            step_++;
            break;
        case 1:
            packet.type_ |= std::uint16_t(byte) << 0;
            packet.type_ = ntohs(packet.type_);
            if (packet.type_ != packets::Type::ReadRequest && packet.type_ != packets::Type::WriteRequest) {
                step_ = 0;
                continue;
            }
            step_++;
            break;
        // Filename
        case 2:
            packet.filename.push_back(byte);

            if (!byte)
                step_++;
            break;
        // Mode
        case 3:
            packet.mode.push_back(byte);

            if (!byte) {
                return ParseResult{true, bytes_read};
            }
            break;
        }
    }
    return ParseResult{false, bytes_read};
}

/// Parse data packet from buffer converting all fields to host byte order
/// @param[buffer] Assumptions: \p buffer is not a nullptr, it's size is greater or equal than \p len
/// @param[len] Assumptions: \p len is greater than zero
/// @n If parsing wasn't successful, \p packet remains in valid but unspecified state
ParseResult parse(std::uint8_t *buffer, std::size_t len, Data &packet) {
    assert(buffer != nullptr);
    assert(len > 0);

    std::size_t step_ = 0, bytes_read = 0;
    for (std::size_t i = 0; i != len; ++i) {
        const auto byte = buffer[i];
        bytes_read++;

        switch (step_) {
        // Opcode (2 bytes)
        case 0:
            packet.type = std::uint16_t(byte) << 8;
            step_++;
            break;
        case 1:
            packet.type |= std::uint16_t(byte) << 0;
            packet.type = ntohs(packet.type);
            if (packet.type != packets::Type::DataPacket) {
                step_ = 0;
                continue;
            }
            step_++;
            break;
        // Block # (2 bytes)
        case 2:
            packet.block = std::uint16_t(byte) << 8;
            step_++;
            break;
        case 3:
            packet.block |= std::uint16_t(byte) << 0;
            packet.block = ntohs(packet.block);
            step_++;
            break;
        // buffer
        case 4:
            packet.data_.push_back(byte);

            if (i == len - 1) {
                return ParseResult{true, bytes_read};
            }

            break;
        }
    }
    return ParseResult{false, bytes_read};
}

/// Parse acknowledgment packet from buffer converting all fields to host byte order
/// @param[buffer] Assumptions: \p buffer is not a nullptr, it's size is greater or equal than \p len
/// @param[len] Assumptions: \p len is greater than zero
/// @n If parsing wasn't successful, \p packet remains in valid but unspecified state
ParseResult parse(std::uint8_t *buffer, std::size_t len, Acknowledgment &packet) {
    assert(buffer != nullptr);
    assert(len > 0);

    std::size_t step_ = 0, bytes_read = 0;
    for (std::size_t i = 0; i != len; ++i) {
        const auto byte = buffer[i];
        bytes_read++;

        switch (step_) {
        // Opcode (2 bytes)
        case 0:
            packet.type = std::uint16_t(byte) << 8;
            step_++;
            break;
        case 1:
            packet.type |= std::uint16_t(byte) << 0;
            packet.type = ntohs(packet.type);
            if (packet.type != packets::Type::AcknowledgmentPacket) {
                step_ = 0;
                continue;
            }
            step_++;
            break;
        // Block # (2 bytes)
        case 2:
            packet.block = std::uint16_t(byte) << 8;
            step_++;
            break;
        case 3:
            packet.block |= std::uint16_t(byte) << 0;
            packet.block = ntohs(packet.block);
            return ParseResult{true, bytes_read};
        }
    }
    return ParseResult{false, bytes_read};
}

/// Parse error packet from buffer converting all fields to host byte order
/// @param[buffer] Assumptions: \p buffer is not a nullptr, it's size is greater or equal than \p len
/// @param[len] Assumptions: \p len is greater than zero
/// @n If parsing wasn't successful, \p packet remains in valid but unspecified state
ParseResult parse(std::uint8_t *buffer, std::size_t len, Error &packet) {
    assert(buffer != nullptr);
    assert(len > 0);

    std::size_t step_ = 0, bytes_read = 0;
    for (std::size_t i = 0; i != len; ++i) {
        const auto byte = buffer[i];
        bytes_read++;

        switch (step_) {
        // Opcode (2 bytes)
        case 0:
            packet.type = std::uint16_t(byte) << 8;
            step_++;
            break;
        case 1:
            packet.type |= std::uint16_t(byte) << 0;
            packet.type = ntohs(packet.type);
            if (packet.type != packets::Type::ErrorPacket) {
                step_ = 0;
                continue;
            }
            step_++;
            break;
        // ErrorCode (2 bytes)
        case 2:
            packet.error_code = std::uint16_t(byte) << 8;
            step_++;
            break;
        case 3:
            packet.error_code |= std::uint16_t(byte) << 0;
            packet.error_code = ntohs(packet.type);
            step_++;
            break;
        // ErrorMessage
        case 4:
            packet.error_message.push_back(byte);

            if (!byte) {
                return ParseResult{true, bytes_read};
            }
            break;
        }
    }
    return ParseResult{false, bytes_read};
}

} // namespace packets

} // namespace tftp_common