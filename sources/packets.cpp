#include "packets.hpp"

namespace packets {

request::request() { }

request::~request() { }

void request::serialize(std::vector<std::uint8_t>& buf) {
	buf.push_back(htonl(static_cast<std::uint16_t>(type)) >> 8);
	buf.push_back(htonl(static_cast<std::uint16_t>(type)) >> 0);
	for (auto byte: filename) {
		buf.push_back(byte);
	}
	for (auto byte: mode) {
		buf.push_back(byte);
	}
}

data::data() { }

data::~data() { }

void data::serialize(std::vector<std::uint8_t>& buf) {
	buf.push_back(htonl(block) >> 8);
	buf.push_back(htonl(block) >> 0);
	for (auto byte: data_) {
		buf.push_back(byte);
	}
}

acknowledgment::acknowledgment() { }

acknowledgment::~acknowledgment() { }

void acknowledgment::serialize(std::vector<std::uint8_t>& buf) {
	buf.push_back(htonl(block) >> 8);
	buf.push_back(htonl(block) >> 0);
}

error::error() { }

error::~error() { }

void error::serialize(std::vector<std::uint8_t>& buf) {
	buf.push_back(htonl(error_code) >> 8);
	buf.push_back(htonl(error_code) >> 0);
	for (auto byte: error_message) {
		buf.push_back(byte);
	}
}

}