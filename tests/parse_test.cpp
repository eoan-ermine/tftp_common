#include <gtest/gtest.h>
#include "../tftp_common/parser.hpp"

using namespace tftp_common::packets;

/// Test that Request packet parsing is going fine
TEST(Request, Parse) {
	std::uint8_t packetBytes[] = {
	  0x00, 0x01, 0x2f, 0x73, 0x72, 0x76, 0x2f, 0x74,
	  0x66, 0x74, 0x70, 0x2f, 0x52, 0x65, 0x61, 0x64,
	  0x46, 0x69, 0x6c, 0x65, 0x00, 0x6e, 0x65, 0x74,
	  0x61, 0x73, 0x63, 0x69, 0x69, 0x00
	};
	auto length = sizeof(packetBytes) / sizeof(std::uint8_t);

	Request packet;
	auto [success, bytesRead] = parse(packetBytes, length, packet);

	ASSERT_EQ(packet.getType(), Type::ReadRequest);
	ASSERT_EQ(packet.getFilename(), "/srv/tftp/ReadFile");
	ASSERT_EQ(packet.getMode(), "netascii");

	ASSERT_EQ(success, true);
	ASSERT_EQ(bytesRead, length);
}

/// Test that Data packet parsing is going fine
TEST(Data, Parse) {
	std::uint8_t packetBytes[] = {
	  0x00, 0x03, 0x00, 0x01, 0x53, 0x6f, 0x6d, 0x65,
	  0x20, 0x63, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74,
	  0x73, 0x2e, 0x2e, 0x2e, 0x0d, 0x0a
	};
	auto length = sizeof(packetBytes) / sizeof(std::uint8_t);

	Data packet;
	auto [success, bytesRead] = parse(packetBytes, length, packet);

	ASSERT_EQ(packet.getType(), Type::DataPacket);
	ASSERT_EQ(packet.getBlock(), 0x01);
	const auto& data = packet.getData();

	std::size_t baseOffset = 2 * sizeof(std::uint16_t);
	for (std::size_t i = 0; i != data.size(); ++i) {
		ASSERT_EQ(data[i], packetBytes[baseOffset + i]);
	}

	ASSERT_EQ(success, true);
	ASSERT_EQ(bytesRead, length);
}

/// Test that Acknowledgment packet parsing is going fine
TEST(Acknowledgment, Parse) {
	std::uint8_t packetBytes[] = {
	  0x00, 0x04, 0x00, 0x01
	};
	auto length = sizeof(packetBytes) / sizeof(std::uint8_t);

	Acknowledgment packet;
	auto [success, bytesRead] = parse(packetBytes, length, packet);

	ASSERT_EQ(packet.getType(), Type::AcknowledgmentPacket);
	ASSERT_EQ(packet.getBlock(), 0x01);	

	ASSERT_EQ(success, true);
	ASSERT_EQ(bytesRead, length);
}

/// Test that Error packet parsing is going fine
TEST(Error, Parse) {
	std::uint8_t packetBytes[] = {
		0x00, 0x05, 0x00, 0x01, 0x46, 0x69, 0x6c, 0x65,
		0x20, 0x6e, 0x6f, 0x74, 0x20, 0x66, 0x6f, 0x75,
		0x6e, 0x64, 0x00
	};
	auto length = sizeof(packetBytes) / sizeof(std::uint8_t);

	Error packet;
	auto [success, bytesRead] = parse(packetBytes, length, packet);

	ASSERT_EQ(packet.getType(), Type::ErrorPacket);
	ASSERT_EQ(packet.getErrorCode(), 0x01);	
	ASSERT_EQ(packet.getErrorMessage(), "File not found");

	ASSERT_EQ(success, true);
	ASSERT_EQ(bytesRead, length);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
