#pragma once

#include <arpa/inet.h>

#include <cstdint>
#include <vector>
#include <string>

namespace tftp_common {

namespace packets {

class request {
public:
	request();
	~request();

	void serialize(std::vector<std::uint8_t>& buf);
public:
	std::uint16_t type;
	std::vector<std::uint8_t> filename;
	std::vector<std::uint8_t> mode;
};

class data {
public:
	data();
	~data();

	void serialize(std::vector<std::uint8_t>& buf);
public:
	std::uint16_t type = 0x3;
	std::uint16_t block;
	std::vector<std::uint8_t> data_;
};

class acknowledgment {
public:
	acknowledgment();
	~acknowledgment();

	void serialize(std::vector<std::uint8_t>& buf);
public:
	std::uint16_t type = 0x4;
	std::uint16_t block;
};

class error {
public:
	error();
	~error();

	void serialize(std::vector<std::uint8_t>& buf);
public:
	std::uint16_t type = 0x5;
	std::uint16_t error_code;
	std::vector<std::uint8_t> error_message;
};

}

}