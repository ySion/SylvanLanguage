#pragma once

#include "pch.hpp"

namespace SylvanLanguage {

	enum class ETokenType {
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
		KEYWORLD_in,
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
		case ETokenDesc::KEYWORLD_in:			return "KEYWORLD_in               ";

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

	enum class ESlgd87Asm {
		MODULE_DEP = 0x8001,
		MODULE_SELF,
		ATTRITUBE_LET,
		GLOBAL_VAR_MALLOC,

		MOVI,
		ATT_MOVI,
		MOVF,
		ATT_MOVF,
		MOVS,
		ATT_MOVS,


		M_MATRIAX_SET2,
		ATT_M_MATRIAX_SET2,
		M_MATRIAX_SET3,
		ATT_M_MATRIAX_SET3,
		M_MATRIAX_SET4,
		ATT_M_MATRIAX_SET4,

		M_MATRIAX_SET_ONE2,
		ATT_M_MATRIAX_SET_ONE2,
		M_MATRIAX_SET_ONE3,
		ATT_M_MATRIAX_SET_ONE3,
		M_MATRIAX_SET_ONE4,
		ATT_M_MATRIAX_SET_ONE4,

		M_MATRIAX_SET_LINE2,
		ATT_M_MATRIAX_SET_LINE2,
		M_MATRIAX_SET_LINE3,
		ATT_M_MATRIAX_SET_LINE3,
		M_MATRIAX_SET_LINE4,
		ATT_M_MATRIAX_SET_LINE4,

		M_MATRIAX_SET_COLUMN2,
		ATT_M_MATRIAX_SET_COLUMN2,
		M_MATRIAX_SET_COLUMN3,
		ATT_M_MATRIAX_SET_COLUMN3,
		M_MATRIAX_SET_COLUMN4,
		ATT_M_MATRIAX_SET_COLUMN4,

		M_MATRIAX_GET_ONE2,
		ATT_M_MATRIAX_GET_ONE2,
		M_MATRIAX_GET_ONE3,
		ATT_M_MATRIAX_GET_ONE3,
		M_MATRIAX_GET_ONE4,
		ATT_M_MATRIAX_GET_ONE4,

		M_MATRIAX_GET_LINE2,
		ATT_M_MATRIAX_GET_LINE2,
		M_MATRIAX_GET_LINE3,
		ATT_M_MATRIAX_GET_LINE3,
		M_MATRIAX_GET_LINE4,
		ATT_M_MATRIAX_GET_LINE4,

		M_MATRIAX_GET_COLUMN2,
		ATT_M_MATRIAX_GET_COLUMN2,
		M_MATRIAX_GET_COLUMN3,
		ATT_M_MATRIAX_GET_COLUMN3,
		M_MATRIAX_GET_COLUMN4,
		ATT_M_MATRIAX_GET_COLUMN4,

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
		FRACT
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
			std::string returnType;
			std::vector<std::string> arguments;
			ESlgd87Asm assembly;
		};

		struct SMemberVaribleDesc {
			std::string Type;
			bool isConstant = false;
		};


		std::unordered_map<std::string, std::unordered_map<std::string, SInlineFunctionDesc>> InlineMemberFunction{};

		std::unordered_map<std::string, std::unordered_map<std::string, SMemberVaribleDesc>> InlineMemberVariable{};

		std::unordered_map<std::string, SInlineFunctionDesc> InlineFunction{};

		std::unordered_map<std::string, double> InlineMarcoFloat{};

		inline void AddMemberFunction(std::string varTypeName, std::string memberFunctionName, std::string ret, std::vector<std::string> args, ESlgd87Asm asmb) {
			InlineMemberFunction[varTypeName][memberFunctionName] = SInlineFunctionDesc{ ret, args, asmb };
		}

		inline void AddMemberVariable(std::string varTypeName, std::string memberVarName, std::string type, bool isConstant) {
			InlineMemberVariable[varTypeName][memberVarName] = SMemberVaribleDesc{ type, isConstant };
		}

		inline void AddStandradFunction(std::string functionName, std::string ret, std::vector<std::string> args, ESlgd87Asm asmb) {
			InlineFunction[functionName] = SInlineFunctionDesc{ ret, args, asmb };
		}

		inline void AddMarcoFloat(std::string MarcoName, double value) {
			InlineMarcoFloat[MarcoName] = value;
		}

		inline CompilerConfig() {

			AddMemberFunction("matrix2x2", "getline", "vector2", { "int" }, ESlgd87Asm::M_MATRIAX_GET_LINE2);
			AddMemberFunction("matrix3x3", "getline", "vector3", { "int" }, ESlgd87Asm::M_MATRIAX_GET_LINE3);
			AddMemberFunction("matrix4x4", "getline", "vector4", { "int" }, ESlgd87Asm::M_MATRIAX_GET_LINE4);

			AddMemberFunction("matrix2x2", "getcolumn", "vector2", { "int" }, ESlgd87Asm::M_MATRIAX_GET_COLUMN2);
			AddMemberFunction("matrix3x3", "getcolumn", "vector3", { "int" }, ESlgd87Asm::M_MATRIAX_GET_COLUMN3);
			AddMemberFunction("matrix4x4", "getcolumn", "vector4", { "int" }, ESlgd87Asm::M_MATRIAX_GET_COLUMN4);

			AddMemberFunction("matrix2x2", "getone", "float", { "int", "int" }, ESlgd87Asm::M_MATRIAX_GET_ONE2);
			AddMemberFunction("matrix3x3", "getone", "float", { "int", "int" }, ESlgd87Asm::M_MATRIAX_GET_ONE3);
			AddMemberFunction("matrix4x4", "getone", "float", { "int", "int" }, ESlgd87Asm::M_MATRIAX_GET_ONE4);

			AddMemberFunction("matrix2x2", "setline", "void", { "int", "vector2" }, ESlgd87Asm::M_MATRIAX_SET_LINE2);
			AddMemberFunction("matrix3x3", "setline", "void", { "int", "vector3" }, ESlgd87Asm::M_MATRIAX_SET_LINE3);
			AddMemberFunction("matrix4x4", "setline", "void", { "int", "vector4" }, ESlgd87Asm::M_MATRIAX_SET_LINE4);

			AddMemberFunction("matrix2x2", "setcolumn", "void", { "int", "vector2" }, ESlgd87Asm::M_MATRIAX_SET_COLUMN2);
			AddMemberFunction("matrix3x3", "setcolumn", "void", { "int", "vector3" }, ESlgd87Asm::M_MATRIAX_SET_COLUMN3);
			AddMemberFunction("matrix4x4", "setcolumn", "void", { "int", "vector4" }, ESlgd87Asm::M_MATRIAX_SET_COLUMN4);

			AddMemberFunction("matrix2x2", "setone", "float", { "int", "int" }, ESlgd87Asm::M_MATRIAX_SET_ONE2);
			AddMemberFunction("matrix3x3", "setone", "float", { "int", "int" }, ESlgd87Asm::M_MATRIAX_SET_ONE3);
			AddMemberFunction("matrix4x4", "setone", "float", { "int", "int" }, ESlgd87Asm::M_MATRIAX_SET_ONE4);


			AddMemberFunction("matrix2x2", "set", "void", { "float", "float",
									"float", "float" }, ESlgd87Asm::M_MATRIAX_SET2);

			AddMemberFunction("matrix3x3", "set", "void", { "float", "float", "float",
									"float", "float", "float",
									"float", "float", "float" }, ESlgd87Asm::M_MATRIAX_SET3);

			AddMemberFunction("matrix4x4", "set", "void", { "float", "float", "float", "float",
									"float", "float", "float", "float" ,
									"float", "float", "float", "float" ,
									"float", "float", "float", "float" }, ESlgd87Asm::M_MATRIAX_SET4);


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
			AddStandradFunction("sin", "float", { "float" }, ESlgd87Asm::SIN);
			AddStandradFunction("sin2", "vector2", { "vector2" }, ESlgd87Asm::SIN2);
			AddStandradFunction("sin3", "vector3", { "vector3" }, ESlgd87Asm::SIN3);
			AddStandradFunction("sin4", "vector4", { "vector4" }, ESlgd87Asm::SIN4);

			AddStandradFunction("cos", "float", { "float" }, ESlgd87Asm::COS);
			AddStandradFunction("cos2", "vector2", { "vector2" }, ESlgd87Asm::COS2);
			AddStandradFunction("cos3", "vector3", { "vector3" }, ESlgd87Asm::COS3);
			AddStandradFunction("cos4", "vector4", { "vector4" }, ESlgd87Asm::COS4);

			AddStandradFunction("tan", "float", { "float" }, ESlgd87Asm::TAN);
			AddStandradFunction("tan2", "vector2", { "vector2" }, ESlgd87Asm::TAN2);
			AddStandradFunction("tan3", "vector3", { "vector3" }, ESlgd87Asm::TAN3);
			AddStandradFunction("tan4", "vector4", { "vector4" }, ESlgd87Asm::TAN4);

			AddStandradFunction("arcsin", "float", { "float" }, ESlgd87Asm::ASIN);
			AddStandradFunction("arcsin2", "vector2", { "vector2" }, ESlgd87Asm::ASIN2);
			AddStandradFunction("arcsin3", "vector3", { "vector3" }, ESlgd87Asm::ASIN3);
			AddStandradFunction("arcsin4", "vector4", { "vector4" }, ESlgd87Asm::ASIN4);

			AddStandradFunction("arccos", "float", { "float" }, ESlgd87Asm::ACOS);
			AddStandradFunction("arccos2", "vector2", { "vector2" }, ESlgd87Asm::ACOS2);
			AddStandradFunction("arccos3", "vector3", { "vector3" }, ESlgd87Asm::ACOS3);
			AddStandradFunction("arccos4", "vector4", { "vector4" }, ESlgd87Asm::ACOS4);

			AddStandradFunction("arctan", "float", { "float" }, ESlgd87Asm::ATAN);
			AddStandradFunction("arctan2", "vector2", { "vector2" }, ESlgd87Asm::ATAN2);
			AddStandradFunction("arctan3", "vector3", { "vector3" }, ESlgd87Asm::ATAN3);
			AddStandradFunction("arctan4", "vector4", { "vector4" }, ESlgd87Asm::ATAN4);

			AddStandradFunction("cross", "vector3", { "vector3", "vector3" }, ESlgd87Asm::CROSS);

			AddStandradFunction("dot2", "float", { "vector2", "vector2" }, ESlgd87Asm::DOT2);
			AddStandradFunction("dot3", "float", { "vector3", "vector3" }, ESlgd87Asm::DOT3);
			AddStandradFunction("dot4", "float", { "vector4", "vector4" }, ESlgd87Asm::DOT4);

			AddStandradFunction("dot2", "float", { "vector2", "vector2" }, ESlgd87Asm::DOT2);
			AddStandradFunction("dot3", "float", { "vector3", "vector3" }, ESlgd87Asm::DOT3);
			AddStandradFunction("dot4", "float", { "vector4", "vector4" }, ESlgd87Asm::DOT4);

			AddStandradFunction("length2", "float", { "vector2" }, ESlgd87Asm::LENGTH2);
			AddStandradFunction("length3", "float", { "vector3" }, ESlgd87Asm::LENGTH3);

			AddStandradFunction("normalize2", "vector2", { "vector2" }, ESlgd87Asm::NORMALIZE2);
			AddStandradFunction("normalize3", "vector3", { "vector3" }, ESlgd87Asm::NORMALIZE3);

			AddStandradFunction("DegToRad", "float", { "float" }, ESlgd87Asm::DEG2RAD);
			AddStandradFunction("RadToDeg", "float", { "float" }, ESlgd87Asm::RAD2DEG);

			AddStandradFunction("Deg2Rad", "float", { "float" }, ESlgd87Asm::DEG2RAD);
			AddStandradFunction("Rad2Deg", "float", { "float" }, ESlgd87Asm::RAD2DEG);

			AddStandradFunction("distance2", "vector2", { "vector2" }, ESlgd87Asm::DISTANCE2);
			AddStandradFunction("distance3", "vector3", { "vector3" }, ESlgd87Asm::DISTANCE3);

			AddStandradFunction("max", "float", { "float", "float" }, ESlgd87Asm::MAX);
			AddStandradFunction("min", "float", { "float", "float" }, ESlgd87Asm::MIN);

			AddStandradFunction("pow", "float", { "float", "float" }, ESlgd87Asm::POW);
			AddStandradFunction("pow2", "vector2", { "vector2", "float" }, ESlgd87Asm::POW2);
			AddStandradFunction("pow3", "vector3", { "vector3", "float" }, ESlgd87Asm::POW3);
			AddStandradFunction("pow4", "vector4", { "vector4", "float" }, ESlgd87Asm::POW4);

			AddStandradFunction("sqrt", "float", { "float" }, ESlgd87Asm::SQRT);
			AddStandradFunction("sqrt2", "vector2", { "vector2" }, ESlgd87Asm::SQRT2);
			AddStandradFunction("sqrt3", "vector3", { "vector3" }, ESlgd87Asm::SQRT3);
			AddStandradFunction("sqrt4", "vector4", { "vector4" }, ESlgd87Asm::SQRT4);

			AddStandradFunction("clamp", "float", { "float", "float", "float" }, ESlgd87Asm::CLAMP);

			AddStandradFunction("lerp", "float", { "float", "float", "float" }, ESlgd87Asm::LERP);
			AddStandradFunction("lerp2", "vector2", { "vector2", "vector2","float" }, ESlgd87Asm::LERP2);
			AddStandradFunction("lerp3", "vector3", { "vector3", "vector3","float" }, ESlgd87Asm::LERP3);
			AddStandradFunction("lerp4", "vector4", { "vector4", "vector4","float" }, ESlgd87Asm::LERP4);

			AddStandradFunction("abs", "float", { "float" }, ESlgd87Asm::ABS);
			AddStandradFunction("abs2", "vector2", { "vector2" }, ESlgd87Asm::ABS2);
			AddStandradFunction("abs3", "vector3", { "vector3" }, ESlgd87Asm::ABS3);
			AddStandradFunction("abs4", "vector4", { "vector4" }, ESlgd87Asm::ABS4);


			AddStandradFunction("floor", "float", { "float" }, ESlgd87Asm::FLOOR);
			AddStandradFunction("ceil", "float", { "float" }, ESlgd87Asm::CEIL);
			AddStandradFunction("fract", "float", { "float" }, ESlgd87Asm::FRACT);

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
			{"in",		ETokenDesc::KEYWORLD_in}
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
					{{"float",	"float"},	"MOV"},

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
					{{"float",	"float"},	"ADDTO"},

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
					{{"float",	"float"},	"SUBTO"},

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
					{{"float",	"float"},	"MULTO"},

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
					{{"float",	"float"},	"DIVTO"},

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
					{{"int", "float"}, {"float", "DIVF"}},

					{{"float", "int"}, {"float", "DIVF"}},
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
					{{"int", "float"}, {"int", "SCMPF"}},

					{{"float", "int"}, {"int", "SCMPF"}},
					{{"float", "float"},{"int", "SCMPF"}},
				}
			},{
				ETokenDesc::LARGER, {
					{{"int", "int"}, {"int", "LCMP"}},
					{{"int", "float"}, {"int", "LCMPF"}},

					{{"float", "int"}, {"int", "LCMPF"}},
					{{"float", "float"}, {"int", "LCMPF" }},
				}
			}, {
				ETokenDesc::SMALLEREQUAL, {
					{{"int", "int"}, {"int", "SECMP"}},
					{{"int", "float"}, {"int", "SECMPF"}},

					{{"float", "int"}, {"int", "SECMPF"}},
					{{"float", "float"}, {"int", "SECMPF"}},
				}
			}, {
				ETokenDesc::LARGEREQUAL, {
					{{"int", "int"}, {"int", "LECMP"}},
					{{"int", "float"}, {"int", "LECMPF"}},

					{{"float", "int"}, {"int", "LECMPF"}},
					{{"float", "float"}, {"int", "LECMPF" }},
				}
			}, {
				ETokenDesc::ISEQUAL, {
					{{"int", "int"}, {"int", "ICMP"}},
					{{"int", "float"}, {"int", "ICMPF"}},

					{{"string", "string"}, {"int", "ICMPSTR"}},

					{{"float", "int"}, {"int", "ICMPF" }},
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
					{{"int", "float"}, {"int", "NCMPF"} },

					{{"string", "string"}, {"int", "NCMPSTR" }},

					{{"float", "int"}, {"int", "NCMPF" }},
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


