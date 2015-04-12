#include "stdafx.h"

#include "error.h"


std::string Error::mkError(ERROR type, std::string &value, unsigned int line, unsigned int column)
{
	std::stringstream ss(file);

	ss << "(" << line << ":" << column << "): ";

	switch (type)
	{
	case ERROR::CONSTANT_TOO_BIG:
		ss << "error: constant too big [" << value << "]"; break;
	default:
		return "unknown error";
	}
	return ss.str();
}