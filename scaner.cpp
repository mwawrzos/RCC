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

	std::filebuf buf;
	buf.open(argv[1], std::ios::in);
	e.file = argv[1];

	auto sa = std::make_unique<symbolArray>();

	scanner<> a(buf, sa.get());

#define Gen(ARG, CLASS, VAL) case ARG: std::cout << "<span class=\"" #CLASS "\">" << VAL << "</span>"; break;

	std::cout <<
		R"(<!DOCTYPE html>
<html>
<body>
<pre>
<style>

span.operator {
	color: goldenrod;
}

span.label {
	color: dodgerblue
}

span.opc {
	color: black;
	font-weight: bold;
}

span.comment {
	color: green;
}

span.number {
	color: mediumblue;
}

span.mode {
	color: chocolate
}

span.prarent {
	color: mediumblue
}

span.modifier {
	color: indigo
}

abbr {
	color: red
}
</style>
)";

	std::unique_ptr<scanner<>::resArray> chunk;

	while ((chunk = a.scan()))
	{
		for (auto t : *chunk)
		{
			switch (std::get<0>(t))
			{
			case ID::REF: std::cout << (*sa)[std::get<1>(t)]; break;
				Gen(ID::DIV, operator, "\\");
				Gen(ID::MUL, operator, "*");
				Gen(ID::PLUS, operator, "+");
				Gen(ID::MINUS, operator, "-");
				Gen(ID::MOD, operator, "%");
				Gen(ID::CPAR, prarent, ")");
				Gen(ID::OPAR, prarent, "(");
				Gen(ID::COMMA, prarent, ",");
			case ID::MODE:
			{
				std::string mode;
				switch (std::get<1>(t))
				{
				case '<': mode = "&lt;"; break;
				case '>': mode = "&gt;"; break;
				case '#': mode = "#";	 break;
				case '$': mode = "$";	 break;
				case '@': mode = "@";	 break;
				}
				std::cout << R"(<span class="mode">)" << mode << "</span>";
				break;
			}
			Gen(ID::WSPC, wspc, (char) (std::get<1>(t)));
			case ID::MODYFIERS:
				switch (static_cast<Modifier>(std::get<1>(t)))
				{
					Gen(Modifier::A, modifier, ".A");
					Gen(Modifier::AB, modifier, ".AB");
					Gen(Modifier::B, modifier, ".B");
					Gen(Modifier::BA, modifier, ".BA");
					Gen(Modifier::X, modifier, ".X");
					Gen(Modifier::I, modifier, ".I");
					Gen(Modifier::F, modifier, ".F");
				}
				break;
			case ID::OPC:
				switch (static_cast<Opcode>(std::get<1>(t)))
				{
					Gen(Opcode::DAT, opc, "DAT");
					Gen(Opcode::MOV, opc, "MOV");
					Gen(Opcode::ADD, opc, "ADD");
					Gen(Opcode::SUB, opc, "SUB");
					Gen(Opcode::MUL, opc, "MUL");
					Gen(Opcode::DIV, opc, "DIV");
					Gen(Opcode::MOD, opc, "MOD");
					Gen(Opcode::JMP, opc, "JMP");
					Gen(Opcode::JMZ, opc, "JMZ");
					Gen(Opcode::JMN, opc, "JMN");
					Gen(Opcode::DJN, opc, "DJN");
					Gen(Opcode::CMP, opc, "CMP");
					Gen(Opcode::SLT, opc, "STL");
					Gen(Opcode::SPL, opc, "SPL");
					Gen(Opcode::ORG, opc, "ORG");
					Gen(Opcode::EQU, opc, "EQU");
					Gen(Opcode::END, opc, "END");
				}
				break;
			case ID::FE: goto cont;
			default: std::cout << "Implement me (main): " << (int) std::get<0>(t);
			}
		}
	cont:;
	}
	std::cout <<
R"(</pre>
</body>
</html>
)";

	system("pause>>null");
}