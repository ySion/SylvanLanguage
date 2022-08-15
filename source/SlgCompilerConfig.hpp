#pragma once

#include "pch.hpp"

namespace SylvanLanguage {

	enum class ETokenType {
		ERRORTOKEN = 0x0000,
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

	enum class ETokenDesc {
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
		KEYWORLD_module,
		KEYWORLD_export,
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
	};

	inline std::string TypeDescString(ETokenDesc desc)
	{
		switch (desc)
		{
		case ETokenDesc::UNKNOWN:			return "UNKNOWN                   ";
		case ETokenDesc::ADD:				return "ADD                       ";
		case ETokenDesc::SUB:				return "SUB                       ";
		case ETokenDesc::MUL:				return "MUL                       ";
		case ETokenDesc::DIV:				return "DIV                       ";
		case ETokenDesc::MOD:				return "MOD                       ";
		case ETokenDesc::EQUAL:				return "EQUAL                     ";
		case ETokenDesc::AND:				return "AND                       ";
		case ETokenDesc::OR:				return "OR                        ";
		case ETokenDesc::NOT:				return "NOT                       ";
		case ETokenDesc::SMALLER:			return "SMALLER                   ";
		case ETokenDesc::LARGER:			return "LARGER                    ";
		case ETokenDesc::XOR:				return "XOR                       ";
		case ETokenDesc::RSHIFT:			return "RSHIFT                    ";
		case ETokenDesc::LSHIFT:			return "LSHIFT                    ";
		case ETokenDesc::SELFADD:			return "SELFADD                   ";
		case ETokenDesc::SELFSUB:			return "SELFSUB                   ";
		case ETokenDesc::LOGICAND:			return "LOGICAND                  ";
		case ETokenDesc::LOGICOR:			return "LOGICOR                   ";
		case ETokenDesc::ADDEQUAL:			return "ADDEQUAL                  ";
		case ETokenDesc::SUBEQUAL:			return "SUBEQUAL                  ";
		case ETokenDesc::MULEQUAL:			return "MULEQUAL                  ";
		case ETokenDesc::DIVEQUAL:			return "DIVEQUAL                  ";
		case ETokenDesc::MODEQUAL:			return "MODEQUAL                  ";
		case ETokenDesc::ISEQUAL:			return "ISEQUAL                   ";
		case ETokenDesc::ANDEQUAL:			return "ANDEQUAL                  ";
		case ETokenDesc::OREQUAL:			return "OREQUAL                   ";
		case ETokenDesc::NOTEQUAL:			return "NOTEQUAL                  ";
		case ETokenDesc::SMALLEREQUAL:			return "SMALLEREQUAL              ";
		case ETokenDesc::LARGEREQUAL:			return "LARGEREQUAL               ";
		case ETokenDesc::XOREQUAL:			return "XOREQUAL                  ";
		case ETokenDesc::RSHIFTEQUAL:			return "RSHIFTEQUAL               ";
		case ETokenDesc::LSHIFTEQUAL:			return "LSHIFTEQUAL               ";
		case ETokenDesc::BRACKET_CURLY_START:		return "BRACKET_CURLY_START       ";
		case ETokenDesc::BRACKET_CURLY_END:		return "BRACKET_CURLY_END         ";
		case ETokenDesc::BRACKET_ROUND_START:		return "BRACKET_ROUND_START       ";
		case ETokenDesc::BRACKET_ROUND_END:		return "BRACKET_ROUND_END         ";
		case ETokenDesc::BRACKET_SQUARE_START:		return "BRACKET_SQUARE_START      ";
		case ETokenDesc::BRACKET_SQUARE_END:		return "BRACKET_SQUARE_END        ";
		case ETokenDesc::COMMA:				return "COMMA                     ";
		case ETokenDesc::MEMBERACCESS:			return "MEMBERACCESS              ";
		case ETokenDesc::END:				return "END                       ";
		case ETokenDesc::STRING:			return "STRING                    ";
		case ETokenDesc::INT:				return "INT                       ";
		case ETokenDesc::FLOAT:				return "FLOAT                     ";
		case ETokenDesc::TYPE_DECLAR:			return "TYPE_DECLAR               ";
		case ETokenDesc::ATTRIBUTE:			return "ATTRIBUTE                 ";
		case ETokenDesc::INVERT:			return "INVERT                    ";
		case ETokenDesc::INVERTEQUAL:			return "INVERTEQUAL               ";
		case ETokenDesc::MODULEACCESS:			return "MODULEACCESS              ";

		case ETokenDesc::IDENTIFIER:			return "IDENTIFIER                ";

		case ETokenDesc::KEYWORLD_using:		return "KEYWORLD_using            ";
		case ETokenDesc::KEYWORLD_module:		return "KEYWORLD_module           ";
		case ETokenDesc::KEYWORLD_export:		return "KEYWORLD_export           ";
		case ETokenDesc::KEYWORLD_extern:		return "KEYWORLD_extern           ";
		case ETokenDesc::KEYWORLD_if:			return "KEYWORLD_if               ";
		case ETokenDesc::KEYWORLD_else:			return "KEYWORLD_else             ";
		case ETokenDesc::KEYWORLD_for:			return "KEYWORLD_for              ";
		case ETokenDesc::KEYWORLD_break:		return "KEYWORLD_break            ";
		case ETokenDesc::KEYWORLD_continue:		return "KEYWORLD_continue         ";
		case ETokenDesc::KEYWORLD_return:		return "KEYWORLD_return           ";
		case ETokenDesc::KEYWORLD_switch:		return "KEYWORLD_switch           ";
		case ETokenDesc::KEYWORLD_case:			return "KEYWORLD_case             ";

		default:
			return "";
		}
	}

	class AssemblyData {

		void* mRoot = nullptr;

		size_t mDataAllSize = 0;
		size_t mDataUsingSize = 0;

		bool ReSize(size_t newSize);
	public:
		AssemblyData(size_t size);

		bool ReadData(const void* p, size_t size);

		bool AddData(const void* p, size_t size);

		bool AddString(std::string& strPtr);

		template<class T>bool AddValue(T p) {
			return AddData(&p, sizeof(T));
		}

		virtual ~AssemblyData();
		inline size_t GetAllSize() const {
			return mDataAllSize;
		}

		inline size_t GetCurrentSize() const {
			return mDataUsingSize;
		}

		void Show();
	};
#define ATT_COMP_ASM(A)  A, ATT_##A
	enum class ESlgd87Asm {
		MODULE_DEP = 0x8001,
		MODULE_SELF,
		ATTRITUBE_LET,
		GLOBAL_VAR_MALLOC,

		ATT_COMP_ASM(M_MATRIAX_SET2),
		ATT_COMP_ASM(M_MATRIAX_SET3),
		ATT_COMP_ASM(M_MATRIAX_SET4),
		ATT_COMP_ASM(M_MATRIAX_SET_ONE2),
		ATT_COMP_ASM(M_MATRIAX_SET_ONE3),
		ATT_COMP_ASM(M_MATRIAX_SET_ONE4),

		ATT_COMP_ASM(M_MATRIAX_SET_LINE2),
		ATT_COMP_ASM(M_MATRIAX_SET_LINE3),
		ATT_COMP_ASM(M_MATRIAX_SET_LINE4),

		ATT_COMP_ASM(M_MATRIAX_SET_COLUMN2),
		ATT_COMP_ASM(M_MATRIAX_SET_COLUMN3),
		ATT_COMP_ASM(M_MATRIAX_SET_COLUMN4),

		ATT_COMP_ASM(M_MATRIAX_GET_ONE2),
		ATT_COMP_ASM(M_MATRIAX_GET_ONE3),
		ATT_COMP_ASM(M_MATRIAX_GET_ONE4),

		ATT_COMP_ASM(M_MATRIAX_GET_LINE2),
		ATT_COMP_ASM(M_MATRIAX_GET_LINE3),
		ATT_COMP_ASM(M_MATRIAX_GET_LINE4),

		ATT_COMP_ASM(M_MATRIAX_GET_COLUMN2),
		ATT_COMP_ASM(M_MATRIAX_GET_COLUMN3),
		ATT_COMP_ASM(M_MATRIAX_GET_COLUMN4),

		ATT_COMP_ASM(SIN),
		ATT_COMP_ASM(SIN2),
		ATT_COMP_ASM(SIN3),
		ATT_COMP_ASM(SIN4),

		ATT_COMP_ASM(COS),
		ATT_COMP_ASM(COS2),
		ATT_COMP_ASM(COS3),
		ATT_COMP_ASM(COS4),

		ATT_COMP_ASM(TAN),
		ATT_COMP_ASM(TAN2),
		ATT_COMP_ASM(TAN3),
		ATT_COMP_ASM(TAN4),

		ATT_COMP_ASM(ASIN),
		ATT_COMP_ASM(ASIN2),
		ATT_COMP_ASM(ASIN3),
		ATT_COMP_ASM(ASIN4),

		ATT_COMP_ASM(ACOS),
		ATT_COMP_ASM(ACOS2),
		ATT_COMP_ASM(ACOS3),
		ATT_COMP_ASM(ACOS4),

		ATT_COMP_ASM(ATAN),
		ATT_COMP_ASM(ATAN2),
		ATT_COMP_ASM(ATAN3),
		ATT_COMP_ASM(ATAN4),

		ATT_COMP_ASM(CROSS),

		ATT_COMP_ASM(DOT2),
		ATT_COMP_ASM(DOT3),
		ATT_COMP_ASM(DOT4),

		ATT_COMP_ASM(LENGTH2),
		ATT_COMP_ASM(LENGTH3),

		ATT_COMP_ASM(DEG2RAD),
		ATT_COMP_ASM(RAD2DEG),

		ATT_COMP_ASM(NORMALIZE2),
		ATT_COMP_ASM(NORMALIZE3),

		ATT_COMP_ASM(DISTANCE2),
		ATT_COMP_ASM(DISTANCE3),

		ATT_COMP_ASM(MAX),
		ATT_COMP_ASM(MIN),

		ATT_COMP_ASM(POW),
		ATT_COMP_ASM(POW2),
		ATT_COMP_ASM(POW3),
		ATT_COMP_ASM(POW4),

		ATT_COMP_ASM(SQRT),
		ATT_COMP_ASM(SQRT2),
		ATT_COMP_ASM(SQRT3),
		ATT_COMP_ASM(SQRT4),
		ATT_COMP_ASM(CLAMP),
		ATT_COMP_ASM(LERP),
		ATT_COMP_ASM(LERP2),
		ATT_COMP_ASM(LERP3),
		ATT_COMP_ASM(LERP4),

		ATT_COMP_ASM(ABS),
		ATT_COMP_ASM(ABS2),
		ATT_COMP_ASM(ABS3),
		ATT_COMP_ASM(ABS4),

		ATT_COMP_ASM(FLOOR),
		ATT_COMP_ASM(CEIL),
		ATT_COMP_ASM(FRACT),

		//基本指令
		MOV,
		MOVIF,
		MOVFI,
		MOVF,
		MOVSTR,
		MOVSTRI,
		MOVSTRF,
		MOVSTRV2,
		MOVSTRV3,
		MOVSTRV4,
		MOVV2,
		MOVV3,
		MOVV4,
		MOVMAT2,
		MOVMAT3,
		MOVMAT4,
		ADDTO,
		ADDTOIF,
		ADDTOFI,
		ADDTOF,
		ADDTOSTR,
		ADDTOSTRI,
		ADDTOSTRF,
		ADDTOSTRV2,
		ADDTOSTRV3,
		ADDTOSTRV4,
		ADDTOV2,
		ADDTOV3,
		ADDTOV4,
		ADDTOMAT2,
		ADDTOMAT3,
		ADDTOMAT4,
		SUBTO,
		SUBTOIF,
		SUBTOFI,
		SUBTOF,
		SUBTOV2,
		SUBTOV3,
		SUBTOV4,
		SUBTOMAT2,
		SUBTOMAT3,
		SUBTOMAT4,
		MULTO,
		MULTOIF,
		MULTOFI,
		MULTOF,
		MULTOV2,
		MULTOV3,
		MULTOV4,
		MULTOV2F,
		MULTOV3F,
		MULTOV4F,
		MULTOV2I,
		MULTOV3I,
		MULTOV4I,
		MULTOV2MAT2,
		MULTOV3MAT3,
		MULTOV4MAT4,
		MULTOMAT2,
		MULTOMAT3,
		MULTOMAT4,
		DIVTO,
		DIVTOIF,
		DIVTOFI,
		DIVTOF,
		DIVTOV2,
		DIVTOV3,
		DIVTOV4,
		DIVTOV2F,
		DIVTOV3F,
		DIVTOV4F,
		DIVTOV2I,
		DIVTOV3I,
		DIVTOV4I,
		DIVTOMAT2,
		DIVTOMAT3,
		DIVTOMAT4,
		MODTO,
		ANDTO,
		ORTO,
		XORTO,
		INVTO,
		RSHTO,
		LSHTO,
		NOT,
		NOTF,
		INV,
		PUSH,
		PUSHIF,
		PUSHFI,
		PUSHF,
		PUSHV2,
		PUSHV3,
		PUSHV4,
		PUSHMAT2,
		PUSHMAT3,
		PUSHMAT4,
		PUSHSTR,
		ADD,
		ADDIF,
		ADDFI,
		ADDF,
		ADDSTRI,
		ADDSTRF,
		ADDSTR,
		ADDSTRV2,
		ADDSTRV3,
		ADDSTRV4,
		ADDV2,
		ADDV3,
		ADDV4,
		ADDMAT2,
		ADDMAT3,
		ADDMAT4,
		SUB,
		SUBIF,
		SUBFI,
		SUBF,
		SUBV2,
		SUBV3,
		SUBV4,
		SUBMAT2,
		SUBMAT3,
		SUBMAT4,
		MUL,
		MULIF,
		MULIV2,
		MULIV3,
		MULIV4,
		MULIMAT2,
		MULIMAT3,
		MULIMAT4,
		MULFI,
		MULF,
		MULFV2,
		MULFV3,
		MULFV4,
		MULFMAT2,
		MULFMAT3,
		MULFMAT4,
		MULV2,
		MULV3,
		MULV4,
		MULV2F,
		MULV3F,
		MULV4F,
		MULV2I,
		MULV3I,
		MULV4I,
		MULMAT2V2,
		MULMAT3V3,
		MULMAT4V4,
		MULMAT2,
		MULMAT3,
		MULMAT4,
		MULMAT2F,
		MULMAT3F,
		MULMAT4F,
		DIV,
		DIVIF,
		DIVFI,
		DIVF,
		DIVV2,
		DIVV3,
		DIVV4,
		DIVV2F,
		DIVV3F,
		DIVV4F,
		DIVV2I,
		DIVV3I,
		DIVV4I,
		DIVMAT2,
		DIVMAT3,
		DIVMAT4,
		MOD,
		LSH,
		RSH,
		SCMP,
		SCMPIF,
		SCMPFI,
		SCMPF,
		LCMP,
		LCMPIF,
		LCMPFI,
		LCMPF,
		SECMP,
		SECMPIF,
		SECMPFI,
		SECMPF,
		LECMP,
		LECMPIF,
		LECMPFI,
		LECMPF,
		ICMP,
		ICMPIF,
		ICMPSTR,
		ICMPFI,
		ICMPF,
		ICMPV2,
		ICMPV3,
		ICMPV4,
		ICMPMAT2,
		ICMPMAT3,
		ICMPMAT4,
		NCMP,
		NCMPIF,
		NCMPSTR,
		NCMPFI,
		NCMPF,
		NCMPV2,
		NCMPV3,
		NCMPV4,
		NCMPMAT2,
		NCMPMAT3,
		NCMPMAT4,
		AND,
		OR,
		XOR,
		LOGICAND,
		LOGICOR,


	};



	enum class e_SlgType {
		INT = 0x01,
		FLOAT,
		STRING,
		VECTOR2,
		VECTOR3,
		VECTOR4,
		MATRIX2X2,
		MATRIX3X3,
		MATRIX4X4
	};

	class CompilerConfig {

	public:
		struct SInlineFunctionDesc {
			std::string returnType{};
			std::vector<std::string> arguments{};
			std::string assemblyStr{};
		};

		struct SMemberVaribleDesc {
			std::string Type{};
			bool isConstant = false;
		};

		std::unordered_map<std::string, std::unordered_map<std::string, SInlineFunctionDesc>> InlineMemberFunction{};

		std::unordered_map<std::string, std::unordered_map<std::string, SMemberVaribleDesc>> InlineMemberVariable{};

		std::unordered_map<std::string, SInlineFunctionDesc> InlineFunction{};

		std::unordered_map<std::string, double> InlineMarcoFloat{};

#define STRMAPASM(A) {#A, ESlgd87Asm::##A}
#define ATT_COMP_ASM_MAP(A) {#A, ESlgd87Asm::##A}, {"ATT_"#A, ESlgd87Asm::ATT_##A}


		std::unordered_map <std::string, ESlgd87Asm> StrMapAsm{

			ATT_COMP_ASM_MAP(M_MATRIAX_SET2),
			ATT_COMP_ASM_MAP(M_MATRIAX_SET3),
			ATT_COMP_ASM_MAP(M_MATRIAX_SET4),
			ATT_COMP_ASM_MAP(M_MATRIAX_SET_ONE2),
			ATT_COMP_ASM_MAP(M_MATRIAX_SET_ONE3),
			ATT_COMP_ASM_MAP(M_MATRIAX_SET_ONE4),

			ATT_COMP_ASM_MAP(M_MATRIAX_SET_LINE2),
			ATT_COMP_ASM_MAP(M_MATRIAX_SET_LINE3),
			ATT_COMP_ASM_MAP(M_MATRIAX_SET_LINE4),

			ATT_COMP_ASM_MAP(M_MATRIAX_SET_COLUMN2),
			ATT_COMP_ASM_MAP(M_MATRIAX_SET_COLUMN3),
			ATT_COMP_ASM_MAP(M_MATRIAX_SET_COLUMN4),

			ATT_COMP_ASM_MAP(M_MATRIAX_GET_ONE2),
			ATT_COMP_ASM_MAP(M_MATRIAX_GET_ONE3),
			ATT_COMP_ASM_MAP(M_MATRIAX_GET_ONE4),

			ATT_COMP_ASM_MAP(M_MATRIAX_GET_LINE2),
			ATT_COMP_ASM_MAP(M_MATRIAX_GET_LINE3),
			ATT_COMP_ASM_MAP(M_MATRIAX_GET_LINE4),

			ATT_COMP_ASM_MAP(M_MATRIAX_GET_COLUMN2),
			ATT_COMP_ASM_MAP(M_MATRIAX_GET_COLUMN3),
			ATT_COMP_ASM_MAP(M_MATRIAX_GET_COLUMN4),

			ATT_COMP_ASM_MAP(SIN),
			ATT_COMP_ASM_MAP(SIN2),
			ATT_COMP_ASM_MAP(SIN3),
			ATT_COMP_ASM_MAP(SIN4),

			ATT_COMP_ASM_MAP(COS),
			ATT_COMP_ASM_MAP(COS2),
			ATT_COMP_ASM_MAP(COS3),
			ATT_COMP_ASM_MAP(COS4),

			ATT_COMP_ASM_MAP(TAN),
			ATT_COMP_ASM_MAP(TAN2),
			ATT_COMP_ASM_MAP(TAN3),
			ATT_COMP_ASM_MAP(TAN4),

			ATT_COMP_ASM_MAP(ASIN),
			ATT_COMP_ASM_MAP(ASIN2),
			ATT_COMP_ASM_MAP(ASIN3),
			ATT_COMP_ASM_MAP(ASIN4),

			ATT_COMP_ASM_MAP(ACOS),
			ATT_COMP_ASM_MAP(ACOS2),
			ATT_COMP_ASM_MAP(ACOS3),
			ATT_COMP_ASM_MAP(ACOS4),

			ATT_COMP_ASM_MAP(ATAN),
			ATT_COMP_ASM_MAP(ATAN2),
			ATT_COMP_ASM_MAP(ATAN3),
			ATT_COMP_ASM_MAP(ATAN4),

			ATT_COMP_ASM_MAP(CROSS),

			ATT_COMP_ASM_MAP(DOT2),
			ATT_COMP_ASM_MAP(DOT3),
			ATT_COMP_ASM_MAP(DOT4),

			ATT_COMP_ASM_MAP(LENGTH2),
			ATT_COMP_ASM_MAP(LENGTH3),

			ATT_COMP_ASM_MAP(DEG2RAD),
			ATT_COMP_ASM_MAP(RAD2DEG),

			ATT_COMP_ASM_MAP(NORMALIZE2),
			ATT_COMP_ASM_MAP(NORMALIZE3),

			ATT_COMP_ASM_MAP(DISTANCE2),
			ATT_COMP_ASM_MAP(DISTANCE3),

			ATT_COMP_ASM_MAP(MAX),
			ATT_COMP_ASM_MAP(MIN),

			ATT_COMP_ASM_MAP(POW),
			ATT_COMP_ASM_MAP(POW2),
			ATT_COMP_ASM_MAP(POW3),
			ATT_COMP_ASM_MAP(POW4),

			ATT_COMP_ASM_MAP(SQRT),
			ATT_COMP_ASM_MAP(SQRT2),
			ATT_COMP_ASM_MAP(SQRT3),
			ATT_COMP_ASM_MAP(SQRT4),
			ATT_COMP_ASM_MAP(CLAMP),
			ATT_COMP_ASM_MAP(LERP),
			ATT_COMP_ASM_MAP(LERP2),
			ATT_COMP_ASM_MAP(LERP3),
			ATT_COMP_ASM_MAP(LERP4),

			ATT_COMP_ASM_MAP(ABS),
			ATT_COMP_ASM_MAP(ABS2),
			ATT_COMP_ASM_MAP(ABS3),
			ATT_COMP_ASM_MAP(ABS4),

			ATT_COMP_ASM_MAP(FLOOR),
			ATT_COMP_ASM_MAP(CEIL),
			ATT_COMP_ASM_MAP(FRACT),


			STRMAPASM(MOV),
			STRMAPASM(MOVIF),
			STRMAPASM(MOVFI),
			STRMAPASM(MOVF),
			STRMAPASM(MOVSTR),
			STRMAPASM(MOVSTRI),
			STRMAPASM(MOVSTRF),
			STRMAPASM(MOVSTRV2),
			STRMAPASM(MOVSTRV3),
			STRMAPASM(MOVSTRV4),
			STRMAPASM(MOVV2),
			STRMAPASM(MOVV3),
			STRMAPASM(MOVV4),
			STRMAPASM(MOVMAT2),
			STRMAPASM(MOVMAT3),
			STRMAPASM(MOVMAT4),
			STRMAPASM(ADDTO),
			STRMAPASM(ADDTOIF),
			STRMAPASM(ADDTOFI),
			STRMAPASM(ADDTOF),
			STRMAPASM(ADDTOSTR),
			STRMAPASM(ADDTOSTRI),
			STRMAPASM(ADDTOSTRF),
			STRMAPASM(ADDTOSTRV2),
			STRMAPASM(ADDTOSTRV3),
			STRMAPASM(ADDTOSTRV4),
			STRMAPASM(ADDTOV2),
			STRMAPASM(ADDTOV3),
			STRMAPASM(ADDTOV4),
			STRMAPASM(ADDTOMAT2),
			STRMAPASM(ADDTOMAT3),
			STRMAPASM(ADDTOMAT4),
			STRMAPASM(SUBTO),
			STRMAPASM(SUBTOIF),
			STRMAPASM(SUBTOFI),
			STRMAPASM(SUBTOF),
			STRMAPASM(SUBTOV2),
			STRMAPASM(SUBTOV3),
			STRMAPASM(SUBTOV4),
			STRMAPASM(SUBTOMAT2),
			STRMAPASM(SUBTOMAT3),
			STRMAPASM(SUBTOMAT4),
			STRMAPASM(MULTO),
			STRMAPASM(MULTOIF),
			STRMAPASM(MULTOFI),
			STRMAPASM(MULTOF),
			STRMAPASM(MULTOV2),
			STRMAPASM(MULTOV3),
			STRMAPASM(MULTOV4),
			STRMAPASM(MULTOV2F),
			STRMAPASM(MULTOV3F),
			STRMAPASM(MULTOV4F),
			STRMAPASM(MULTOV2I),
			STRMAPASM(MULTOV3I),
			STRMAPASM(MULTOV4I),
			STRMAPASM(MULTOV2MAT2),
			STRMAPASM(MULTOV3MAT3),
			STRMAPASM(MULTOV4MAT4),
			STRMAPASM(MULTOMAT2),
			STRMAPASM(MULTOMAT3),
			STRMAPASM(MULTOMAT4),
			STRMAPASM(DIVTO),
			STRMAPASM(DIVTOIF),
			STRMAPASM(DIVTOFI),
			STRMAPASM(DIVTOF),
			STRMAPASM(DIVTOV2),
			STRMAPASM(DIVTOV3),
			STRMAPASM(DIVTOV4),
			STRMAPASM(DIVTOV2F),
			STRMAPASM(DIVTOV3F),
			STRMAPASM(DIVTOV4F),
			STRMAPASM(DIVTOV2I),
			STRMAPASM(DIVTOV3I),
			STRMAPASM(DIVTOV4I),
			STRMAPASM(DIVTOMAT2),
			STRMAPASM(DIVTOMAT3),
			STRMAPASM(DIVTOMAT4),
			STRMAPASM(MODTO),
			STRMAPASM(ANDTO),
			STRMAPASM(ORTO),
			STRMAPASM(XORTO),
			STRMAPASM(INVTO),
			STRMAPASM(RSHTO),
			STRMAPASM(LSHTO),
			STRMAPASM(NOT),
			STRMAPASM(NOTF),
			STRMAPASM(INV),
			STRMAPASM(PUSH),
			STRMAPASM(PUSHIF),
			STRMAPASM(PUSHFI),
			STRMAPASM(PUSHF),
			STRMAPASM(PUSHV2),
			STRMAPASM(PUSHV3),
			STRMAPASM(PUSHV4),
			STRMAPASM(PUSHMAT2),
			STRMAPASM(PUSHMAT3),
			STRMAPASM(PUSHMAT4),
			STRMAPASM(PUSHSTR),
			STRMAPASM(ADD),
			STRMAPASM(ADDIF),
			STRMAPASM(ADDFI),
			STRMAPASM(ADDF),
			STRMAPASM(ADDSTRI),
			STRMAPASM(ADDSTRF),
			STRMAPASM(ADDSTR),
			STRMAPASM(ADDSTRV2),
			STRMAPASM(ADDSTRV3),
			STRMAPASM(ADDSTRV4),
			STRMAPASM(ADDV2),
			STRMAPASM(ADDV3),
			STRMAPASM(ADDV4),
			STRMAPASM(ADDMAT2),
			STRMAPASM(ADDMAT3),
			STRMAPASM(ADDMAT4),
			STRMAPASM(SUB),
			STRMAPASM(SUBIF),
			STRMAPASM(SUBFI),
			STRMAPASM(SUBF),
			STRMAPASM(SUBV2),
			STRMAPASM(SUBV3),
			STRMAPASM(SUBV4),
			STRMAPASM(SUBMAT2),
			STRMAPASM(SUBMAT3),
			STRMAPASM(SUBMAT4),
			STRMAPASM(MUL),
			STRMAPASM(MULIF),
			STRMAPASM(MULIV2),
			STRMAPASM(MULIV3),
			STRMAPASM(MULIV4),
			STRMAPASM(MULIMAT2),
			STRMAPASM(MULIMAT3),
			STRMAPASM(MULIMAT4),
			STRMAPASM(MULFI),
			STRMAPASM(MULF),
			STRMAPASM(MULFV2),
			STRMAPASM(MULFV3),
			STRMAPASM(MULFV4),
			STRMAPASM(MULFMAT2),
			STRMAPASM(MULFMAT3),
			STRMAPASM(MULFMAT4),
			STRMAPASM(MULV2),
			STRMAPASM(MULV3),
			STRMAPASM(MULV4),
			STRMAPASM(MULV2F),
			STRMAPASM(MULV3F),
			STRMAPASM(MULV4F),
			STRMAPASM(MULV2I),
			STRMAPASM(MULV3I),
			STRMAPASM(MULV4I),
			STRMAPASM(MULMAT2V2),
			STRMAPASM(MULMAT3V3),
			STRMAPASM(MULMAT4V4),
			STRMAPASM(MULMAT2),
			STRMAPASM(MULMAT3),
			STRMAPASM(MULMAT4),
			STRMAPASM(MULMAT2F),
			STRMAPASM(MULMAT3F),
			STRMAPASM(MULMAT4F),
			STRMAPASM(DIV),
			STRMAPASM(DIVIF),
			STRMAPASM(DIVFI),
			STRMAPASM(DIVF),
			STRMAPASM(DIVV2),
			STRMAPASM(DIVV3),
			STRMAPASM(DIVV4),
			STRMAPASM(DIVV2F),
			STRMAPASM(DIVV3F),
			STRMAPASM(DIVV4F),
			STRMAPASM(DIVV2I),
			STRMAPASM(DIVV3I),
			STRMAPASM(DIVV4I),
			STRMAPASM(DIVMAT2),
			STRMAPASM(DIVMAT3),
			STRMAPASM(DIVMAT4),
			STRMAPASM(MOD),
			STRMAPASM(LSH),
			STRMAPASM(RSH),
			STRMAPASM(SCMP),
			STRMAPASM(SCMPIF),
			STRMAPASM(SCMPFI),
			STRMAPASM(SCMPF),
			STRMAPASM(LCMP),
			STRMAPASM(LCMPIF),
			STRMAPASM(LCMPFI),
			STRMAPASM(LCMPF),
			STRMAPASM(SECMP),
			STRMAPASM(SECMPIF),
			STRMAPASM(SECMPFI),
			STRMAPASM(SECMPF),
			STRMAPASM(LECMP),
			STRMAPASM(LECMPIF),
			STRMAPASM(LECMPFI),
			STRMAPASM(LECMPF),
			STRMAPASM(ICMP),
			STRMAPASM(ICMPIF),
			STRMAPASM(ICMPSTR),
			STRMAPASM(ICMPFI),
			STRMAPASM(ICMPF),
			STRMAPASM(ICMPV2),
			STRMAPASM(ICMPV3),
			STRMAPASM(ICMPV4),
			STRMAPASM(ICMPMAT2),
			STRMAPASM(ICMPMAT3),
			STRMAPASM(ICMPMAT4),
			STRMAPASM(NCMP),
			STRMAPASM(NCMPIF),
			STRMAPASM(NCMPSTR),
			STRMAPASM(NCMPFI),
			STRMAPASM(NCMPF),
			STRMAPASM(NCMPV2),
			STRMAPASM(NCMPV3),
			STRMAPASM(NCMPV4),
			STRMAPASM(NCMPMAT2),
			STRMAPASM(NCMPMAT3),
			STRMAPASM(NCMPMAT4),
			STRMAPASM(AND),
			STRMAPASM(OR),
			STRMAPASM(XOR),
			STRMAPASM(LOGICAND),
			STRMAPASM(LOGICOR),

		};

		inline void AddMemberFunction(std::string varTypeName, std::string memberFunctionName, std::string ret, std::vector<std::string> args, std::string asmb) {
			InlineMemberFunction[varTypeName][memberFunctionName] = SInlineFunctionDesc{ ret, args, asmb };
		}

		inline void AddMemberVariable(std::string varTypeName, std::string memberVarName, std::string type, bool isConstant) {
			InlineMemberVariable[varTypeName][memberVarName] = SMemberVaribleDesc{ type, isConstant };
		}

		inline void AddStandradFunction(std::string functionName, std::string ret, std::vector<std::string> args, std::string asmb) {
			InlineFunction[functionName] = SInlineFunctionDesc{ ret, args, asmb };
		}

		inline void AddMarcoFloat(std::string MarcoName, double value) {
			InlineMarcoFloat[MarcoName] = value;
		}

		inline std::optional<SInlineFunctionDesc> FindMemberFunction(const std::string& variableType, const std::string& memberFuntionName) {

			auto itor1 = InlineMemberFunction.find(variableType);

			if (itor1 != InlineMemberFunction.end()) {
				auto itor2 = itor1->second.find(memberFuntionName);
				if (itor2 != itor1->second.end()) {
					return itor2->second;
				}
			}

			return std::nullopt;
		}

		inline std::optional<SMemberVaribleDesc> FindMemberVariable(const std::string& variableType, const std::string& memberName) {

			auto itor1 = InlineMemberVariable.find(variableType);

			if (itor1 != InlineMemberVariable.end()) {
				auto itor2 = itor1->second.find(memberName);
				if (itor2 != itor1->second.end()) {
					return itor2->second;
				}
			}

			return std::nullopt;
		}

		inline std::optional<SInlineFunctionDesc> FindInlineFunction(const std::string& functionName) {
			auto itor1 = InlineFunction.find(functionName);

			if (itor1 != InlineFunction.end()) {
				return itor1->second;
			}

			return std::nullopt;
		}


		inline CompilerConfig() {

			AddMemberFunction("matrix2x2", "getline", "vector2", { "int" }, "M_MATRIAX_GET_LINE2");
			AddMemberFunction("matrix3x3", "getline", "vector3", { "int" }, "M_MATRIAX_GET_LINE3");
			AddMemberFunction("matrix4x4", "getline", "vector4", { "int" }, "M_MATRIAX_GET_LINE4");

			AddMemberFunction("matrix2x2", "getcolumn", "vector2", { "int" }, "M_MATRIAX_GET_COLUMN2");
			AddMemberFunction("matrix3x3", "getcolumn", "vector3", { "int" }, "M_MATRIAX_GET_COLUMN3");
			AddMemberFunction("matrix4x4", "getcolumn", "vector4", { "int" }, "M_MATRIAX_GET_COLUMN4");

			AddMemberFunction("matrix2x2", "getone", "float", { "int", "int" }, "M_MATRIAX_GET_ONE2");
			AddMemberFunction("matrix3x3", "getone", "float", { "int", "int" }, "M_MATRIAX_GET_ONE3");
			AddMemberFunction("matrix4x4", "getone", "float", { "int", "int" }, "M_MATRIAX_GET_ONE4");

			AddMemberFunction("matrix2x2", "setline", "void", { "int", "vector2" }, "M_MATRIAX_SET_LINE2");
			AddMemberFunction("matrix3x3", "setline", "void", { "int", "vector3" }, "M_MATRIAX_SET_LINE3");
			AddMemberFunction("matrix4x4", "setline", "void", { "int", "vector4" }, "M_MATRIAX_SET_LINE4");

			AddMemberFunction("matrix2x2", "setcolumn", "void", { "int", "vector2" }, "M_MATRIAX_SET_COLUMN2");
			AddMemberFunction("matrix3x3", "setcolumn", "void", { "int", "vector3" }, "M_MATRIAX_SET_COLUMN3");
			AddMemberFunction("matrix4x4", "setcolumn", "void", { "int", "vector4" }, "M_MATRIAX_SET_COLUMN4");

			AddMemberFunction("matrix2x2", "setone", "float", { "int", "int" }, "M_MATRIAX_SET_ONE2");
			AddMemberFunction("matrix3x3", "setone", "float", { "int", "int" }, "M_MATRIAX_SET_ONE3");
			AddMemberFunction("matrix4x4", "setone", "float", { "int", "int" }, "M_MATRIAX_SET_ONE4");


			AddMemberFunction("matrix2x2", "set", "void", { "float", "float",
									"float", "float" }, "M_MATRIAX_SET2");

			AddMemberFunction("matrix3x3", "set", "void", { "float", "float", "float",
									"float", "float", "float",
									"float", "float", "float" }, "M_MATRIAX_SET3");

			AddMemberFunction("matrix4x4", "set", "void", { "float", "float", "float", "float",
									"float", "float", "float", "float" ,
									"float", "float", "float", "float" ,
									"float", "float", "float", "float" }, "M_MATRIAX_SET4");


			//添加宏
			AddMarcoFloat("_PI", 3.1416);
			AddMarcoFloat("_2PI", 3.1416 * 2.0);
			AddMarcoFloat("_HPI", 3.1416 / 2.0);


			//添加成员函数
			AddMemberVariable("vector2", "x", "float", false);
			AddMemberVariable("vector2", "y", "float", false);

			AddMemberVariable("vector3", "x", "float", false);
			AddMemberVariable("vector3", "y", "float", false);
			AddMemberVariable("vector3", "z", "float", false);

			AddMemberVariable("vector4", "x", "float", false);
			AddMemberVariable("vector4", "y", "float", false);
			AddMemberVariable("vector4", "z", "float", false);
			AddMemberVariable("vector4", "w", "float", false);


			//添加函数
			//向量运算
			AddStandradFunction("sin", "float", { "float" }, "SIN");
			AddStandradFunction("sin2", "vector2", { "vector2" }, "SIN2");
			AddStandradFunction("sin3", "vector3", { "vector3" }, "SIN3");
			AddStandradFunction("sin4", "vector4", { "vector4" }, "SIN4");

			AddStandradFunction("cos", "float", { "float" }, "COS");
			AddStandradFunction("cos2", "vector2", { "vector2" }, "COS2");
			AddStandradFunction("cos3", "vector3", { "vector3" }, "COS3");
			AddStandradFunction("cos4", "vector4", { "vector4" }, "COS4");

			AddStandradFunction("tan", "float", { "float" }, "TAN");
			AddStandradFunction("tan2", "vector2", { "vector2" }, "TAN2");
			AddStandradFunction("tan3", "vector3", { "vector3" }, "TAN3");
			AddStandradFunction("tan4", "vector4", { "vector4" }, "TAN4");

			AddStandradFunction("arcsin", "float", { "float" }, "ASIN");
			AddStandradFunction("arcsin2", "vector2", { "vector2" }, "ASIN2");
			AddStandradFunction("arcsin3", "vector3", { "vector3" }, "ASIN3");
			AddStandradFunction("arcsin4", "vector4", { "vector4" }, "ASIN4");

			AddStandradFunction("arccos", "float", { "float" }, "ACOS");
			AddStandradFunction("arccos2", "vector2", { "vector2" }, "ACOS2");
			AddStandradFunction("arccos3", "vector3", { "vector3" }, "ACOS3");
			AddStandradFunction("arccos4", "vector4", { "vector4" }, "ACOS4");

			AddStandradFunction("arctan", "float", { "float" }, "ATAN");
			AddStandradFunction("arctan2", "vector2", { "vector2" }, "ATAN2");
			AddStandradFunction("arctan3", "vector3", { "vector3" }, "ATAN3");
			AddStandradFunction("arctan4", "vector4", { "vector4" }, "ATAN4");

			AddStandradFunction("cross", "vector3", { "vector3", "vector3" }, "CROSS");

			AddStandradFunction("dot2", "float", { "vector2", "vector2" }, "DOT2");
			AddStandradFunction("dot3", "float", { "vector3", "vector3" }, "DOT3");
			AddStandradFunction("dot4", "float", { "vector4", "vector4" }, "DOT4");

			AddStandradFunction("dot2", "float", { "vector2", "vector2" }, "DOT2");
			AddStandradFunction("dot3", "float", { "vector3", "vector3" }, "DOT3");
			AddStandradFunction("dot4", "float", { "vector4", "vector4" }, "DOT4");

			AddStandradFunction("length2", "float", { "vector2" }, "LENGTH2");
			AddStandradFunction("length3", "float", { "vector3" }, "LENGTH3");

			AddStandradFunction("normalize2", "vector2", { "vector2" }, "NORMALIZE2");
			AddStandradFunction("normalize3", "vector3", { "vector3" }, "NORMALIZE3");

			AddStandradFunction("DegToRad", "float", { "float" }, "DEG2RAD");
			AddStandradFunction("RadToDeg", "float", { "float" }, "RAD2DEG");

			AddStandradFunction("Deg2Rad", "float", { "float" }, "DEG2RAD");
			AddStandradFunction("Rad2Deg", "float", { "float" }, "RAD2DEG");

			AddStandradFunction("distance2", "vector2", { "vector2" }, "DISTANCE2");
			AddStandradFunction("distance3", "vector3", { "vector3" }, "DISTANCE3");

			AddStandradFunction("max", "float", { "float", "float" }, "MAX");
			AddStandradFunction("min", "float", { "float", "float" }, "MIN");

			AddStandradFunction("pow", "float", { "float", "float" }, "POW");
			AddStandradFunction("pow2", "vector2", { "vector2", "float" }, "POW2");
			AddStandradFunction("pow3", "vector3", { "vector3", "float" }, "POW3");
			AddStandradFunction("pow4", "vector4", { "vector4", "float" }, "POW4");

			AddStandradFunction("sqrt", "float", { "float" }, "SQRT");
			AddStandradFunction("sqrt2", "vector2", { "vector2" }, "SQRT2");
			AddStandradFunction("sqrt3", "vector3", { "vector3" }, "SQRT3");
			AddStandradFunction("sqrt4", "vector4", { "vector4" }, "SQRT4");

			AddStandradFunction("clamp", "float", { "float", "float", "float" }, "CLAMP");

			AddStandradFunction("lerp", "float", { "float", "float", "float" }, "LERP");
			AddStandradFunction("lerp2", "vector2", { "vector2", "vector2","float" }, "LERP2");
			AddStandradFunction("lerp3", "vector3", { "vector3", "vector3","float" }, "LERP3");
			AddStandradFunction("lerp4", "vector4", { "vector4", "vector4","float" }, "LERP4");

			AddStandradFunction("abs", "float", { "float" }, "ABS");
			AddStandradFunction("abs2", "vector2", { "vector2" }, "ABS2");
			AddStandradFunction("abs3", "vector3", { "vector3" }, "ABS3");
			AddStandradFunction("abs4", "vector4", { "vector4" }, "ABS4");


			AddStandradFunction("floor", "float", { "float" }, "FLOOR");
			AddStandradFunction("ceil", "float", { "float" }, "CEIL");
			AddStandradFunction("fract", "float", { "float" }, "FRACT");

		}
	private:
		unsigned short AsmIdx = 0x8000;
	};


	class D87AssemblyWriter {
		AssemblyData* data;
	public:
		inline D87AssemblyWriter() : data(nullptr) {}
		inline D87AssemblyWriter(AssemblyData* AsmData) : data(AsmData) {}

		inline void BindAsmData(AssemblyData* AsmData) { data = AsmData; }

		inline void MODULE_DEP(std::string& path) {
			data->AddValue<unsigned short>((unsigned short)ESlgd87Asm::MODULE_DEP);
			data->AddString(path);
		}

		inline void MODULE_SELF(std::string& path) {
			data->AddValue<unsigned short>((unsigned short)ESlgd87Asm::MODULE_SELF);
			data->AddString(path);
		}

		inline void GLOBAL_VAR_MALLOC(size_t memSize) {
			data->AddValue<unsigned short>((unsigned short)ESlgd87Asm::GLOBAL_VAR_MALLOC);
			data->AddValue<size_t>(memSize);
		}

		inline void ATTRITUBE_LET(std::string name, e_SlgType type, bool isPrivateAttribute) {
			data->AddValue<unsigned short>((unsigned short)ESlgd87Asm::ATTRITUBE_LET);
			data->AddString(name);
			data->AddValue<unsigned short>((unsigned short)type);
			data->AddValue<unsigned char>((unsigned char)(isPrivateAttribute ? 1 : 0));
		}
	};
	class RuleTable {
	public:
		using SlgINT = long long;
		using SlgFLOAT = double;
		using SlgSTRING = std::string;
		using TokenValueType = std::variant<SlgINT, SlgFLOAT, SlgSTRING>;

		static inline std::unordered_map<char, ETokenDesc> Brackets{
			{'{', ETokenDesc::BRACKET_CURLY_START},
			{'}', ETokenDesc::BRACKET_CURLY_END},
			{'(', ETokenDesc::BRACKET_ROUND_START},
			{')', ETokenDesc::BRACKET_ROUND_END},
			{'[', ETokenDesc::BRACKET_SQUARE_START},
			{']', ETokenDesc::BRACKET_SQUARE_END},
		};

		static inline std::unordered_map<std::string, ETokenDesc> KeyWord{

			{"using",	ETokenDesc::KEYWORLD_using},
			{"module",	ETokenDesc::KEYWORLD_module},
			{"export",	ETokenDesc::KEYWORLD_export},
			{"extern",	ETokenDesc::KEYWORLD_extern},
			{"if",		ETokenDesc::KEYWORLD_if},
			{"else",	ETokenDesc::KEYWORLD_else},
			{"return",	ETokenDesc::KEYWORLD_return},
			{"switch",	ETokenDesc::KEYWORLD_switch},
			{"case",	ETokenDesc::KEYWORLD_case},
			{"for",		ETokenDesc::KEYWORLD_for},
			{"break",	ETokenDesc::KEYWORLD_break},
			{"continue",	ETokenDesc::KEYWORLD_continue},
			{"null",	ETokenDesc::KEYWORLD_null},
		};

		static inline std::unordered_map<char, ETokenDesc> OperatorsSingle{
			{'+', ETokenDesc::ADD},
			{'-', ETokenDesc::SUB},
			{'*', ETokenDesc::MUL},
			{'/', ETokenDesc::DIV},
			{'%', ETokenDesc::MOD},
			{'=', ETokenDesc::EQUAL},
			{'&', ETokenDesc::AND},
			{'|', ETokenDesc::OR},
			{'!', ETokenDesc::NOT},
			{'<', ETokenDesc::SMALLER},
			{'>', ETokenDesc::LARGER},
			{'^', ETokenDesc::XOR},
			{'~', ETokenDesc::INVERT},
		};

		static inline std::unordered_map<std::string, ETokenDesc> OperatorsMulti{
			{"++", ETokenDesc::SELFADD},
			{"--", ETokenDesc::SELFSUB},
			{"&&", ETokenDesc::LOGICAND},
			{"||", ETokenDesc::LOGICOR},
			{"+=", ETokenDesc::ADDEQUAL},
			{"-=", ETokenDesc::ISEQUAL},
			{"*=", ETokenDesc::MULEQUAL},
			{"/=", ETokenDesc::DIVEQUAL},
			{"%=", ETokenDesc::MODEQUAL},
			{"==", ETokenDesc::ISEQUAL},
			{"&=", ETokenDesc::ANDEQUAL},
			{"|=", ETokenDesc::OREQUAL},
			{"!=", ETokenDesc::NOTEQUAL},
			{"<=", ETokenDesc::SMALLEREQUAL},
			{">=", ETokenDesc::LARGEREQUAL},
			{"^=", ETokenDesc::XOREQUAL},
			{"<<", ETokenDesc::LSHIFT},
			{">>", ETokenDesc::RSHIFT},
			{"<<=", ETokenDesc::LSHIFTEQUAL},
			{">>=", ETokenDesc::RSHIFTEQUAL},
			{"~=",  ETokenDesc::INVERTEQUAL}
		};

		static inline std::unordered_map<ETokenDesc, int> OperatorsPriority{

			{ ETokenDesc::INVERT,               14 },
			{ ETokenDesc::NOT,                  14 },

			{ ETokenDesc::MOD,                  13 },

			{ ETokenDesc::MUL,                  12 },
			{ ETokenDesc::DIV,                  12 },

			{ ETokenDesc::ADD,                  11 },
			{ ETokenDesc::SUB,                  11 },

			{ ETokenDesc::LSHIFT,               10 },
			{ ETokenDesc::RSHIFT,               10 },

			//大小比较
			{ ETokenDesc::SMALLER,              9 },
			{ ETokenDesc::LARGER,               9 },
			{ ETokenDesc::SMALLEREQUAL,         9 },
			{ ETokenDesc::LARGEREQUAL,          9 },

			//等于比较
			{ ETokenDesc::ISEQUAL,              8 },
			{ ETokenDesc::NOTEQUAL,             7 },

			//位与
			{ ETokenDesc::AND,                  6 },
			//异或
			{ ETokenDesc::XOR,                  5 },
			//位或
			{ ETokenDesc::OR,                   4 },
			//逻辑与
			{ ETokenDesc::LOGICAND,             3 },
			//逻辑或
			{ ETokenDesc::LOGICOR,              2 },

			//赋值运算符
			{ ETokenDesc::EQUAL,                1 },
			{ ETokenDesc::ADDEQUAL,             1 },
			{ ETokenDesc::SUBEQUAL,             1 },
			{ ETokenDesc::MULEQUAL,             1 },
			{ ETokenDesc::DIVEQUAL,             1 },
			{ ETokenDesc::MODEQUAL,             1 },
			{ ETokenDesc::ANDEQUAL,             1 },
			{ ETokenDesc::OREQUAL,              1 },
			{ ETokenDesc::XOREQUAL,             1 },
			{ ETokenDesc::INVERTEQUAL,          1 },
			{ ETokenDesc::LSHIFTEQUAL,		1 },
			{ ETokenDesc::RSHIFTEQUAL,		1 },
		};

		static inline std::unordered_map<char, ETokenDesc> StatementOpeartor{
			{'.', ETokenDesc::MEMBERACCESS},
			{',', ETokenDesc::COMMA},
			{';', ETokenDesc::END},
			{'@', ETokenDesc::ATTRIBUTE}
		};
	};

	class TypeRuler {
	public:
		static inline std::unordered_map<std::string, size_t> SlgTypeSize{
			{"int", 8},
			{"float", 8},
			{"string", 8},
			{"vector2", 16},
			{"vector3", 32},
			{"vector4", 32},
			{"matrix2x2", 32},
			{"matrix3x3", 72},
			{"matrix4x4", 128},
		};

		static inline std::unordered_map<std::string, e_SlgType> TypeAsm{
			{"int",    e_SlgType::INT,},
			{"float",  e_SlgType::FLOAT,},
			{"string", e_SlgType::STRING,},
			{"vector2",e_SlgType::VECTOR2},
			{"vector3",e_SlgType::VECTOR3},
			{"vector4",e_SlgType::VECTOR4},
			{"matrix2x2",e_SlgType::MATRIX2X2},
			{"matrix3x3",e_SlgType::MATRIX3X3},
			{"matrix4x4",e_SlgType::MATRIX4X4}
		};


		// op A = B 最后一个汇编指令的字符串

		static inline size_t StrHashCalc(const char* s)
		{
			unsigned long h = 0;
			for (; *s; ++s)
				h = 5 * h + *s;
			return size_t(h);
		}

		struct SStrPairHash
		{
			std::size_t operator()(const std::pair<std::string, std::string>& key) const noexcept
			{
				return std::hash<size_t>()(StrHashCalc(key.first.c_str())) + std::hash<size_t>()(StrHashCalc(key.second.c_str()));
			}
		};

		// op A = B 最后一个汇编指令的字符串
		static inline std::unordered_map <ETokenDesc, std::unordered_map<std::pair<std::string, std::string>, std::string, SStrPairHash>> TypeAssignmentOperatorCompatible
		{
			{
				ETokenDesc::EQUAL, {
					{{"int",	"int"},		"MOV"},
					{{"int",	"float"},	"MOVIF"},
					{{"float",	"int"},		"MOVFI"},
					{{"float",	"float"},	"MOVF"},

					{{"string",	"string"},	"MOVSTR"},
					{{"string",	"int"},		"MOVSTRI"},
					{{"string",	"float"},	"MOVSTRF"},
					{{"string",	"vector2"},	"MOVSTRV2"},
					{{"string",	"vector3"},	"MOVSTRV3"},
					{{"string",	"vector4"},	"MOVSTRV4"},

					{{"vector2",	"vector2"},	"MOVV2"},
					{{"vector3",	"vector3"},	"MOVV3"},
					{{"vector4",	"vector4"},	"MOVV4"},

					{{"matrix2x2",	"matrix2x2"},	"MOVMAT2"},
					{{"matrix3x3",	"matrix3x3"},	"MOVMAT3"},
					{{"matrix4x4",	"matrix4x4"},	"MOVMAT4"},
				}
			},{
				ETokenDesc::ADDEQUAL, {
					{{"int",	"int"},		"ADDTO"},
					{{"int",	"float"},	"ADDTOIF"},
					{{"float",	"int"},		"ADDTOFI"},
					{{"float",	"float"},	"ADDTOF"},

					{{"string",	"string"},	"ADDTOSTR"},
					{{"string",	"int"},		"ADDTOSTRI"},
					{{"string",	"float"},	"ADDTOSTRF"},
					{{"string",	"vector2"},	"ADDTOSTRV2"},
					{{"string",	"vector3"},	"ADDTOSTRV3"},
					{{"string",	"vector4"},	"ADDTOSTRV4"},

					{{"vector2",	"vector2"},	"ADDTOV2"},
					{{"vector3",	"vector3"},	"ADDTOV3"},
					{{"vector4",	"vector4"},	"ADDTOV4"},

					{{"matrix2x2",	"matrix2x2"},	"ADDTOMAT2"},
					{{"matrix3x3",	"matrix3x3"},	"ADDTOMAT3"},
					{{"matrix4x4",	"matrix4x4"},	"ADDTOMAT4"},
				}
			},{
				ETokenDesc::SUBEQUAL, {
					{{"int",	"int"},		"SUBTO"},
					{{"int",	"float"},	"SUBTOIF"},
					{{"float",	"int"},		"SUBTOFI"},
					{{"float",	"float"},	"SUBTOF"},

					{{"vector2",	"vector2"},	"SUBTOV2"},
					{{"vector3",	"vector3"},	"SUBTOV3"},
					{{"vector4",	"vector4"},	"SUBTOV4"},

					{{"matrix2x2",	"matrix2x2"},	"SUBTOMAT2"},
					{{"matrix3x3",	"matrix3x3"},	"SUBTOMAT3"},
					{{"matrix4x4",	"matrix4x4"},	"SUBTOMAT4"},
				}
			},{
				ETokenDesc::MULEQUAL, {
					{{"int",	"int"},		"MULTO"},
					{{"int",	"float"},	"MULTOIF"},
					{{"float",	"int"},		"MULTOFI"},
					{{"float",	"float"},	"MULTOF"},

					{{"vector2",	"vector2"},	"MULTOV2"},
					{{"vector3",	"vector3"},	"MULTOV3"},
					{{"vector4",	"vector4"},	"MULTOV4"},

					{{"vector2",	"float"},	"MULTOV2F"},
					{{"vector3",	"float"},	"MULTOV3F"},
					{{"vector4",	"float"},	"MULTOV4F"},

					{{"vector2",	"int"},		"MULTOV2I"},
					{{"vector3",	"int"},		"MULTOV3I"},
					{{"vector4",	"int"},		"MULTOV4I"},

					{{"vector2",	"matrix2x2"},	"MULTOV2MAT2"},
					{{"vector3",	"matrix3x3"},	"MULTOV3MAT3"},
					{{"vector4",	"matrix4x4"},	"MULTOV4MAT4"},

					{{"matrix2x2",	"matrix2x2"},	"MULTOMAT2"},
					{{"matrix3x3",	"matrix3x3"},	"MULTOMAT3"},
					{{"matrix4x4",	"matrix4x4"},	"MULTOMAT4"},
				}
			},{
				ETokenDesc::DIVEQUAL, {
					{{"int",	"int"},		"DIVTO"},
					{{"int",	"float"},	"DIVTOIF"},
					{{"float",	"int"},		"DIVTOFI"},
					{{"float",	"float"},	"DIVTOF"},

					{{"vector2",	"vector2"},	"DIVTOV2"},
					{{"vector3",	"vector3"},	"DIVTOV3"},
					{{"vector4",	"vector4"},	"DIVTOV4"},

					{{"vector2",	"float"},	"DIVTOV2F"},
					{{"vector3",	"float"},	"DIVTOV3F"},
					{{"vector4",	"float"},	"DIVTOV4F"},

					{{"vector2",	"int"},		"DIVTOV2I"},
					{{"vector3",	"int"},		"DIVTOV3I"},
					{{"vector4",	"int"},		"DIVTOV4I"},

					{{"matrix2x2",	"matrix2x2"},	"DIVTOMAT2"},
					{{"matrix3x3",	"matrix3x3"},	"DIVTOMAT3"},
					{{"matrix4x4",	"matrix4x4"},	"DIVTOMAT4"},
				}
			},{
				ETokenDesc::MODEQUAL, {
					{{"int",	"int"},		"MODTO"},
				}
			},{
				ETokenDesc::ANDEQUAL, {
					{{"int",	"int"},		"ANDTO"},
				}
			}, {
				ETokenDesc::OREQUAL, {
					{{"int",	"int"},		"ORTO"},
				}
			}, {
				ETokenDesc::XOREQUAL, {
					{{"int",	"int"},		"XORTO"},
				}
			}, {
				ETokenDesc::INVERTEQUAL, {
					{{"int",	"int"},		"INVTO"},
				}
			}, {
				ETokenDesc::RSHIFTEQUAL, {
					{{"int",	"int"},		"RSHTO"},
				}
			}, {
				ETokenDesc::LSHIFTEQUAL, {
					{{"int",	"int"},		"LSHTO"},
				}
			},
		};


		static inline std::unordered_map <ETokenDesc, std::unordered_map<std::string, std::string>> TypeUnaryOperatorCompatible
		{
			{
				ETokenDesc::NOT, {
					{ "int", "NOT"},
					{ "float", "NOTF"}
				}
			},{
				ETokenDesc::INVERT, {
					{ "int", "INV"}
				}
			}
		};

		static inline std::unordered_map <std::string, std::unordered_map<std::string, std::string>> TypePushRulerCompatible
		{
			{
				"int", {
					{ "int", "PUSH"},
					{ "float", "PUSHIF"},
				}
			},{
				"float", {
					{ "int", "PUSHFI"},
					{ "float", "PUSHF"},
				}
			},{
				"vector2", {
					{ "vector2", "PUSHV2"},
				}
			},{
				"vector3", {
					{ "vector2", "PUSHV3"},
				}
			},{
				"vector4", {
					{ "vector2", "PUSHV4"},
				}
			},{
				"matrix2x2", {
					{ "matrix2x2", "PUSHMAT2"},
				}
			},{
				"matrix3x3", {
					{ "matrix3x3", "PUSHMAT3"},
				}
			},{
				"matrix4x4", {
					{ "matrix4x4", "PUSHMAT4"},
				}
			},{
				"string", {
					{ "string", "PUSHSTR"},
				}
			}
		};

		// A op B = C 最后一个汇编指令的字符串
		static inline std::unordered_map < ETokenDesc, std::unordered_map<std::pair<std::string, std::string>, std::pair<std::string, std::string>, SStrPairHash>> TypeBinaryOperatorCompatible
		{
			{
				ETokenDesc::ADD, {
					{{"int", "int"}, {"int", "ADD"}},
					{{"int", "float"}, {"float", "ADDIF"}},

					{{"float", "int"}, {"float", "ADDFI" }},
					{{"float", "float"}, {"float", "ADDF"}},

					{{"string", "int"}, {"string", "ADDSTRI"}},
					{{"string", "float"}, {"string", "ADDSTRF"}},
					{{"string", "string"}, {"string", "ADDSTR" }},
					{{"string", "vector2"}, {"string", "ADDSTRV2"}},
					{{"string", "vector3"}, {"string", "ADDSTRV3"}},
					{{"string", "vector4"}, {"string", "ADDSTRV4"}},

					{{"vector2", "vector2"}, {"vector2", "ADDV2"}},
					{{"vector3", "vector3"}, {"vector3", "ADDV3"}},
					{{"vector4", "vector4"}, {"vector4", "ADDV4"} },

					{{"matrix2x2", "matrix2x2"}, {"matrix2x2", "ADDMAT2"}},
					{{"matrix3x3", "matrix3x3"}, {"matrix3x3", "ADDMAT3"}},
					{{"matrix4x4", "matrix4x4"}, {"matrix4x4", "ADDMAT4"} },
				}
			},{
				ETokenDesc::SUB, {
					{ {"int", "int"}, {"int", "SUB"}},
					{ {"int", "float"}, {"float", "SUBIF"}},

					{ {"float", "int"}, {"float", "SUBFI"} },
					{ {"float", "float"}, {"float", "SUBF" }},

					{ {"vector2", "vector2"}, {"vector2", "SUBV2"}},
					{ {"vector3", "vector3"}, {"vector3", "SUBV3"}},
					{ {"vector4", "vector4"}, {"vector4", "SUBV4"}},

					{ {"matrix2x2", "matrix2x2"}, {"matrix2x2", "SUBMAT2" }},
					{ {"matrix3x3", "matrix3x3"}, {"matrix3x3", "SUBMAT3" }},
					{ {"matrix4x4", "matrix4x4"}, {"matrix4x4", "SUBMAT4" }},
				}
			},{
				ETokenDesc::MUL, {
					{{"int", "int"}, {"int", "MUL"}},
					{{"int", "float"}, {"float", "MULIF"}},

					{{"int", "vector2"}, {"vector2", "MULIV2"} },
					{{"int", "vector3"}, {"vector3", "MULIV3" }},
					{{"int", "vector4"}, {"vector4", "MULIV4"} },

					{{"int", "matrix2x2"}, {"matrix2x2", "MULIMAT2" }},
					{{"int", "matrix3x3"}, {"matrix3x3", "MULIMAT3" }},
					{{"int", "matrix4x4"}, {"matrix4x4", "MULIMAT4" }},



					{{"float", "int"}, {"float", "MULFI"} },
					{{"float", "float"}, {"float", "MULF" }},

					{{"float", "vector2"}, {"vector2", "MULFV2"} },
					{{"float", "vector3"}, {"vector3", "MULFV3" }},
					{{"float", "vector4"}, {"vector4", "MULFV4"} },

					{{"float", "matrix2x2"}, {"matrix2x2", "MULFMAT2" }},
					{{"float", "matrix3x3"}, {"matrix3x3", "MULFMAT3" }},
					{{"float", "matrix4x4"}, {"matrix4x4", "MULFMAT4" }},



					{{"vector2", "vector2"}, {"vector2", "MULV2"}},
					{{"vector3", "vector3"}, {"vector3", "MULV3"}},
					{{"vector4", "vector4"}, {"vector4", "MULV4"}},

					{{"vector2", "float"}, {"vector2", "MULV2F"}},
					{{"vector3", "float"}, {"vector3", "MULV3F"}},
					{{"vector4", "float"}, {"vector4", "MULV4F"}},

					{{"vector2", "int"}, {"vector2", "MULV2I"}},
					{{"vector3", "int"}, {"vector3", "MULV3I"}},
					{{"vector4", "int"}, {"vector4", "MULV4I"}},


					//使用行主序列， 向量是列向}量
					{{"matrix2x2", "vector2"}, {"vector2", "MULMAT2V2"}},
					{{"matrix3x3", "vector3"}, {"vector3", "MULMAT3V3"}},
					{{"matrix4x4", "vector4"}, {"vector4", "MULMAT4V4"}},

					{{"matrix2x2", "matrix2x2"}, {"matrix2x2", "MULMAT2"}},
					{{"matrix3x3", "matrix3x3"}, {"matrix3x3", "MULMAT3"}},
					{{"matrix4x4", "matrix4x4"}, {"matrix4x4", "MULMAT4"}},

					{{"matrix2x2", "float"}, {"matrix2x2", "MULMAT2F"}},
					{{"matrix3x3", "float"}, {"matrix3x3", "MULMAT3F"}},
					{{"matrix4x4", "float"}, {"matrix4x4", "MULMAT4F"}},

				}
			},{
				ETokenDesc::DIV, {
					{{"int", "int"}, {"int", "DIV"}},
					{{"int", "float"}, {"float", "DIVIF"}},

					{{"float", "int"}, {"float", "DIVFI"}},
					{{"float", "float"}, {"float", "DIVF"}},

					{{"vector2", "vector2"}, {"vector2", "DIVV2"}},
					{{"vector3", "vector3"}, {"vector3", "DIVV3"}},
					{{"vector4", "vector4"}, {"vector4", "DIVV4"}},

					{{"vector2", "float"}, {"vector2", "DIVV2F"}},
					{{"vector3", "float"}, {"vector3", "DIVV3F"}},
					{{"vector4", "float"}, {"vector4", "DIVV4F"}},

					{{"vector2", "int"}, {"vector2", "DIVV2I"}},
					{{"vector3", "int"}, {"vector3", "DIVV3I"}},
					{{"vector4", "int"}, {"vector4", "DIVV4I"}},

					{{"matrix2x2", "matrix2x2"}, {"matrix2x2", "DIVMAT2"}},
					{{"matrix3x3", "matrix3x3"}, {"matrix3x3", "DIVMAT3"}},
					{{"matrix4x4", "matrix4x4"}, {"matrix4x4", "DIVMAT4"}},
				}
			},{
				ETokenDesc::MOD, {
					{{"int", "int"}, {"int", "MOD"}},
				}
			},{
				ETokenDesc::LSHIFT, {
					{{"int", "int"}, {"int", "LSH"}},
				}
			},{
				ETokenDesc::RSHIFT, {
					{{"int", "int"}, {"int", "RSH"}},
				}
			},{
				ETokenDesc::SMALLER, {
					{{"int", "int"}, {"int", "SCMP"}},
					{{"int", "float"}, {"int", "SCMPIF"}},

					{{"float", "int"}, {"int", "SCMPFI"}},
					{{"float", "float"},{"int", "SCMPF"}},
				}
			},{
				ETokenDesc::LARGER, {
					{{"int", "int"}, {"int", "LCMP"}},
					{{"int", "float"}, {"int", "LCMPIF"}},

					{{"float", "int"}, {"int", "LCMPFI"}},
					{{"float", "float"}, {"int", "LCMPF" }},
				}
			}, {
				ETokenDesc::SMALLEREQUAL, {
					{{"int", "int"}, {"int", "SECMP"}},
					{{"int", "float"}, {"int", "SECMPIF"}},

					{{"float", "int"}, {"int", "SECMPFI"}},
					{{"float", "float"}, {"int", "SECMPF"}},
				}
			}, {
				ETokenDesc::LARGEREQUAL, {
					{{"int", "int"}, {"int", "LECMP"}},
					{{"int", "float"}, {"int", "LECMPIF"}},

					{{"float", "int"}, {"int", "LECMPFI"}},
					{{"float", "float"}, {"int", "LECMPF" }},
				}
			}, {
				ETokenDesc::ISEQUAL, {
					{{"int", "int"}, {"int", "ICMP"}},
					{{"int", "float"}, {"int", "ICMPIF"}},

					{{"string", "string"}, {"int", "ICMPSTR"}},

					{{"float", "int"}, {"int", "ICMPFI" }},
					{{"float", "float"}, {"int", "ICMPF" }},

					{{"vector2", "vector2"}, {"int", "ICMPV2"}},
					{{"vector3", "vector3"}, {"int", "ICMPV3"}},
					{{"vector4", "vector4"}, {"int", "ICMPV4"}},


					{{"matrix2x2", "matrix2x2"}, {"int", "ICMPMAT2"}},
					{{"matrix3x3", "matrix3x3"}, {"int", "ICMPMAT3"}},
					{{"matrix4x4", "matrix4x4"}, {"int", "ICMPMAT4"}},
				}
			}, {
				ETokenDesc::NOTEQUAL, {
					{{"int", "int"}, {"int", "NCMP"}},
					{{"int", "float"}, {"int", "NCMPIF"} },

					{{"string", "string"}, {"int", "NCMPSTR" }},

					{{"float", "int"}, {"int", "NCMPFI" }},
					{{"float", "float"}, {"int", "NCMPF" }},

					{{"vector2", "vector2"}, {"int", "NCMPV2"}},
					{{"vector3", "vector3"}, {"int", "NCMPV3"}},
					{{"vector4", "vector4"}, {"int", "NCMPV4"}},

					{{"matrix2x2", "matrix2x2"}, {"int", "NCMPMAT2"}},
					{{"matrix3x3", "matrix3x3"}, {"int", "NCMPMAT3"}},
					{{"matrix4x4", "matrix4x4"}, {"int", "NCMPMAT4" }},
				}
			}, {
				ETokenDesc::AND, {
					{{"int", "int"}, {"int", "AND"}},
				}
			}, {
				ETokenDesc::OR, {
					{{"int", "int"}, {"int", "OR"}},
				}
			}, {
				ETokenDesc::XOR, {
					{{"int", "int"}, {"int", "XOR"}},
				}
			}, {
				ETokenDesc::LOGICAND, {
					{{"int", "int"}, {"int", "LOGICAND"}},
				}
			}, {
				ETokenDesc::LOGICOR, {
					{{"int", "int"}, {"int", "LOGICOR"}},
				}
			},
		};
	};
};


