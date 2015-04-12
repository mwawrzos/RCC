#pragma once

enum class ERROR
{
	CONSTANT_TOO_BIG
};

class Error
{
public:
	std::string mkError(ERROR type, std::string &message, unsigned int line, unsigned int column);

	std::string file = "";
};