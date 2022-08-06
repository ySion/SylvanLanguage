#pragma once
#include <iostream>
#include <unordered_map>

class SlgAssemblyData {

	void* m_Root;

	size_t m_DataAllSize = 0;
	size_t m_DataUsingSize = 0;

	inline void ExternSize(size_t newSize) {
		m_Root = realloc(m_Root, newSize);
		m_DataAllSize = newSize;
	}
public:
	inline SlgAssemblyData(size_t size) : m_DataAllSize(size) {
		m_Root = malloc(size);
	}

	inline void AddData(const void* p, size_t size) {
		if (m_DataUsingSize + size > m_DataAllSize) {
			ExternSize(m_DataUsingSize + size + 4096);
		}
		memcpy((unsigned char*)m_Root + m_DataUsingSize, p, size);
		m_DataUsingSize += size;
	}

	inline void AddString(std::string& strPtr) {
		AddData(strPtr.c_str(), strPtr.size() + 1);
	}

	template<class T>void AddValue(T p) {
		AddData(&p, sizeof(T));
	}

	inline virtual ~SlgAssemblyData() {
		free(m_Root);
	}
	inline size_t GetAllSize() const {
		return m_DataAllSize;
	}

	inline size_t GetCurrentSize() const {
		return m_DataUsingSize;
	}

	inline void Show() {
		for (size_t i = 0; i < m_DataUsingSize; i++) {
			printf("%02X  ", ((unsigned char*)m_Root)[i]);


			if ((i + 1) % 16 == 0) {
				printf("      ");
				for (size_t j = i - 15; j <= i; j++) {
					if (isprint(((unsigned char*)m_Root)[j])) {
						printf("%c ", ((unsigned char*)m_Root)[j]);
					}
					else {
						printf(". ");
					}

				}
				printf("\n");
			}
		}
		printf("      ");
		for (size_t j = m_DataUsingSize - m_DataUsingSize % 16; j < m_DataUsingSize; j++) {

			if (isprint(((unsigned char*)m_Root)[j])) {
				printf("%c ", ((unsigned char*)m_Root)[j]);
			}
			else {
				printf(". ");
			}
		}
	}
};

enum class Slgd87Asm {
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

std::unordered_map<std::string, size_t> SlgTypeSize{
	{"int", 8},
	{"float", 8},
	{"string", 8},
	{"vector2", 8},
	{"vector3", 16},
	{"vector4", 16},
	{"matrix2x2", 16},
	{"matrix3x3", 36},
	{"matrix4x4", 64},
};

enum class e_SlgExternType {
	NONE = 0x00,
	LIST,
	MAP
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

std::unordered_map<std::string, e_SlgType> SlgTypeAsm{
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

std::unordered_map<std::string, e_SlgExternType> SlgExternTypeAsm{
	{"list",    e_SlgExternType::LIST,},
	{"map",     e_SlgExternType::MAP,},
};

std::unordered_set<std::string> SlgSupportExternTypeAsm{
	{"vector2 list"},
	{"vector3 list"},
	{"vector4 list"},
	{"int list"},
	{"float list"},
};

class SlgCompilerConfig {

public:
	struct s_SlgFunction {
		std::string returnType;
		std::vector<std::string> arguments;
		Slgd87Asm assembly;
	};

	struct s_SlgMemberVarible {
		std::string Type;
		bool isConstant = false;
	};


	std::unordered_map<std::string, std::unordered_map<std::string, s_SlgFunction>> MemberFunction{};

	std::unordered_map<std::string, std::unordered_map<std::string, s_SlgMemberVarible>> MemberVariable{};

	std::unordered_map<std::string, s_SlgFunction> StandardFunction{};

	std::unordered_map<std::string, double> MarcoFloat{};

	std::unordered_map<std::string, unsigned short>Slgd87AsmTable{};


	void AddMemberFunction(std::string varTypeName, std::string memberFunctionName, std::string ret, std::vector<std::string> args, Slgd87Asm asmb) {
		MemberFunction[varTypeName][memberFunctionName] = s_SlgFunction{ ret, args, asmb };
	}

	void AddMemberVariable(std::string varTypeName, std::string memberVarName, std::string type, bool isConstant) {
		MemberVariable[varTypeName][memberVarName] = s_SlgMemberVarible{ type, isConstant };
	}

	void AddStandradFunction(std::string functionName, std::string ret, std::vector<std::string> args, Slgd87Asm asmb) {
		StandardFunction[functionName] = s_SlgFunction{ ret, args, asmb };
	}

	void AddMarcoFloat(std::string MarcoName, double value) {
		MarcoFloat[MarcoName] = value;
	}


	SlgCompilerConfig() {

		AddMemberFunction("matrix2x2", "getline", "vector2", { "int" }, Slgd87Asm::M_MATRIAX_GET_LINE2);
		AddMemberFunction("matrix3x3", "getline", "vector3", { "int" }, Slgd87Asm::M_MATRIAX_GET_LINE3);
		AddMemberFunction("matrix4x4", "getline", "vector4", { "int" }, Slgd87Asm::M_MATRIAX_GET_LINE4);

		AddMemberFunction("matrix2x2", "getcolumn", "vector2", { "int" }, Slgd87Asm::M_MATRIAX_GET_COLUMN2);
		AddMemberFunction("matrix3x3", "getcolumn", "vector3", { "int" }, Slgd87Asm::M_MATRIAX_GET_COLUMN3);
		AddMemberFunction("matrix4x4", "getcolumn", "vector4", { "int" }, Slgd87Asm::M_MATRIAX_GET_COLUMN4);

		AddMemberFunction("matrix2x2", "getone", "float", { "int", "int" }, Slgd87Asm::M_MATRIAX_GET_ONE2);
		AddMemberFunction("matrix3x3", "getone", "float", { "int", "int" }, Slgd87Asm::M_MATRIAX_GET_ONE3);
		AddMemberFunction("matrix4x4", "getone", "float", { "int", "int" }, Slgd87Asm::M_MATRIAX_GET_ONE4);

		AddMemberFunction("matrix2x2", "setline", "void", { "int", "vector2" }, Slgd87Asm::M_MATRIAX_SET_LINE2);
		AddMemberFunction("matrix3x3", "setline", "void", { "int", "vector3" }, Slgd87Asm::M_MATRIAX_SET_LINE3);
		AddMemberFunction("matrix4x4", "setline", "void", { "int", "vector4" }, Slgd87Asm::M_MATRIAX_SET_LINE4);

		AddMemberFunction("matrix2x2", "setcolumn", "void", { "int", "vector2" }, Slgd87Asm::M_MATRIAX_SET_COLUMN2);
		AddMemberFunction("matrix3x3", "setcolumn", "void", { "int", "vector3" }, Slgd87Asm::M_MATRIAX_SET_COLUMN3);
		AddMemberFunction("matrix4x4", "setcolumn", "void", { "int", "vector4" }, Slgd87Asm::M_MATRIAX_SET_COLUMN4);

		AddMemberFunction("matrix2x2", "setone", "float", { "int", "int" }, Slgd87Asm::M_MATRIAX_SET_ONE2);
		AddMemberFunction("matrix3x3", "setone", "float", { "int", "int" }, Slgd87Asm::M_MATRIAX_SET_ONE3);
		AddMemberFunction("matrix4x4", "setone", "float", { "int", "int" }, Slgd87Asm::M_MATRIAX_SET_ONE4);


		AddMemberFunction("matrix2x2", "set", "void", { "float", "float",
								"float", "float" }, Slgd87Asm::M_MATRIAX_SET2);

		AddMemberFunction("matrix3x3", "set", "void", { "float", "float", "float",
								"float", "float", "float",
								"float", "float", "float" }, Slgd87Asm::M_MATRIAX_SET3);

		AddMemberFunction("matrix4x4", "set", "void", { "float", "float", "float", "float",
								"float", "float", "float", "float" ,
								"float", "float", "float", "float" ,
								"float", "float", "float", "float" }, Slgd87Asm::M_MATRIAX_SET4);


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
		AddStandradFunction("sin", "float", { "float" }, Slgd87Asm::SIN);
		AddStandradFunction("sin2", "vector2", { "vector2" }, Slgd87Asm::SIN2);
		AddStandradFunction("sin3", "vector3", { "vector3" }, Slgd87Asm::SIN3);
		AddStandradFunction("sin4", "vector4", { "vector4" }, Slgd87Asm::SIN4);

		AddStandradFunction("cos", "float", { "float" }, Slgd87Asm::COS);
		AddStandradFunction("cos2", "vector2", { "vector2" }, Slgd87Asm::COS2);
		AddStandradFunction("cos3", "vector3", { "vector3" }, Slgd87Asm::COS3);
		AddStandradFunction("cos4", "vector4", { "vector4" }, Slgd87Asm::COS4);

		AddStandradFunction("tan", "float", { "float" }, Slgd87Asm::TAN);
		AddStandradFunction("tan2", "vector2", { "vector2" }, Slgd87Asm::TAN2);
		AddStandradFunction("tan3", "vector3", { "vector3" }, Slgd87Asm::TAN3);
		AddStandradFunction("tan4", "vector4", { "vector4" }, Slgd87Asm::TAN4);

		AddStandradFunction("arcsin", "float", { "float" }, Slgd87Asm::ASIN);
		AddStandradFunction("arcsin2", "vector2", { "vector2" }, Slgd87Asm::ASIN2);
		AddStandradFunction("arcsin3", "vector3", { "vector3" }, Slgd87Asm::ASIN3);
		AddStandradFunction("arcsin4", "vector4", { "vector4" }, Slgd87Asm::ASIN4);

		AddStandradFunction("arccos", "float", { "float" }, Slgd87Asm::ACOS);
		AddStandradFunction("arccos2", "vector2", { "vector2" }, Slgd87Asm::ACOS2);
		AddStandradFunction("arccos3", "vector3", { "vector3" }, Slgd87Asm::ACOS3);
		AddStandradFunction("arccos4", "vector4", { "vector4" }, Slgd87Asm::ACOS4);

		AddStandradFunction("arctan", "float", { "float" }, Slgd87Asm::ATAN);
		AddStandradFunction("arctan2", "vector2", { "vector2" }, Slgd87Asm::ATAN2);
		AddStandradFunction("arctan3", "vector3", { "vector3" }, Slgd87Asm::ATAN3);
		AddStandradFunction("arctan4", "vector4", { "vector4" }, Slgd87Asm::ATAN4);

		AddStandradFunction("cross", "vector3", { "vector3", "vector3" }, Slgd87Asm::CROSS);

		AddStandradFunction("dot2", "float", { "vector2", "vector2" }, Slgd87Asm::DOT2);
		AddStandradFunction("dot3", "float", { "vector3", "vector3" }, Slgd87Asm::DOT3);
		AddStandradFunction("dot4", "float", { "vector4", "vector4" }, Slgd87Asm::DOT4);

		AddStandradFunction("dot2", "float", { "vector2", "vector2" }, Slgd87Asm::DOT2);
		AddStandradFunction("dot3", "float", { "vector3", "vector3" }, Slgd87Asm::DOT3);
		AddStandradFunction("dot4", "float", { "vector4", "vector4" }, Slgd87Asm::DOT4);

		AddStandradFunction("length2", "float", { "vector2" }, Slgd87Asm::LENGTH2);
		AddStandradFunction("length3", "float", { "vector3" }, Slgd87Asm::LENGTH3);

		AddStandradFunction("normalize2", "vector2", { "vector2" }, Slgd87Asm::NORMALIZE2);
		AddStandradFunction("normalize3", "vector3", { "vector3" }, Slgd87Asm::NORMALIZE3);

		AddStandradFunction("DegToRad", "float", { "float" }, Slgd87Asm::DEG2RAD);
		AddStandradFunction("RadToDeg", "float", { "float" }, Slgd87Asm::RAD2DEG);

		AddStandradFunction("Deg2Rad", "float", { "float" }, Slgd87Asm::DEG2RAD);
		AddStandradFunction("Rad2Deg", "float", { "float" }, Slgd87Asm::RAD2DEG);

		AddStandradFunction("distance2", "vector2", { "vector2" }, Slgd87Asm::DISTANCE2);
		AddStandradFunction("distance3", "vector3", { "vector3" }, Slgd87Asm::DISTANCE3);

		AddStandradFunction("max", "float", { "float", "float" }, Slgd87Asm::MAX);
		AddStandradFunction("min", "float", { "float", "float" }, Slgd87Asm::MIN);

		AddStandradFunction("pow", "float", { "float", "float" }, Slgd87Asm::POW);
		AddStandradFunction("pow2", "vector2", { "vector2", "float" }, Slgd87Asm::POW2);
		AddStandradFunction("pow3", "vector3", { "vector3", "float" }, Slgd87Asm::POW3);
		AddStandradFunction("pow4", "vector4", { "vector4", "float" }, Slgd87Asm::POW4);

		AddStandradFunction("sqrt", "float", { "float" }, Slgd87Asm::SQRT);
		AddStandradFunction("sqrt2", "vector2", { "vector2" }, Slgd87Asm::SQRT2);
		AddStandradFunction("sqrt3", "vector3", { "vector3" }, Slgd87Asm::SQRT3);
		AddStandradFunction("sqrt4", "vector4", { "vector4" }, Slgd87Asm::SQRT4);

		AddStandradFunction("clamp", "float", { "float", "float", "float" }, Slgd87Asm::CLAMP);

		AddStandradFunction("lerp", "float", { "float", "float", "float" }, Slgd87Asm::LERP);
		AddStandradFunction("lerp2", "vector2", { "vector2", "vector2","float" }, Slgd87Asm::LERP2);
		AddStandradFunction("lerp3", "vector3", { "vector3", "vector3","float" }, Slgd87Asm::LERP3);
		AddStandradFunction("lerp4", "vector4", { "vector4", "vector4","float" }, Slgd87Asm::LERP4);

		AddStandradFunction("abs", "float", { "float" }, Slgd87Asm::ABS);
		AddStandradFunction("abs2", "vector2", { "vector2" }, Slgd87Asm::ABS2);
		AddStandradFunction("abs3", "vector3", { "vector3" }, Slgd87Asm::ABS3);
		AddStandradFunction("abs4", "vector4", { "vector4" }, Slgd87Asm::ABS4);


		AddStandradFunction("floor", "float", { "float" }, Slgd87Asm::FLOOR);
		AddStandradFunction("ceil", "float", { "float" }, Slgd87Asm::CEIL);
		AddStandradFunction("fract", "float", { "float" }, Slgd87Asm::FRACT);

	}

	unsigned short AsmIdx = 0x8000;
};


class Slgd87AssemblyGenerator {
	SlgAssemblyData* data;
public:
	inline Slgd87AssemblyGenerator() : data(nullptr) {}
	inline Slgd87AssemblyGenerator(SlgAssemblyData* AsmData) : data(AsmData) {}

	inline void BindAsmData(SlgAssemblyData* AsmData) { data = AsmData; }

	inline void MODULE_DEP(std::string& path) {
		data->AddValue<unsigned short>((unsigned short)Slgd87Asm::MODULE_DEP);
		data->AddString(path);
	}

	inline void MODULE_SELF(std::string& path) {
		data->AddValue<unsigned short>((unsigned short)Slgd87Asm::MODULE_SELF);
		data->AddString(path);
	}

	inline void GLOBAL_VAR_MALLOC(size_t memSize) {
		data->AddValue<unsigned short>((unsigned short)Slgd87Asm::GLOBAL_VAR_MALLOC);
		data->AddValue<size_t>(memSize);
	}

	inline void ATTRITUBE_LET(std::string name, e_SlgType type, e_SlgExternType externType, bool isPrivateAttribute) {
		data->AddValue<unsigned short>((unsigned short)Slgd87Asm::ATTRITUBE_LET);
		data->AddString(name);
		data->AddValue<unsigned short>((unsigned short)type);
		data->AddValue<unsigned short>((unsigned short)externType);
		data->AddValue<unsigned char>((unsigned char)(isPrivateAttribute ? 1 : 0));
	}
};