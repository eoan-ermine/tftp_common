#include <arpa/inet.h>

#include <cstdint>
#include <vector>
#include <string>

namespace packets {

class request {
public:
	request();
	~request();

	void serialize(std::vector<std::uint8_t>& buf);
public:
	enum class request_type : std::uint16_t {
		read = 1, write = 2
	} type;
	std::vector<std::uint8_t> filename;
	std::vector<std::uint8_t> mode;
};

class data {
public:
	data();
	~data();

	void serialize(std::vector<std::uint8_t>& buf);
public:
	std::uint16_t block;
	std::vector<std::uint8_t> data_;
};

class acknowledgment {
public:
	acknowledgment();
	~acknowledgment();

	void serialize(std::vector<std::uint8_t>& buf);
public:
	std::uint16_t block;
};

class error {
public:
	error();
	~error();

	void serialize(std::vector<std::uint8_t>& buf);
public:
	std::uint16_t error_code;
	std::vector<std::uint8_t> error_message;
};

}