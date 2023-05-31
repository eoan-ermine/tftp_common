#pragma once

#include <arpa/inet.h>

#include <string_view>
#include <cstdint>
#include <vector>
#include <string>

namespace tftp_common {

namespace packets {

class request {
public:
	request() { }
	request(std::uint16_t type_, std::string_view filename_, std::string_view mode_)
		: type(type_), filename(filename_.begin(), filename_.end() + 1), mode(mode_.begin(), mode_.end() + 1) { }
	~request() { }

	void serialize(std::vector<std::uint8_t>& buf) {
		buf.push_back(htons(type) >> 8);
		buf.push_back(htons(type) >> 0);
		for (auto byte: filename) {
			buf.push_back(byte);
		}
		for (auto byte: mode) {
			buf.push_back(byte);
		}
	}
public:
	std::uint16_t type;
	std::vector<std::uint8_t> filename;
	std::vector<std::uint8_t> mode;
};

class data {
public:
	data() { }
	data(std::uint16_t block_, const std::vector<std::uint8_t>& buffer_)
		: block(block_), data_(buffer_.begin(), buffer_.end()) { }
	data(std::uint16_t block_, std::vector<std::uint8_t>&& buffer_)
		: block(block_), data_(std::move(buffer_)) { }
	~data() { }

	void serialize(std::vector<std::uint8_t>& buf) {
		buf.push_back(htons(type) >> 8);
		buf.push_back(htons(type) >> 0);
		buf.push_back(htons(block) >> 8);
		buf.push_back(htons(block) >> 0);
		for (auto byte: data_) {
			buf.push_back(byte);
		}
	}
public:
	std::uint16_t type = 0x3;
	std::uint16_t block;
	std::vector<std::uint8_t> data_;
};

class acknowledgment {
public:
	acknowledgment() { }
	acknowledgment(std::uint16_t block_)
		: block(block_) { }
	~acknowledgment() { }

	void serialize(std::vector<std::uint8_t>& buf) { 
		buf.push_back(htons(type) >> 8);
		buf.push_back(htons(type) >> 0);
		buf.push_back(htons(block) >> 8);
		buf.push_back(htons(block) >> 0);
	}
public:
	std::uint16_t type = 0x4;
	std::uint16_t block;
};

class error {
public:
	error() { }
	error(std::uint16_t error_code_, std::string_view error_message_)
		: error_code(error_code_), error_message(error_message_.begin(), error_message_.end() + 1) { }
	~error() { }

	void serialize(std::vector<std::uint8_t>& buf) { 
		buf.push_back(htons(type) >> 8);
		buf.push_back(htons(type) >> 0);
		buf.push_back(htons(error_code) >> 8);
		buf.push_back(htons(error_code) >> 0);
		for (auto byte: error_message) {
			buf.push_back(byte);
		}
	}
public:
	std::uint16_t type = 0x5;
	std::uint16_t error_code;
	std::vector<std::uint8_t> error_message;
};

}

}