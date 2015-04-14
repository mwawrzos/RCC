#include "stdafx.h"

#include "error.h"


std::tuple<std::string, std::string> Error::mkError(ERROR type, const std::string &value, unsigned int line, unsigned int column)
{
	std::stringstream ss(file);
	std::string v;

	ss << "(" << line << ":" << column << "): ";

	switch (type)
	{
	case ERROR::SYNTAX_ERROR:
		v = value;
		ss << "syntax error: unexpected character [" << value << "]"; break;
	case ERROR::CONSTANT_TOO_BIG:
		v = value;
		ss << "error: constant too big [" << value << "]"; break;
	case ERROR::MODIFIER_EXPECTED:
		v = ".";
		ss << "syntax error: modyfier expected"; break;
	default:
		return std::make_tuple("unknown error", value);
	}
	return std::make_tuple(ss.str(), v);
}