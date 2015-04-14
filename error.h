#pragma once

enum class ERROR
{
	SYNTAX_ERROR,
	CONSTANT_TOO_BIG,
	MODIFIER_EXPECTED
};

class Error
{
public:
	std::tuple<std::string, std::string> mkError(ERROR type, const std::string &message, unsigned int line, unsigned int column);

	std::string file = "";
};