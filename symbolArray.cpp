#include "stdafx.h"
#include "symbolArray.h"

typedef size_t refId;

symbolArray::symbolArray()
{
}


symbolArray::~symbolArray()
{
}


std::ostream &operator<<(std::ostream &out, const entry &entry)
{
	auto &val = std::get<1>(entry);
	switch (std::get<0>(entry))
	{
	case TYPES::ERROR:
		out << *val.string; break;
	case TYPES::NUMBER:
		out << val.number; break;
	case TYPES::LABEL:
		out << *val.string; break;
	default:
		out << "Implement me for " << (int) std::get<0>(entry);
	}
	return out;
}