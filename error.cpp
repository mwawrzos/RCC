#include "stdafx.h"

#include "error.h"


std::string Error::mkError(ERROR type, const std::string &value, unsigned int line, unsigned int column)
{
	std::stringstream ss(file);

	ss << "(" << line << ":" << column << "): ";

	switch (type)
	{
	case ERROR::CONSTANT_TOO_BIG:
		ss << "error: constant too big [" << value << "]"; break;
	case ERROR::MODIFIER_EXPECTED:
		ss << "syntax error: modyfier expected"; break;
	default:
		return "unknown error";
	}
	return ss.str();
}