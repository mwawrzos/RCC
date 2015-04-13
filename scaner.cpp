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
		case ID::MODYFIERS:
			switch (static_cast<Modifier>(std::get<1>(t)))
			{
			case Modifier::A: std::cout << ".A"; break;
			case Modifier::AB: std::cout << ".AB"; break;
			case Modifier::B: std::cout << ".B"; break;
			case Modifier::BA: std::cout << ".BA"; break;
			case Modifier::X: std::cout << ".X"; break;
			case Modifier::I: std::cout << ".I"; break;
			case Modifier::F: std::cout << ".F";
			}
			break;
		case ID::OPC:
			switch (static_cast<Opcode>(std::get<1>(t)))
			{
			case Opcode::DAT: std::cout << "DAT"; break;
			case Opcode::MOV: std::cout << "MOV"; break;
			case Opcode::ADD: std::cout << "ADD"; break;
			case Opcode::SUB: std::cout << "SUB"; break;
			case Opcode::MUL: std::cout << "MUL"; break;
			case Opcode::DIV: std::cout << "DIV"; break;
			case Opcode::MOD: std::cout << "MOD"; break;
			case Opcode::JMP: std::cout << "JMP"; break;
			case Opcode::JMZ: std::cout << "JMZ"; break;
			case Opcode::JMN: std::cout << "JMN"; break;
			case Opcode::DJN: std::cout << "DJN"; break;
			case Opcode::CMP: std::cout << "CMP"; break;
			case Opcode::SLT: std::cout << "STL"; break;
			case Opcode::SPL: std::cout << "SPL"; break;
			case Opcode::ORG: std::cout << "ORG"; break;
			case Opcode::EQU: std::cout << "EQU"; break;
			case Opcode::END: std::cout << "END"; break;
			}
			break;
		case ID::FE: goto cont;
		default: std::cout << "Implement me (main): " << (int)std::get<0>(t);
		}
		std::cout << std::endl;
	}
	cont:

	auto start = std::chrono::high_resolution_clock::now();
	while (buf.sbumpc() != EOF);
	auto test1 = std::chrono::high_resolution_clock::now() - start;
	system("pause>>null");
}