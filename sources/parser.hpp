#pragma once

#include "packets.hpp"

namespace tftp_common {

class parser
{
public:
	bool parse(std::uint8_t* data, std::size_t len, std::size_t bytes_read, packets::request& packet);
	bool parse(std::uint8_t* data, std::size_t len, std::size_t bytes_read, packets::data& packet);
	bool parse(std::uint8_t* data, std::size_t len, std::size_t bytes_read, packets::acknowledgment& packet);
	bool parse(std::uint8_t* data, std::size_t len, std::size_t bytes_read, packets::error& packet);
private:
	int step_ = 0;
};

}