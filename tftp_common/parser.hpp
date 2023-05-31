#pragma once

#include "packets.hpp"

namespace tftp_common {

class parser
{
public:
	bool parse(std::uint8_t* data, std::size_t len, std::size_t bytes_read, packets::request& packet) {
		bytes_read = 0;
		for (std::size_t i = 0; i != len; ++i) {
			const auto byte = data[i];
			bytes_read++;

			switch (step_) {
			// Opcode (2 bytes)
			case 0:
				packet.type = std::uint16_t(byte) << 8;
				step_++;
				break;
			case 1:
				packet.type |= std::uint16_t(byte) << 0;
				packet.type = ntohs(packet.type);
				if (packet.type != 0x01 || packet.type != 0x02) {
					step_ = 0;
					continue;
				}
				step_++;
				break;
			// Filename
			case 2:
				packet.filename.push_back(byte);

				if (!byte)
					step_++;
				break;
			// Mode
			case 3:
				packet.mode.push_back(byte);

				if (!byte) {
					step_ = 0;
					return true;
				}
				break;
			}
		}
		return false;
	}
	bool parse(std::uint8_t* data, std::size_t len, std::size_t bytes_read, packets::data& packet) {
		bytes_read = 0;
		for (std::size_t i = 0; i != len; ++i) {
			const auto byte = data[i];
			bytes_read++;

			switch (step_) {
			// Opcode (2 bytes)
			case 0:
				packet.type = std::uint16_t(byte) << 8;
				step_++;
				break;
			case 1:
				packet.type |= std::uint16_t(byte) << 0;
				packet.type = ntohs(packet.type);
				if (packet.type != 0x03) {
					step_ = 0;
					continue;
				}
				step_++;
				break;
			// Block # (2 bytes)
			case 2:
				packet.block = std::uint16_t(byte) << 8;
				step_++;
				break;
			case 3:
				packet.block |= std::uint16_t(byte) << 0;
				packet.block = ntohs(packet.block);
				step_++;
				break;
			// Data
			case 4:
				packet.data_.push_back(byte);

				if (i == len - 1) {
					step_ = 0;
					return true;
				}

				break;
			}
		}
		return false;
	}
	bool parse(std::uint8_t* data, std::size_t len, std::size_t bytes_read, packets::acknowledgment& packet) {
		bytes_read = 0;
		for (std::size_t i = 0; i != len; ++i) {
			const auto byte = data[i];
			bytes_read++;

			switch (step_) {
			// Opcode (2 bytes)
			case 0:
				packet.type = std::uint16_t(byte) << 8;
				step_++;
				break;
			case 1:
				packet.type |= std::uint16_t(byte) << 0;
				packet.type = ntohs(packet.type);
				if (packet.type != 0x04) {
					step_ = 0;
					continue;
				}
				step_++;
				break;
			// Block # (2 bytes)
			case 2:
				packet.block = std::uint16_t(byte) << 8;
				step_++;
				break;
			case 3:
				packet.block |= std::uint16_t(byte) << 0;
				packet.block = ntohs(packet.block);
				step_ = 0;
				return true;
			}
		}
		return false;
	}
	bool parse(std::uint8_t* data, std::size_t len, std::size_t bytes_read, packets::error& packet) {
		bytes_read = 0;
		for (std::size_t i = 0; i != len; ++i) {
			const auto byte = data[i];
			bytes_read++;

			switch (step_) {
			// Opcode (2 bytes)
			case 0:
				packet.type = std::uint16_t(byte) << 8;
				step_++;
				break;
			case 1:
				packet.type |= std::uint16_t(byte) << 0;
				packet.type = ntohs(packet.type);
				if (packet.type != 0x05) {
					step_ = 0;
					continue;
				}
				step_++;
				break;
			// ErrorCode (2 bytes)
			case 2:
				packet.error_code = std::uint16_t(byte) << 8;
				step_++;
				break;
			case 3:
				packet.error_code |= std::uint16_t(byte) << 0;
				packet.error_code = ntohs(packet.type);
				step_++;
				break;
			// ErrorMessage
			case 4:
				packet.error_message.push_back(byte);

				if (!byte) {
					step_ = 0;
					return true;
				}
				break;
			}
		}
		return false;
	}
private:
	int step_ = 0;
};

}