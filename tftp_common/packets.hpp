#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif

#include <cassert>
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
    ErrorPacket = 0x05,
    // Option Acknowledgment (OACK) operation code
    OptionAcknowledgmentPacket = 0x06
};

/// Read/Write Request (RRQ/WRQ) Trivial File Transfer Protocol packet
class Request {
  public:
    /// Use with parsing functions only
    Request() {}
    /// @param[type] Assumptions: The \p type is either ::ReadRequest or ::WriteRequest
    /// @param[error_message] Assumptions: The \p filename is a view to **null-terminated string**
    /// @param[mode] Assumptions: The \p mode is a view to **null-terminated string**
    Request(Type type, std::string_view filename, std::string_view mode)
        : type_(type), filename(filename.begin(), filename.end() + 1), mode(mode.begin(), mode.end() + 1) {
        assert(type == Type::ReadRequest || type == Type::WriteRequest);
        assert(filename[filename.size()] == '\0');
        assert(mode[mode.size()] == '\0');
    }
    Request(Type type, std::string_view filename, std::string_view mode, const std::vector<std::string>& optionsNames, const std::vector<std::string>& optionsValues)
        : Request(type, filename, mode) {
        this->optionsNames = optionsNames;
        this->optionsValues = optionsValues;
    }
    ~Request() {}

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[it] Requirements: \p *(it) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator it) {
        assert(optionsNames.size() == optionsValues.size());

        *(it++) = static_cast<std::uint8_t>(htons(type_) >> 0);
        *(it++) = static_cast<std::uint8_t>(htons(type_) >> 8);

        for (auto byte : filename) {
            *(it++) = static_cast<std::uint8_t>(byte);
        }
        for (auto byte : mode) {
            *(it++) = static_cast<std::uint8_t>(byte);
        }

        std::size_t optionsSize = 0;
        for (std::size_t idx = 0; idx != optionsNames.size(); ++idx) {
            for (auto byte: optionsNames[idx]) {
                *(it++) = static_cast<std::uint8_t>(byte);
            }
            *(it++) = '\0';
            for (auto byte: optionsValues[idx]) {
                *(it++) = static_cast<std::uint8_t>(byte);
            }
            *(it++) = '\0';
            optionsSize += optionsNames[idx].size() + optionsValues[idx].size() + 2;
        }

        return sizeof(type_) + filename.size() + mode.size() + optionsSize;
    }

    std::uint16_t getType() const { return type_; }

    std::string_view getFilename() const {
        return std::string_view(reinterpret_cast<const char *>(filename.data()), filename.size() - 1);
    }

    std::string_view getMode() const {
        return std::string_view(reinterpret_cast<const char *>(mode.data()), mode.size() - 1);
    }

    std::string_view getOptionName(std::size_t idx) const {
        return std::string_view(reinterpret_cast<const char*>(optionsNames[idx].data()), optionsNames[idx].size());
    }

    std::string_view getOptionValue(std::size_t idx) const {
        return std::string_view(reinterpret_cast<const char*>(optionsValues[idx].data()), optionsValues[idx].size());
    }

  private:
    friend ParseResult parse(std::uint8_t *buffer, std::size_t len, Request &packet);

    std::uint16_t type_;
    std::vector<std::uint8_t> filename;
    std::vector<std::uint8_t> mode;
    std::vector<std::string> optionsNames;
    std::vector<std::string> optionsValues;
};

/// Data Trivial File Transfer Protocol packet
class Data {
  public:
    /// Use with parsing functions only
    Data() {}
    /// @param[block] Assumptions: The \p block value is greater than one
    /// @param[buffer] Assumptions: The \p buffer size is greater or equal than 0 and less or equal than 512
    Data(std::uint16_t block, const std::vector<std::uint8_t> &buffer)
        : block(block), data_(buffer.begin(), buffer.end()) {
        // The block numbers on data packets begin with one and increase by one for each new block of data
        assert(block >= 1);
        // The data field is from zero to 512 bytes long
        assert(buffer.size() >= 0 && buffer.size() <= 512);
    }
    /// @param[block] Assumptions: The \p block value is greater than one
    /// @param[buffer] Assumptions: The \p buffer size is greater or equal than 0 and less or equal than 512
    Data(std::uint16_t block, std::vector<std::uint8_t> &&buffer) : block(block), data_(std::move(buffer)) {
        assert(block >= 1);
        assert(buffer.size() >= 0 && buffer.size() <= 512);
    }
    ~Data() {}

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[it] Requirements: \p *(it) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator it) {
        *(it++) = static_cast<std::uint8_t>(htons(type) >> 0);
        *(it++) = static_cast<std::uint8_t>(htons(type) >> 8);
        *(it++) = static_cast<std::uint8_t>(htons(block) >> 0);
        *(it++) = static_cast<std::uint8_t>(htons(block) >> 8);
        for (auto byte : data_) {
            *(it++) = static_cast<std::uint8_t>(byte);
        }

        return sizeof(type) + sizeof(block) + data_.size();
    }

    std::uint16_t getType() const { return type; }

    std::uint16_t getBlock() const { return block; }

    const std::vector<std::uint8_t> &getData() const { return data_; }

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
    /// @param[block] Assumptions: the \p block is equal or greater than one
    Acknowledgment(std::uint16_t block) : block(block) {
        // The block numbers on data packets begin with one and increase by one for each new block of data
        assert(block >= 1);
    }
    ~Acknowledgment() {}

    std::uint16_t getType() const { return type; }

    std::uint16_t getBlock() const { return block; }

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[it] Requirements: \p *(it) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator it) {
        *(it++) = static_cast<std::uint8_t>(htons(type) >> 0);
        *(it++) = static_cast<std::uint8_t>(htons(type) >> 8);
        *(it++) = static_cast<std::uint8_t>(htons(block) >> 0);
        *(it++) = static_cast<std::uint8_t>(htons(block) >> 8);

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
    /// @param[error_code] Assumptions: The \p error_code is equal or greater than zero and less or equal than eight
    /// @param[error_message] Assumptions: The \p error_message is a view to **null-terminated string**
    Error(std::uint16_t error_code, std::string_view error_message)
        : error_code(error_code), error_message(error_message.begin(), error_message.end() + 1) {
        assert(error_code >= 0 && error_code <= 8);
        assert(error_message[error_message.size()] == '\0');
    }
    ~Error() {}

    std::uint16_t getType() const { return type; }

    std::uint16_t getErrorCode() const { return error_code; }

    std::string_view getErrorMessage() const {
        return std::string_view(reinterpret_cast<const char *>(error_message.data()), error_message.size() - 1);
    }

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[it] Requirements: \p *(it) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator it) {
        *(it++) = static_cast<std::uint8_t>(htons(type) >> 0);
        *(it++) = static_cast<std::uint8_t>(htons(type) >> 8);
        *(it++) = static_cast<std::uint8_t>(htons(error_code) >> 0);
        *(it++) = static_cast<std::uint8_t>(htons(error_code) >> 8);
        for (auto byte : error_message) {
            *(it++) = static_cast<std::uint8_t>(byte);
        }

        return sizeof(type) + sizeof(error_code) + error_message.size();
    }

  private:
    friend ParseResult parse(std::uint8_t *buffer, std::size_t len, Error &packet);

    std::uint16_t type = Type::ErrorPacket;
    std::uint16_t error_code;
    std::vector<std::uint8_t> error_message;
};

/// Option Acknowledgment Trivial File Transfer Protocol packet
class OptionAcknowledgment {
public:
    /// Use with parsing functions only
    OptionAcknowledgment() { }
    OptionAcknowledgment(const std::vector<std::string>& optionsNames, const std::vector<std::string>& optionsValues)
        : optionsNames(optionsNames.begin(), optionsNames.end()), optionsValues(optionsValues.begin(), optionsValues.end()) { }
    ~OptionAcknowledgment() { }

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[it] Requirements: \p *(it) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator it) {
        *(it++) = static_cast<std::uint8_t>(htons(type) >> 0);
        *(it++) = static_cast<std::uint8_t>(htons(type) >> 8);

        assert(optionsNames.size() == optionsValues.size());
        std::size_t optionsSize = 0;
        for (std::size_t idx = 0; idx != optionsNames.size(); ++idx) {
            for (auto byte: optionsNames[idx]) {
                *(it++) = static_cast<std::uint8_t>(byte);
            }
            *(it++) = '\0';
            for (auto byte: optionsValues[idx]) {
                *(it++) = static_cast<std::uint8_t>(byte);
            }
            *(it++) = '\0';
            optionsSize += optionsNames[idx].size() + optionsValues[idx].size() + 2;
        }

        return sizeof(type) + optionsSize;
    }

    std::uint16_t getType() const { return type; }

    std::string_view getOptionName(std::size_t idx) const {
        return std::string_view(reinterpret_cast<const char*>(optionsNames[idx].data()), optionsNames[idx].size());
    }

    std::string_view getOptionValue(std::size_t idx) const {
        return std::string_view(reinterpret_cast<const char*>(optionsValues[idx].data()), optionsValues[idx].size());
    }
private:
    friend ParseResult parse(std::uint8_t *buffer, std::size_t len, OptionAcknowledgment &packet);

    std::uint16_t type = Type::OptionAcknowledgmentPacket;
    std::vector<std::string> optionsNames, optionsValues;
};

} // namespace packets

} // namespace tftp_common
