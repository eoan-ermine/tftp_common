#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace tftp_common {

namespace packets {

struct ParseResult;

/// Trivial File Transfer Protocol packet type
enum Type : std::uint16_t {
    /// Read request (RRQ) operation code
    ReadRequest = 0x01,
    /// Write request (WRQ) operation code
    WriteRequest = 0x02,
    /// Data (DATA) operation code
    DataPacket = 0x03,
    /// Acknowledgment (ACK) operation code
    AcknowledgmentPacket = 0x04,
    /// Error (ERROR) operation code
    ErrorPacket = 0x05
};

/// Read/Write Request (RRQ/WRQ) Trivial File Transfer Protocol packet
class Request {
  public:
    /// Use with parsing functions only
    Request() {}
    /// @param[error_message] Assumptions: \p The filename is a **null-terminated string**
    /// @param[mode] Assumptions: \p The mode is a **null-terminated string**
    Request(Type type, std::string_view filename, std::string_view mode)
        : type_(type), filename(filename.begin(), filename.end() + 1), mode(mode.begin(), mode.end() + 1) {}
    ~Request() {}

    /// Convert packet to network byte order and serialize it into the given buffer
    std::size_t serialize(std::vector<std::uint8_t> &buf) {
        buf.push_back(htons(type_) >> 8);
        buf.push_back(htons(type_) >> 0);
        for (auto byte : filename) {
            buf.push_back(byte);
        }
        for (auto byte : mode) {
            buf.push_back(byte);
        }

        return sizeof(type_) + filename.size() + mode.size();
    }

  private:
    friend ParseResult parse(std::uint8_t *buffer, std::size_t len, Request &packet);

    std::uint16_t type_;
    std::vector<std::uint8_t> filename;
    std::vector<std::uint8_t> mode;
};

/// Data Trivial File Transfer Protocol packet
class Data {
  public:
    /// Use with parsing functions only
    Data() {}
    Data(std::uint16_t block, const std::vector<std::uint8_t> &buffer)
        : block(block), data_(buffer.begin(), buffer.end()) {}
    Data(std::uint16_t block, std::vector<std::uint8_t> &&buffer) : block(block), data_(std::move(buffer)) {}
    ~Data() {}

    /// Convert packet to network byte order and serialize it into the given buffer
    std::size_t serialize(std::vector<std::uint8_t> &buf) {
        buf.push_back(htons(type) >> 8);
        buf.push_back(htons(type) >> 0);
        buf.push_back(htons(block) >> 8);
        buf.push_back(htons(block) >> 0);
        for (auto byte : data_) {
            buf.push_back(byte);
        }

        return sizeof(type) + sizeof(block) + data_.size();
    }

  private:
    friend ParseResult parse(std::uint8_t *buffer, std::size_t len, Data &packet);

    std::uint16_t type = Type::DataPacket;
    std::uint16_t block;
    std::vector<std::uint8_t> data_;
};

/// Acknowledgment Trivial File Transfer Protocol packet
class Acknowledgment {
  public:
    /// Use with parsing functions only
    Acknowledgment() {}
    Acknowledgment(std::uint16_t block) : block(block) {}
    ~Acknowledgment() {}

    /// Convert packet to network byte order and serialize it into the given buffer
    std::size_t serialize(std::vector<std::uint8_t> &buf) {
        buf.push_back(htons(type) >> 8);
        buf.push_back(htons(type) >> 0);
        buf.push_back(htons(block) >> 8);
        buf.push_back(htons(block) >> 0);

        return sizeof(type) + sizeof(block);
    }

  private:
    friend ParseResult parse(std::uint8_t *buffer, std::size_t len, Acknowledgment &packet);

    std::uint16_t type = Type::AcknowledgmentPacket;
    std::uint16_t block;
};

/// Error Trivial File Transfer Protocol packet
class Error {
  public:
    /// Use with parsing functions only
    Error() {}
    /// @param[error_message] Assumptions: \p The error message is a **null-terminated string**
    Error(std::uint16_t error_code, std::string_view error_message)
        : error_code(error_code), error_message(error_message.begin(), error_message.end() + 1) {}
    ~Error() {}

    /// Convert packet to network byte order and serialize it into the given buffer
    std::size_t serialize(std::vector<std::uint8_t> &buf) {
        buf.push_back(htons(type) >> 8);
        buf.push_back(htons(type) >> 0);
        buf.push_back(htons(error_code) >> 8);
        buf.push_back(htons(error_code) >> 0);
        for (auto byte : error_message) {
            buf.push_back(byte);
        }

        return sizeof(type) + sizeof(error_code) + error_message.size();
    }

  private:
    friend ParseResult parse(std::uint8_t *buffer, std::size_t len, Error &packet);

    std::uint16_t type = Type::ErrorPacket;
    std::uint16_t error_code;
    std::vector<std::uint8_t> error_message;
};

} // namespace packets

} // namespace tftp_common
