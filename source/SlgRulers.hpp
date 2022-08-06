#pragma once
#include <iostream>
#include <unordered_set>
#include <unordered_map>

class SlgRuleTable {
public:
	enum class e_SlgTokenType {
		ERROR = 0x0000,
		NULLVALUE = 0x0100,

		NUMBER = 0x0400,
		INT,
		STRING,
		FLOAT,

		IDENTIFIER = 0x0800,

		KEYWORD = 0x8000,
		BRACKET = 0x4000,
		OPERATOR = 0x2000,
		STATEMENTOPERATOR = 0x1000
	};

	enum class e_SlgTokenDesc {
		UNKNOWN = 0x0000,

		INT = 0x0401,
		STRING,
		FLOAT,

		COMMA = 0x1001,		// ,
		MEMBERACCESS,		// .
		MODULEACCESS,		// ::
		END,			// ;		
		ATTRIBUTE,		// @
		TYPE_DECLAR,		// :
		SELFADD,		// ++ 自增自减不能用在运算表达中, 它属于语句运算符
		SELFSUB,		// --

		BinaryOperator = 0x2200, // 二元运算符
		ADD,		// +
		SUB,		// - 
		MUL,		// *
		DIV,		// /
		MOD,		// %
		AND,		// &
		OR,		// |
		SMALLER,	// <
		LARGER,		// >
		LSHIFT,		// <<
		RSHIFT,		// >>
		XOR,		// ^
		LOGICAND,	// &&
		LOGICOR,	// ||
		NOTEQUAL,	// !=
		SMALLEREQUAL,   // <=
		LARGEREQUAL,    // >=
		ISEQUAL,	// ==

		UnaryOperator = 0x2400, // 一元运算符
		NOT,			// ! 
		INVERT,			// ~
		NEGATIVE,		// -

		AssignmentOperator = 0x2800, // 只用于变量  赋值运算符
		EQUAL,		// = 
		ADDEQUAL,	// +=
		SUBEQUAL,	// -=
		MULEQUAL,	// *=
		DIVEQUAL,	// /=
		MODEQUAL,	// %=
		ANDEQUAL,	// &=
		OREQUAL,	// |=
		XOREQUAL,	// ^=
		LSHIFTEQUAL,	// <<=
		RSHIFTEQUAL,	// >>=
		INVERTEQUAL,	// ~=


		BRACKET_CURLY_START = 0x4001,	// {
		BRACKET_CURLY_END,		// }
		BRACKET_ROUND_START,		// (
		BRACKET_ROUND_END,		// )
		BRACKET_SQUARE_START,		// [
		BRACKET_SQUARE_END,		// ]

		IDENTIFIER = 0x0800,		// identifier
		KEYWORLD_using = 0x8001,
		KEYWORLD_let,
		KEYWORLD_module,
		KEYWORLD_export,
		KEYWORLD_function,
		KEYWORLD_extern,
		KEYWORLD_if,
		KEYWORLD_else,
		KEYWORLD_return,
		KEYWORLD_switch,
		KEYWORLD_case,
		KEYWORLD_break,
		KEYWORLD_for,
		KEYWORLD_continue,
		KEYWORLD_null,
		KEYWORLD_in,
		KEYWORLD_ref,
		KEYWORLD_attribute,
	};

	enum class e_SlgErrorNumber {
		ERROR_UNKNOWN = 0,
		ERROR_ILLEGAL_LINE_CHANGE,
		ERROR_UNEXPECTED_TOKEN,
		ERROR_MEANINGLESS_STATEMENT,
		ERROR_UNEXPECTED_TYPE,
		ERROR_UNEXPECTED_END,
		ERROR_UNEXPECTED_ARGUMENT_COUNT,
		ERROR_ILLEGAL_KEYWORD_USING_POSITION,
		ERROR_DOUBLE_DEFINE,
		ERROR_OPERATOR_CONTENT,
		ERROR_UNEXPECTED_BRACKETS,
	};

	std::unordered_map<char, e_SlgTokenDesc> Brackets{
		{'{', e_SlgTokenDesc::BRACKET_CURLY_START},
		{'}', e_SlgTokenDesc::BRACKET_CURLY_END},
		{'(', e_SlgTokenDesc::BRACKET_ROUND_START},
		{')', e_SlgTokenDesc::BRACKET_ROUND_END},
		{'[', e_SlgTokenDesc::BRACKET_SQUARE_START},
		{']', e_SlgTokenDesc::BRACKET_SQUARE_END},
	};

	std::unordered_map<std::string, e_SlgTokenDesc> KeyWord{

		{"using",	e_SlgTokenDesc::KEYWORLD_using},
		{"let",		e_SlgTokenDesc::KEYWORLD_let},
		{"module",	e_SlgTokenDesc::KEYWORLD_module},
		{"export",	e_SlgTokenDesc::KEYWORLD_export},
		{"function",	e_SlgTokenDesc::KEYWORLD_function},
		{"extern",	e_SlgTokenDesc::KEYWORLD_extern},
		{"if",		e_SlgTokenDesc::KEYWORLD_if},
		{"else",	e_SlgTokenDesc::KEYWORLD_else},
		{"return",	e_SlgTokenDesc::KEYWORLD_return},
		{"switch",	e_SlgTokenDesc::KEYWORLD_switch},
		{"case",	e_SlgTokenDesc::KEYWORLD_case},
		{"for",		e_SlgTokenDesc::KEYWORLD_for},
		{"break",	e_SlgTokenDesc::KEYWORLD_break},
		{"continue",	e_SlgTokenDesc::KEYWORLD_continue},
		{"null",	e_SlgTokenDesc::KEYWORLD_null},
		{"in",		e_SlgTokenDesc::KEYWORLD_in},
		{"ref",		e_SlgTokenDesc::KEYWORLD_ref},
		{"attribute",	e_SlgTokenDesc::KEYWORLD_attribute},
	};

	std::unordered_map<char, e_SlgTokenDesc> OperatorsSingle{
		{'+', e_SlgTokenDesc::ADD},
		{'-', e_SlgTokenDesc::SUB},
		{'*', e_SlgTokenDesc::MUL},
		{'/', e_SlgTokenDesc::DIV},
		{'%', e_SlgTokenDesc::MOD},
		{'=', e_SlgTokenDesc::EQUAL},
		{'&', e_SlgTokenDesc::AND},
		{'|', e_SlgTokenDesc::OR},
		{'!', e_SlgTokenDesc::NOT},
		{'<', e_SlgTokenDesc::SMALLER},
		{'>', e_SlgTokenDesc::LARGER},
		{'^', e_SlgTokenDesc::XOR},
		{'~', e_SlgTokenDesc::INVERT},
	};

	std::unordered_map<std::string, e_SlgTokenDesc> OperatorsMulti{
		{"++", e_SlgTokenDesc::SELFADD},
		{"--", e_SlgTokenDesc::SELFSUB},
		{"&&", e_SlgTokenDesc::LOGICAND},
		{"||", e_SlgTokenDesc::LOGICOR},
		{"+=", e_SlgTokenDesc::ADDEQUAL},
		{"-=", e_SlgTokenDesc::ISEQUAL},
		{"*=", e_SlgTokenDesc::MULEQUAL},
		{"/=", e_SlgTokenDesc::DIVEQUAL},
		{"%=", e_SlgTokenDesc::MODEQUAL},
		{"==", e_SlgTokenDesc::ISEQUAL},
		{"&=", e_SlgTokenDesc::ANDEQUAL},
		{"|=", e_SlgTokenDesc::OREQUAL},
		{"!=", e_SlgTokenDesc::NOTEQUAL},
		{"<=", e_SlgTokenDesc::SMALLEREQUAL},
		{">=", e_SlgTokenDesc::LARGEREQUAL},
		{"^=", e_SlgTokenDesc::XOREQUAL},
		{"<<", e_SlgTokenDesc::LSHIFT},
		{">>", e_SlgTokenDesc::RSHIFT},
		{"<<=", e_SlgTokenDesc::LSHIFTEQUAL},
		{">>=", e_SlgTokenDesc::RSHIFTEQUAL},
		{"~=",  e_SlgTokenDesc::INVERTEQUAL}
	};

	std::unordered_map<e_SlgTokenDesc, int> OperatorsPriority{

		{ e_SlgTokenDesc::INVERT,               14 },
		{ e_SlgTokenDesc::NOT,                  14 },

		{ e_SlgTokenDesc::MOD,                  13 },

		{ e_SlgTokenDesc::MUL,                  12 },
		{ e_SlgTokenDesc::DIV,                  12 },


		{ e_SlgTokenDesc::ADD,                  11 },
		{ e_SlgTokenDesc::SUB,                  11 },

		{ e_SlgTokenDesc::LSHIFT,               10 },
		{ e_SlgTokenDesc::RSHIFT,               10 },

		//大小比较
		{ e_SlgTokenDesc::SMALLER,              9 },
		{ e_SlgTokenDesc::LARGER,               9 },
		{ e_SlgTokenDesc::SMALLEREQUAL,         9 },
		{ e_SlgTokenDesc::LARGEREQUAL,          9 },

		//等于比较
		{ e_SlgTokenDesc::ISEQUAL,              8 },
		{ e_SlgTokenDesc::NOTEQUAL,             7 },

		//位与
		{ e_SlgTokenDesc::AND,                  6 },
		//异或
		{ e_SlgTokenDesc::XOR,                  5 },
		//位或
		{ e_SlgTokenDesc::OR,                   4 },
		//逻辑与
		{ e_SlgTokenDesc::LOGICAND,             3 },
		//逻辑或
		{ e_SlgTokenDesc::LOGICOR,              2 },

		//赋值运算符
		{ e_SlgTokenDesc::EQUAL,                1 },
		{ e_SlgTokenDesc::ADDEQUAL,             1 },
		{ e_SlgTokenDesc::SUBEQUAL,             1 },
		{ e_SlgTokenDesc::MULEQUAL,             1 },
		{ e_SlgTokenDesc::DIVEQUAL,             1 },
		{ e_SlgTokenDesc::MODEQUAL,             1 },
		{ e_SlgTokenDesc::ANDEQUAL,             1 },
		{ e_SlgTokenDesc::OREQUAL,              1 },
		{ e_SlgTokenDesc::XOREQUAL,             1 },
		{ e_SlgTokenDesc::INVERTEQUAL,          1 },
		{ e_SlgTokenDesc::LSHIFTEQUAL,		1 },
		{ e_SlgTokenDesc::RSHIFTEQUAL,		1 },
	};

	std::unordered_map<char, e_SlgTokenDesc> StatementOpeartor{
		{'.', e_SlgTokenDesc::MEMBERACCESS},
		{',', e_SlgTokenDesc::COMMA},
		{';', e_SlgTokenDesc::END},
		{':', e_SlgTokenDesc::TYPE_DECLAR},
		{'@', e_SlgTokenDesc::ATTRIBUTE}
	};
};


std::string TypeDescString(SlgRuleTable::e_SlgTokenDesc desc)
{
	switch (desc)
	{
	case SlgRuleTable::e_SlgTokenDesc::UNKNOWN:			return "UNKNOWN                   ";
	case SlgRuleTable::e_SlgTokenDesc::ADD:				return "ADD                       ";
	case SlgRuleTable::e_SlgTokenDesc::SUB:				return "SUB                       ";
	case SlgRuleTable::e_SlgTokenDesc::MUL:				return "MUL                       ";
	case SlgRuleTable::e_SlgTokenDesc::DIV:				return "DIV                       ";
	case SlgRuleTable::e_SlgTokenDesc::MOD:				return "MOD                       ";
	case SlgRuleTable::e_SlgTokenDesc::EQUAL:			return "EQUAL                     ";
	case SlgRuleTable::e_SlgTokenDesc::AND:				return "AND                       ";
	case SlgRuleTable::e_SlgTokenDesc::OR:				return "OR                        ";
	case SlgRuleTable::e_SlgTokenDesc::NOT:				return "NOT                       ";
	case SlgRuleTable::e_SlgTokenDesc::SMALLER:			return "SMALLER                   ";
	case SlgRuleTable::e_SlgTokenDesc::LARGER:			return "LARGER                    ";
	case SlgRuleTable::e_SlgTokenDesc::XOR:				return "XOR                       ";
	case SlgRuleTable::e_SlgTokenDesc::RSHIFT:			return "RSHIFT                    ";
	case SlgRuleTable::e_SlgTokenDesc::LSHIFT:			return "LSHIFT                    ";
	case SlgRuleTable::e_SlgTokenDesc::SELFADD:			return "SELFADD                   ";
	case SlgRuleTable::e_SlgTokenDesc::SELFSUB:			return "SELFSUB                   ";
	case SlgRuleTable::e_SlgTokenDesc::LOGICAND:			return "LOGICAND                  ";
	case SlgRuleTable::e_SlgTokenDesc::LOGICOR:			return "LOGICOR                   ";
	case SlgRuleTable::e_SlgTokenDesc::ADDEQUAL:			return "ADDEQUAL                  ";
	case SlgRuleTable::e_SlgTokenDesc::SUBEQUAL:			return "SUBEQUAL                  ";
	case SlgRuleTable::e_SlgTokenDesc::MULEQUAL:			return "MULEQUAL                  ";
	case SlgRuleTable::e_SlgTokenDesc::DIVEQUAL:			return "DIVEQUAL                  ";
	case SlgRuleTable::e_SlgTokenDesc::MODEQUAL:			return "MODEQUAL                  ";
	case SlgRuleTable::e_SlgTokenDesc::ISEQUAL:			return "ISEQUAL                   ";
	case SlgRuleTable::e_SlgTokenDesc::ANDEQUAL:			return "ANDEQUAL                  ";
	case SlgRuleTable::e_SlgTokenDesc::OREQUAL:			return "OREQUAL                   ";
	case SlgRuleTable::e_SlgTokenDesc::NOTEQUAL:			return "NOTEQUAL                  ";
	case SlgRuleTable::e_SlgTokenDesc::SMALLEREQUAL:		return "SMALLEREQUAL              ";
	case SlgRuleTable::e_SlgTokenDesc::LARGEREQUAL:			return "LARGEREQUAL               ";
	case SlgRuleTable::e_SlgTokenDesc::XOREQUAL:			return "XOREQUAL                  ";
	case SlgRuleTable::e_SlgTokenDesc::RSHIFTEQUAL:			return "RSHIFTEQUAL               ";
	case SlgRuleTable::e_SlgTokenDesc::LSHIFTEQUAL:			return "LSHIFTEQUAL               ";
	case SlgRuleTable::e_SlgTokenDesc::BRACKET_CURLY_START:		return "BRACKET_CURLY_START       ";
	case SlgRuleTable::e_SlgTokenDesc::BRACKET_CURLY_END:		return "BRACKET_CURLY_END         ";
	case SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_START:		return "BRACKET_ROUND_START       ";
	case SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_END:		return "BRACKET_ROUND_END         ";
	case SlgRuleTable::e_SlgTokenDesc::BRACKET_SQUARE_START:	return "BRACKET_SQUARE_START      ";
	case SlgRuleTable::e_SlgTokenDesc::BRACKET_SQUARE_END:		return "BRACKET_SQUARE_END        ";
	case SlgRuleTable::e_SlgTokenDesc::COMMA:			return "COMMA                     ";
	case SlgRuleTable::e_SlgTokenDesc::MEMBERACCESS:		return "MEMBERACCESS              ";
	case SlgRuleTable::e_SlgTokenDesc::END:				return "END                       ";
	case SlgRuleTable::e_SlgTokenDesc::STRING:			return "STRING                    ";
	case SlgRuleTable::e_SlgTokenDesc::INT:				return "INT                       ";
	case SlgRuleTable::e_SlgTokenDesc::FLOAT:			return "FLOAT                     ";
	case SlgRuleTable::e_SlgTokenDesc::TYPE_DECLAR:			return "TYPE_DECLAR               ";
	case SlgRuleTable::e_SlgTokenDesc::ATTRIBUTE:			return "ATTRIBUTE                 ";
	case SlgRuleTable::e_SlgTokenDesc::INVERT:			return "INVERT                    ";
	case SlgRuleTable::e_SlgTokenDesc::INVERTEQUAL:			return "INVERTEQUAL               ";
	case SlgRuleTable::e_SlgTokenDesc::MODULEACCESS:		return "MODULEACCESS              ";

	case SlgRuleTable::e_SlgTokenDesc::IDENTIFIER:			return "IDENTIFIER                ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_using:		return "KEYWORLD_using            ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_module:		return "KEYWORLD_module           ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_export:		return "KEYWORLD_export           ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_let:		return "KEYWORLD_let              ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_function:		return "KEYWORLD_function         ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_extern:		return "KEYWORLD_extern           ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_if:			return "KEYWORLD_if               ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_else:		return "KEYWORLD_else             ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_for:		return "KEYWORLD_for              ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_break:		return "KEYWORLD_break            ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_continue:		return "KEYWORLD_continue         ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_return:		return "KEYWORLD_return           ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_switch:		return "KEYWORLD_switch           ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_case:		return "KEYWORLD_case             ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_in:			return "KEYWORLD_in               ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_ref:		return "KEYWORLD_ref              ";
	case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_attribute:		return "KEYWORLD_attribute        ";

	default:
		return "";
	}
}