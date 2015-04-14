#include "stdafx.h"
#include "symbolArray.h"

typedef size_t refId;

symbolArray::symbolArray()
{
	push_back(std::make_tuple(TYPES::MOCK, -1));
}


symbolArray::~symbolArray()
{	
	while (!empty())
	{
		auto t = back();

		switch (std::get<0>(t))
		{
		case TYPES::COMMENT: delete std::get<1>(t).string; break;
		case TYPES::ERROR: delete std::get<1>(t).errorMsg; break;
		case TYPES::LABEL: delete std::get<1>(t).string;   break;
		}

		pop_back();
	}
}


#define Gen(ARG, CLASS, VAL) case ARG: out << "<span class=\"" #CLASS "\">" << VAL << "</span>"; break;
std::ostream &operator<<(std::ostream &out, const entry &entry)
{
	auto &val = std::get<1>(entry);
	switch (std::get<0>(entry))
	{
	case TYPES::ERROR:
	{
		auto a = std::get<1>(*val.errorMsg);
		encode(a);
		out << "<abbr title=\"" << std::get<0>(*val.errorMsg) << "\">" << a << "</abbr>"; break;
	}
	case TYPES::COMMENT: 
	{
		std::string a = *val.string;
		encode(a);
		if (val.string->size())
			out << R"(<span class="comment">)" << a << "</span>";
		out << std::endl;
		break;
	}
		Gen(TYPES::NUMBER, number, val.number);
		Gen(TYPES::LABEL, label, *val.string);
	default:
		out << "Implement me for " << (int) std::get<0>(entry);
	}
	return out;
}