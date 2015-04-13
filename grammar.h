#pragma once

enum class ID
{
	OPC,
	REF,
	PLUS,
	MINUS,
	MUL,
	DIV,
	MOD,
	OPAR,
	CPAR,
	MODE,
	FE,
	MODYFIERS
};

enum class TYPES
{
	NUMBER,
	LABEL,
	ERROR,
	MODE
};

#define MOV_CONSTR(A, B, FIELD) public : A(std::unique_ptr<B> &arg) : FIELD(std::move(arg)) {}
#define FIELD(T1, T2, NAME) std::unique_ptr<T2> NAME; MOV_CONSTR(T1, T2, NAME)

#define TYPE(NAME) class NAME { protected: NAME(); };
#define CONST(TYPE, CONT, NAME) class TYPE##NAME { FIELD(TYPE##NAME, CONT, val); };

class Token {};

typedef int WholeNumber;

class Number
{
	WholeNumber value;
public:
	Number(unsigned int value);
};
class PositiveNumber : Number 
{
public:
	PositiveNumber(unsigned int val) : Number(val) {}
};
class NegativeNumber : Number
{
	NegativeNumber(unsigned int val) : Number(val) {}
};

typedef std::string Label;
TYPE(Term);

class Expr
{
protected:
	Expr(Term &a) : a(a) {}
	Term a;
};
class ExprOp : public Expr
{
protected:
	ExprOp(Term &a, std::unique_ptr<Expr> &b) : Expr(a), b(std::move(b)) {}
	std::unique_ptr<Expr> b;
};
#define EXPR(OP) class Expr##OP : public ExprOp { public: Expr##OP(Term &a, std::unique_ptr<Expr> &b) : ExprOp(a, b) {} };
EXPR(Add)
EXPR(Sub)
EXPR(Mul)
EXPR(Div)
EXPR(Mod)

CONST(Term, Label, Label);
CONST(Term, Number, Number);
CONST(Term, Expr, Parenthies);

enum class Mode
{
	HASH, DOLLAR, AT, LT, GT, E
};

enum class Modifier
{
	A, B, AB, BA, F, X, I
};

enum class Opcode
{
	DAT, MOV, ADD, SUB, MUL, DIV, MOD,
	JMP, JMZ, JMN, DJN, CMP, SLT, SPL,
	ORG, EQU, END
};

class Operation
{
protected:
	Opcode opc;
public:
	Operation(Opcode & opc) : opc(opc) {}
};
class OperationMod : public Operation
{
	Modifier mod;
public:
	OperationMod(Opcode & opc, Modifier &mod) : Operation(opc), mod(mod) {}
};

class LabelList {};
class LabelListNonEmpty : public LabelList
{
	FIELD(LabelListNonEmpty, Label, label)
};
class LabelListNonEmptyNode : public LabelList
{
	std::unique_ptr<Label> label;
	std::unique_ptr<LabelList> next;
public:
	LabelListNonEmptyNode(std::unique_ptr<Label> label,
		std::unique_ptr<LabelList> next)
		:
		label(std::move(label)),
		next(std::move(next)) {}
};

class Comment {};
class CommentNonEmpty : public Comment
{
	std::string content;
public:
	CommentNonEmpty(std::string &&content);
};

class Instruction
{
protected:
	std::unique_ptr<LabelList> labelList;
	std::unique_ptr<Operation> operation;
	std::unique_ptr<Comment> comment;
	Instruction(std::unique_ptr<LabelList> &labelList,
		std::unique_ptr<Operation> &operation,
		std::unique_ptr<Comment>   &comment)
		:
		labelList(std::move(labelList)),
		operation(std::move(operation)),
		comment(std::move(comment)) {}
};
class InstructionTwoArg : public Instruction
{
	Mode                  mode1;
	std::unique_ptr<Expr> expr1;
	Mode                  mode2;
	std::unique_ptr<Expr> expr2;
public:
	InstructionTwoArg(std::unique_ptr<LabelList> &labelList,
		std::unique_ptr<Operation> &operation,
		Mode                       &mode1,
		std::unique_ptr<Expr>      &expr1,
		Mode                       &mode2,
		std::unique_ptr<Expr>      &expr2,
		std::unique_ptr<Comment>   &comment)
		:
		Instruction(labelList, operation, comment),
		mode1(std::move(mode1)),
		expr1(std::move(expr1)),
		mode2(std::move(mode2)),
		expr2(std::move(expr2)) {}
};
class InstructionOneArg : public Instruction
{
	Mode                  mode;
	std::unique_ptr<Expr> field;
public:
	InstructionOneArg(std::unique_ptr<LabelList> &labelList,
		std::unique_ptr<Operation> &operation,
		Mode                       &mode,
		std::unique_ptr<Expr>      &field,
		std::unique_ptr<Comment>   &comment)
		:
		Instruction(labelList, operation, comment),
		mode(mode),
		field(std::move(field)) {}
};

class Line
{
protected:
	Line();
};
class LineCmnt : public Line
{
	FIELD(LineCmnt, Comment, comment)
};
class LineInstr : public Line
{
	FIELD(LineInstr, Instruction, instruction)
};

class List
{
protected:
	FIELD(List, Line, line)
};
class ListNode : public List
{
	std::unique_ptr<List> list;
public:
	ListNode(std::unique_ptr<Line> &content, std::unique_ptr<List> &next) : List(content), list(std::move(next)) {}
};

class AssemblyFile
{
	FIELD(AssemblyFile, List, list)
};