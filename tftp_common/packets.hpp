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
#include <unordered_map>
#include <vector>
#include <iostream>

namespace tftp_common::packets {

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
    Request() = default;
    /// @param[Type] Assumptions: The \p type is either ::ReadRequest or ::WriteRequest
    Request(Type Type, std::string_view Filename, std::string_view Mode)
        : Type_(Type), Filename(Filename), Mode(Mode) {
        assert(Type == Type::ReadRequest || Type == Type::WriteRequest);
    }
    /// @param[Type] Assumptions: The \p type is either ::ReadRequest or ::WriteRequest
    Request(Type Type, std::string &&Filename, std::string &&Mode)
        : Type_(Type), Filename(std::move(Filename)), Mode(std::move(Mode)) {
        assert(Type == Type::ReadRequest || Type == Type::WriteRequest);
    }
    /// @param[Type] Assumptions: The \p type is either ::ReadRequest or ::WriteRequest
    Request(Type Type, std::string_view Filename, std::string_view Mode, const std::vector<std::string> &OptionsNames,
            const std::vector<std::string> &OptionsValues)
        : Request(Type, Filename, Mode) {
        this->OptionsNames = OptionsNames;
        this->OptionsValues = OptionsValues;
    }
    /// @param[Type] Assumptions: The \p type is either ::ReadRequest or ::WriteRequest
    Request(Type Type, std::string &&Filename, std::string &&Mode, std::vector<std::string> &&OptionsNames,
            std::vector<std::string> &&OptionsValues)
        : Type_(Type), Filename(std::move(Filename)), Mode(std::move(Mode)),
          OptionsNames(std::move(OptionsNames)), OptionsValues(std::move(OptionsValues)) {
        assert(Type == Type::ReadRequest || Type == Type::WriteRequest);
    }

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[It] Requirements: \p *(It) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator It) {
        assert(OptionsNames.size() == OptionsValues.size());

        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 0);
        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 8);

        for (auto Byte : Filename) {
            *(It++) = static_cast<std::uint8_t>(Byte);
        }
        *(It++) = '\0';

        for (auto Byte : Mode) {
            *(It++) = static_cast<std::uint8_t>(Byte);
        }
        *(It++) = '\0';

        std::size_t OptionsSize = 0;
        for (std::size_t Idx = 0; Idx != OptionsNames.size(); ++Idx) {
            for (auto Byte : OptionsNames[Idx]) {
                *(It++) = static_cast<std::uint8_t>(Byte);
            }
            *(It++) = '\0';
            for (auto Byte : OptionsValues[Idx]) {
                *(It++) = static_cast<std::uint8_t>(Byte);
            }
            *(It++) = '\0';
            OptionsSize += OptionsNames[Idx].size() + OptionsValues[Idx].size() + 2;
        }

        return sizeof(Type) + Filename.size() + Mode.size() + OptionsSize + 2;
    }

    std::uint16_t getType() const { return Type_; }

    std::string_view getFilename() const {
        return std::string_view(Filename.data(), Filename.size());
    }

    std::string_view getMode() const {
        return std::string_view(Mode.data(), Mode.size());
    }

    std::string_view getOptionName(std::size_t Idx) const {
        return std::string_view(OptionsNames[Idx].data(), OptionsNames[Idx].size());
    }

    std::string_view getOptionValue(std::size_t Idx) const {
        return std::string_view(OptionsValues[Idx].data(), OptionsValues[Idx].size());
    }

  private:
    friend ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, Request &Packet);

    std::uint16_t Type_;
    std::string Filename;
    std::string Mode;
    std::vector<std::string> OptionsNames;
    std::vector<std::string> OptionsValues;
};

/// Data Trivial File Transfer Protocol packet
class Data {
  public:
    /// Use with parsing functions only
    Data() = default;
    /// @param[Block] Assumptions: The \p Block value is greater than one
    /// @param[Buffer] Assumptions: The \p Buffer size is greater or equal than 0 and less or equal than 512
    Data(std::uint16_t Block, const std::vector<std::uint8_t> &Buffer)
        : Block(Block), DataBuffer(Buffer.begin(), Buffer.end()) {
        // The block numbers on data packets begin with one and increase by one for each new block of data
        assert(Block >= 1);
        // The data field is from zero to 512 bytes long
        assert(Buffer.size() >= 0 && Buffer.size() <= 512);
    }
    /// @param[Block] Assumptions: The \p Block value is greater than one
    /// @param[Buffer] Assumptions: The \p Buffer size is greater or equal than 0 and less or equal than 512
    Data(std::uint16_t Block, std::vector<std::uint8_t> &&Buffer) : Block(Block) {
        // The block numbers on data packets begin with one and increase by one for each new block of data
        assert(Block >= 1);
        // The data field is from zero to 512 bytes long
        assert(Buffer.size() >= 0 && Buffer.size() <= 512);
        this->DataBuffer = std::move(Buffer);
    }

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[It] Requirements: \p *(It) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator It) const {
        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 0);
        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 8);
        *(It++) = static_cast<std::uint8_t>(htons(Block) >> 0);
        *(It++) = static_cast<std::uint8_t>(htons(Block) >> 8);
        for (auto Byte : DataBuffer) {
            *(It++) = Byte;
        }

        return sizeof(Type) + sizeof(Block) + DataBuffer.size();
    }

    std::uint16_t getType() const { return Type_; }

    std::uint16_t getBlock() const { return Block; }

    const std::vector<std::uint8_t> &getData() const { return DataBuffer; }

  private:
    friend ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, Data &Packet);

    std::uint16_t Type_ = Type::DataPacket;
    std::uint16_t Block;
    std::vector<std::uint8_t> DataBuffer;
};

/// Acknowledgment Trivial File Transfer Protocol packet
class Acknowledgment {
  public:
    /// Use with parsing functions only
    Acknowledgment() = default;
    /// @param[Block] Assumptions: the \p Block is equal or greater than one
    explicit Acknowledgment(std::uint16_t Block) : Block(Block) {
        // The block numbers on data packets begin with one and increase by one for each new block of data
        assert(Block >= 1);
    }

    std::uint16_t getType() const { return Type_; }

    std::uint16_t getBlock() const { return Block; }

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[It] Requirements: \p *(It) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator It) const {
        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 0);
        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 8);
        *(It++) = static_cast<std::uint8_t>(htons(Block) >> 0);
        *(It++) = static_cast<std::uint8_t>(htons(Block) >> 8);

        return sizeof(Type) + sizeof(Block);
    }

  private:
    friend ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, Acknowledgment &Packet);

    std::uint16_t Type_ = Type::AcknowledgmentPacket;
    std::uint16_t Block;
};

/// Error Trivial File Transfer Protocol packet
class Error {
  public:
    /// Use with parsing functions only
    Error() = default;
    /// @param[ErrorCode] Assumptions: The \p ErrorCode is equal or greater than zero and less or equal than eight
    Error(std::uint16_t ErrorCode, std::string_view ErrorMessage)
        : ErrorCode(ErrorCode), ErrorMessage(ErrorMessage) {
        assert(ErrorCode >= 0 && ErrorCode <= 8);
    }

    std::uint16_t getType() const { return Type_; }

    std::uint16_t getErrorCode() const { return ErrorCode; }

    std::string_view getErrorMessage() const {
        return std::string_view(ErrorMessage.data(), ErrorMessage.size());
    }

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[It] Requirements: \p *(It) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator It) const {
        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 0);
        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 8);
        *(It++) = static_cast<std::uint8_t>(htons(ErrorCode) >> 0);
        *(It++) = static_cast<std::uint8_t>(htons(ErrorCode) >> 8);
        
        for (auto Byte : ErrorMessage) {
            *(It++) = Byte;
        }
        *(It++) = '\0';

        return sizeof(Type) + sizeof(ErrorCode) + ErrorMessage.size() + 1;
    }

  private:
    friend ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, Error &Packet);

    std::uint16_t Type_ = Type::ErrorPacket;
    std::uint16_t ErrorCode;
    std::string ErrorMessage;
};

/// Option Acknowledgment Trivial File Transfer Protocol packet
class OptionAcknowledgment {
  public:
    /// Use with parsing functions only
    OptionAcknowledgment() = default;
    OptionAcknowledgment(std::unordered_map<std::string, std::string> Options) : Options(std::move(Options)) {}

    /// Convert packet to network byte order and serialize it into the given buffer by the iterator
    /// @param[It] Requirements: \p *(It) must be assignable from \p std::uint8_t
    /// @return Size of the packet (in bytes)
    template <class OutputIterator> std::size_t serialize(OutputIterator It) {
        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 0);
        *(It++) = static_cast<std::uint8_t>(htons(Type_) >> 8);

        std::size_t OptionsSize = 0;
        for (const auto &[Key, Value] : Options) {
            for (auto Byte : Key) {
                *(It++) = static_cast<std::uint8_t>(Byte);
            }
            *(It++) = '\0';
            for (auto Byte : Value) {
                *(It++) = static_cast<std::uint8_t>(Byte);
            }
            *(It++) = '\0';
            OptionsSize += Key.size() + Value.size() + 2;
        }

        return sizeof(Type) + OptionsSize;
    }

    std::uint16_t getType() const { return Type_; }

    /// Get all options
    const std::unordered_map<std::string, std::string> &getOptions() const { return Options; }

    /// Get option value by its name
    /// @throws std::out_of_range if there's no option with the specified name
    std::string_view getOptionValue(const std::string &OptionName) const { return Options.at(OptionName); }

  private:
    friend ParseResult parse(const std::uint8_t *Buffer, std::size_t Len, OptionAcknowledgment &Packet);

    std::uint16_t Type_ = Type::OptionAcknowledgmentPacket;
    // According to the RFC, the order in which options are specified is not significant, so it's fine
    std::unordered_map<std::string, std::string> Options;
};

} // namespace tftp_common::packets
