#pragma once

#include "packets.hpp"

namespace tftp_common::packets {

/// The result of parsing a single packet
struct ParseResult {
    /// if the parsing was successful
    bool Success;
    /// count of bytes read
    std::size_t BytesRead;
};

/// Parse read/write request packet from buffer converting all fields to host byte order
/// @param[Buffer] Assumptions: \p Buffer is not a nullptr, it's size is greater or equal than \p Len
/// @param[Len] Assumptions: \p Len is greater than zero
/// @n If parsing wasn't successful, \p Packet remains in valid but unspecified state
ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, Request &Packet) {
    assert(Buffer != nullptr);
    assert(Len > 0);

    std::string Name;
    std::string Value;
    std::size_t Step = 0;
    std::size_t BytesRead = 0;
    for (std::size_t Idx = 0; Idx != Len; ++Idx) {
        const auto Byte = Buffer[Idx];
        BytesRead++;

        switch (Step) {
        // Opcode (2 bytes)
        case 0:
            Packet.Type_ = std::uint16_t(Byte) << 0;
            Step++;
            break;
        case 1:
            Packet.Type_ |= std::uint16_t(Byte) << 8;
            Packet.Type_ = ntohs(Packet.Type_);
            if (Packet.Type_ != packets::Type::ReadRequest && Packet.Type_ != packets::Type::WriteRequest) {
                Step = 0;
                continue;
            }
            Step++;
            break;
        // Filename
        case 2:
            Packet.Filename.push_back(Byte);

            if (Byte == 0u) {
                Step++;
            }
            break;
        // Mode
        case 3:
            Packet.Mode.push_back(Byte);

            if (Byte == 0u) {
                if (Idx == Len - 1) {
                    return ParseResult{true, BytesRead};
                }
                Step++;
            }
            break;
        // Option name
        case 4:
            if (Byte == 0u) {
                Packet.OptionsNames.push_back(std::move(Name));
                Name.clear();
                Step++;
            } else {
                Name.push_back(Byte);
            }
            break;
        // Option value
        case 5:
            if (Byte == 0u) {
                Packet.OptionsValues.push_back(std::move(Value));
                Value.clear();

                if (Idx == Len - 1) {
                    return ParseResult{true, BytesRead};
                }
                Step--;
            } else {
                Value.push_back(Byte);
            }
            break;
        default:
            assert(false);
        }
    }
    return ParseResult{false, BytesRead};
}

/// Parse data packet from buffer converting all fields to host byte order
/// @param[Buffer] Assumptions: \p Buffer is not a nullptr, it's size is greater or equal than \p Len
/// @param[Len] Assumptions: \p Len is greater than zero
/// @n If parsing wasn't successful, \p Packet remains in valid but unspecified state
ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, Data &Packet) {
    assert(Buffer != nullptr);
    assert(Len > 0);

    std::size_t Step = 0;
    std::size_t BytesRead = 0;
    for (std::size_t Idx = 0; Idx != Len; ++Idx) {
        const auto Byte = Buffer[Idx];
        BytesRead++;

        switch (Step) {
        // Opcode (2 bytes)
        case 0:
            Packet.Type_ = std::uint16_t(Byte) << 0;
            Step++;
            break;
        case 1:
            Packet.Type_ |= std::uint16_t(Byte) << 8;
            Packet.Type_ = ntohs(Packet.Type_);
            if (Packet.Type_ != packets::Type::DataPacket) {
                Step = 0;
                continue;
            }
            Step++;
            break;
        // Block # (2 bytes)
        case 2:
            Packet.Block = std::uint16_t(Byte) << 0;
            Step++;
            break;
        case 3:
            Packet.Block |= std::uint16_t(Byte) << 8;
            Packet.Block = ntohs(Packet.Block);
            Step++;
            break;
        // buffer
        case 4:
            Packet.DataBuffer.push_back(Byte);

            if (Idx == Len - 1) {
                return ParseResult{true, BytesRead};
            }

            break;
        default:
            assert(false);
        }
    }
    return ParseResult{false, BytesRead};
}

/// Parse acknowledgment packet from buffer converting all fields to host byte order
/// @param[Buffer] Assumptions: \p Buffer is not a nullptr, it's size is greater or equal than \p Len
/// @param[Len] Assumptions: \p Len is greater than zero
/// @n If parsing wasn't successful, \p Packet remains in valid but unspecified state
ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, Acknowledgment &Packet) {
    assert(Buffer != nullptr);
    assert(Len > 0);

    std::size_t Step = 0;
    std::size_t BytesRead = 0;
    for (std::size_t Idx = 0; Idx != Len; ++Idx) {
        const auto Byte = Buffer[Idx];
        BytesRead++;

        switch (Step) {
        // Opcode (2 bytes)
        case 0:
            Packet.Type_ = std::uint16_t(Byte) << 0;
            Step++;
            break;
        case 1:
            Packet.Type_ |= std::uint16_t(Byte) << 8;
            Packet.Type_ = ntohs(Packet.Type_);
            if (Packet.Type_ != packets::Type::AcknowledgmentPacket) {
                Step = 0;
                continue;
            }
            Step++;
            break;
        // Block # (2 bytes)
        case 2:
            Packet.Block = std::uint16_t(Byte) << 0;
            Step++;
            break;
        case 3:
            Packet.Block |= std::uint16_t(Byte) << 8;
            Packet.Block = ntohs(Packet.Block);
            return ParseResult{true, BytesRead};
        default:
            assert(false);
        }
    }
    return ParseResult{false, BytesRead};
}

/// Parse error packet from buffer converting all fields to host byte order
/// @param[Buffer] Assumptions: \p Buffer is not a nullptr, it's size is greater or equal than \p Len
/// @param[Len] Assumptions: \p Len is greater than zero
/// @n If parsing wasn't successful, \p Packet remains in valid but unspecified state
ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, Error &Packet) {
    assert(Buffer != nullptr);
    assert(Len > 0);

    std::size_t Step = 0;
    std::size_t BytesRead = 0;
    for (std::size_t Idx = 0; Idx != Len; ++Idx) {
        const auto Byte = Buffer[Idx];
        BytesRead++;

        switch (Step) {
        // Opcode (2 bytes)
        case 0:
            Packet.Type_ = std::uint16_t(Byte) << 0;
            Step++;
            break;
        case 1:
            Packet.Type_ |= std::uint16_t(Byte) << 8;
            Packet.Type_ = ntohs(Packet.Type_);
            if (Packet.Type_ != packets::Type::ErrorPacket) {
                Step = 0;
                continue;
            }
            Step++;
            break;
        // ErrorCode (2 bytes)
        case 2:
            Packet.ErrorCode = std::uint16_t(Byte) << 0;
            Step++;
            break;
        case 3:
            Packet.ErrorCode |= std::uint16_t(Byte) << 8;
            Packet.ErrorCode = ntohs(Packet.ErrorCode);
            Step++;
            break;
        // ErrorMessage
        case 4:
            Packet.ErrorMessage.push_back(Byte);

            if (Byte == 0u) {
                return ParseResult{true, BytesRead};
            }
            break;
        default:
            assert(false);
        }
    }
    return ParseResult{false, BytesRead};
}

/// Parse error packet from buffer converting all fields to host byte order
/// @param[Buffer] Assumptions: \p Buffer is not a nullptr, it's size is greater or equal than \p Len
/// @param[Len] Assumptions: \p Len is greater than zero
/// @n If parsing wasn't successful, \p Packet remains in valid but unspecified state
ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, OptionAcknowledgment &Packet) {
    assert(Buffer != nullptr);
    assert(Len > 0);

    std::string Name;
    std::string Value;
    std::size_t Step = 0;
    std::size_t BytesRead = 0;
    for (std::size_t Idx = 0; Idx != Len; ++Idx) {
        const auto Byte = Buffer[Idx];
        BytesRead++;

        switch (Step) {
        // Opcode (2 bytes)
        case 0:
            Packet.Type_ = std::uint16_t(Byte) << 0;
            Step++;
            break;
        case 1:
            Packet.Type_ |= std::uint16_t(Byte) << 8;
            Packet.Type_ = ntohs(Packet.Type_);
            if (Packet.Type_ != packets::Type::OptionAcknowledgmentPacket) {
                Step = 0;
                continue;
            }
            Step++;
            break;
        // Option name
        case 2:
            if (Byte == 0u)
                Step++;
            else
                Name.push_back(Byte);
            break;
        // Option value
        case 3:
            if (Byte == 0u) {
                Packet.Options.emplace(std::move(Name), std::move(Value));

                Name.clear();
                Value.clear();

                if (Idx == Len - 1) {
                    return ParseResult{true, BytesRead};
                }
                Step--;
            } else {
                Value.push_back(Byte);
            }
            break;
        default:
            assert(false);
        }
    }
    return ParseResult{false, BytesRead};
}

} // namespace tftp_common::packets
