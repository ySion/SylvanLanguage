#pragma once
#include "pch.hpp"

#include "SlgCompilerConfig.hpp"
#include "SlgRulers.hpp"


class SlgRuleTable;

SlgRuleTable g_RuleTable;
SlgCompilerConfig g_Config;

class SlgTokenItem {
public:



	SlgRuleTable::TokenValueType m_Value;
	SlgRuleTable::e_SlgTokenType m_Type;
	SlgRuleTable::e_SlgTokenDesc m_Desc;
	int m_Priority;
	int m_LineNumber;
	int m_ColNumber;
	SlgTokenItem() : m_Type(SlgRuleTable::e_SlgTokenType::NULLVALUE), m_Value(0), m_Desc(SlgRuleTable::e_SlgTokenDesc::UNKNOWN), m_Priority(-1), m_LineNumber(-1), m_ColNumber(-1) {};

	SlgTokenItem(SlgRuleTable::e_SlgTokenType t, SlgRuleTable::e_SlgTokenDesc desc, SlgRuleTable::TokenValueType v, int lineNumber, int colNumber)
		: m_Type(t), m_Value(v), m_Desc(desc), m_Priority(g_RuleTable.OperatorsPriority[desc]), m_LineNumber(lineNumber), m_ColNumber(colNumber) {
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
			}, m_Value);
	}

	inline int GetOperatorProperity() {
		auto itor = g_RuleTable.OperatorsPriority.find(m_Desc);
		if (itor == g_RuleTable.OperatorsPriority.end()) {
			return -1;
		}
		else {
			return itor->second;
		}
	}
	inline bool IsOperator() {
		return m_Type == SlgRuleTable::e_SlgTokenType::OPERATOR;
	}

	inline bool IsBracket() {
		return m_Type == SlgRuleTable::e_SlgTokenType::BRACKET;
	}

	inline bool IsStatementOperator() {
		return m_Type == SlgRuleTable::e_SlgTokenType::STATEMENTOPERATOR;
	}

	inline bool IsNumber() {
		return ((unsigned short)m_Type & 0xFF00) == (unsigned short)SlgRuleTable::e_SlgTokenType::NUMBER;
	}

	inline bool IsKeyWord() {
		return m_Type == SlgRuleTable::e_SlgTokenType::KEYWORD;
	}

	inline bool IsUnaryOperator() {
		return ((unsigned short)m_Desc & (unsigned short)SlgRuleTable::e_SlgTokenDesc::UnaryOperator) == (unsigned short)SlgRuleTable::e_SlgTokenDesc::UnaryOperator;
	}

	inline bool IsAssignmentOperator() {
		return ((unsigned short)m_Desc & (unsigned short)SlgRuleTable::e_SlgTokenDesc::AssignmentOperator) == (unsigned short)SlgRuleTable::e_SlgTokenDesc::AssignmentOperator;
	}

	inline bool IsBinaryOperator() {
		return ((unsigned short)m_Desc & (unsigned short)SlgRuleTable::e_SlgTokenDesc::BinaryOperator) == (unsigned short)SlgRuleTable::e_SlgTokenDesc::BinaryOperator;
	}

	void Show() {
		switch (m_Type)
		{
		case SlgRuleTable::e_SlgTokenType::IDENTIFIER:
			std::cout << "[IDENTIFIER        |" << TypeDescString(m_Desc) << " | Priority: " << m_Priority << " \t| pos( " << m_LineNumber << "," << m_ColNumber << " )  " << GetValue<std::string>() << "\n";
			break;
		case SlgRuleTable::e_SlgTokenType::INT:
			std::cout << "[NUMBER(int)       |" << TypeDescString(m_Desc) << " | Priority: " << m_Priority << " \t| pos( " << m_LineNumber << "," << m_ColNumber << ") | " << GetValue<int>() << "\n";
			break;
		case SlgRuleTable::e_SlgTokenType::FLOAT:
			std::cout << "[NUMBER(Float)     |" << TypeDescString(m_Desc) << " | Priority: " << m_Priority << " \t| pos( " << m_LineNumber << "," << m_ColNumber << ") | " << GetValue<SlgRuleTable::SlgFLOAT>() << "\n";
			break;
		case SlgRuleTable::e_SlgTokenType::STRING:
			std::cout << "[STRING            |" << TypeDescString(m_Desc) << " | Priority: " << m_Priority << " \t| pos( " << m_LineNumber << "," << m_ColNumber << ") | " << GetValue<std::string>() << "\n";
			break;
		case SlgRuleTable::e_SlgTokenType::BRACKET:
			std::cout << "[BRACKET           |" << TypeDescString(m_Desc) << " | Priority: " << m_Priority << " \t| pos( " << m_LineNumber << "," << m_ColNumber << ") | " << GetValue<std::string>() << "\n";
			break;
		case SlgRuleTable::e_SlgTokenType::OPERATOR:
			std::cout << "[OPERATOR          |" << TypeDescString(m_Desc) << " | Priority: " << m_Priority << " \t| pos( " << m_LineNumber << "," << m_ColNumber << ") | " << GetValue<std::string>() << "\n";
			break;
		case SlgRuleTable::e_SlgTokenType::KEYWORD:
			std::cout << "[KEYWORLD          |" << TypeDescString(m_Desc) << " | Priority: " << m_Priority << " \t| pos( " << m_LineNumber << "," << m_ColNumber << ") | " << GetValue<std::string>() << "\n";
			break;
		case SlgRuleTable::e_SlgTokenType::STATEMENTOPERATOR:
			std::cout << "[STATEMENTOPERATOR |" << TypeDescString(m_Desc) << " | Priority: " << m_Priority << " \t| pos( " << m_LineNumber << "," << m_ColNumber << ") | " << GetValue<std::string>() << "\n";
			break;
		}
	}
};

class SlgErrorMachine {
public:
	struct s_SlgErrorMahchine {
		std::string m_ErrorMessage{};
		int m_ErrorLine{};
		int m_ErrorColumn{};
		SlgRuleTable::SlgRuleTable::e_SlgErrorNumber m_ErrorNumber{};
	};

	std::vector <s_SlgErrorMahchine> m_Errors;
	void AddError(std::string error, int line, int column, SlgRuleTable::SlgRuleTable::e_SlgErrorNumber number) {
		s_SlgErrorMahchine error_machine;
		error_machine.m_ErrorMessage = error;
		error_machine.m_ErrorLine = line;
		error_machine.m_ErrorColumn = column;
		error_machine.m_ErrorNumber = number;
		m_Errors.push_back(error_machine);
	}

	void ShowErrors() {
		for (auto& i : m_Errors) {
			std::cout << "Error: " << i.m_ErrorMessage << " at line " << i.m_ErrorLine << " column " << i.m_ErrorColumn << std::endl;
		}
	}

	bool Success() {
		return m_Errors.size() == 0;
	}
};

class SlgTokenizer {

	int m_Pointer;
	std::string m_Code;
	SlgTokenItem m_CurrentToken;
	std::vector<SlgTokenItem> m_Tokens;
	int m_Line = 1;
	int m_Col = 0;
	int m_PassCharacterCount = 0;

	SlgErrorMachine* m_ErrorMachine;
public:

	inline SlgTokenizer(std::string line, SlgErrorMachine* errorMachine) : m_Code(line), m_Pointer(0), m_ErrorMachine(errorMachine) {
		StartToken();
	}

	inline void StartToken() {
		while (m_Code.length() > m_Pointer) {
			m_CurrentToken = GetNextToken();
			if (!Literal()) { return; }
		}
	}

	inline bool Literal() {

		decltype(g_RuleTable.KeyWord)::iterator itor;
		switch ((SlgRuleTable::e_SlgTokenType)((int)m_CurrentToken.m_Type & 0xFF00))
		{
		case SlgRuleTable::e_SlgTokenType::ERROR:
			m_ErrorMachine->AddError(m_CurrentToken.GetValue<std::string>(), m_CurrentToken.m_LineNumber, 0, SlgRuleTable::e_SlgErrorNumber::ERROR_ILLEGAL_LINE_CHANGE);
			return false;

		case SlgRuleTable::e_SlgTokenType::IDENTIFIER:
			itor = g_RuleTable.KeyWord.find(m_CurrentToken.GetValue<std::string>());
			if (itor != std::end(g_RuleTable.KeyWord)) {
				m_CurrentToken.m_Type = SlgRuleTable::e_SlgTokenType::KEYWORD;
				m_CurrentToken.m_Desc = itor->second;
			}
			else {
				m_CurrentToken.m_Type = SlgRuleTable::e_SlgTokenType::IDENTIFIER;
				m_CurrentToken.m_Desc = SlgRuleTable::e_SlgTokenDesc::IDENTIFIER;
			}
		case SlgRuleTable::e_SlgTokenType::NUMBER:
		case SlgRuleTable::e_SlgTokenType::BRACKET:
		case SlgRuleTable::e_SlgTokenType::OPERATOR:
		case SlgRuleTable::e_SlgTokenType::STATEMENTOPERATOR:
			if (m_CurrentToken.m_Desc == SlgRuleTable::e_SlgTokenDesc::ATTRIBUTE)
			{
				m_CurrentToken.m_Type = SlgRuleTable::e_SlgTokenType::IDENTIFIER;
			}
			m_Tokens.emplace_back(m_CurrentToken);
			break;
		}
		return true;
	}

	inline SlgTokenItem GetNextToken() {
		std::string token = "";

		if (m_Code[m_Pointer] == '\n') {
			m_Line++;
			m_PassCharacterCount = m_Pointer;
			m_Col = 0;
		}
		else {
			m_Col = m_Pointer - m_PassCharacterCount;
		}

		//去注释
		if (m_Code[m_Pointer] == '/') {
			if (m_Code.length() > m_Pointer + 1) {
				if (m_Code[m_Pointer + 1] == '/') {
					while (m_Code.length() > m_Pointer && m_Code[m_Pointer] != '\n') {
						m_Pointer++;
					}
					return SlgTokenItem(SlgRuleTable::e_SlgTokenType::NULLVALUE, SlgRuleTable::e_SlgTokenDesc::UNKNOWN, "", -1, -1);
				}
				else if (m_Code[m_Pointer + 1] == '*') {

					while (m_Code.length() > m_Pointer + 1 && (m_Code[m_Pointer] != '*' || m_Code[m_Pointer + 1] != '/')) {
						if (m_Code[m_Pointer] == '\n') {
							m_Line++;
							m_PassCharacterCount = m_Pointer;
						}
						m_Pointer++;
					}
					m_Pointer += 2;
					return SlgTokenItem(SlgRuleTable::e_SlgTokenType::NULLVALUE, SlgRuleTable::e_SlgTokenDesc::UNKNOWN, "", -1, -1);
				}
			}
		}

		if (m_Code[m_Pointer] == '-') {
			m_Pointer++;
			if (IsNumber(m_Code[m_Pointer])) {
				do {
					token += m_Code[m_Pointer];
					m_Pointer++;
				} while (m_Code.length() > m_Pointer && IsNumber(m_Code[m_Pointer]));

				if (m_Code[m_Pointer] == '.') {
					m_Pointer++;
					std::string Decimal = "";
					while (m_Code.length() > m_Pointer && IsNumber(m_Code[m_Pointer])) {
						Decimal += m_Code[m_Pointer];
						m_Pointer++;
					}
					return SlgTokenItem(SlgRuleTable::e_SlgTokenType::FLOAT, SlgRuleTable::e_SlgTokenDesc::FLOAT, -std::stof(token + "." + Decimal), m_Line, m_Col);
				}
				else {
					return SlgTokenItem(SlgRuleTable::e_SlgTokenType::INT, SlgRuleTable::e_SlgTokenDesc::INT, -std::stoll(token), m_Line, m_Col);
				}
			}
			else {
				m_Pointer--;
			}
		}

		//数字
		if (IsNumber(m_Code[m_Pointer])) {
			do {
				token += m_Code[m_Pointer];
				m_Pointer++;
			} while (m_Code.length() > m_Pointer && IsNumber(m_Code[m_Pointer]));

			if (m_Code[m_Pointer] == '.') {
				m_Pointer++;
				std::string Decimal = "";
				while (m_Code.length() > m_Pointer && IsNumber(m_Code[m_Pointer])) {
					Decimal += m_Code[m_Pointer];
					m_Pointer++;
				}
				return SlgTokenItem(SlgRuleTable::e_SlgTokenType::FLOAT, SlgRuleTable::e_SlgTokenDesc::FLOAT, std::stof(token + "." + Decimal), m_Line, m_Col);
			}
			else {
				return SlgTokenItem(SlgRuleTable::e_SlgTokenType::INT, SlgRuleTable::e_SlgTokenDesc::INT, std::stoll(token), m_Line, m_Col);
			}
		}

		//字段 标识符
		if (Isalpha(m_Code[m_Pointer]) || m_Code[m_Pointer] == '_') {
			do {
				token += m_Code[m_Pointer];
				m_Pointer++;
			} while (m_Code.length() > m_Pointer && (Isalnum(m_Code[m_Pointer]) || m_Code[m_Pointer] == '_'));
			return SlgTokenItem(SlgRuleTable::e_SlgTokenType::IDENTIFIER, SlgRuleTable::e_SlgTokenDesc::IDENTIFIER, token, m_Line, m_Col);
		}

		//字符串
		if (m_Code[m_Pointer] == '\"') {
			m_Pointer++;
			while (m_Code.length() > m_Pointer && (m_Code[m_Pointer] != '\"' || m_Code[m_Pointer - 1] == '\\')) {
				if (m_Code[m_Pointer] == '\n') {
					return SlgTokenItem(SlgRuleTable::e_SlgTokenType::ERROR, SlgRuleTable::e_SlgTokenDesc::STRING, "The string must be fully written on one line", m_Line, m_Col);
				}
				else {
					token += m_Code[m_Pointer];
					m_Pointer++;
				}
			}
			m_Pointer++;
			return SlgTokenItem(SlgRuleTable::e_SlgTokenType::STRING, SlgRuleTable::e_SlgTokenDesc::STRING, token, m_Line, m_Col);
		}

		//括号
		auto Bitor = g_RuleTable.Brackets.find(m_Code[m_Pointer]);
		if (Bitor != std::end(g_RuleTable.Brackets)) {
			token += m_Code[m_Pointer];
			m_Pointer++;
			return SlgTokenItem(SlgRuleTable::e_SlgTokenType::BRACKET, Bitor->second, token, m_Line, m_Col);
		}

		//运算符
		auto itor1 = g_RuleTable.OperatorsSingle.find((char)m_Code[m_Pointer]);
		if (itor1 != g_RuleTable.OperatorsSingle.end()) {

			token += m_Code[m_Pointer];

			if (m_Pointer + 2 < m_Code.length()) {
				auto itor2 = g_RuleTable.OperatorsMulti.find(token + m_Code[m_Pointer + 1] + m_Code[m_Pointer + 2]);
				if (itor2 != g_RuleTable.OperatorsMulti.end()) {
					m_Pointer += 3;
					return SlgTokenItem(SlgRuleTable::e_SlgTokenType::OPERATOR, itor2->second, itor2->first, m_Line, m_Col);
				}
			}

			if (m_Pointer + 1 < m_Code.length()) {
				auto itor2 = g_RuleTable.OperatorsMulti.find(token + m_Code[m_Pointer + 1]);
				if (itor2 != g_RuleTable.OperatorsMulti.end()) {
					m_Pointer += 2;
					return SlgTokenItem(SlgRuleTable::e_SlgTokenType::OPERATOR, itor2->second, itor2->first, m_Line, m_Col);
				}
			}

			m_Pointer++;
			return SlgTokenItem(SlgRuleTable::e_SlgTokenType::OPERATOR, itor1->second, token, m_Line, m_Col);
		}

		auto Sitor = g_RuleTable.StatementOpeartor.find((char)m_Code[m_Pointer]);
		if (Sitor != g_RuleTable.StatementOpeartor.end()) {
			token += Sitor->first;
			m_Pointer++;
			if (Sitor->first == ':' && m_Code[m_Pointer] == ':') {
				m_Pointer++;
				token = "::";
				return SlgTokenItem(SlgRuleTable::e_SlgTokenType::STATEMENTOPERATOR, SlgRuleTable::e_SlgTokenDesc::MODULEACCESS, token, m_Line, m_Col);
			}
			return SlgTokenItem(SlgRuleTable::e_SlgTokenType::STATEMENTOPERATOR, Sitor->second, token, m_Line, m_Col);
		}

		m_Pointer++;
		return SlgTokenItem(SlgRuleTable::e_SlgTokenType::NULLVALUE, SlgRuleTable::e_SlgTokenDesc::UNKNOWN, "", -1, -1);
	}

	inline std::vector<SlgTokenItem>& GetTokens() {
		return m_Tokens;
	}

	inline void PrintTokens() {
		int id = 0;
		for (auto& i : m_Tokens) {
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

class SlgStatementCompile {

	struct s_SlgAttributeDesc {
		std::string m_TypeStr;
		e_SlgExternType m_VariableExternType{};
		bool isPrivateAttribute = false;
	};

	struct s_SlgLocalVariableDesc {
		size_t depth = 0;
		std::string m_TypeStr;
		e_SlgExternType m_VariableExternType{};
		size_t m_VariableSize{};
		size_t m_VariableOffest{};
	};

	struct s_SlgFunctionDesc {
		std::string returnType;
		std::vector<std::string> arguments;
	};

	struct s_SlgProgramTable {
		std::string m_SelfModuleName{};
		std::unordered_set<std::string> m_ModuleDependence{};
		std::unordered_map<std::string, s_SlgAttributeDesc> m_Attribute{};
		std::unordered_map<std::string, s_SlgFunctionDesc> m_FunctionTable{};
	};


	struct s_StatementSegment {
		size_t idx{};

		int iType = -1; //0是变量， 1是int， 2是float, 3是string, 4是subStatement, 5是运算符, 6是栈空间，7是寄存器，8是RET返回寄存器, -1是错误
		SlgTokenItem iToken;

		int type{};	// type， 0-错误， 1-属性， 2-属性成员变量， 3-属性成员函数， 4-变量， 5-变量的成员变量, 6-变量的成员函数, 7-普通函数
		std::string ModuleName{};
		std::string MainName{};
		std::string MemberName{};
		size_t argbegin{};
		size_t end{};

		std::string desc{};
		std::string varType = "";

		//寄存器
		int Reg = -1;
		int StackIdx = -1;
	};

	std::vector<SlgTokenItem> m_Tokens;

	SlgErrorMachine* m_ErrorMachine{};

	std::unique_ptr<SlgAssemblyData> m_AssemblyData{};
	Slgd87AssemblyGenerator m_AsmGen{};

	s_SlgProgramTable m_ProgramTable{};

	size_t m_LocalVariableSize{};
	std::unordered_map<std::string, s_SlgLocalVariableDesc> m_LocalVariable{};

	std::array<char, 64> m_Register{  };

public:
	inline SlgStatementCompile(SlgTokenizer* tokenizer, SlgErrorMachine* errorMachine) : m_ErrorMachine(errorMachine) {
		tokenizer->PrintTokens();
		m_Tokens = std::move(tokenizer->GetTokens());
		m_AssemblyData = std::unique_ptr<SlgAssemblyData>(new SlgAssemblyData(5));
		m_AsmGen.BindAsmData(m_AssemblyData.get());
		Splitstatement();
	}

	inline void Splitstatement() {

		size_t tokenSize = m_Tokens.size();
		size_t bracketDepth = 0;

		for (size_t i = 0; i < tokenSize; ++i) {

			switch (m_Tokens[i].m_Type)
			{
			case SlgRuleTable::e_SlgTokenType::KEYWORD: {

				switch (m_Tokens[i].m_Desc)
				{
				case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_using:
					if (!D_using(i, bracketDepth))
						return;
					break;
				case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_module:
					if (!D_module(i, bracketDepth))
						return;
					break;
				case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_let:
					if (!D_Var(i, bracketDepth, false))
						return;
					break;
				case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_attribute:
					if (!D_Var(i, bracketDepth, true))
						return;
					break;
				case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_export:
					if (!D_functionDefine(i, bracketDepth))
						return;
					break;
				case SlgRuleTable::e_SlgTokenDesc::KEYWORLD_function:
					if (!D_functionDefine(i, bracketDepth))
						return;
					break;
				default:
					break;
				}
				break;
			}

			case SlgRuleTable::e_SlgTokenType::IDENTIFIER:
				if (!D_IntentifierAnalysis(i, bracketDepth))
					return;
				break;

			case SlgRuleTable::e_SlgTokenType::BRACKET: {
				if (m_Tokens[i].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_CURLY_START) {
					bracketDepth++;
				}
				if (m_Tokens[i].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_CURLY_END) {
					bracketDepth--;
					if (bracketDepth == 0) {
						//m_AsmGen.GLOBAL_VAR_MALLOC(m_ProgramTable.m_GlobalVariableSize);
					}
				}
				break;
			}
			default:
				m_ErrorMachine->AddError("Meaningless Statement.", m_Tokens[i].m_LineNumber, m_Tokens[i].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_MEANINGLESS_STATEMENT);
				return;
			}
			if (!m_ErrorMachine->Success()) {
				return;
			}
		}
	}

	inline void ShowAsm() {
		std::cout << "Total Size(Byte):" << m_AssemblyData->GetAllSize() << "\n";
		std::cout << "Using Size(Byte):" << m_AssemblyData->GetCurrentSize() << "\n";

		m_AssemblyData->Show();
	}

	void ShowProgramTable() {

		for (auto i : m_ProgramTable.m_Attribute) {
			std::cout << "@" << i.first << "\t\tType:" << i.second.m_TypeStr << "\tSize:" << (unsigned short)i.second.m_VariableExternType << "\tIsPrivate:" << (i.second.isPrivateAttribute ? "yes" : "no") << std::endl;
		}
	}
private:

	inline bool D_using(size_t& idx, size_t& bDepth) {
		if (idx + 2 > m_Tokens.size()) {
			m_ErrorMachine->AddError("Missing content after using. Format: using \"module Path\";", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_ARGUMENT_COUNT);
			return false;
		}

		if (bDepth != 0) {
			m_ErrorMachine->AddError("Using statement is not allowed in bracket.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_ILLEGAL_KEYWORD_USING_POSITION);
			return false;
		}
		idx++;
		if (m_Tokens[idx].m_Desc != SlgRuleTable::e_SlgTokenDesc::STRING) {
			m_ErrorMachine->AddError("Using stament need a const string as a argument.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
			return false;
		}

		std::string moduleName = m_Tokens[idx].GetValue<std::string>();
		m_AsmGen.MODULE_DEP(moduleName);

		idx++;
		if (m_Tokens[idx].m_Desc != SlgRuleTable::e_SlgTokenDesc::END) {
			m_ErrorMachine->AddError("Losing ';' after statement.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_END);

		}

		return true;
	}

	inline bool D_module(size_t& idx, size_t& bDepth) {
		if (idx + 2 > m_Tokens.size()) {
			m_ErrorMachine->AddError("Missing content after module.. Format: module \"module Path\"{ your code }", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_ARGUMENT_COUNT);
			return false;
		}

		if (bDepth != 0) {
			m_ErrorMachine->AddError("module statement is not allowed in bracket.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_ILLEGAL_KEYWORD_USING_POSITION);
			return false;
		}

		if (m_ProgramTable.m_SelfModuleName != "") {
			m_ErrorMachine->AddError("Only 1 module is allowed per source file", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_ARGUMENT_COUNT);
			return false;
		}
		idx++;
		if (m_Tokens[idx].m_Desc != SlgRuleTable::e_SlgTokenDesc::STRING) {
			m_ErrorMachine->AddError("module stament need a const string as a argument.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
			return false;
		}

		std::string moduleName = m_Tokens[idx].GetValue<std::string>();
		if (moduleName == "") {
			m_ErrorMachine->AddError("Module name can't be empty! Character you can use: [0-9 a-z A-Z _]. ", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_END);
			return false;
		}

		m_ProgramTable.m_SelfModuleName = moduleName;
		m_AsmGen.MODULE_SELF(moduleName);

		idx++;
		if (m_Tokens[idx].m_Desc != SlgRuleTable::e_SlgTokenDesc::BRACKET_CURLY_START) {
			m_ErrorMachine->AddError("Can't declar empty module, Maybe you Losing '{' after statement.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_END);
			return false;
		}
		bDepth++;
		return true;
	}

	inline bool D_Var(size_t& idx, size_t& bDepth, bool isAttribute, bool statementCompile = false) {
		if (isAttribute) {
			if (bDepth == 0) {
				m_ErrorMachine->AddError("let statement must be in code block.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_ILLEGAL_KEYWORD_USING_POSITION);
				return false;
			}
		}
		else {
			if (bDepth != 1) {
				m_ErrorMachine->AddError("attribute statement must be in module's code block.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_ILLEGAL_KEYWORD_USING_POSITION);
				return false;
			}
		}

		idx++;
		if (isAttribute) {
			if (m_Tokens[idx].m_Desc != SlgRuleTable::e_SlgTokenDesc::ATTRIBUTE) {
				m_ErrorMachine->AddError("Attribute variable name need a \"@\" at the beginning of the name.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_ARGUMENT_COUNT);
				return false;
			}
			idx++;
		}

		if (m_Tokens[idx].m_Desc != SlgRuleTable::e_SlgTokenDesc::IDENTIFIER) {
			m_ErrorMachine->AddError("let / attribute statement need a identifier as a argument.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
			return false;
		}

		if (m_Tokens[idx].IsKeyWord()) {
			m_ErrorMachine->AddError("Variable name can't be a keyword.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
			return false;
		}

		std::string variableName = m_Tokens[idx].GetValue<std::string>();
		if (isAttribute) {
			variableName = "@" + variableName;
		}

		if (variableName.length() > 64) {

			std::string erro = "Variable  \"" + variableName + "\"'s name is too long, max count limited by 64.";
			m_ErrorMachine->AddError(erro, m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
			return false;
		}

		idx++;
		if (m_Tokens[idx].m_Desc != SlgRuleTable::e_SlgTokenDesc::TYPE_DECLAR) {
			m_ErrorMachine->AddError("let / attribute statement need a \":\" between variable name and type declar.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_ARGUMENT_COUNT);
			return false;
		}

		idx++;
		//Type
		if (m_Tokens[idx].m_Desc != SlgRuleTable::e_SlgTokenDesc::IDENTIFIER) {
			m_ErrorMachine->AddError("Must use a identifier as a variable type.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
			return false;
		}

		std::string typeName = m_Tokens[idx].GetValue<std::string>();
		bool isPrivate = false;
		if (typeName == "private") {
			if (isAttribute) {
				isPrivate = true;
			}
			else {
				m_ErrorMachine->AddError("\"private\" can't used in normal variable defintion, only for attribute defintion.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
				return false;
			}
			idx++;
			typeName = m_Tokens[idx].GetValue<std::string>();
		}
		//计算类型
		auto titor = SlgTypeAsm.find(typeName);
		if (titor == SlgTypeAsm.end()) {
			m_ErrorMachine->AddError(typeName + " is a error type.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
			return false;
		}

		idx++;

		//检测是否是list map
		e_SlgExternType externType = e_SlgExternType::NONE;
		if (m_Tokens[idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::IDENTIFIER) {
			std::string afterIdentifer = m_Tokens[idx].GetValue<std::string>();

			auto extitor = SlgExternTypeAsm.find(afterIdentifer);
			if (extitor == SlgExternTypeAsm.end()) {
				m_ErrorMachine->AddError(afterIdentifer + " is a error extern type.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
				return false;
			}
			externType = extitor->second;
			idx++;
		}


		//是否遗漏分号
		if (m_Tokens[idx].m_Desc != SlgRuleTable::e_SlgTokenDesc::END) {
			m_ErrorMachine->AddError("Losing ';' after statement.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_END);
		}

		if (isAttribute) {
			//检查是否重复定义
			auto itor = m_ProgramTable.m_Attribute.find(variableName);
			if (itor != m_ProgramTable.m_Attribute.end()) {
				m_ErrorMachine->AddError("Attribute \"" + variableName + "\" is already defined.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
				return false;
			}

			s_SlgAttributeDesc temp = { typeName, externType, isPrivate };
			m_ProgramTable.m_Attribute.insert({ variableName , temp });
			m_AsmGen.ATTRITUBE_LET(variableName, titor->second, externType, isPrivate);
		}
		else {

			auto sitor = SlgTypeSize.find(typeName);
			int size = sitor->second;

			if (bDepth == 1) { //全局变量
				m_ErrorMachine->AddError("Only variables can be declared in moudule's parentheses \"", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
				return false;
			}
			else { //局部变量
				auto itor = m_LocalVariable.find(variableName);
				if (itor != m_LocalVariable.end()) {
					m_ErrorMachine->AddError("Local variable \"" + variableName + "\" is already defined.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
					return false;
				}
				s_SlgLocalVariableDesc temp = { bDepth,  typeName , externType , m_LocalVariableSize };
				m_LocalVariable.insert({ variableName , temp });
				m_LocalVariableSize += size;
			}
		}
		return true;
	}

	inline bool D_functionDefine(size_t& idx, size_t& bDepth) {
		if (bDepth != 1) {
			m_ErrorMachine->AddError("Function defintion statement must be in module's code block.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_ILLEGAL_KEYWORD_USING_POSITION);
			return false;
		}

		return true;
	}

	inline bool D_IntentifierAnalysis(size_t& idx, size_t& bDepth, bool StaticCompile = false) {
		if (StaticCompile == false) {
			if (bDepth == 0) {
				m_ErrorMachine->AddError("Identifier operation only write in code block.", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_ILLEGAL_KEYWORD_USING_POSITION);
				return false;
			}
		}
		size_t r_idx = idx;

		size_t bracketMask = 0;
		size_t end_idx = 0;

		while (r_idx < m_Tokens.size()) {

			if (r_idx >= m_Tokens.size()) {
				m_ErrorMachine->AddError("Statement missing end \";\".", m_Tokens[idx].m_LineNumber, m_Tokens[idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_END);
				return false;
			}
			else if (m_Tokens[r_idx].IsKeyWord()) {
				m_ErrorMachine->AddError("Missing end \";\" before keyword.", m_Tokens[r_idx].m_LineNumber, m_Tokens[r_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_END);
				return false;
			}
			else if (m_Tokens[r_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_START) {
				bracketMask++;
			}
			else if (m_Tokens[r_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_END) {
				bracketMask--;
			}
			else if (m_Tokens[r_idx].IsBracket()) {
				m_ErrorMachine->AddError("Only round bracket is allowed to used in statement.", m_Tokens[r_idx].m_LineNumber, m_Tokens[r_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_BRACKETS);
				return false;
			}
			else if (m_Tokens[r_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::END) {
				end_idx = r_idx - 1;
				break;
			}
			r_idx++;
		}

		if (bracketMask != 0) {
			m_ErrorMachine->AddError("The number of parentheses does not match.", m_Tokens[r_idx].m_LineNumber, m_Tokens[r_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_BRACKETS);
			return false;
		}

		ExprSolver(idx, end_idx);
		FreeAllRegister();
		idx = r_idx;
		return m_ErrorMachine->Success();
	}

	inline s_StatementSegment ExprSolver(size_t start_idx, size_t end_idx, int recursionDepth = 0) {

		std::vector<s_StatementSegment> statementList;

		size_t r_idx = start_idx;
		int type;
		std::string moduleName;
		std::string mainName;
		std::string memberName;
		size_t argbegin;
		size_t end;
		size_t bracketMask = 0;

		size_t bracketStart = 0;
		while (r_idx <= end_idx) {

			if (m_Tokens[r_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_START) {
				if (bracketMask == 0) {
					bracketStart = r_idx;
				}
				bracketMask++;

			}
			else if (m_Tokens[r_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_END) {
				bracketMask--;
				if (bracketMask == 0) {
					if (r_idx - bracketStart < 4) {
						m_ErrorMachine->AddError("Meaningless sub statement.", m_Tokens[bracketStart].m_LineNumber, m_Tokens[bracketStart].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_MEANINGLESS_STATEMENT);
						return s_StatementSegment{};
					}
					s_StatementSegment temp{};
					temp.idx = bracketStart;
					temp.iType = 4;
					temp.end = r_idx;
					statementList.push_back(temp);
				}
			}
			else if (bracketMask == 0 && m_Tokens[r_idx].IsOperator()) {
				s_StatementSegment temp{};
				temp.idx = r_idx;
				temp.iType = 5;
				temp.iToken = m_Tokens[r_idx];
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && m_Tokens[r_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::INT) {
				s_StatementSegment temp{};
				temp.idx = r_idx;
				temp.iType = 1;
				temp.iToken = m_Tokens[r_idx];
				temp.varType = "int";
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && m_Tokens[r_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::FLOAT) {
				s_StatementSegment temp{};
				temp.idx = r_idx;
				temp.iType = 2;
				temp.iToken = m_Tokens[r_idx];
				temp.varType = "float";
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && m_Tokens[r_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::STRING) {
				s_StatementSegment temp{};
				temp.idx = r_idx;
				temp.iType = 3;
				temp.iToken = m_Tokens[r_idx];
				temp.varType = "string";
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && GetIdentifierSegment(r_idx, end_idx, type, moduleName, mainName, memberName, argbegin, end)) {

				if (moduleName != "") {
					switch (type) {
					case 2: case 3:
						m_ErrorMachine->AddError("External Attribute don't allow the use of member functions or member variables! but you can use \"=\" to Assignment it by a local variable or Copy it to a local variable to write or read it.", m_Tokens[r_idx].m_LineNumber, m_Tokens[r_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
						return s_StatementSegment{};
					}
				}

				s_StatementSegment temp{};
				temp.idx = r_idx;
				temp.iType = 0;
				temp.type = type;
				temp.ModuleName = moduleName;
				temp.MainName = mainName;
				temp.MemberName = memberName;
				temp.argbegin = argbegin;
				temp.end = end;
				statementList.push_back(temp);
				r_idx = end;
			}
			else {
				if (bracketMask == 0) {
					m_ErrorMachine->AddError("Statement error.", m_Tokens[r_idx].m_LineNumber, m_Tokens[r_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
			}
			r_idx++;
		}

		if (statementList.size() == 0) {
			return s_StatementSegment{};
		}


		//检测运算符错误
		std::vector<int> propList{};

		int minProp = 99;
		int maxProp = 0;

		size_t list_idx = 0;
		s_StatementSegment prev = statementList[list_idx];
		list_idx++;

		bool hasAssignmentOperator = false;
		for (; list_idx < statementList.size(); ++list_idx) {
			if (statementList[list_idx].iType == -1) {
				m_ErrorMachine->AddError("Unknown error, type = error.", m_Tokens[statementList[list_idx].idx].m_ColNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
				return s_StatementSegment{};
			}

			if (statementList[list_idx].iType < 5) {
				if (prev.iType != 5) {
					m_ErrorMachine->AddError("Missing operator before identifier.", m_Tokens[statementList[list_idx].idx].m_ColNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
			}
			else if (statementList[list_idx].iType == 5) {
				if (statementList[list_idx].iToken.m_Desc == SlgRuleTable::e_SlgTokenDesc::SELFADD || statementList[list_idx].iToken.m_Desc == SlgRuleTable::e_SlgTokenDesc::SELFSUB) {
					if (list_idx != 1) {
						m_ErrorMachine->AddError("Self Add/Sub operator is only allowed in the second location.", m_Tokens[statementList[list_idx].idx].m_ColNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
						return s_StatementSegment{};
					}
					else if (!(prev.iType == 0 && (prev.type == 1 || prev.type == 2 || prev.type == 4 || prev.type == 5))) {
						m_ErrorMachine->AddError("Self Add/Sub operator can only be used with variables, not with subexpressions and functions.", m_Tokens[statementList[list_idx].idx].m_ColNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
						return s_StatementSegment{};
					}
				}
				else if (statementList[list_idx].iToken.IsUnaryOperator()) {
					if (prev.iType == 5 && prev.iToken.IsUnaryOperator()) {
						m_ErrorMachine->AddError("Can't using double unary operator.", m_Tokens[statementList[list_idx].idx].m_LineNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
						return s_StatementSegment{};

					}if (prev.iType == 5 && prev.iToken.IsBinaryOperator()) {
						//correct
					}
					else {
						if (list_idx != 2) {
							m_ErrorMachine->AddError("Missing operator before unary operator.", m_Tokens[statementList[list_idx].idx].m_LineNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
							return s_StatementSegment{};
						}
					}
				}
				else if (statementList[list_idx].iToken.IsBinaryOperator()) {
					if (prev.iType < 5) {
						//correct
					}
					else {
						m_ErrorMachine->AddError("Missing number before binary operator.", m_Tokens[statementList[list_idx].idx].m_LineNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TYPE);
						return s_StatementSegment{};
					}
				}
				else if (statementList[list_idx].iToken.IsAssignmentOperator()) {
					if (recursionDepth != 0) {
						m_ErrorMachine->AddError("Assignment operator is not allowed in the subexpressions.", m_Tokens[statementList[list_idx].idx].m_ColNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
						return s_StatementSegment{};
					}
					if (list_idx != 1) {
						m_ErrorMachine->AddError("Assignment operator is only allowed in the second location.", m_Tokens[statementList[list_idx].idx].m_ColNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
						return s_StatementSegment{};
					}
					else if (!(prev.iType == 0 && (prev.type == 1 || prev.type == 2 || prev.type == 4 || prev.type == 5))) {
						m_ErrorMachine->AddError("Assignment operator can only be used with variables, not with subexpressions and functions.", m_Tokens[statementList[list_idx].idx].m_ColNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
						return s_StatementSegment{};
					}
					hasAssignmentOperator = true;
				}

				propList.push_back(statementList[list_idx].iToken.GetOperatorProperity());
			}
			else {
				m_ErrorMachine->AddError("Expressions unkown error.", m_Tokens[statementList[list_idx].idx].m_ColNumber, m_Tokens[statementList[list_idx].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
				return s_StatementSegment{};
			}
			prev = statementList[list_idx];
		}


		//获取运算符优先级
		std::sort(propList.begin(), propList.end(), std::less<int>());
		propList.erase(std::unique(propList.begin(), propList.end()), propList.end());

		if (propList.empty()) {
			minProp = 0;
			maxProp = 99;
		}
		else if (propList.size() == 1) {
			minProp = propList[0];
			maxProp = propList[0];
		}
		else {
			if (hasAssignmentOperator) {
				minProp = propList[1];
			}
			else {
				minProp = propList[0];
			}
			maxProp = propList.back();
		}

		if (statementList.size() == 1) {
			return SingleStatementSegmentSolver(statementList[0]);
		}
		else if (statementList.size() == 2 && recursionDepth == 0) { //自增自减运算符
			return statementList[0];
		}
		else if (hasAssignmentOperator) {
			//有赋值语句
			s_StatementSegment B = ExprSolverCall(minProp, maxProp, minProp, statementList, 2, statementList.size() - 1);
			s_StatementSegment A = VariableSolver(statementList[0]);
			if (!m_ErrorMachine->Success()) {
				return s_StatementSegment{};
			}
			s_StatementSegment res = SovlerAsmAssignmentOperator(statementList[1], A, B);
			if (!m_ErrorMachine->Success()) {
				return s_StatementSegment{};
			}
			else {
				return res;
			}
		}
		else {
			s_StatementSegment res = ExprSolverCall(minProp, maxProp, minProp, statementList, 0, statementList.size() - 1);
			if (!m_ErrorMachine->Success()) {
				return s_StatementSegment{};
			}
			else {
				return res;
			}
		}

	}

	inline int GetFreeRegister() {
		for (int i = 0; i < 64; i++) {
			if (m_Register[i] == 0) {
				m_Register[i] = 1;
				return i;
			}
			else {
				continue;
			}
		}
		return 63;
	}

	inline void FreeAllRegister() {
		for (int i = 0; i < 64; i++) {
			m_Register[i] = 0;
		}
	}

	inline s_StatementSegment SingleStatementSegmentSolver(s_StatementSegment& item) {
		if (item.iType == 0) {
			if (item.type == 1 || item.type == 2 || item.type == 4 || item.type == 5) {
				return VariableSolver(item);
			}
			else if (item.type == 3 || item.type == 6 || item.type == 7) {
				return FunctionSolver(item);
			}
			else {
				m_ErrorMachine->AddError("Unknow Token whitch type = error.", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
				return s_StatementSegment{};
			}
		}
		else if (item.iType == 1 || item.iType == 2 || item.iType == 3) {
			return ConstSolver(item);
		}
		else if (item.iType == 4) {
			return ExprSolver(item.idx + 1, item.end - 1, 1);
		}
		else {
			m_ErrorMachine->AddError("Unknow Token whitch itype = error.", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
			return s_StatementSegment{};
		}
	}

	inline s_StatementSegment ExprSolverCall(size_t minprop, size_t maxprop, size_t currentprop, std::vector<s_StatementSegment>& items, size_t start, size_t end) {

		if (currentprop > maxprop + 1) { return s_StatementSegment{}; }

		//单语句
		if (end == start) {
			return SingleStatementSegmentSolver(items[start]);
		}

		if (currentprop == 14 && items[start].iType == 5 && items[start].iToken.IsUnaryOperator()) { //一元运算符
			size_t next = start + 1;
			s_StatementSegment val = SingleStatementSegmentSolver(items[next]);
			return SovlerAsmUnaryOperator(items[start], val);
		}
		else {
			//0是变量， 1是int， 2是float, 3是string, 4是subStatement, 5是运算符, 6是栈空间，

			// type， 0-错误， 1-属性， 2-属性成员变量， 3-属性成员函数， 4-变量， 5-变量的成员变量, 6-变量的成员函数, 7-普通函数
			// MName 所在模组的名字
			// VName - 变量 / 属性 / 函数 名字 
			// FName - 成员变量名字 / 成员函数名字 
			// argsBegin, argsEnd 如果有函数， 函数参数的范围， 包含 "(" 和 ")", argsEnd也是分析结束的地址

			std::vector<s_StatementSegment> varlayer;
			std::vector<s_StatementSegment> oplayer;

			int nearlestPropDelta = 999;
			for (size_t i = start; i <= end; i++) {
				if (items[i].iType == 5) {
					int propDelta = items[i].iToken.GetOperatorProperity() - currentprop;
					if (propDelta > 0) {
						nearlestPropDelta = std::min(nearlestPropDelta, propDelta);
					}
				}
			}
			size_t s_mask = start;

			if (nearlestPropDelta > 20) {
				nearlestPropDelta = 0;
			}

			for (size_t i = start; i <= end; i++) {
				if (items[i].iType == 5) {

					if (items[i].iToken.GetOperatorProperity() == currentprop) {
						oplayer.push_back(items[i]);
						s_StatementSegment newVar = ExprSolverCall(minprop, maxprop, currentprop + nearlestPropDelta, items, s_mask, i - 1);
						if (!m_ErrorMachine->Success()) {
							return s_StatementSegment{};
						}
						varlayer.push_back(newVar);
						s_mask = i + 1;
					}
				}
			}

			s_StatementSegment newVar = ExprSolverCall(minprop, maxprop, currentprop + nearlestPropDelta, items, s_mask, end);
			if (!m_ErrorMachine->Success()) {
				return s_StatementSegment{};
			}
			varlayer.push_back(newVar);

			if (oplayer.size() + 1 != varlayer.size()) {
				m_ErrorMachine->AddError("Opeartor list is null.", m_Tokens[items[start].idx].m_LineNumber, m_Tokens[items[start].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
				return s_StatementSegment{};
			}

			if (oplayer.size() == 0) {
				return newVar;
			}

			for (size_t i = 0; i < varlayer.size(); i++) {
				if (varlayer[i].varType == "void") {
					m_ErrorMachine->AddError("Function which doesn't have a return value can't used in operation statement.", m_Tokens[items[0].idx].m_LineNumber, m_Tokens[items[0].idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
			}

			s_StatementSegment A = varlayer[0];
			for (size_t i = 0; i < oplayer.size(); i++) {
				s_StatementSegment B = varlayer[i + 1];
				A = SovlerAsmBinaryOperator(oplayer[i], A, B);
			}
			DescGenerator(A);
			return A;
		}
	}

	inline s_StatementSegment FunctionSolver(s_StatementSegment& item) {
		//函数参数分离
		//检查部分

		std::vector<std::string> args{};
		std::string retType{};

		bool NeedCheck = true;

		//获取函数信息部分
		if (item.type == 7) { // 如果是普通函数
			if (item.ModuleName != "") {  //如果是外部函数
				NeedCheck = false;
			}
			else {
				auto it = g_Config.StandardFunction.find(item.MainName);
				if (it != g_Config.StandardFunction.end()) {
					retType = it->second.returnType;
					args = it->second.arguments;
				}
				else {
					m_ErrorMachine->AddError("Function: \"" + item.MainName + "\" is not defined", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
			}
		}
		else {
			if (item.type == 6) { // 如果是成员函数
				auto it = g_Config.MemberFunction.find(item.varType);
				if (it != g_Config.MemberFunction.end() && it->second.find(item.MemberName) != it->second.end()) {
					auto& func = it->second[item.MemberName];
					retType = func.returnType;
					args = func.arguments;
				}
				else {
					m_ErrorMachine->AddError("Local Variable: \"" + item.MainName + " \",Type: \"" + item.varType + "\" doesn't have Member Function \"" + item.MemberName + "\" not defined", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
			}
			else if (item.type == 3) {

				auto it = g_Config.MemberFunction.find(item.varType);
				if (it != g_Config.MemberFunction.end() && it->second.find(item.MemberName) != it->second.end()) {
					auto& func = it->second[item.MemberName];
					retType = func.returnType;
					args = func.arguments;
				}
				else {
					m_ErrorMachine->AddError("Attribute: \"" + item.MainName + " \",Type: \"" + item.varType + "\" doesn't have Member Function \"" + item.MemberName + "\" not defined", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
			}
		}

		size_t begin = item.argbegin + 1;
		size_t end = item.end - 1;

		std::vector<std::pair<size_t, size_t>> cArgs{};

		//获取现有函数的参数
		if (end - begin >= 0) {

			size_t bracketMask = 0;
			size_t argStart = begin;
			size_t idx = begin;
			for (; idx <= end; idx++) {
				if (m_Tokens[idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_START) {
					bracketMask++;
				}
				else if (m_Tokens[idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_END) {
					bracketMask--;
				}
				else if (m_Tokens[idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::COMMA && bracketMask == 0) {
					cArgs.push_back({ argStart, idx - 1 });
					argStart = idx + 1;
				}
			}
			if (argStart < idx) {
				cArgs.push_back({ argStart, idx - 1 });
			}
		}

		if (NeedCheck) {
			if (cArgs.size() != args.size()) {
				if (item.type == 3 || item.type == 6) {
					m_ErrorMachine->AddError("Member Function: \"" + item.MemberName + " \" in Type: \"" + item.varType + "\" need the same number of arguments (need " + std::to_string(args.size()) + " arguments, but now have " + std::to_string(cArgs.size()) + ").", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
				else {
					m_ErrorMachine->AddError("Function: \"" + item.MainName + "\" need the same number of arguments (need " + std::to_string(args.size()) + " arguments, but now have " + std::to_string(cArgs.size()) + ").", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
			}
		}

		//函数执行
		for (size_t i = 0; i < cArgs.size(); ++i) {
			s_StatementSegment temp = ExprSolver(cArgs[i].first, cArgs[i].second);

			if (NeedCheck) {
				if (temp.varType != args[i]) {
					if (temp.varType == "float" && args[i] == "int") {
						if (temp.iType == 2) {
							temp.iType = 1;
							temp.iToken.m_Value = temp.iToken.GetValue<SlgRuleTable::SlgINT>();
						}
						temp.varType = "int";
					}
					else if (temp.varType == "int" && args[i] == "float") {
						if (temp.iType == 1) {
							temp.iType = 2;
							temp.iToken.m_Value = temp.iToken.GetValue<SlgRuleTable::SlgFLOAT>();
						}
						temp.varType = "float";
					}
					else {
						if (item.type == 3 || item.type == 6) {
							m_ErrorMachine->AddError("Member Function: \"" + item.MemberName + " \" in Type: \"" + item.varType + "\" argument " + std::to_string(i) + "," + args[i] + ")is not match with \"" + temp.varType + "\"", m_Tokens[cArgs[i].first].m_LineNumber, m_Tokens[cArgs[i].first].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
							return s_StatementSegment{};
						}
						else {
							m_ErrorMachine->AddError("Function: \"" + item.MainName + "\" argument (" + std::to_string(i) + "," + args[i] + ") is not match with \"" + temp.varType + "\"", m_Tokens[cArgs[i].first].m_LineNumber, m_Tokens[cArgs[i].first].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
							return s_StatementSegment{};
						}
					}
				}
			}
			DescGenerator(temp);
			printf("PUSH %s\n", temp.desc.c_str());
		}

		if (item.type == 3)
		{
			if (item.ModuleName != "")
			{
				item.desc = item.ModuleName + "::";
			}
			item.desc += item.MainName + "." + item.MemberName;
			printf("CALLA %s\n", item.desc.c_str());
		}
		else if (item.type == 6)
		{
			item.desc += item.MainName + "." + item.MemberName;
			printf("CALLV %s\n", item.desc.c_str());
		}
		else if (item.type == 7)
		{
			if (item.ModuleName != "")
			{
				item.desc = item.ModuleName + "::";
			}
			item.desc += item.MainName;
			printf("CALL %s\n", item.desc.c_str());
		}

		s_StatementSegment vtemp;
		vtemp.iType = 7;
		vtemp.Reg = GetFreeRegister();
		vtemp.varType = retType;
		DescGenerator(vtemp);
		if (retType != "void") {
			std::cout << "MOV " << DescGenerator(vtemp).desc << ", RET\n";
		}
		return vtemp;
	}

	inline s_StatementSegment DescGenerator(s_StatementSegment& item) {

		if (item.iType == 1) {
			item.desc = std::to_string(item.iToken.GetValue<SlgRuleTable::SlgINT>());
		}
		else if (item.iType == 2) {
			item.desc = std::to_string(item.iToken.GetValue<SlgRuleTable::SlgFLOAT>());
		}
		else if (item.iType == 4) {
			item.desc = item.iToken.GetValue<std::string>();
		}
		else if (item.iType == 0) {
			if (item.type == 1) {
				if (item.ModuleName != "")
				{
					item.desc = item.ModuleName + "::";
				}
				item.desc += item.MainName;
			}
			else if (item.type == 2) {
				if (item.ModuleName != "")
				{
					item.desc = item.ModuleName + "::";
				}
				item.desc += item.MainName + "." + item.MemberName;
			}
			else if (item.type == 4) {
				item.desc = item.MainName;
			}
			else if (item.type == 5) {
				item.desc = item.MainName + "." + item.MemberName;
			}
		}
		else if (item.iType == 7) {
			item.desc = "R" + std::to_string(item.Reg);
		}
		return item;
	}

	inline s_StatementSegment VariableSolver(s_StatementSegment& item) {

		if (item.type == 1 || item.type == 2) {
			if (item.ModuleName == "") { //如果是内部函数
				if (m_ProgramTable.m_Attribute.find(item.MainName) == m_ProgramTable.m_Attribute.end()) {
					m_ErrorMachine->AddError("Attribute \"" + item.MainName + "\" not defined", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
				if (item.type == 2) {
					if (item.type == 2) {
						auto mFind = g_Config.MemberVariable.find(item.varType);
						if (mFind == g_Config.MemberVariable.end() || mFind->second.find(item.MemberName) == mFind->second.end()) {
							m_ErrorMachine->AddError("Attribute: \"" + item.MainName + " \",Type: \"" + item.varType + "\" doesn't have Member Variable \"" + item.MemberName + "\" not defined", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
							return s_StatementSegment{};
						}
					}
				}
			}
		}
		else if (item.type == 4 || item.type == 5) {

			if (m_ProgramTable.m_Attribute.find(item.MainName) == m_ProgramTable.m_Attribute.end()) {
				m_ErrorMachine->AddError("Local varaible \"" + item.MainName + "\" not defined", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
				return s_StatementSegment{};
			}

			if (item.type == 5) {
				auto mFind = g_Config.MemberVariable.find(item.varType);
				if (mFind == g_Config.MemberVariable.end() || mFind->second.find(item.MemberName) == mFind->second.end()) {
					m_ErrorMachine->AddError("Local varaible: \"" + item.MainName + " \",Type: \"" + item.varType + "\" doesn't have Member Variable \"" + item.MemberName + "\" not defined", m_Tokens[item.idx].m_LineNumber, m_Tokens[item.idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return s_StatementSegment{};
				}
			}
		}

		//varType
		return DescGenerator(item);
	}

	inline s_StatementSegment ConstSolver(s_StatementSegment& item) {

		return DescGenerator(item);
	}

	inline s_StatementSegment SovlerAsmUnaryOperator(s_StatementSegment& op, s_StatementSegment& A) {

		s_StatementSegment reg{};

		if (A.iType == 1) {
			reg = A;
			reg.iToken.m_Value = IntConstexprCalcValueUnary(op.iToken.m_Desc, A.iToken.GetValue<SlgRuleTable::SlgINT>());
			DescGenerator(reg);
			return reg;
		}
		else if (A.iType == 2) {

			std::cout << "GGGGGGG";
			reg = A;
			reg.iToken.m_Value = FloatConstexprCalcValueUnary(op.iToken.m_Desc, A.iToken.GetValue<SlgRuleTable::SlgFLOAT>());
			reg.iType = 2;
			reg.varType = "float";
			DescGenerator(reg);
		}
		else if (A.iType != 7) {
			reg.iType = 7;
			reg.varType = A.varType;
			reg.Reg = GetFreeRegister();
			DescGenerator(reg);
			printf("MOV %s, %s\n", reg.desc.c_str(), A.desc.c_str());
		}
		else {
			reg = A;
			DescGenerator(reg);
		}

		switch (op.iToken.m_Desc) {
		case SlgRuleTable::e_SlgTokenDesc::INVERT:
			printf("INV %s\n", A.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::NOT:
			printf("NOT %s\n", A.desc.c_str());
			return A;
		default:
			return s_StatementSegment{};
		}
	}
	//reg.varType = A.varType;
	inline s_StatementSegment SovlerAsmBinaryOperator(s_StatementSegment& op, s_StatementSegment& A, s_StatementSegment& B) {
		//检查部分

		//执行部分
		s_StatementSegment reg{};
		reg.varType = A.varType;


		if (A.iType == 1 && B.iType == 1) {
			reg = A;
			reg.iToken.m_Value = IntConstexprCalcValueBinary(op.iToken.m_Desc, A.iToken.GetValue<SlgRuleTable::SlgINT>(), B.iToken.GetValue<SlgRuleTable::SlgINT>());
			DescGenerator(reg);
			return reg;
		}
		else if (A.iType == 2 && B.iType == 2 || A.iType == 2 && B.iType == 1 || A.iType == 1 && B.iType == 2) {
			reg = A;
			reg.iToken.m_Value = FloatConstexprCalcValueBinary(op.iToken.m_Desc, A.iToken.GetValue<SlgRuleTable::SlgFLOAT>(), B.iToken.GetValue<SlgRuleTable::SlgFLOAT>());
			reg.iType = 2;
			reg.varType = "float";
			DescGenerator(reg);
			return reg;
		}
		else if (A.iType != 7) {
			reg.iType = 7;
			reg.Reg = GetFreeRegister();
			DescGenerator(reg);
			printf("MOV %s, %s\n", reg.desc.c_str(), A.desc.c_str());
		}
		else {
			reg = A;
			DescGenerator(reg);
		}

		switch (op.iToken.m_Desc) {
		case SlgRuleTable::e_SlgTokenDesc::ADD:
			printf("ADD %s, %s\n", reg.desc.c_str(), B.desc.c_str());
			return reg;
		case SlgRuleTable::e_SlgTokenDesc::SUB:
			printf("SUB %s, %s\n", reg.desc.c_str(), B.desc.c_str());
			return reg;
		case SlgRuleTable::e_SlgTokenDesc::MUL:
			printf("MUL %s, %s\n", reg.desc.c_str(), B.desc.c_str());
			return reg;
		case SlgRuleTable::e_SlgTokenDesc::DIV:
			printf("DIV %s, %s\n", reg.desc.c_str(), B.desc.c_str());
			return reg;
		case SlgRuleTable::e_SlgTokenDesc::MOD:
			printf("MOD %s, %s\n", reg.desc.c_str(), B.desc.c_str());
			return reg;
		default:
			return s_StatementSegment{};
		}
	}


	inline s_StatementSegment SovlerAsmAssignmentOperator(s_StatementSegment& op, s_StatementSegment& A, s_StatementSegment& B) {
		switch (op.iToken.m_Desc) {
		case SlgRuleTable::e_SlgTokenDesc::ADDEQUAL:
			printf("ADDTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::SUBEQUAL:
			printf("SUBTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::MULEQUAL:
			printf("SUBTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::DIVEQUAL:
			printf("DIVTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::MODEQUAL:
			printf("MODTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::EQUAL:
			printf("MOV %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::ANDEQUAL:
			printf("ANDTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::OREQUAL:
			printf("ORTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::XOREQUAL:
			printf("XORTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::INVERTEQUAL:
			printf("INVTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::LSHIFTEQUAL:
			printf("LSHTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		case SlgRuleTable::e_SlgTokenDesc::RSHIFTEQUAL:
			printf("RSHTO %s, %s\n", A.desc.c_str(), B.desc.c_str());
			return A;
		default:
			return s_StatementSegment{};
		}
	}

	// type， 0-错误， 1-属性， 2-属性成员变量， 3-属性成员函数， 4-变量， 5-变量的成员变量, 6-变量的成员函数, 7-普通函数
	// MName 所在模组的名字
	// VName - 变量 / 属性 / 函数 名字 
	// FName - 成员变量名字 / 成员函数名字 
	// argsBegin, argsEnd 如果有函数， 函数参数的范围， 包含 "(" 和 ")", argsEnd也是分析结束的地址

	inline bool GetIdentifierSegment(size_t start_idx, size_t limited_end_idx, int& type, std::string& MName, std::string& VName, std::string& FName, size_t& argsBegin, size_t& argsEnd) {
		size_t r_idx = start_idx;
		size_t bracketMask = 0;

		type = 0;
		MName = "";
		VName = "";
		FName = "";
		argsBegin = 0;
		argsEnd = 0;

		if (m_Tokens[start_idx].IsNumber()) {
			return false;
		}

		//GetModule
		bool hasModuleAccess = false;
		size_t temp_idx = r_idx;
		while (temp_idx <= limited_end_idx) {
			if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::MODULEACCESS) {
				m_ErrorMachine->AddError("Missing module name part before \"::\".", m_Tokens[temp_idx].m_LineNumber, m_Tokens[temp_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
				return false;
			}
			else if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::IDENTIFIER) {
				if (m_Tokens[temp_idx + 1].m_Desc == SlgRuleTable::e_SlgTokenDesc::MODULEACCESS) {
					MName += m_Tokens[temp_idx].GetValue<std::string>();
					MName += "::";
					hasModuleAccess = true;
					temp_idx++;
				}
				else {
					if (!hasModuleAccess) {
						MName = "";
					}
					else {
						MName.pop_back();
						MName.pop_back();
					}
					break;
				}
			}
			else {
				if (!hasModuleAccess) {
					MName = "";
				}
				else {
					MName.pop_back();
					MName.pop_back();
				}
				break;
			}
			temp_idx++;
		}

		bool isAttribute = false;
		if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::ATTRIBUTE) {
			temp_idx++;
			if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::IDENTIFIER) {
				isAttribute = true;
				VName = "@" + m_Tokens[temp_idx].GetValue<std::string>();
			}
			else {
				m_ErrorMachine->AddError("Missing attribute name after \"@\".", m_Tokens[temp_idx].m_LineNumber, m_Tokens[temp_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
				return false;
			}
		}
		else {
			if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::IDENTIFIER) {

				VName = m_Tokens[temp_idx].GetValue<std::string>();
			}
			else {
				if (hasModuleAccess) {
					m_ErrorMachine->AddError("Missing identifier after \"::\".", m_Tokens[temp_idx].m_LineNumber, m_Tokens[temp_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
				}
				else {
					m_ErrorMachine->AddError("Unknown opeartion in statement.", m_Tokens[temp_idx].m_LineNumber, m_Tokens[temp_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
				}
				return false;
			}
		}

		temp_idx++;

		//是否是函数
		if (!isAttribute) {
			if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_START) {

				argsBegin = temp_idx;
				bracketMask++;
				temp_idx++;
				while (temp_idx <= limited_end_idx) {
					if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_START) {
						bracketMask++;
					}
					else if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_END) {
						bracketMask--;
						if (bracketMask == 0) {

							if (m_Tokens[temp_idx + 1].m_Desc == SlgRuleTable::e_SlgTokenDesc::MEMBERACCESS) {
								m_ErrorMachine->AddError("Member Access can't used for function.", m_Tokens[temp_idx + 1].m_LineNumber, m_Tokens[temp_idx + 1].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
								return false;
							}
							else {
								type = 7;
								argsEnd = temp_idx;
								return true;
							}
						}
					}
					temp_idx++;
				}
			}
		}


		if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::MEMBERACCESS) {
			temp_idx++;
			if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::IDENTIFIER) {

				if (!isAttribute && hasModuleAccess) {
					type = 0;
					argsEnd = temp_idx;
					m_ErrorMachine->AddError("Access to normal variables of other modules is not allowed.", m_Tokens[temp_idx].m_LineNumber, m_Tokens[temp_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return false;
				}

				FName = m_Tokens[temp_idx].GetValue<std::string>();
				if (m_Tokens[temp_idx + 1].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_START) {
					temp_idx++;
					argsBegin = temp_idx;
					bracketMask++;
					temp_idx++;
					while (temp_idx <= limited_end_idx) {
						if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_START) {
							bracketMask++;
						}
						else if (m_Tokens[temp_idx].m_Desc == SlgRuleTable::e_SlgTokenDesc::BRACKET_ROUND_END) {
							bracketMask--;
							if (bracketMask == 0) {

								if (m_Tokens[temp_idx + 1].m_Desc == SlgRuleTable::e_SlgTokenDesc::MEMBERACCESS) {
									m_ErrorMachine->AddError("Member Access can't used for function.", m_Tokens[temp_idx + 1].m_LineNumber, m_Tokens[temp_idx + 1].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
									return false;
								}
								else {
									if (isAttribute) {
										type = 3;
									}
									else {
										type = 6;
									}
									argsEnd = temp_idx;
									return true;
								}
							}
						}
						temp_idx++;
					}
				}
				else {
					if (isAttribute) {
						type = 2;
						argsEnd = temp_idx;
						return true;
					}
					else {
						if (!hasModuleAccess) {

							type = 5;
							argsEnd = temp_idx;
							return true;
						}
						else {
							type = 0;
							argsEnd = temp_idx;
							m_ErrorMachine->AddError("Access to normal variables of other modules is not allowed.", m_Tokens[temp_idx].m_LineNumber, m_Tokens[temp_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
							return false;
						}
					}
				}
			}
		}
		else {
			//这个时候指针指向了 ; 或者其他符号
			if (isAttribute) {
				type = 1;
				argsEnd = temp_idx - 1;
				return true;
			}
			else {
				if (!hasModuleAccess) {

					type = 4;
					argsEnd = temp_idx - 1;
					return true;
				}
				else {
					type = 0;
					argsEnd = temp_idx - 1;
					m_ErrorMachine->AddError("Access to normal variables of other modules is not allowed.", m_Tokens[temp_idx].m_LineNumber, m_Tokens[temp_idx].m_ColNumber, SlgRuleTable::e_SlgErrorNumber::ERROR_UNEXPECTED_TOKEN);
					return false;
				}

			}
		}

		return false;
	}

	inline std::optional<SlgCompilerConfig::s_SlgFunction> FindFunction(const std::string& functionName, std::string& returnType, std::vector<std::string> argsType) {

	}

	inline std::optional<SlgCompilerConfig::s_SlgFunction> FindMemberFunction(const std::string& variableType, const std::string& memberFuntionName, std::string& returnType, std::vector<std::string> argsType) {

	}

	inline std::optional<SlgCompilerConfig::s_SlgMemberVarible> FindMemberVariable(const std::string& variableType, const std::string& memberName, std::string& type, bool isPrivate) {

	}

	inline SlgRuleTable::SlgINT IntConstexprCalcValueUnary(SlgRuleTable::e_SlgTokenDesc desc, SlgRuleTable::SlgINT value) {
		switch (desc) {
		case SlgRuleTable::e_SlgTokenDesc::NOT:
			return value == 0 ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::INVERT:
			return ~value;
		}
		return 0;
	}

	inline SlgRuleTable::SlgINT IntConstexprCalcValueBinary(SlgRuleTable::e_SlgTokenDesc desc, SlgRuleTable::SlgINT A, SlgRuleTable::SlgINT B) {
		switch (desc) {
		case SlgRuleTable::e_SlgTokenDesc::ADD:
			return A + B;
		case SlgRuleTable::e_SlgTokenDesc::SUB:
			return A - B;
		case SlgRuleTable::e_SlgTokenDesc::MUL:
			return A * B;
		case SlgRuleTable::e_SlgTokenDesc::DIV:
			if (B == 0) {
				return 0;
			}
			return A / B;
		case SlgRuleTable::e_SlgTokenDesc::MOD:
			if (A == 0 || B == 0) {
				return 0;
			}
			return A % B;
		case SlgRuleTable::e_SlgTokenDesc::AND:
			return A & B;
		case SlgRuleTable::e_SlgTokenDesc::OR:
			return A | B;
		case SlgRuleTable::e_SlgTokenDesc::SMALLER:
			return  A < B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::LARGER:
			return  A > B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::LSHIFT:
			return A << B;
		case SlgRuleTable::e_SlgTokenDesc::RSHIFT:
			return A >> B;
		case SlgRuleTable::e_SlgTokenDesc::XOR:
			return A ^ B;
		case SlgRuleTable::e_SlgTokenDesc::LOGICAND:
			return A && B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::LOGICOR:
			return  A || B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::NOTEQUAL:
			return  A != B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::ISEQUAL:
			return  A == B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::SMALLEREQUAL:
			return  A <= B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::LARGEREQUAL:
			return  A >= B ? 1 : 0;
		default:
			return 0;
		}
	}

	inline SlgRuleTable::SlgFLOAT FloatConstexprCalcValueUnary(SlgRuleTable::e_SlgTokenDesc desc, SlgRuleTable::SlgFLOAT value) {
		switch (desc) {
		case SlgRuleTable::e_SlgTokenDesc::NOT:
		case SlgRuleTable::e_SlgTokenDesc::INVERT:
			return std::abs(value) < 0.0001 == 0 ? 1.0 : 0.0;
		}
		return 0.0;
	}

	inline SlgRuleTable::SlgFLOAT FloatConstexprCalcValueBinary(SlgRuleTable::e_SlgTokenDesc desc, SlgRuleTable::SlgFLOAT A, SlgRuleTable::SlgFLOAT B) {
		switch (desc) {
		case SlgRuleTable::e_SlgTokenDesc::ADD:
			return A + B;
		case SlgRuleTable::e_SlgTokenDesc::SUB:
			return A - B;
		case SlgRuleTable::e_SlgTokenDesc::MUL:
			return A * B;
		case SlgRuleTable::e_SlgTokenDesc::DIV:
			if (std::abs(B) < 0.0001) {
				return 0;
			}
			return A / B;
		case SlgRuleTable::e_SlgTokenDesc::SMALLER:
			return  A < B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::LARGER:
			return  A > B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::LOGICAND:
			return A && B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::LOGICOR:
			return  A || B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::NOTEQUAL:
			return  A != B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::ISEQUAL:
			return  A == B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::SMALLEREQUAL:
			return  A <= B ? 1 : 0;
		case SlgRuleTable::e_SlgTokenDesc::LARGEREQUAL:
			return  A >= B ? 1 : 0;

			//浮点数不支持的操作
		case SlgRuleTable::e_SlgTokenDesc::LSHIFT:
		case SlgRuleTable::e_SlgTokenDesc::RSHIFT:
		case SlgRuleTable::e_SlgTokenDesc::XOR:
		case SlgRuleTable::e_SlgTokenDesc::MOD:
		case SlgRuleTable::e_SlgTokenDesc::AND:
		case SlgRuleTable::e_SlgTokenDesc::OR:
		default:
			return 0.0;
		}

	}

};