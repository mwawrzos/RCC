#pragma once
#include "grammar.h"
#include "symbolArray.h"

bool myIsalnum(int c) { 
	auto res = isalnum(c) || c == '_';
	return res;
}

//   _  __   _                      
//  / ) )_) / `    _ _   _   _  _ _ 
// (_/ /   (_.    ) ) ) (_( (_ ) (_)
#define genOPC(A, KEY, NAlN, ...)							\
case #@A:													\
{															\
	KEY;													\
	if (takeChar() && myIsalnum(cc))						\
		switch (toupper(cc))								\
		{													\
			__VA_ARGS__										\
						 									\
		default:											\
			readAlnum();									\
		}													\
	else													\
	{														\
		putBack();											\
		putVal(ID::REF, sa->insert<TYPES::LABEL>(key, key));\
	}														\
	break;													\
}
// starnard OPC
#define OPC(A, ...) genOPC(A, keyInitClear, readAlnum();, __VA_ARGS__)
// nested OPC
#define nOPC(A, ...) genOPC(A, key += static_cast<char>(cc), readAlnum();, __VA_ARGS__)
// is whole OPC?
#define wOPC(A,B,C) 										\
if (!takeChar() || !myIsalnum(cc))							\
{															\
	putBack();												\
	putVal(ID::OPC, static_cast<refId>(Opcode::A##B##C));	\
}															\
else														\
	readAlnum();

#define OPCs(A,B,C) 										\
case #@B:													\
	key += static_cast<char>(cc);							\
	if (takeChar() && toupper(cc) == #@C)					\
	{														\
		key += static_cast<char>(cc);						\
		wOPC(A,B,C)											\
	}														\
	else													\
		readAlnum();										\
	break;
//   _  __   _                          ___      __  
//  / ) )_) / `    _ _   _   _  _ _     )_  )\ ) ) ) 
// (_/ /   (_.    ) ) ) (_( (_ ) (_)   (__ (  ( /_/  
                                                  
                                                                            

template <int resultSize = 8096>
class scanner
{
public:
	typedef std::array<std::tuple<ID, refId>, resultSize> resArray;

	std::unique_ptr<resArray> scan();

	scanner(std::streambuf &i, symbolArray *sa);
virtual ~scanner();

private:
	bool takeChar() { return cc = input.sbumpc(), ++column, true; }
	void putBack() { if(cc != EOF) --column, input.sputbackc(static_cast<char>(cc)); }
	void putVal(ID id, refId val = -1) { (*result)[index] = std::move(std::make_tuple(id, val)); }
	void putMod(Modifier mod) { putVal(ID::MODYFIERS, static_cast<refId>(mod)); }

	template <typename SeqEl, typename Flush, typename Conv>
	void readSequence(SeqEl se, Flush f, Conv c = [] {});

	void readAlnum()
	{
		readSequence(
			myIsalnum,
			[this] { putVal(ID::REF, sa->insert<TYPES::LABEL>(key, key)); }, [] {}
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

#define keyInitClear ScopeGuard kg([this] { key = ""; }, [this] { key = static_cast<char>(cc); });
#define keyClear ScopeGuard kg([this] { key = ""; });

template<int resultSize>
std::unique_ptr<typename scanner<resultSize>::resArray> scanner<resultSize>::scan()
{
	if (takeChar() && cc != EOF)
		putBack();
	else
		return nullptr;

	result = std::make_unique<resArray>();
	index = 0;

	bool eof = false;
	while (takeChar() && index < resultSize - 1 && !eof)
	{
		switch (toupper(cc))
		{
			case '+': putVal(ID::PLUS);  break;
			case '-': putVal(ID::MINUS); break;
			case '*': putVal(ID::MUL);   break;
			case '/': putVal(ID::DIV);   break;
			case '%': putVal(ID::MOD);   break;
			case '(': putVal(ID::OPAR);  break;
			case ')': putVal(ID::CPAR);  break;
			case ',': putVal(ID::COMMA); break;
			case '#': 
			case '$':
			case '@':
			case '<':
			case '>': putVal(ID::MODE, cc); break;

			case '.':
#pragma region
				if(takeChar())
					switch (toupper(cc))
					{
					case 'F': putMod(Modifier::F); break;
					case 'I': putMod(Modifier::I); break;
					case 'X': putMod(Modifier::X); break;
					case 'A':
						if (takeChar() && toupper(cc) == 'B')
							putMod(Modifier::AB);
						else
						{
							putBack();
							putMod(Modifier::A);
						}
						break;
					case 'B':
						if (takeChar() && toupper(cc) == 'A')
							putMod(Modifier::BA);
						else
						{
							putBack();
							putMod(Modifier::B);
						}
						break;
					default:
						putBack();
						const auto &errorMsg = e.mkError(ERROR::MODIFIER_EXPECTED, std::string(), line, column - 1);
						putVal(ID::REF, sa->insert<TYPES::ERROR>(std::get<0>(errorMsg), errorMsg));
					}
				else
				{
					const auto &errorMsg = e.mkError(ERROR::MODIFIER_EXPECTED, std::string(), line, column - 1);
					putVal(ID::REF, sa->insert<TYPES::ERROR>(std::get<0>(errorMsg), errorMsg));
				}
				break;
#pragma endregion
			case ' ':
			case '\t': putVal(ID::WSPC, static_cast<char>(cc)); break;
				
			case EOF: eof = true;
			case '\n':
				if (takeChar() && cc != '\r')
					putBack();
			case '\r':
				if (takeChar() && cc != '\n')
					putBack();
				
				column = 0;
				++line;

				putVal(ID::REF, sa->insert<TYPES::COMMENT>(std::string("\n"), std::string()));
				break;
			case ';':
			{
				keyClear;
				readSequence(
					[](int c) { return c != '\n' && c != '\r' && c != EOF; },
					[this] { putVal(ID::REF, sa->insert<TYPES::COMMENT>(key, key)); },
					[] {}
				);
				if (!takeChar())
					eof = true;

				column = 0;
				++line;

				break;
			}
				

//    _  __   _                      
//   / ) )_) / `   ( _   ) _   _ ( _ 
//  (_/ /   (_.     )_) ( (_) (_  )\ 
                                 
				OPC(A, OPCs(A, D, D););
				OPC(C, OPCs(C, M, P););
				OPC(O, OPCs(O, R, G););
				OPC(E, OPCs(E, Q, U); OPCs(E, N, D));
				OPC(D, OPCs(D, A, T); OPCs(D, I, V); OPCs(D, J, N););
				OPC(M, nOPC(O, case 'V': wOPC(M, O, V) break; case 'D': wOPC(M, O, D) break;); OPCs(M, U, L););
				OPC(S, OPCs(S, U, B); OPCs(S, L, T); OPCs(S, P, L););
				OPC(J, nOPC(M, case 'P': wOPC(J, M, P) break; case 'Z': wOPC(J, M, Z) break; case 'N': OPCs(J, M, N) break;));
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
						if (number >= 0)
							putVal(ID::REF, sa->insert<TYPES::NUMBER>(key, WholeNumber(number)));
						else
						{
							const auto &errorMsg = e.mkError(ERROR::CONSTANT_TOO_BIG, key, line, column - key.length());
							putVal(ID::REF, sa->insert<TYPES::ERROR>(std::get<0>(errorMsg), errorMsg));
						}
					};

					keyClear;
					readSequence(isdigit, flush, convert);
					break;
				}
				if (isalpha(cc) || cc == '_')
				{
					keyClear;
					readAlnum();
					break;
				}
				auto errorMsg = e.mkError(ERROR::SYNTAX_ERROR, std::string(1, static_cast<char>(cc)), line, column - 1);
				putVal(ID::REF, sa->insert<TYPES::ERROR>(std::get<0>(errorMsg), errorMsg));
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
		if (cc != EOF)
			key += static_cast<char>(cc);

		c();
	} while (takeChar() && se(cc));
	putBack();

	f();
}
