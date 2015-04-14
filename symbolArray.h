#pragma once
#include "grammar.h"

typedef size_t refId;

union Val {
	WholeNumber number;
	Token *token;
	std::string *string;
	std::tuple<std::string, std::string> *errorMsg;

	Val(WholeNumber number) : number(number) {}
	Val(Token *token) : token(token) {}
	Val(std::string *string) : string(string) {}
	Val(std::tuple<std::string, std::string> *errorMsg) : errorMsg(errorMsg) {}
};

typedef Val val;

typedef std::tuple<TYPES, val> entry;

class symbolArray : public std::vector<entry>
{
public:

	template <TYPES T, class V>
	refId insert(const std::string &key, const V &val);
	template <>
	refId insert<TYPES::NUMBER>(const std::string &key, const WholeNumber &val);

	symbolArray();
	~symbolArray();

private:
	typedef std::unordered_map<std::string, refId> refMap;

	refMap symbolRefs;
};

std::ostream &operator<<(std::ostream &out, const entry &entry);

template<TYPES T, class V>
refId symbolArray::insert(const std::string &key, const V &val)
{
	auto &id = symbolRefs[key];

	if (id) return id;

	push_back(std::move(entry(T, new V(val))));
	return id = size() - 1;
}

template<>
refId symbolArray::insert<TYPES::NUMBER, WholeNumber>(const std::string &key, const WholeNumber &n)
{
	auto id = symbolRefs[key];

	if (id) return id;

	push_back(std::move(entry(TYPES::NUMBER, n)));
	return id = size() - 1;
}