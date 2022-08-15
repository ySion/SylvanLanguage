#pragma once
#include "pch.hpp"

#include "SlgCompilerConfig.hpp"
#include "SlgInterpreter.hpp"

namespace SylvanLanguage {

	class RuleTable;
	class RunTimeNetWork;

	struct SModuleInfo;
	struct SModuleAttributeDesc;
	struct SModuleFuntionDesc;

	class SlgTokenItem {
	public:

		RuleTable::TokenValueType mValue;
		ETokenType mType;
		ETokenDesc mDesc;
		int mPriority;
		int mLineNumber;
		int mColNumber;
		SlgTokenItem() : mType(ETokenType::NULLVALUE), mValue(0), mDesc(ETokenDesc::UNKNOWN), mPriority(-1), mLineNumber(-1), mColNumber(-1) {};

		SlgTokenItem(ETokenType t, ETokenDesc desc, RuleTable::TokenValueType v, int lineNumber, int colNumber)
			: mType(t), mValue(v), mDesc(desc), mPriority(RuleTable::OperatorsPriority[desc]), mLineNumber(lineNumber), mColNumber(colNumber) {
		}

		template<class T> T GetValue() const {
			return std::visit([](auto const& val) -> T {
				using V = std::decay_t<decltype(val)>;
				if constexpr (!std::is_constructible_v<T, V>) {
					throw std::runtime_error("Wrong Data Type");
				}
				else {
					return T(val);
				}
				}, mValue);
		}

		inline int GetOperatorProperity() {
			auto itor = RuleTable::OperatorsPriority.find(mDesc);
			if (itor == RuleTable::OperatorsPriority.end()) {
				return -1;
			}
			else {
				return itor->second;
			}
		}
		inline bool IsOperator() {
			return mType == ETokenType::OPERATOR;
		}

		inline bool IsBracket() {
			return mType == ETokenType::BRACKET;
		}

		inline bool IsStatementOperator() {
			return mType == ETokenType::STATEMENTOPERATOR;
		}

		inline bool IsNumber() {
			return ((unsigned short)mType & 0xFF00) == (unsigned short)ETokenType::NUMBER;
		}

		inline bool IsKeyWord() {
			return mType == ETokenType::KEYWORD;
		}

		inline bool IsUnaryOperator() {
			return ((unsigned short)mDesc & (unsigned short)ETokenDesc::UnaryOperator) == (unsigned short)ETokenDesc::UnaryOperator;
		}

		inline bool IsAssignmentOperator() {
			return ((unsigned short)mDesc & (unsigned short)ETokenDesc::AssignmentOperator) == (unsigned short)ETokenDesc::AssignmentOperator;
		}

		inline bool IsBinaryOperator() {
			return ((unsigned short)mDesc & (unsigned short)ETokenDesc::BinaryOperator) == (unsigned short)ETokenDesc::BinaryOperator;
		}

		void Show() {
			switch (mType)
			{
			case ETokenType::IDENTIFIER:
				std::cout << "[IDENTIFIER        |" << TypeDescString(mDesc) << " | Priority: " << mPriority << " \t| pos( " << mLineNumber << "," << mColNumber << " )  " << GetValue<std::string>() << "\n";
				break;
			case ETokenType::INT:
				std::cout << "[NUMBER(int)       |" << TypeDescString(mDesc) << " | Priority: " << mPriority << " \t| pos( " << mLineNumber << "," << mColNumber << ") | " << GetValue<int>() << "\n";
				break;
			case ETokenType::FLOAT:
				std::cout << "[NUMBER(Float)     |" << TypeDescString(mDesc) << " | Priority: " << mPriority << " \t| pos( " << mLineNumber << "," << mColNumber << ") | " << GetValue<RuleTable::SlgFLOAT>() << "\n";
				break;
			case ETokenType::STRING:
				std::cout << "[STRING            |" << TypeDescString(mDesc) << " | Priority: " << mPriority << " \t| pos( " << mLineNumber << "," << mColNumber << ") | " << GetValue<std::string>() << "\n";
				break;
			case ETokenType::BRACKET:
				std::cout << "[BRACKET           |" << TypeDescString(mDesc) << " | Priority: " << mPriority << " \t| pos( " << mLineNumber << "," << mColNumber << ") | " << GetValue<std::string>() << "\n";
				break;
			case ETokenType::OPERATOR:
				std::cout << "[OPERATOR          |" << TypeDescString(mDesc) << " | Priority: " << mPriority << " \t| pos( " << mLineNumber << "," << mColNumber << ") | " << GetValue<std::string>() << "\n";
				break;
			case ETokenType::KEYWORD:
				std::cout << "[KEYWORLD          |" << TypeDescString(mDesc) << " | Priority: " << mPriority << " \t| pos( " << mLineNumber << "," << mColNumber << ") | " << GetValue<std::string>() << "\n";
				break;
			case ETokenType::STATEMENTOPERATOR:
				std::cout << "[STATEMENTOPERATOR |" << TypeDescString(mDesc) << " | Priority: " << mPriority << " \t| pos( " << mLineNumber << "," << mColNumber << ") | " << GetValue<std::string>() << "\n";
				break;
			}
		}
	};

	class ErrorMachine {
	public:
		struct s_SlgErrorMahchine {
			std::string mErrorMessage{};
			int mErrorLine{};
			int mErrorColumn{};
		};

		std::vector <s_SlgErrorMahchine> mErrors;
		inline void AddError(std::string error, int line, int column) {
			s_SlgErrorMahchine error_machine;
			error_machine.mErrorMessage = error;
			error_machine.mErrorLine = line;
			error_machine.mErrorColumn = column;
			mErrors.push_back(error_machine);
		}

		inline void ShowErrors() {
			for (auto& i : mErrors) {
				std::cout << "Error: " << i.mErrorMessage << " at line " << i.mErrorLine << " column " << i.mErrorColumn << std::endl;
			}
		}

		inline bool Success() {
			return mErrors.size() == 0;
		}
	};

	class Tokenizer {

		int mPointer;
		std::string mCode;
		SlgTokenItem mCurrentToken;
		std::vector<SlgTokenItem> mTokens;
		int mLine = 1;
		int mCol = 0;
		int mPassCharacterCount = 0;

		ErrorMachine* mErrorMachine;
	public:

		Tokenizer(std::string codes, ErrorMachine* errorMachine);

		void StartToken();

		bool Literal();

		SlgTokenItem GetNextToken();

		inline std::vector<SlgTokenItem>& GetTokens() {
			return mTokens;
		}

		inline void PrintTokens() {
			int id = 0;
			for (auto& i : mTokens) {
				std::cout << id << "\t";
				id++;
				i.Show();
			}
		}

	private:
		inline bool IsNumber(int ch) {
			return  (ch >= '0' && ch <= '9');
		}

		inline bool Isalpha(int ch) {
			return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
		}

		inline bool Isalnum(int ch) {
			return IsNumber(ch) || Isalpha(ch);
		}
	};

	struct SLocalVariableDesc {
		std::string mTypeStr;
		size_t mVariableSize{};
		size_t mVariableOffest{};
	};

	struct SLocalVariableCodeBlock {
		size_t mSize;
		std::unordered_map<std::string, SLocalVariableDesc>mLocalVariable{};
	};

	struct SStatementSegment {
		size_t Idx{};

		int BasicType = -1; //0是变量， 1是int， 2是float, 3是string, 4是subStatement, 5是运算符, 6是栈空间，7是寄存器，8是RET返回寄存器, -1是错误
		SlgTokenItem Token;

		int VariableType{};	// type， 0-错误， 1-属性， 2-属性成员变量， 3-属性成员函数， 4-变量， 5-变量的成员变量, 6-变量的成员函数, 7-普通函数
		std::string ModuleName{};
		std::string MainName{};
		std::string MemberName{};
		size_t ArgBegin{};
		size_t EndPos{};

		std::string VarNameDesc{};
		std::string VarTypeDesc = "";

		//寄存器
		int RegisterID = -1;
		int StackIdx = -1;
	};

	class SourceCodeCompile {

		RunTimeNetWork* mNetWork;
		CompilerConfig* mCompilerConfig;
		SModuleInfo* mModuleInfo{};

		std::unique_ptr<Tokenizer> mToken;
		std::vector<SlgTokenItem> mTokens;
		std::unique_ptr<ErrorMachine> mErrorMachine{};

		D87AssemblyWriter mAsmGen{};

		size_t mLocalVariableStackSize{};
		std::vector<SLocalVariableCodeBlock> mLocalVariable{};

		std::array<char, 128> mRegister{  };
	public:
		SourceCodeCompile(std::string codes, CompilerConfig* config, RunTimeNetWork* netWork, SModuleInfo* moduleInfo);

		bool GetResult() const;

		void ShowAsm();

		void ShowProgramTable();
	private:

		void Splitstatement();

		bool D_using(size_t& idx, size_t& bDepth);

		bool D_module(size_t& idx, size_t& bDepth);

		bool D_Declare(size_t& idx, size_t& bDepth, bool statementCompile = false);

		bool IsDeclare(size_t idx);

		bool FunctionDefine(std::string returnType, std::string functionName, size_t& idx);

		bool AttributeDefaultDefine(int r_idx, std::string varType, std::string varName, bool isPrivate);

		bool LocalVariableDefaultDefine(int r_idx, std::string varType, std::string varName, int depth);

		bool AttributeAssignmentDefine(std::string varType, std::string varName, size_t& idx, bool isPrivate);

		bool LocalVariableAssignmentDefine(std::string varType, std::string varName, size_t& idx, int depth);

		bool D_IntentifierAnalysis(size_t& idx, size_t& bDepth, bool StaticCompile = false);

		int GetFreeRegister();

		void FreeAllRegister();

		size_t GetTypeSize(std::string tp);

		SStatementSegment ExprSolver(size_t start_idx, size_t end_idx, int recursionDepth = 0);

		SStatementSegment ExprSolverCall(size_t minprop, size_t maxprop, size_t currentprop, std::vector<SStatementSegment>& items, size_t start, size_t end);

		SStatementSegment SingleStatementSegmentSolver(SStatementSegment& item);

		SStatementSegment FunctionSolver(SStatementSegment& item);

		SStatementSegment VariableSolver(SStatementSegment& item);

		SStatementSegment ConstSolver(SStatementSegment& item);

		SStatementSegment DescGenerator(SStatementSegment& item);

		std::optional<std::string> GetTypeCompatibleAsmForPush(std::string A, std::string B);

		//返回 返回类型 和汇编
		std::optional<std::pair<std::string, std::string>> GetTypeCompatibleAsmForBinaryOperator(ETokenDesc op, std::string A, std::string B);

		std::optional<std::string> GetTypeCompatibleAsmForAssignmentOperator(ETokenDesc op, std::string A, std::string B);

		std::optional<std::string> GetTypeCompatibleAsmForUnaryOperator(ETokenDesc op, std::string A);

		// type， 0-错误， 1-属性， 2-属性成员变量， 3-属性成员函数， 4-变量， 5-变量的成员变量, 6-变量的成员函数, 7-普通函数
		// MName 所在模组的名字
		// VName - 变量 / 属性 / 函数 名字 
		// FName - 成员变量名字 / 成员函数名字 
		// argsBegin, argsEnd 如果有函数， 函数参数的范围， 包含 "(" 和 ")", argsEnd也是分析结束的地址
		bool GetIdentifierSegment(size_t start_idx, size_t limited_end_idx, int& type, std::string& MName, std::string& VName, std::string& FName, size_t& argsBegin, size_t& argsEnd);

		std::optional<CompilerConfig::SInlineFunctionDesc> FindMemberFunction(const std::string& variableType, const std::string& memberFuntionName);

		std::optional<CompilerConfig::SMemberVaribleDesc> FindMemberVariable(const std::string& variableType, const std::string& memberName);

		std::optional<CompilerConfig::SInlineFunctionDesc> FindInlineFunction(const std::string& functionName);

		std::optional<std::pair<size_t, SLocalVariableDesc>> FindLocalVariable(const std::string& varName);

		std::optional<SModuleAttributeDesc> FindAttribute(const std::string& ModuleName, const std::string& varName);

		std::optional<SModuleFuntionDesc> FindCustomFunction(const std::string& ModuleName, const std::string& varName);

		SStatementSegment SovlerAsmUnaryOperator(SStatementSegment& op, SStatementSegment& A);

		SStatementSegment SovlerAsmBinaryOperator(SStatementSegment& op, SStatementSegment& A, SStatementSegment& B);

		SStatementSegment SovlerAsmAssignmentOperator(SStatementSegment& op, SStatementSegment& A, SStatementSegment& B);

		RuleTable::SlgINT IntConstexprCalcValueUnary(ETokenDesc desc, RuleTable::SlgINT value);

		RuleTable::SlgINT IntConstexprCalcValueBinary(ETokenDesc desc, RuleTable::SlgINT A, RuleTable::SlgINT B);

		RuleTable::SlgFLOAT FloatConstexprCalcValueUnary(ETokenDesc desc, RuleTable::SlgFLOAT value);

		RuleTable::SlgFLOAT FloatConstexprCalcValueBinary(ETokenDesc desc, RuleTable::SlgFLOAT A, RuleTable::SlgFLOAT B);
	};
}