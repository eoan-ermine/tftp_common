#pragma once

#include "packets.hpp"

namespace tftp_common {

namespace packets {

bool parse(std::uint8_t *buffer, std::size_t len, std::size_t &bytes_read, packets::request &packet) {
    std::size_t step_ = 0;
    bytes_read = 0;
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
            if (packet.type_ != packets::type::read_request && packet.type_ != packets::type::write_request) {
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
                return true;
            }
            break;
        }
    }
    return false;
}

/// Parse data packet from buffer converting all fields to host byte order
/// @return if the parsing was successful
/// @n If parsing wasn't successful, \p packet remains in valid but unspecified state
bool parse(std::uint8_t *buffer, std::size_t len, std::size_t &bytes_read, packets::data &packet) {
    std::size_t step_ = 0;
    bytes_read = 0;
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
            if (packet.type != packets::type::data_packet) {
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
                return true;
            }

            break;
        }
    }
    return false;
}

/// Parse acknowledgment packet from buffer converting all fields to host byte order
/// @return if the parsing was successful
/// @n If parsing wasn't successful, \p packet remains in valid but unspecified state
bool parse(std::uint8_t *buffer, std::size_t len, std::size_t &bytes_read, packets::acknowledgment &packet) {
    std::size_t step_ = 0;
    bytes_read = 0;
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
            if (packet.type != packets::type::acknowledgment_packet) {
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
            return true;
        }
    }
    return false;
}

/// Parse error packet from buffer converting all fields to host byte order
/// @return if the parsing was successful
/// @n If parsing wasn't successful, \p packet remains in valid but unspecified state
bool parse(std::uint8_t *buffer, std::size_t len, std::size_t &bytes_read, packets::error &packet) {
    std::size_t step_ = 0;
    bytes_read = 0;
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
            if (packet.type != packets::type::error_packet) {
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
                return true;
            }
            break;
        }
    }
    return false;
}

} // namespace packets

} // namespace tftp_common