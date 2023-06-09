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

/// Trivial File Transfer Protocol packet type
enum type : std::uint16_t {
	/// Read request (RRQ) operation code
    read_request = 0x01 , 
    /// Write request (WRQ) operation code
    write_request = 0x02,
    /// Data (DATA) operation code
    data_packet = 0x03,
    /// Acknowledgment (ACK) operation code
    acknowledgment_packet = 0x04,
   	/// Error (ERROR) operation code
    error_packet = 0x05
};

/// Read/Write Request (RRQ/WRQ) Trivial File Transfer Protocol packet
class request {
  public:
  	/// Use with parsing functions only
    request() {}
    /// @param[error_message] Assumptions: \p The filename is a **null-terminated string**
    /// @param[mode] Assumptions: \p The mode is a **null-terminated string**
    request(type type, std::string_view filename, std::string_view mode)
        : type_(type), filename(filename.begin(), filename.end() + 1), mode(mode.begin(), mode.end() + 1) {}
    ~request() {}

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

  public:
    std::uint16_t type_;
    std::vector<std::uint8_t> filename;
    std::vector<std::uint8_t> mode;
};

/// Data Trivial File Transfer Protocol packet
class data {
  public:
  	/// Use with parsing functions only
    data() {}
    data(std::uint16_t block, const std::vector<std::uint8_t> &buffer)
        : block(block), data_(buffer.begin(), buffer.end()) {}
    data(std::uint16_t block, std::vector<std::uint8_t> &&buffer) : block(block), data_(std::move(buffer)) {}
    ~data() {}

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

  public:
    std::uint16_t type = type::data_packet;
    std::uint16_t block;
    std::vector<std::uint8_t> data_;
};

/// Acknowledgment Trivial File Transfer Protocol packet
class acknowledgment {
  public:
  	/// Use with parsing functions only
    acknowledgment() {}
    acknowledgment(std::uint16_t block) : block(block) {}
    ~acknowledgment() {}

    /// Convert packet to network byte order and serialize it into the given buffer
    std::size_t serialize(std::vector<std::uint8_t> &buf) {
        buf.push_back(htons(type) >> 8);
        buf.push_back(htons(type) >> 0);
        buf.push_back(htons(block) >> 8);
        buf.push_back(htons(block) >> 0);

        return sizeof(type) + sizeof(block);
    }

  public:
    std::uint16_t type = type::acknowledgment_packet;
    std::uint16_t block;
};

/// Error Trivial File Transfer Protocol packet
class error {
  public:
  	/// Use with parsing functions only
    error() {}
    /// @param[error_message] Assumptions: \p The error message is a **null-terminated string**
    error(std::uint16_t error_code, std::string_view error_message)
        : error_code(error_code), error_message(error_message.begin(), error_message.end() + 1) {}
    ~error() {}

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

  public:
    std::uint16_t type = type::error_packet;
    std::uint16_t error_code;
    std::vector<std::uint8_t> error_message;
};

} // namespace packets

} // namespace tftp_common
