#pragma once

#include <arpa/inet.h>

#include <string_view>
#include <cstdint>
#include <vector>
#include <string>

namespace tftp_common {

namespace packets {

enum type: std::uint16_t {
	read_request = 0x01, write_request = 0x02, data_packet = 0x03, acknowledgment_packet = 0x04, error_packet = 0x05
};

class request {
public:
	request() { }
	request(type type, std::string_view filename, std::string_view mode)
		: type_(type), filename(filename.begin(), filename.end() + 1), mode(mode.begin(), mode.end() + 1) { }
	~request() { }

	std::size_t serialize(std::vector<std::uint8_t>& buf) {
		buf.push_back(htons(type_) >> 8);
		buf.push_back(htons(type_) >> 0);
		for (auto byte: filename) {
			buf.push_back(byte);
		}
		for (auto byte: mode) {
			buf.push_back(byte);
		}

		return sizeof(type_) + filename.size() + mode.size();
	}
public:
	std::uint16_t type_;
	std::vector<std::uint8_t> filename;
	std::vector<std::uint8_t> mode;
};

class data {
public:
	data() { }
	data(std::uint16_t block, const std::vector<std::uint8_t>& buffer)
		: block(block), data_(buffer.begin(), buffer.end()) { }
	data(std::uint16_t block, std::vector<std::uint8_t>&& buffer)
		: block(block), data_(std::move(buffer)) { }
	~data() { }

	std::size_t serialize(std::vector<std::uint8_t>& buf) {
		buf.push_back(htons(type) >> 8);
		buf.push_back(htons(type) >> 0);
		buf.push_back(htons(block) >> 8);
		buf.push_back(htons(block) >> 0);
		for (auto byte: data_) {
			buf.push_back(byte);
		}

		return sizeof(type) + sizeof(block) + data_.size();
	}
public:
	std::uint16_t type = type::data_packet;
	std::uint16_t block;
	std::vector<std::uint8_t> data_;
};

class acknowledgment {
public:
	acknowledgment() { }
	acknowledgment(std::uint16_t block)
		: block(block) { }
	~acknowledgment() { }

	std::size_t serialize(std::vector<std::uint8_t>& buf) { 
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

class error {
public:
	error() { }
	error(std::uint16_t error_code, std::string_view error_message)
		: error_code(error_code), error_message(error_message.begin(), error_message.end() + 1) { }
	~error() { }

	std::size_t serialize(std::vector<std::uint8_t>& buf) { 
		buf.push_back(htons(type) >> 8);
		buf.push_back(htons(type) >> 0);
		buf.push_back(htons(error_code) >> 8);
		buf.push_back(htons(error_code) >> 0);
		for (auto byte: error_message) {
			buf.push_back(byte);
		}

		return sizeof(type) + sizeof(error_code) + error_message.size();
	}
public:
	std::uint16_t type = type::error_packet;
	std::uint16_t error_code;
	std::vector<std::uint8_t> error_message;
};

}

}
