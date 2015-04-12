#include "stdafx.h"

#include "grammar.h"
#include "error.h"
#include "scanner.h"

std::vector<entry>  symbolArrayy;
Error e;

int main(int argc, char **argv)
{

	if (argc != 2)
	{
		std::cout << "U¿ycie: " << argv[0] << " redCodeFile";
		system("pause>>null");
		return 1;
	}
	std::cout << isalnum('a') << isalnum('1') << isalnum('_') << isalnum('b') << isalnum('2') << std::endl;

	std::filebuf buf;
	buf.open(argv[1], std::ios::in);
	e.file = argv[1];

	auto sa = std::make_unique<symbolArray>();

	scanner<> a(buf, sa.get());
	auto chunk = a.scan();

	for (auto t : *chunk)
	{
		switch (std::get<0>(t))
		{
		case ID::DIV: std::cout << "/"; break;
		case ID::MUL: std::cout << "*"; break;
		case ID::PLUS: std::cout << "+"; break;
		case ID::MINUS: std::cout << "-"; break;
		case ID::MOD: std::cout << "%"; break;
		case ID::CPAR: std::cout << ")"; break;
		case ID::OPAR: std::cout << "("; break;
		case ID::REF: std::cout << (*sa)[std::get<1>(t)]; break;
		case ID::MODE: std::cout << (char)(std::get<1>(t)); break;
		case ID::FE: goto cont;
		default: std::cout << "Implement me: " << (int)std::get<0>(t);
		}
		std::cout << std::endl;
	}
	cont:

	auto start = std::chrono::high_resolution_clock::now();
	while (buf.sbumpc() != EOF);
	auto test1 = std::chrono::high_resolution_clock::now() - start;
	system("pause>>null");
}