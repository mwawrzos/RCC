#include "stdafx.h"

#include "grammar.h"
#include "error.h"

union Val {
	WholeNumber number;
	Token *token;
	std::string *string;

	Val(WholeNumber number) : number(number) {}
	Val(Token *token ) : token(token) {}
	Val(std::string *string) : string(string) {}
};

typedef Val val;
typedef size_t refId;
typedef std::tuple<TYPES, val> entry;
typedef std::unordered_map<std::string, refId> refMap;

refMap symbolRefs;
std::vector<entry>  symbolArray;
Error e;

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
		out << "Implement me for " << (int)std::get<0>(entry);
	}
	return out;
}
std::ostream &operator<<(std::ostream &out, const std::tuple<ID, refId> t)
{
	switch (std::get<0>(t))
	{
	case ID::DIV: out << "/"; break;
	case ID::MUL: out << "*"; break;
	case ID::PLUS: out << "+"; break;
	case ID::MINUS: out << "-"; break;
	case ID::MOD: out << "%"; break;
	case ID::REF: out << symbolArray[std::get<1>(t)];
	}

	return out;
}

void scan(std::streambuf &input);

template<TYPES T, class V>
refId insert(const std::string &key, V &val)
{
	auto id = symbolRefs[key];

	if (id) return id;

	symbolArray.push_back(std::move(entry(T, new V(val))));
	return id = symbolArray.size() - 1;
}
template<>
refId insert<TYPES::NUMBER, int>(const std::string &key, int &n)
{
	auto id = symbolRefs[key];

	if (id) return id;

	symbolArray.push_back(std::move(entry(TYPES::NUMBER, n)));
	return id = symbolArray.size() - 1;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "U¿ycie: " << argv[0] << " redCodeFile";
		system("pause>>null");
		return 1;
	}
	std::cout << std::numeric_limits<int>::max() << std::endl;

	std::filebuf buf;
	buf.open(argv[1], std::ios::in);
	e.file = argv[1];

	scan(buf);

	auto start = std::chrono::high_resolution_clock::now();
	while (buf.sbumpc() != EOF);
	auto test1 = std::chrono::high_resolution_clock::now() - start;
	system("pause>>null");
}

#define setConst(ID) result[i] = std::make_tuple(ID, -1); break;

void scan(std::streambuf &input)
{
	const int resultSize = 8096;
	std::tuple<ID, refId> result[resultSize];

	unsigned int line = 1, column = 0;
	std::streambuf::int_type a;
	unsigned int i = 0;
	std::string key;
	key.reserve(20);
	while ((a = input.sbumpc()) != EOF && i < resultSize)
	{
		++column;
		switch (a)
		{
		case '+': setConst(ID::PLUS);
		case '-': setConst(ID::MINUS);
		case '*': setConst(ID::MUL);
		case '/': setConst(ID::DIV);
		case '%': setConst(ID::MOD);
		default:
			if (isdigit(a))
			{
				int number = 0;
				char digit;
				do
				{
					key += a;

					if (number < 0)
						continue;

					digit = a - '0';
					const auto max = std::numeric_limits<int>::max();
					if (number <= max / 10 - (max % 10 < digit))
						number = 10 * number + digit;
					else
						number = -1;

				} while ((a = input.sbumpc()) != EOF && isdigit(a));
				if (a != EOF)
					input.sputbackc(a);

				if (number > 0)
					result[i] = std::move(std::make_tuple(ID::REF, insert<TYPES::NUMBER>(key, number)));
				else
				{
					auto &errorMsg = e.mkError(ERROR::CONSTANT_TOO_BIG, key, line, column);
					result[i] = std::move(std::make_tuple(ID::REF, insert<TYPES::ERROR>(key, errorMsg)));
				}

				column += key.length() - 1;
				key = "";

				break;
			}
			if (isalpha(a))
			{
				do
				{
					key += a;
				} while ((a = input.sbumpc()) != EOF && isalnum(a));
				if (a != EOF)
					input.sputbackc(a);

				result[i] = std::move(std::make_tuple(ID::REF, insert<TYPES::LABEL>(key, key)));

				column += key.length();
				key = "";

				break;
			}
		}
		++i;
	}

	std::cout << std::endl;
	while(i --> 0)
		std::cout << i << ": " << result[i] << std::endl;
}