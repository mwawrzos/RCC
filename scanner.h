#pragma once
#include "grammar.h"
#include "symbolArray.h"

bool myIsalnum(int c) { return isalnum(c) || c == '_'; }

template <int resultSize = 8096>
class scanner
{
public:
	typedef std::array<std::tuple<ID, refId>, resultSize> resArray;

	std::unique_ptr<resArray> scan();

	scanner(std::streambuf &input, symbolArray *sa);
virtual ~scanner();

private:
	bool takeChar() { return (cc = input.sbumpc()) != EOF && ++column; }
	void putVal(ID id, refId val = -1) { (*result)[index] = std::move(std::make_tuple(id, val)); }

	template <typename SeqEl, typename Flush, typename Conv>
	void readSequence(SeqEl se, Flush f, Conv c = [] {});

	void readAlnum()
	{
		readSequence(
			myIsalnum,
			[this] { putVal(ID::REF, sa->insert<TYPES::LABEL>(key, Label(key))); }, [] {}
		);
	}

	unsigned int line = 1, column = 0;

	std::streambuf &input;
	std::unique_ptr<resArray> result;
	unsigned int index;

	symbolArray *sa;
#define dsa (*sa)

	std::string key;
	std::streambuf::int_type cc;
};

template<int resultSize>
std::unique_ptr<typename scanner<resultSize>::resArray> scanner<resultSize>::scan()
{
	result = std::make_unique<resArray>();
	index = 0;

	while (takeChar() && index < resultSize)
	{
		switch (cc)
		{
			case '+': putVal(ID::PLUS);  break;
			case '-': putVal(ID::MINUS); break;
			case '*': putVal(ID::MUL);   break;
			case '/': putVal(ID::DIV);   break;
			case '%': putVal(ID::MOD);   break;
			case '(': putVal(ID::OPAR);  break;
			case ')': putVal(ID::CPAR);  break;
			case '#': 
			case '$':
			case '@':
			case '<':
			case '>': putVal(ID::MODE, cc); break;
			case 'e': 
				if (takeChar() && myIsalnum(cc))
				{
					key = 'e';
					readAlnum();
				}
				else
				{
					input.sputbackc(static_cast<char>(cc));
					putVal(ID::MODE, 'e');
				}
				break;
			default :
				if (isdigit(cc))
				{
					int number = 0;

					auto convert = [&number, this]
					{
						if (number < 0)
							return;

						char digit = static_cast<char>(cc) - '0';
						const auto max = std::numeric_limits<int>::max();
						if (number <= max / 10 - (max % 10 < digit))
							number = 10 * number + digit;
						else
							number = -1;
					};
					auto flush = [&number, this]
					{
						if (number > 0)
							putVal(ID::REF, sa->insert<TYPES::NUMBER>(key, WholeNumber(number)));
						else
						{
							auto &errorMsg = e.mkError(ERROR::CONSTANT_TOO_BIG, key, line, column);
							putVal(ID::REF, sa->insert<TYPES::ERROR>(key, errorMsg));
						}
					};

					readSequence(isdigit, flush, convert);
					break;
				}
				if (isalpha(cc) || cc == '_')
				{
					readAlnum();
					break;
				}
		}
		++index;
	}

	putVal(ID::FE);

	return std::move(result);
}

template <int resultSize>
scanner<resultSize>::scanner(std::streambuf &input, symbolArray *sa) 
	: input(input)
	, sa(sa)
{
	key.reserve(20);
}


template <int resultSize>
scanner<resultSize>::~scanner()
{
}

template<int resultSize>
template <typename SeqEl, typename Flush, typename Conv>
inline void scanner<resultSize>::readSequence(SeqEl se, Flush f, Conv c)
{
	do
	{
		key += static_cast<char>(cc);

		c();
	} while (takeChar() && se(cc));
	if (cc != EOF)
		input.sputbackc(static_cast<char>(cc));

	f();

	column += key.length() - 1;
	key = "";
}
