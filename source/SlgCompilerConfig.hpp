#pragma once

#include "pch.hpp"
namespace SylvanLanguage {

	class SourceCodeCompile;

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
		KEYWORLD_if,
		KEYWORLD_else,
		KEYWORLD_return,
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
		case ETokenDesc::INVERT:			return "INVERT                    ";
		case ETokenDesc::INVERTEQUAL:			return "INVERTEQUAL               ";
		case ETokenDesc::MODULEACCESS:			return "MODULEACCESS              ";

		case ETokenDesc::IDENTIFIER:			return "IDENTIFIER                ";

		case ETokenDesc::KEYWORLD_using:		return "KEYWORLD_using            ";
		case ETokenDesc::KEYWORLD_module:		return "KEYWORLD_module           ";
		case ETokenDesc::KEYWORLD_export:		return "KEYWORLD_export           ";
		case ETokenDesc::KEYWORLD_if:			return "KEYWORLD_if               ";
		case ETokenDesc::KEYWORLD_else:			return "KEYWORLD_else             ";
		case ETokenDesc::KEYWORLD_for:			return "KEYWORLD_for              ";
		case ETokenDesc::KEYWORLD_break:		return "KEYWORLD_break            ";
		case ETokenDesc::KEYWORLD_continue:		return "KEYWORLD_continue         ";
		case ETokenDesc::KEYWORLD_return:		return "KEYWORLD_return           ";

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

		bool AddString(const std::string& strPtr);

		template<class T>bool AddValue(T p) {
			return AddData(&p, sizeof(T));
		}

		virtual ~AssemblyData();

		constexpr size_t GetAllSize() const {
			return mDataAllSize;
		}

		constexpr size_t GetCurrentSize() const {
			return mDataUsingSize;
		}

		inline void* GetData() {
			return mRoot;
		}

		inline AssemblyData(const AssemblyData& item) {
			mRoot = malloc(item.GetCurrentSize());
			if (mRoot != nullptr && item.mRoot != nullptr) {
				memcpy(mRoot, item.mRoot, item.GetCurrentSize());
			}
		}

		inline AssemblyData& operator=(const AssemblyData& item) {
			mRoot = malloc(item.GetCurrentSize());
			if (mRoot != nullptr && item.mRoot != nullptr) {
				memcpy(mRoot, item.mRoot, item.GetCurrentSize());
			}
		}

		void Show();
	};



#define ASM_BASIC(A) A##_L_C,A##_L_G,A##_L_R  ,A##_L_L,	\
			A##_G_C, A##_G_G, A##_G_R, A##_G_L,	\
			A##_R_C, A##_R_G, A##_R_R, A##_R_L
#define ASM_BASIC_UN(A) A##_C, A##_G, A##_L, A##_R
	enum class ESlgd87Asm {

		CALL = 0,

		M_MATRIAX_SET2,
		M_MATRIAX_SET3,
		M_MATRIAX_SET4,
		M_MATRIAX_SET_ONE2,
		M_MATRIAX_SET_ONE3,
		M_MATRIAX_SET_ONE4,

		M_MATRIAX_SET_LINE2,
		M_MATRIAX_SET_LINE3,
		M_MATRIAX_SET_LINE4,

		M_MATRIAX_SET_COLUMN2,
		M_MATRIAX_SET_COLUMN3,
		M_MATRIAX_SET_COLUMN4,

		M_MATRIAX_GET_ONE2,
		M_MATRIAX_GET_ONE3,
		M_MATRIAX_GET_ONE4,

		M_MATRIAX_GET_LINE2,
		M_MATRIAX_GET_LINE3,
		M_MATRIAX_GET_LINE4,

		M_MATRIAX_GET_COLUMN2,
		M_MATRIAX_GET_COLUMN3,
		M_MATRIAX_GET_COLUMN4,

		SIN,
		SIN2,
		SIN3,
		SIN4,

		COS,
		COS2,
		COS3,
		COS4,

		TAN,
		TAN2,
		TAN3,
		TAN4,

		ASIN,
		ASIN2,
		ASIN3,
		ASIN4,

		ACOS,
		ACOS2,
		ACOS3,
		ACOS4,

		ATAN,
		ATAN2,
		ATAN3,
		ATAN4,

		CROSS,

		DOT2,
		DOT3,
		DOT4,

		LENGTH2,
		LENGTH3,

		DEG2RAD,
		RAD2DEG,

		NORMALIZE2,
		NORMALIZE3,

		DISTANCE2,
		DISTANCE3,

		MAX,
		MIN,

		POW,
		POW2,
		POW3,
		POW4,

		SQRT,
		SQRT2,
		SQRT3,
		SQRT4,
		CLAMP,
		LERP,
		LERP2,
		LERP3,
		LERP4,

		ABS,
		ABS2,
		ABS3,
		ABS4,

		FLOOR,
		CEIL,
		FRACT,

		//基本指令
		ASM_BASIC(MOV),
		ASM_BASIC(MOVIF),
		ASM_BASIC(MOVFI),
		ASM_BASIC(MOVF),
		ASM_BASIC(MOVSTR),
		ASM_BASIC(MOVSTRI),
		ASM_BASIC(MOVSTRF),
		ASM_BASIC(MOVSTRV2),
		ASM_BASIC(MOVSTRV3),
		ASM_BASIC(MOVSTRV4),
		ASM_BASIC(MOVV2),
		ASM_BASIC(MOVV3),
		ASM_BASIC(MOVV4),
		ASM_BASIC(MOVMAT2),
		ASM_BASIC(MOVMAT3),
		ASM_BASIC(MOVMAT4),
		ASM_BASIC(ADDTO),
		ASM_BASIC(ADDTOIF),
		ASM_BASIC(ADDTOFI),
		ASM_BASIC(ADDTOF),
		ASM_BASIC(ADDTOSTR),
		ASM_BASIC(ADDTOSTRI),
		ASM_BASIC(ADDTOSTRF),
		ASM_BASIC(ADDTOSTRV2),
		ASM_BASIC(ADDTOSTRV3),
		ASM_BASIC(ADDTOSTRV4),
		ASM_BASIC(ADDTOV2),
		ASM_BASIC(ADDTOV3),
		ASM_BASIC(ADDTOV4),
		ASM_BASIC(ADDTOMAT2),
		ASM_BASIC(ADDTOMAT3),
		ASM_BASIC(ADDTOMAT4),
		ASM_BASIC(SUBTO),
		ASM_BASIC(SUBTOIF),
		ASM_BASIC(SUBTOFI),
		ASM_BASIC(SUBTOF),
		ASM_BASIC(SUBTOV2),
		ASM_BASIC(SUBTOV3),
		ASM_BASIC(SUBTOV4),
		ASM_BASIC(SUBTOMAT2),
		ASM_BASIC(SUBTOMAT3),
		ASM_BASIC(SUBTOMAT4),
		ASM_BASIC(MULTO),
		ASM_BASIC(MULTOIF),
		ASM_BASIC(MULTOFI),
		ASM_BASIC(MULTOF),
		ASM_BASIC(MULTOV2),
		ASM_BASIC(MULTOV3),
		ASM_BASIC(MULTOV4),
		ASM_BASIC(MULTOV2F),
		ASM_BASIC(MULTOV3F),
		ASM_BASIC(MULTOV4F),
		ASM_BASIC(MULTOV2I),
		ASM_BASIC(MULTOV3I),
		ASM_BASIC(MULTOV4I),
		ASM_BASIC(MULTOV2MAT2),
		ASM_BASIC(MULTOV3MAT3),
		ASM_BASIC(MULTOV4MAT4),
		ASM_BASIC(MULTOMAT2),
		ASM_BASIC(MULTOMAT3),
		ASM_BASIC(MULTOMAT4),
		ASM_BASIC(DIVTO),
		ASM_BASIC(DIVTOIF),
		ASM_BASIC(DIVTOFI),
		ASM_BASIC(DIVTOF),
		ASM_BASIC(DIVTOV2),
		ASM_BASIC(DIVTOV3),
		ASM_BASIC(DIVTOV4),
		ASM_BASIC(DIVTOV2F),
		ASM_BASIC(DIVTOV3F),
		ASM_BASIC(DIVTOV4F),
		ASM_BASIC(DIVTOV2I),
		ASM_BASIC(DIVTOV3I),
		ASM_BASIC(DIVTOV4I),
		ASM_BASIC(DIVTOMAT2),
		ASM_BASIC(DIVTOMAT3),
		ASM_BASIC(DIVTOMAT4),
		ASM_BASIC(MODTO),
		ASM_BASIC(ANDTO),
		ASM_BASIC(ORTO),
		ASM_BASIC(XORTO),
		ASM_BASIC(INVTO),
		ASM_BASIC(RSHTO),
		ASM_BASIC(LSHTO),
		ASM_BASIC(ADD),
		ASM_BASIC(ADDIF),
		ASM_BASIC(ADDFI),
		ASM_BASIC(ADDF),
		ASM_BASIC(ADDSTRI),
		ASM_BASIC(ADDSTRF),
		ASM_BASIC(ADDSTR),
		ASM_BASIC(ADDSTRV2),
		ASM_BASIC(ADDSTRV3),
		ASM_BASIC(ADDSTRV4),
		ASM_BASIC(ADDV2),
		ASM_BASIC(ADDV3),
		ASM_BASIC(ADDV4),
		ASM_BASIC(ADDMAT2),
		ASM_BASIC(ADDMAT3),
		ASM_BASIC(ADDMAT4),
		ASM_BASIC(SUB),
		ASM_BASIC(SUBIF),
		ASM_BASIC(SUBFI),
		ASM_BASIC(SUBF),
		ASM_BASIC(SUBV2),
		ASM_BASIC(SUBV3),
		ASM_BASIC(SUBV4),
		ASM_BASIC(SUBMAT2),
		ASM_BASIC(SUBMAT3),
		ASM_BASIC(SUBMAT4),
		ASM_BASIC(MUL),
		ASM_BASIC(MULIF),
		ASM_BASIC(MULIV2),
		ASM_BASIC(MULIV3),
		ASM_BASIC(MULIV4),
		ASM_BASIC(MULIMAT2),
		ASM_BASIC(MULIMAT3),
		ASM_BASIC(MULIMAT4),
		ASM_BASIC(MULFI),
		ASM_BASIC(MULF),
		ASM_BASIC(MULFV2),
		ASM_BASIC(MULFV3),
		ASM_BASIC(MULFV4),
		ASM_BASIC(MULFMAT2),
		ASM_BASIC(MULFMAT3),
		ASM_BASIC(MULFMAT4),
		ASM_BASIC(MULV2),
		ASM_BASIC(MULV3),
		ASM_BASIC(MULV4),
		ASM_BASIC(MULV2F),
		ASM_BASIC(MULV3F),
		ASM_BASIC(MULV4F),
		ASM_BASIC(MULV2I),
		ASM_BASIC(MULV3I),
		ASM_BASIC(MULV4I),
		ASM_BASIC(MULMAT2V2),
		ASM_BASIC(MULMAT3V3),
		ASM_BASIC(MULMAT4V4),
		ASM_BASIC(MULMAT2),
		ASM_BASIC(MULMAT3),
		ASM_BASIC(MULMAT4),
		ASM_BASIC(MULMAT2F),
		ASM_BASIC(MULMAT3F),
		ASM_BASIC(MULMAT4F),
		ASM_BASIC(DIV),
		ASM_BASIC(DIVIF),
		ASM_BASIC(DIVFI),
		ASM_BASIC(DIVF),
		ASM_BASIC(DIVV2),
		ASM_BASIC(DIVV3),
		ASM_BASIC(DIVV4),
		ASM_BASIC(DIVV2F),
		ASM_BASIC(DIVV3F),
		ASM_BASIC(DIVV4F),
		ASM_BASIC(DIVV2I),
		ASM_BASIC(DIVV3I),
		ASM_BASIC(DIVV4I),
		ASM_BASIC(DIVMAT2),
		ASM_BASIC(DIVMAT3),
		ASM_BASIC(DIVMAT4),
		ASM_BASIC(MOD),
		ASM_BASIC(LSH),
		ASM_BASIC(RSH),
		ASM_BASIC(SCMP),
		ASM_BASIC(SCMPIF),
		ASM_BASIC(SCMPFI),
		ASM_BASIC(SCMPF),
		ASM_BASIC(LCMP),
		ASM_BASIC(LCMPIF),
		ASM_BASIC(LCMPFI),
		ASM_BASIC(LCMPF),
		ASM_BASIC(SECMP),
		ASM_BASIC(SECMPIF),
		ASM_BASIC(SECMPFI),
		ASM_BASIC(SECMPF),
		ASM_BASIC(LECMP),
		ASM_BASIC(LECMPIF),
		ASM_BASIC(LECMPFI),
		ASM_BASIC(LECMPF),
		ASM_BASIC(ICMP),
		ASM_BASIC(ICMPIF),
		ASM_BASIC(ICMPSTR),
		ASM_BASIC(ICMPFI),
		ASM_BASIC(ICMPF),
		ASM_BASIC(ICMPV2),
		ASM_BASIC(ICMPV3),
		ASM_BASIC(ICMPV4),
		ASM_BASIC(ICMPMAT2),
		ASM_BASIC(ICMPMAT3),
		ASM_BASIC(ICMPMAT4),
		ASM_BASIC(NCMP),
		ASM_BASIC(NCMPIF),
		ASM_BASIC(NCMPSTR),
		ASM_BASIC(NCMPFI),
		ASM_BASIC(NCMPF),
		ASM_BASIC(NCMPV2),
		ASM_BASIC(NCMPV3),
		ASM_BASIC(NCMPV4),
		ASM_BASIC(NCMPMAT2),
		ASM_BASIC(NCMPMAT3),
		ASM_BASIC(NCMPMAT4),
		ASM_BASIC(AND),
		ASM_BASIC(OR),
		ASM_BASIC(XOR),
		ASM_BASIC(LOGICAND),
		ASM_BASIC(LOGICOR),



		ASM_BASIC_UN(NOT),
		ASM_BASIC_UN(NOTF),
		ASM_BASIC_UN(INV),
		ASM_BASIC_UN(PUSH),
		ASM_BASIC_UN(PUSHIF),
		ASM_BASIC_UN(PUSHFI),
		ASM_BASIC_UN(PUSHF),
		ASM_BASIC_UN(PUSHV2),
		ASM_BASIC_UN(PUSHV3),
		ASM_BASIC_UN(PUSHV4),
		ASM_BASIC_UN(PUSHMAT2),
		ASM_BASIC_UN(PUSHMAT3),
		ASM_BASIC_UN(PUSHMAT4),
		ASM_BASIC_UN(PUSHSTR),

		POP,
		MARKBP,
		FUNCTIONRET,
		POPSTR,
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
			unsigned short AddressOffest = 0;
		};

		struct SBindingFunctionDesc {
			std::string returnType{};
			std::vector<std::string> arguments{};
			std::function<void(void*)> bindLambda;
		};
	private:
		std::unordered_map<std::string, std::unordered_map<std::string, SInlineFunctionDesc>> InlineMemberFunction{};

		std::unordered_map<std::string, std::unordered_map<std::string, SMemberVaribleDesc>> InlineMemberVariable{};

		std::unordered_map<std::string, SInlineFunctionDesc> InlineFunction{};

		std::unordered_map<std::string, SBindingFunctionDesc> BindingFunction{}; //自定义函数

		std::unordered_map<std::string, double> InlineMarcoFloat{};

#define STRMAPASM(A) {#A, ESlgd87Asm::##A}
#define STRMAPASM_BASIC(A) STRMAPASM(A##_L_C),STRMAPASM(A##_L_G),STRMAPASM(A##_L_R)  ,STRMAPASM(A##_L_L),	\
			STRMAPASM(A##_G_C), STRMAPASM(A##_G_G), STRMAPASM(A##_G_R), STRMAPASM(A##_G_L),	\
			STRMAPASM(A##_R_C), STRMAPASM(A##_R_G), STRMAPASM(A##_R_R), STRMAPASM(A##_R_L)

#define  STRMAPASM_BASIC_UN(A)  STRMAPASM(A##_C), STRMAPASM(A##_G), STRMAPASM(A##_L), STRMAPASM(A##_R)

		std::unordered_map <std::string, ESlgd87Asm> StrMapAsm{

			STRMAPASM(CALL),
			STRMAPASM(POP),
			STRMAPASM(MARKBP),
			STRMAPASM(FUNCTIONRET),
			STRMAPASM(POPSTR),

			STRMAPASM(M_MATRIAX_SET2),
			STRMAPASM(M_MATRIAX_SET3),
			STRMAPASM(M_MATRIAX_SET4),
			STRMAPASM(M_MATRIAX_SET_ONE2),
			STRMAPASM(M_MATRIAX_SET_ONE3),
			STRMAPASM(M_MATRIAX_SET_ONE4),

			STRMAPASM(M_MATRIAX_SET_LINE2),
			STRMAPASM(M_MATRIAX_SET_LINE3),
			STRMAPASM(M_MATRIAX_SET_LINE4),

			STRMAPASM(M_MATRIAX_SET_COLUMN2),
			STRMAPASM(M_MATRIAX_SET_COLUMN3),
			STRMAPASM(M_MATRIAX_SET_COLUMN4),

			STRMAPASM(M_MATRIAX_GET_ONE2),
			STRMAPASM(M_MATRIAX_GET_ONE3),
			STRMAPASM(M_MATRIAX_GET_ONE4),

			STRMAPASM(M_MATRIAX_GET_LINE2),
			STRMAPASM(M_MATRIAX_GET_LINE3),
			STRMAPASM(M_MATRIAX_GET_LINE4),

			STRMAPASM(M_MATRIAX_GET_COLUMN2),
			STRMAPASM(M_MATRIAX_GET_COLUMN3),
			STRMAPASM(M_MATRIAX_GET_COLUMN4),

			STRMAPASM(SIN),
			STRMAPASM(SIN2),
			STRMAPASM(SIN3),
			STRMAPASM(SIN4),

			STRMAPASM(COS),
			STRMAPASM(COS2),
			STRMAPASM(COS3),
			STRMAPASM(COS4),

			STRMAPASM(TAN),
			STRMAPASM(TAN2),
			STRMAPASM(TAN3),
			STRMAPASM(TAN4),

			STRMAPASM(ASIN),
			STRMAPASM(ASIN2),
			STRMAPASM(ASIN3),
			STRMAPASM(ASIN4),

			STRMAPASM(ACOS),
			STRMAPASM(ACOS2),
			STRMAPASM(ACOS3),
			STRMAPASM(ACOS4),

			STRMAPASM(ATAN),
			STRMAPASM(ATAN2),
			STRMAPASM(ATAN3),
			STRMAPASM(ATAN4),

			STRMAPASM(CROSS),

			STRMAPASM(DOT2),
			STRMAPASM(DOT3),
			STRMAPASM(DOT4),

			STRMAPASM(LENGTH2),
			STRMAPASM(LENGTH3),

			STRMAPASM(DEG2RAD),
			STRMAPASM(RAD2DEG),

			STRMAPASM(NORMALIZE2),
			STRMAPASM(NORMALIZE3),

			STRMAPASM(DISTANCE2),
			STRMAPASM(DISTANCE3),

			STRMAPASM(MAX),
			STRMAPASM(MIN),

			STRMAPASM(POW),
			STRMAPASM(POW2),
			STRMAPASM(POW3),
			STRMAPASM(POW4),

			STRMAPASM(SQRT),
			STRMAPASM(SQRT2),
			STRMAPASM(SQRT3),
			STRMAPASM(SQRT4),
			STRMAPASM(CLAMP),
			STRMAPASM(LERP),
			STRMAPASM(LERP2),
			STRMAPASM(LERP3),
			STRMAPASM(LERP4),

			STRMAPASM(ABS),
			STRMAPASM(ABS2),
			STRMAPASM(ABS3),
			STRMAPASM(ABS4),

			STRMAPASM(FLOOR),
			STRMAPASM(CEIL),
			STRMAPASM(FRACT),



			//以上是函数

				STRMAPASM_BASIC(MOV),
				STRMAPASM_BASIC(MOVIF),
				STRMAPASM_BASIC(MOVFI),
				STRMAPASM_BASIC(MOVF),
				STRMAPASM_BASIC(MOVSTR),
				STRMAPASM_BASIC(MOVSTRI),
				STRMAPASM_BASIC(MOVSTRF),
				STRMAPASM_BASIC(MOVSTRV2),
				STRMAPASM_BASIC(MOVSTRV3),
				STRMAPASM_BASIC(MOVSTRV4),
				STRMAPASM_BASIC(MOVV2),
				STRMAPASM_BASIC(MOVV3),
				STRMAPASM_BASIC(MOVV4),
				STRMAPASM_BASIC(MOVMAT2),
				STRMAPASM_BASIC(MOVMAT3),
				STRMAPASM_BASIC(MOVMAT4),
				STRMAPASM_BASIC(ADDTO),
				STRMAPASM_BASIC(ADDTOIF),
				STRMAPASM_BASIC(ADDTOFI),
				STRMAPASM_BASIC(ADDTOF),
				STRMAPASM_BASIC(ADDTOSTR),
				STRMAPASM_BASIC(ADDTOSTRI),
				STRMAPASM_BASIC(ADDTOSTRF),
				STRMAPASM_BASIC(ADDTOSTRV2),
				STRMAPASM_BASIC(ADDTOSTRV3),
				STRMAPASM_BASIC(ADDTOSTRV4),
				STRMAPASM_BASIC(ADDTOV2),
				STRMAPASM_BASIC(ADDTOV3),
				STRMAPASM_BASIC(ADDTOV4),
				STRMAPASM_BASIC(ADDTOMAT2),
				STRMAPASM_BASIC(ADDTOMAT3),
				STRMAPASM_BASIC(ADDTOMAT4),
				STRMAPASM_BASIC(SUBTO),
				STRMAPASM_BASIC(SUBTOIF),
				STRMAPASM_BASIC(SUBTOFI),
				STRMAPASM_BASIC(SUBTOF),
				STRMAPASM_BASIC(SUBTOV2),
				STRMAPASM_BASIC(SUBTOV3),
				STRMAPASM_BASIC(SUBTOV4),
				STRMAPASM_BASIC(SUBTOMAT2),
				STRMAPASM_BASIC(SUBTOMAT3),
				STRMAPASM_BASIC(SUBTOMAT4),
				STRMAPASM_BASIC(MULTO),
				STRMAPASM_BASIC(MULTOIF),
				STRMAPASM_BASIC(MULTOFI),
				STRMAPASM_BASIC(MULTOF),
				STRMAPASM_BASIC(MULTOV2),
				STRMAPASM_BASIC(MULTOV3),
				STRMAPASM_BASIC(MULTOV4),
				STRMAPASM_BASIC(MULTOV2F),
				STRMAPASM_BASIC(MULTOV3F),
				STRMAPASM_BASIC(MULTOV4F),
				STRMAPASM_BASIC(MULTOV2I),
				STRMAPASM_BASIC(MULTOV3I),
				STRMAPASM_BASIC(MULTOV4I),
				STRMAPASM_BASIC(MULTOV2MAT2),
				STRMAPASM_BASIC(MULTOV3MAT3),
				STRMAPASM_BASIC(MULTOV4MAT4),
				STRMAPASM_BASIC(MULTOMAT2),
				STRMAPASM_BASIC(MULTOMAT3),
				STRMAPASM_BASIC(MULTOMAT4),
				STRMAPASM_BASIC(DIVTO),
				STRMAPASM_BASIC(DIVTOIF),
				STRMAPASM_BASIC(DIVTOFI),
				STRMAPASM_BASIC(DIVTOF),
				STRMAPASM_BASIC(DIVTOV2),
				STRMAPASM_BASIC(DIVTOV3),
				STRMAPASM_BASIC(DIVTOV4),
				STRMAPASM_BASIC(DIVTOV2F),
				STRMAPASM_BASIC(DIVTOV3F),
				STRMAPASM_BASIC(DIVTOV4F),
				STRMAPASM_BASIC(DIVTOV2I),
				STRMAPASM_BASIC(DIVTOV3I),
				STRMAPASM_BASIC(DIVTOV4I),
				STRMAPASM_BASIC(DIVTOMAT2),
				STRMAPASM_BASIC(DIVTOMAT3),
				STRMAPASM_BASIC(DIVTOMAT4),
				STRMAPASM_BASIC(MODTO),
				STRMAPASM_BASIC(ANDTO),
				STRMAPASM_BASIC(ORTO),
				STRMAPASM_BASIC(XORTO),
				STRMAPASM_BASIC(INVTO),
				STRMAPASM_BASIC(RSHTO),
				STRMAPASM_BASIC(LSHTO),
				STRMAPASM_BASIC(ADD),
				STRMAPASM_BASIC(ADDIF),
				STRMAPASM_BASIC(ADDFI),
				STRMAPASM_BASIC(ADDF),
				STRMAPASM_BASIC(ADDSTRI),
				STRMAPASM_BASIC(ADDSTRF),
				STRMAPASM_BASIC(ADDSTR),
				STRMAPASM_BASIC(ADDSTRV2),
				STRMAPASM_BASIC(ADDSTRV3),
				STRMAPASM_BASIC(ADDSTRV4),
				STRMAPASM_BASIC(ADDV2),
				STRMAPASM_BASIC(ADDV3),
				STRMAPASM_BASIC(ADDV4),
				STRMAPASM_BASIC(ADDMAT2),
				STRMAPASM_BASIC(ADDMAT3),
				STRMAPASM_BASIC(ADDMAT4),
				STRMAPASM_BASIC(SUB),
				STRMAPASM_BASIC(SUBIF),
				STRMAPASM_BASIC(SUBFI),
				STRMAPASM_BASIC(SUBF),
				STRMAPASM_BASIC(SUBV2),
				STRMAPASM_BASIC(SUBV3),
				STRMAPASM_BASIC(SUBV4),
				STRMAPASM_BASIC(SUBMAT2),
				STRMAPASM_BASIC(SUBMAT3),
				STRMAPASM_BASIC(SUBMAT4),
				STRMAPASM_BASIC(MUL),
				STRMAPASM_BASIC(MULIF),
				STRMAPASM_BASIC(MULIV2),
				STRMAPASM_BASIC(MULIV3),
				STRMAPASM_BASIC(MULIV4),
				STRMAPASM_BASIC(MULIMAT2),
				STRMAPASM_BASIC(MULIMAT3),
				STRMAPASM_BASIC(MULIMAT4),
				STRMAPASM_BASIC(MULFI),
				STRMAPASM_BASIC(MULF),
				STRMAPASM_BASIC(MULFV2),
				STRMAPASM_BASIC(MULFV3),
				STRMAPASM_BASIC(MULFV4),
				STRMAPASM_BASIC(MULFMAT2),
				STRMAPASM_BASIC(MULFMAT3),
				STRMAPASM_BASIC(MULFMAT4),
				STRMAPASM_BASIC(MULV2),
				STRMAPASM_BASIC(MULV3),
				STRMAPASM_BASIC(MULV4),
				STRMAPASM_BASIC(MULV2F),
				STRMAPASM_BASIC(MULV3F),
				STRMAPASM_BASIC(MULV4F),
				STRMAPASM_BASIC(MULV2I),
				STRMAPASM_BASIC(MULV3I),
				STRMAPASM_BASIC(MULV4I),
				STRMAPASM_BASIC(MULMAT2V2),
				STRMAPASM_BASIC(MULMAT3V3),
				STRMAPASM_BASIC(MULMAT4V4),
				STRMAPASM_BASIC(MULMAT2),
				STRMAPASM_BASIC(MULMAT3),
				STRMAPASM_BASIC(MULMAT4),
				STRMAPASM_BASIC(MULMAT2F),
				STRMAPASM_BASIC(MULMAT3F),
				STRMAPASM_BASIC(MULMAT4F),
				STRMAPASM_BASIC(DIV),
				STRMAPASM_BASIC(DIVIF),
				STRMAPASM_BASIC(DIVFI),
				STRMAPASM_BASIC(DIVF),
				STRMAPASM_BASIC(DIVV2),
				STRMAPASM_BASIC(DIVV3),
				STRMAPASM_BASIC(DIVV4),
				STRMAPASM_BASIC(DIVV2F),
				STRMAPASM_BASIC(DIVV3F),
				STRMAPASM_BASIC(DIVV4F),
				STRMAPASM_BASIC(DIVV2I),
				STRMAPASM_BASIC(DIVV3I),
				STRMAPASM_BASIC(DIVV4I),
				STRMAPASM_BASIC(DIVMAT2),
				STRMAPASM_BASIC(DIVMAT3),
				STRMAPASM_BASIC(DIVMAT4),
				STRMAPASM_BASIC(MOD),
				STRMAPASM_BASIC(LSH),
				STRMAPASM_BASIC(RSH),
				STRMAPASM_BASIC(SCMP),
				STRMAPASM_BASIC(SCMPIF),
				STRMAPASM_BASIC(SCMPFI),
				STRMAPASM_BASIC(SCMPF),
				STRMAPASM_BASIC(LCMP),
				STRMAPASM_BASIC(LCMPIF),
				STRMAPASM_BASIC(LCMPFI),
				STRMAPASM_BASIC(LCMPF),
				STRMAPASM_BASIC(SECMP),
				STRMAPASM_BASIC(SECMPIF),
				STRMAPASM_BASIC(SECMPFI),
				STRMAPASM_BASIC(SECMPF),
				STRMAPASM_BASIC(LECMP),
				STRMAPASM_BASIC(LECMPIF),
				STRMAPASM_BASIC(LECMPFI),
				STRMAPASM_BASIC(LECMPF),
				STRMAPASM_BASIC(ICMP),
				STRMAPASM_BASIC(ICMPIF),
				STRMAPASM_BASIC(ICMPSTR),
				STRMAPASM_BASIC(ICMPFI),
				STRMAPASM_BASIC(ICMPF),
				STRMAPASM_BASIC(ICMPV2),
				STRMAPASM_BASIC(ICMPV3),
				STRMAPASM_BASIC(ICMPV4),
				STRMAPASM_BASIC(ICMPMAT2),
				STRMAPASM_BASIC(ICMPMAT3),
				STRMAPASM_BASIC(ICMPMAT4),
				STRMAPASM_BASIC(NCMP),
				STRMAPASM_BASIC(NCMPIF),
				STRMAPASM_BASIC(NCMPSTR),
				STRMAPASM_BASIC(NCMPFI),
				STRMAPASM_BASIC(NCMPF),
				STRMAPASM_BASIC(NCMPV2),
				STRMAPASM_BASIC(NCMPV3),
				STRMAPASM_BASIC(NCMPV4),
				STRMAPASM_BASIC(NCMPMAT2),
				STRMAPASM_BASIC(NCMPMAT3),
				STRMAPASM_BASIC(NCMPMAT4),
				STRMAPASM_BASIC(AND),
				STRMAPASM_BASIC(OR),
				STRMAPASM_BASIC(XOR),
				STRMAPASM_BASIC(LOGICAND),
				STRMAPASM_BASIC(LOGICOR),



				STRMAPASM_BASIC_UN(PUSH),
				STRMAPASM_BASIC_UN(PUSHIF),
				STRMAPASM_BASIC_UN(PUSHFI),
				STRMAPASM_BASIC_UN(PUSHF),
				STRMAPASM_BASIC_UN(PUSHV2),
				STRMAPASM_BASIC_UN(PUSHV3),
				STRMAPASM_BASIC_UN(PUSHV4),
				STRMAPASM_BASIC_UN(PUSHMAT2),
				STRMAPASM_BASIC_UN(PUSHMAT3),
				STRMAPASM_BASIC_UN(PUSHMAT4),
				STRMAPASM_BASIC_UN(PUSHSTR),
				STRMAPASM_BASIC_UN(NOT),
				STRMAPASM_BASIC_UN(NOTF),
				STRMAPASM_BASIC_UN(INV),

		};

		inline void AddMemberFunction(std::string varTypeName, std::string memberFunctionName, std::string ret, std::vector<std::string> args, std::string asmb) {
			InlineMemberFunction[varTypeName][memberFunctionName] = SInlineFunctionDesc{ ret, args, asmb };
		}

		inline void AddMemberVariable(std::string varTypeName, std::string memberVarName, std::string type, bool isConstant, unsigned short offest) {
			InlineMemberVariable[varTypeName][memberVarName] = SMemberVaribleDesc{ type, isConstant, offest };
		}

		inline void AddStandradFunction(std::string functionName, std::string ret, std::vector<std::string> args, std::string asmb) {
			InlineFunction[functionName] = SInlineFunctionDesc{ ret, args, asmb };
		}

		inline void AddMarcoFloat(std::string MarcoName, double value) {
			InlineMarcoFloat[MarcoName] = value;
		}
	public:
		inline std::optional<SInlineFunctionDesc> FindMemberFunction(const std::string& variableType, const std::string& memberFunctionName) {

			auto itor1 = InlineMemberFunction.find(variableType);

			if (itor1 != InlineMemberFunction.end()) {
				auto itor2 = itor1->second.find(memberFunctionName);
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

		inline std::optional<SBindingFunctionDesc> FindBindingFunction(const std::string& functionName) {
			auto itor1 = BindingFunction.find(functionName);

			if (itor1 != BindingFunction.end()) {
				return itor1->second;
			}

			return std::nullopt;
		}


		template<class F>
		inline bool AddBindingFunction(std::string functionName, const std::vector<std::string>& type, const std::string retType, const F func) {

		}

		inline bool RemoveBindingFunction(std::string functionName) {

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
			AddMemberVariable("vector2", "x", "float", false, 0);
			AddMemberVariable("vector2", "y", "float", false, 8);

			AddMemberVariable("vector3", "x", "float", false, 0);
			AddMemberVariable("vector3", "y", "float", false, 8);
			AddMemberVariable("vector3", "z", "float", false, 16);

			AddMemberVariable("vector4", "x", "float", false, 0);
			AddMemberVariable("vector4", "y", "float", false, 8);
			AddMemberVariable("vector4", "z", "float", false, 16);
			AddMemberVariable("vector4", "w", "float", false, 24);


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

		inline std::optional<ESlgd87Asm> FindAsm(const std::string& asmStr) {
			auto itor = StrMapAsm.find(asmStr);
			if (itor == StrMapAsm.end()) {
				return std::nullopt;
			}
			else {
				return itor->second;
			}
		}

	private:
		unsigned short AsmIdx = 0x8000;
	};


	class D87AssemblyWriter {
	public:
		AssemblyData* data;
		CompilerConfig* mConfig;

		inline D87AssemblyWriter(AssemblyData* AsmData, CompilerConfig* cfg) : data(AsmData), mConfig(cfg) {}
		//二元运算符 和 赋值

		inline bool BIN_OPERATE(const std::string& ASM, const std::string& A, const std::string& B,
			const std::string& AModulePath, const std::string& BModulePath,
			int offestA, int offestB,
			unsigned short memberOffestA, unsigned short memberOffestB,
			short regA, short regB,
			int constTypeA, int constTypeB,
			long long conLLA, long long conLLB,
			double conDA, double conDB,
			std::string conSA, std::string conSB) {

			std::string STRASM = ASM;
			if (regA != -1) {
				STRASM += "_R";
			}
			else if (AModulePath != "") {
				STRASM += "_G";
			}
			else if (constTypeA != 0) {
				return false;
			}
			else {
				STRASM += "_L";
			}

			if (regB != -1) {
				STRASM += "_R";
			}
			else if (BModulePath != "") {
				STRASM += "_G";
			}
			else if (constTypeB != 0) {
				STRASM += "_C";
			}
			else {
				STRASM += "_L";
			}

			auto it = FindAsm(STRASM);

			if (it.has_value()) {
				data->AddValue<unsigned short>((unsigned short)it.value());
				std::cout << STRASM << " ";
				if (regA != -1) {  // 寄存器
					data->AddValue<unsigned char>(regA);

					if (regA == 127) {
						std::cout << regA << "(RET), ";
					}
					else {
						std::cout << regA << ", ";
					}
				}
				else if (AModulePath != "") { //全局变量
					data->AddString(AModulePath);
					data->AddString(A);

					std::cout << AModulePath << " ";
					std::cout << A << " ";
					std::cout << memberOffestA << ", ";
				}
				else if (constTypeA != 0) { //常量
					return false;
				}
				else {//局部变量(栈上变量)
					data->AddValue<int>(offestA + memberOffestA);
					data->AddValue<unsigned short>(memberOffestA);

					std::cout << offestA << "+";
					std::cout << memberOffestA << "=";
					std::cout << offestA + memberOffestA << ", ";
				}


				//B
				if (regB != -1) {
					data->AddValue<unsigned char>(regB);

					if (regB == 127) {
						std::cout << regB << "(RET)\n";
					}
					else {
						std::cout << regB << "\n";
					}
				}
				else if (BModulePath != "") {

					data->AddString(BModulePath);
					data->AddString(B);
					data->AddValue<unsigned short>(memberOffestB);

					std::cout << BModulePath << " ";
					std::cout << B << " ";
					std::cout << memberOffestB << "\n";
				}
				else if (constTypeB != 0) {

					if (constTypeB == 1) {
						data->AddValue<long long>(conLLB);
						std::cout << conLLB << "\n";

					}
					else if (constTypeB == 2) {
						data->AddValue<double>(conDB);
						std::cout << conDB << "\n";
					}
					else if (constTypeB == 3) {
						data->AddString(conSB);
						std::cout << conSB << "\n";
					}
				}
				else {
					data->AddValue<int>(offestB + memberOffestB);

					std::cout << offestB << "+";
					std::cout << memberOffestB << "=";
					std::cout << offestB + memberOffestB << "\n";
				}

				return true;
			}
			else {
				std::cout << "Error: Can not Find Asm :  " << STRASM << "\n";
			}
			return false;
		}


		//一元运算符
		inline bool UNARY_OPERATE(const std::string& ASM, const std::string& A, const std::string& AModulePath, int offestA, unsigned short memberOffestA, short regA, int constType, long long conLL, double conD, std::string conS) {

			std::string STRASM = ASM;
			if (regA != -1) {
				STRASM += "_R";
			}
			else if (AModulePath != "") {
				STRASM += "_G";
			}
			else if (constType != 0) {
				STRASM += "_C";
			}
			else {
				STRASM += "_L";
			}


			auto it = FindAsm(STRASM);
			if (it.has_value()) {
				data->AddValue<unsigned short>((unsigned short)it.value());
				std::cout << STRASM << " ";
				if (regA != -1) { // 寄存器

					data->AddValue<unsigned char>(regA);


					if (regA == 127) {
						std::cout << regA << "(RET)\n";
					}
					else {
						std::cout << regA << "\n";
					}
				}
				else if (AModulePath != "") { //全局变量

					data->AddString(AModulePath);
					data->AddString(A);
					data->AddValue<unsigned short>(memberOffestA);

					std::cout << AModulePath << " ";
					std::cout << A << " ";
					std::cout << memberOffestA << "\n";
				}
				else if (constType != 0) { // 常量

					if (constType == 1) {
						data->AddValue<long long>(conLL);
						printf("%lld\n", conLL);
					}
					else if (constType == 2) {
						data->AddValue<double>(conD);
						printf("%llf\n", conD);
					}
					else if (constType == 3) {
						data->AddString(conS);
						printf("%s\n", conS.c_str());
					}
				}
				else { // 局部变量
					data->AddValue<int>(offestA + memberOffestA);

					std::cout << offestA << "+";
					std::cout << memberOffestA << "=";
					std::cout << offestA + memberOffestA << "\n";
				}

				return true;
			}
			else {
				std::cout << "Error: Can not Find Asm :  " << STRASM << "\n";
			}
			return false;
		}

		inline void POP_STR(int offest) {
			auto it = FindAsm("POPSTR");
			if (it.has_value()) {
				std::cout << "POPSTR ";
				data->AddValue<unsigned short>((unsigned short)it.value());
				data->AddValue<int>(offest);

				std::cout << offest << "\n";
			}
		}

		inline bool CALL_INLINE(const std::string& functionName) {
			auto it = FindAsm(functionName);
			if (it.has_value()) {
				std::cout << functionName << "\n";
				data->AddValue<unsigned short>((unsigned short)it.value());
				return true;
			}
			return false;
		}

		inline bool CALL(const std::string& moduleName, const std::string& functionName) {
			auto it = FindAsm("CALL");
			if (it.has_value()) {
				std::cout << "CALL ";

				data->AddValue<unsigned short>((unsigned short)it.value());
				data->AddString(moduleName);
				data->AddString(functionName);

				std::cout << moduleName << " ";
				std::cout << functionName << "\n";
				return true;
			}
			return false;
		}

		inline bool CALL_MEMBER_FUNC(const std::string& functionName, const std::string& modulePath, const std::string& globalVarName, int localvarOffest) {
			auto it = FindAsm(functionName);
			if (it.has_value()) {
				std::cout << functionName << " ";
				data->AddValue<unsigned short>((unsigned short)it.value());
				if (globalVarName != "") {
					data->AddValue<unsigned char>((unsigned char)0x01);

					std::cout << "0x01 ";
					data->AddString(modulePath);
					data->AddString(globalVarName);

					std::cout << modulePath << " ";
					std::cout << globalVarName << "\n";
				}
				else {
					data->AddValue<unsigned char>((unsigned char)0x00);
					std::cout << "0x01 ";
					data->AddValue<int>(localvarOffest);
					std::cout << localvarOffest << "\n";
				}
				return true;
			}
			return false;
		}

		inline void MARK_BP(int offest) {
			auto it = FindAsm("MARKBP");
			if (it.has_value()) {
				data->AddValue<unsigned short>((unsigned short)it.value());
				data->AddValue<int>(offest);
				std::cout << "MARKBP" << " " << offest << "\n";
			}
		}

		inline void FUNTCION_RET() {
			auto it = FindAsm("FUNCTIONRET");
			if (it.has_value()) {
				data->AddValue<unsigned short>((unsigned short)it.value());
				std::cout << "FUNCTIONRET" << "\n";
			}
		}

	private:
		inline std::optional<ESlgd87Asm> FindAsm(const std::string& asmStr) {
			return mConfig->FindAsm(asmStr);
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
			{"if",		ETokenDesc::KEYWORLD_if},
			{"else",	ETokenDesc::KEYWORLD_else},
			{"return",	ETokenDesc::KEYWORLD_return},
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
		};
	};

	class TypeRuler {
	public:
		static inline std::unordered_map<std::string, size_t> SlgTypeSize{
			{"void", 8},
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


