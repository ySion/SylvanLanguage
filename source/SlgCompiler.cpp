#include "SlgCompiler.hpp"

namespace SylvanLanguage {

	SourceCodeCompile::SourceCodeCompile(std::string codes, CompilerConfig* config, RunTimeNetWork* netWork, SModuleInfo* moduleInfo) : mCompilerConfig(config), mNetWork(netWork), mModuleInfo(moduleInfo) {
		mErrorMachine = std::make_unique<ErrorMachine>();
		mToken = std::make_unique<Tokenizer>(codes, mErrorMachine.get());
		if (mErrorMachine->Success()) {
			mToken->PrintTokens();
			mTokens = std::move(mToken->GetTokens());
			mModuleInfo->mAssemblyData = std::unique_ptr<AssemblyData>(new AssemblyData(1024));
			mAsmGen.BindAsmData(mModuleInfo->mAssemblyData.get());
			Splitstatement();
		}
		else {
			mErrorMachine->ShowErrors();
		}
	}

	bool SourceCodeCompile::GetResult() const {
		if (mErrorMachine->Success()) {
			return true;
		}
		else {
			mErrorMachine->ShowErrors();
			return false;
		}
	}

	void SourceCodeCompile::ShowAsm() {
		mModuleInfo->mAssemblyData->Show();
	}

	void SourceCodeCompile::ShowProgramTable() {

		for (auto i : mModuleInfo->mRunTimeAttributeTable) {
			std::cout << i.first << "\t\tType:" << i.second.mType << "\tIsPrivate:" << (i.second.mIsPrivate ? "yes" : "no") << std::endl;
		}
	}

	void SourceCodeCompile::Splitstatement() {

		size_t tokenSize = mTokens.size();
		size_t bracketDepth = 0;

		for (size_t i = 0; i < tokenSize; ++i) {

			switch (mTokens[i].mType)
			{
			case ETokenType::KEYWORD: {

				switch (mTokens[i].mDesc)
				{
				case ETokenDesc::KEYWORLD_using:
					if (!D_using(i, bracketDepth))
						return;
					break;
				case ETokenDesc::KEYWORLD_module:
					if (!D_module(i, bracketDepth))
						return;
					break;
				default:

					break;
				}
				break;
			}

			case ETokenType::IDENTIFIER:
				if (IsDeclare(i)) {
					if (!D_Declare(i, bracketDepth)) {
						return;
					}
					break;
				}
				else if (!D_IntentifierAnalysis(i, bracketDepth))
					return;
				break;

			case ETokenType::BRACKET: {
				if (mTokens[i].mDesc == ETokenDesc::BRACKET_CURLY_START) {
					bracketDepth++;
				}
				if (mTokens[i].mDesc == ETokenDesc::BRACKET_CURLY_END) {
					bracketDepth--;
				}
				break;
			}
			default:
				mErrorMachine->AddError("Meaningless Statement.", mTokens[i].mLineNumber, mTokens[i].mColNumber);
				return;
			}
			if (!mErrorMachine->Success()) {
				return;
			}
		}
	}

	bool SourceCodeCompile::D_using(size_t& idx, size_t& bDepth) {
		if (idx + 2 > mTokens.size()) {
			mErrorMachine->AddError("Missing content after using. Format: using \"module Path\";", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}

		if (bDepth != 0) {
			mErrorMachine->AddError("Using statement is not allowed in bracket.", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}
		idx++;
		if (mTokens[idx].mDesc != ETokenDesc::STRING) {
			mErrorMachine->AddError("Using stament need a const string as a argument.", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}

		std::string moduleName = mTokens[idx].GetValue<std::string>();
		mAsmGen.MODULE_DEP(moduleName);

		idx++;
		if (mTokens[idx].mDesc != ETokenDesc::END) {
			mErrorMachine->AddError("Losing ';' after statement.", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);

		}

		return true;
	}

	bool SourceCodeCompile::D_module(size_t& idx, size_t& bDepth) {
		if (idx + 2 > mTokens.size()) {
			mErrorMachine->AddError("Missing content after module.. Format: module \"module Path\"{ your code }", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}

		if (bDepth != 0) {
			mErrorMachine->AddError("module statement is not allowed in bracket.", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}

		if (mModuleInfo->mCurrentModuleName != "") {
			mErrorMachine->AddError("Only 1 module is allowed per source file", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}
		idx++;
		if (mTokens[idx].mDesc != ETokenDesc::STRING) {
			mErrorMachine->AddError("module stament need a const string as a argument.", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}

		std::string moduleName = mTokens[idx].GetValue<std::string>();
		if (moduleName == "") {
			mErrorMachine->AddError("Module name can't be empty! Character you can use: [0-9 a-z A-Z _]. ", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}

		mModuleInfo->mCurrentModuleName = moduleName;
		mAsmGen.MODULE_SELF(moduleName);

		idx++;
		if (mTokens[idx].mDesc != ETokenDesc::BRACKET_CURLY_START) {
			mErrorMachine->AddError("Can't declar empty module, Maybe you Losing '{' after statement.", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}
		bDepth++;
		return true;
	}

	bool SourceCodeCompile::D_Declare(size_t& idx, size_t& bDepth, bool statementCompile) {

		if (bDepth == 0) {
			mErrorMachine->AddError("Declare statement must be in code block.", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}

		bool attribute = false;
		bool function = false;
		bool Private = false;

		std::string typeName;
		std::string varName;

		typeName = mTokens.at(idx).GetValue<std::string>(); 	//获取类型名字
		idx++;

		if (mTokens.at(idx).mDesc == ETokenDesc::IDENTIFIER && mTokens.at(idx).GetValue<std::string>() == "private") {
			Private = true;
			idx++;
		}

		if (mTokens.at(idx).mDesc == ETokenDesc::ATTRIBUTE) {
			attribute = true;
			idx++;
		}

		if (mTokens.at(idx).mDesc != ETokenDesc::IDENTIFIER) {
			mErrorMachine->AddError("Declaration identifier must be followed by an identifier(a-z, A-Z, _, 0-9).", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}

		varName = mTokens.at(idx).GetValue<std::string>(); //获取字段名
		idx++;

		//检查默认定义, 赋值定义, 函数定义
		if (mTokens.at(idx).mDesc == ETokenDesc::END) {
			//默认定义
			if (attribute) {
				AttributeDefaultDefine(idx, typeName, varName, Private);
			}
			else {
				if (bDepth < 2) {
					mErrorMachine->AddError("Local variables can only be defined in functions", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
					return false;
				}
				else {
					LocalVariableDefaultDefine(idx, typeName, varName, bDepth);
				}
			}
		}
		else if (mTokens.at(idx).mDesc == ETokenDesc::EQUAL) {
			//赋值定义
			if (attribute) {
				AttributeAssignmentDefine(typeName, varName, idx, Private);
			}
			else {
				LocalVariableAssignmentDefine(typeName, varName, idx, bDepth);
			}
		}
		else if (mTokens.at(idx).mDesc == ETokenDesc::BRACKET_ROUND_START) {
			//函数定义
			FunctionDefine(typeName, varName, idx);
		}
		else {
			//错误
			mErrorMachine->AddError("Invalid statement found in the Declaration statement.", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
			return false;
		}

		return true;
	}

	size_t SourceCodeCompile::GetTypeSize(std::string tp) {
		auto itor = TypeRuler::SlgTypeSize.find(tp);
		if (itor != TypeRuler::SlgTypeSize.end()) {
			return itor->second;
		}
		return 0;
	}

	bool SourceCodeCompile::IsDeclare(size_t idx) {
		std::string name = mTokens[idx].GetValue<std::string>();
		auto itor = TypeRuler::SlgTypeSize.find(name);
		if (itor != TypeRuler::SlgTypeSize.end()) {
			return true;
		}
		else {
			return false;
		}
	}

	bool SourceCodeCompile::FunctionDefine(std::string returnType, std::string functionName, size_t& idx) {


		return true;
	}

	bool SourceCodeCompile::AttributeDefaultDefine(int r_idx, std::string varType, std::string varName, bool isPrivate) {

		varName = "@" + varName;
		if (mModuleInfo->mRunTimeAttributeTable.find(varName) == mModuleInfo->mRunTimeAttributeTable.end()) {

			mModuleInfo->mRunTimeAttributeTable[varName] = std::move(SModuleAttributeTable{ varType , isPrivate });
			return true;
		}
		else {
			mErrorMachine->AddError("Attribute \"" + varName + "\" has been defined.", mTokens[r_idx].mLineNumber, mTokens[r_idx].mColNumber);
			return false;
		}

	}

	bool SourceCodeCompile::LocalVariableDefaultDefine(int r_idx, std::string varType, std::string varName, int depth) {

		if (mLocalVariable.size() < depth - 1) {

			mLocalVariable.push_back(SLocalVariableCodeBlock{});
		}

		if (mLocalVariable[depth].mLocalVariable.find(varName) == mLocalVariable[depth].mLocalVariable.end()) {

			size_t size = GetTypeSize(varType);

			mLocalVariable[depth].mLocalVariable[varName].mTypeStr = varType;
			mLocalVariable[depth].mLocalVariable[varName].mVariableSize = size;
			mLocalVariable[depth].mLocalVariable[varName].mVariableOffest = mLocalVariableStackSize;
			mLocalVariable[depth].mSize += size;
			mLocalVariableStackSize += size;

			return true;
		}
		else {
			mErrorMachine->AddError("Local variable \"" + varName + "\"" + "has been defined.", mTokens[r_idx].mLineNumber, mTokens[r_idx].mColNumber);
			return false;
		}
	}

	bool SourceCodeCompile::AttributeAssignmentDefine(std::string varType, std::string varName, size_t& idx, bool isPrivate) {
		if (AttributeDefaultDefine(idx, varType, varName, isPrivate)) {
			size_t r_idx = idx - 2;
			size_t Bdepth = 0;
			if (D_IntentifierAnalysis(r_idx, Bdepth, true)) {
				idx = r_idx;
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	bool SourceCodeCompile::LocalVariableAssignmentDefine(std::string varType, std::string varName, size_t& idx, int depth) {
		if (LocalVariableDefaultDefine(idx, varType, varName, depth)) {
			size_t r_idx = idx - 1;
			size_t Bdepth = 0;
			if (D_IntentifierAnalysis(r_idx, Bdepth, false)) {
				idx = r_idx;
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	bool SourceCodeCompile::D_IntentifierAnalysis(size_t& idx, size_t& bDepth, bool ignoreDepth) {
		if (ignoreDepth == false) {
			if (bDepth == 0) {
				mErrorMachine->AddError("Identifier operation only write in code block.", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
				return false;
			}
		}
		size_t r_idx = idx;

		size_t bracketMask = 0;
		size_t end_idx = 0;

		while (r_idx < mTokens.size()) {

			if (r_idx >= mTokens.size()) {
				mErrorMachine->AddError("Statement missing end \";\".", mTokens[idx].mLineNumber, mTokens[idx].mColNumber);
				return false;
			}
			else if (mTokens[r_idx].IsKeyWord()) {
				mErrorMachine->AddError("Missing end \";\" before keyword.", mTokens[r_idx].mLineNumber, mTokens[r_idx].mColNumber);
				return false;
			}
			else if (mTokens[r_idx].mDesc == ETokenDesc::BRACKET_ROUND_START) {
				bracketMask++;
			}
			else if (mTokens[r_idx].mDesc == ETokenDesc::BRACKET_ROUND_END) {
				bracketMask--;
			}
			else if (mTokens[r_idx].IsBracket()) {
				mErrorMachine->AddError("Only round bracket is allowed to used in statement.", mTokens[r_idx].mLineNumber, mTokens[r_idx].mColNumber);
				return false;
			}
			else if (mTokens[r_idx].mDesc == ETokenDesc::END) {
				end_idx = r_idx - 1;
				break;
			}
			r_idx++;
		}

		if (bracketMask != 0) {
			mErrorMachine->AddError("The number of parentheses does not match.", mTokens[r_idx].mLineNumber, mTokens[r_idx].mColNumber);
			return false;
		}

		ExprSolver(idx, end_idx);
		FreeAllRegister();
		idx = r_idx;
		return mErrorMachine->Success();
	}

	int SourceCodeCompile::GetFreeRegister() {
		for (int i = 0; i < 64; i++) {
			if (mRegister[i] == 0) {
				mRegister[i] = 1;
				return i;
			}
		}
		return 63;
	}

	void SourceCodeCompile::FreeAllRegister() {
		for (int i = 0; i < 64; i++) {
			mRegister[i] = 0;
		}
	}

	SStatementSegment SourceCodeCompile::ExprSolver(size_t start_idx, size_t end_idx, int recursionDepth) {

		std::vector<SStatementSegment> statementList;

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

			if (mTokens[r_idx].mDesc == ETokenDesc::BRACKET_ROUND_START) {
				if (bracketMask == 0) {
					bracketStart = r_idx;
				}
				bracketMask++;

			}
			else if (mTokens[r_idx].mDesc == ETokenDesc::BRACKET_ROUND_END) {
				bracketMask--;
				if (bracketMask == 0) {
					if (r_idx - bracketStart < 4) {
						mErrorMachine->AddError("Meaningless sub statement.", mTokens[bracketStart].mLineNumber, mTokens[bracketStart].mColNumber);
						return SStatementSegment{};
					}
					SStatementSegment temp{};
					temp.Idx = bracketStart;
					temp.BasicType = 4;
					temp.EndPos = r_idx;
					statementList.push_back(temp);
				}
			}
			else if (bracketMask == 0 && mTokens[r_idx].IsOperator()) {
				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = 5;
				temp.Token = mTokens[r_idx];
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && mTokens[r_idx].mDesc == ETokenDesc::INT) {
				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = 1;
				temp.Token = mTokens[r_idx];
				temp.VarTypeDesc = "int";
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && mTokens[r_idx].mDesc == ETokenDesc::FLOAT) {
				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = 2;
				temp.Token = mTokens[r_idx];
				temp.VarTypeDesc = "float";
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && mTokens[r_idx].mDesc == ETokenDesc::STRING) {
				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = 3;
				temp.Token = mTokens[r_idx];
				temp.VarTypeDesc = "string";
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && GetIdentifierSegment(r_idx, end_idx, type, moduleName, mainName, memberName, argbegin, end)) {

				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = 0;
				temp.VariableType = type;
				temp.ModuleName = moduleName;
				temp.MainName = mainName;
				temp.MemberName = memberName;
				temp.ArgBegin = argbegin;
				temp.EndPos = end;
				statementList.push_back(temp);

				r_idx = end;
			}
			else {
				if (bracketMask == 0) {
					mErrorMachine->AddError("Statement error.", mTokens[r_idx].mLineNumber, mTokens[r_idx].mColNumber);
					return SStatementSegment{};
				}
			}
			r_idx++;
		}

		if (statementList.size() == 0) {
			return SStatementSegment{};
		}


		//检测运算符错误
		std::vector<int> propList{};

		int minProp = 99;
		int maxProp = 0;

		size_t list_idx = 0;
		SStatementSegment prev = statementList[list_idx];
		list_idx++;

		if (prev.BasicType == 5) {
			propList.push_back(prev.Token.GetOperatorProperity());
		}

		bool hasAssignmentOperator = false;
		for (; list_idx < statementList.size(); ++list_idx) {
			if (statementList[list_idx].BasicType == -1) {
				mErrorMachine->AddError("Unknown error, type = error.", mTokens[statementList[list_idx].Idx].mColNumber, mTokens[statementList[list_idx].Idx].mColNumber);
				return SStatementSegment{};
			}

			if (statementList[list_idx].BasicType < 5) {
				if (prev.BasicType != 5) {
					std::cout << "Error Pos" << statementList[list_idx].Idx << "\n";
					mErrorMachine->AddError("Missing operator before identifier.", mTokens[statementList[list_idx].Idx].mColNumber, mTokens[statementList[list_idx].Idx].mColNumber);
					return SStatementSegment{};
				}
			}
			else if (statementList[list_idx].BasicType == 5) {
				if (statementList[list_idx].Token.mDesc == ETokenDesc::SELFADD || statementList[list_idx].Token.mDesc == ETokenDesc::SELFSUB) {
					if (list_idx != 1) {
						mErrorMachine->AddError("Self Add/Sub operator is only allowed in the second location.", mTokens[statementList[list_idx].Idx].mColNumber, mTokens[statementList[list_idx].Idx].mColNumber);
						return SStatementSegment{};
					}
					else if (!(prev.BasicType == 0 && (prev.VariableType == 1 || prev.VariableType == 2 || prev.VariableType == 4 || prev.VariableType == 5))) {
						mErrorMachine->AddError("Self Add/Sub operator can only be used with variables, not with subexpressions and functions.", mTokens[statementList[list_idx].Idx].mColNumber, mTokens[statementList[list_idx].Idx].mColNumber);
						return SStatementSegment{};
					}
				}
				else if (statementList[list_idx].Token.IsUnaryOperator()) {
					if (prev.BasicType == 5 && prev.Token.IsUnaryOperator()) {
						mErrorMachine->AddError("Can't using double unary operator.", mTokens[statementList[list_idx].Idx].mLineNumber, mTokens[statementList[list_idx].Idx].mColNumber);
						return SStatementSegment{};

					}if (prev.BasicType == 5 && prev.Token.IsBinaryOperator()) {
						//correct
					}
					else {
						if (list_idx != 2) {
							mErrorMachine->AddError("Missing operator before unary operator.", mTokens[statementList[list_idx].Idx].mLineNumber, mTokens[statementList[list_idx].Idx].mColNumber);
							return SStatementSegment{};
						}
					}
				}
				else if (statementList[list_idx].Token.IsBinaryOperator()) {
					if (prev.BasicType < 5) {
						//correct
					}
					else {
						mErrorMachine->AddError("Missing number before binary operator.", mTokens[statementList[list_idx].Idx].mLineNumber, mTokens[statementList[list_idx].Idx].mColNumber);
						return SStatementSegment{};
					}
				}
				else if (statementList[list_idx].Token.IsAssignmentOperator()) {
					if (recursionDepth != 0) {
						mErrorMachine->AddError("Assignment operator is not allowed in the subexpressions.", mTokens[statementList[list_idx].Idx].mColNumber, mTokens[statementList[list_idx].Idx].mColNumber);
						return SStatementSegment{};
					}
					if (list_idx != 1) {
						mErrorMachine->AddError("Assignment operator is only allowed in the second location.", mTokens[statementList[list_idx].Idx].mColNumber, mTokens[statementList[list_idx].Idx].mColNumber);
						return SStatementSegment{};
					}
					else if (!(prev.BasicType == 0 && (prev.VariableType == 1 || prev.VariableType == 2 || prev.VariableType == 4 || prev.VariableType == 5))) {
						mErrorMachine->AddError("Assignment operator can only be used with variables, not with subexpressions and functions.", mTokens[statementList[list_idx].Idx].mColNumber, mTokens[statementList[list_idx].Idx].mColNumber);
						return SStatementSegment{};
					}
					hasAssignmentOperator = true;
				}

				propList.push_back(statementList[list_idx].Token.GetOperatorProperity());
			}
			else {
				mErrorMachine->AddError("Expressions unkown error.", mTokens[statementList[list_idx].Idx].mColNumber, mTokens[statementList[list_idx].Idx].mColNumber);
				return SStatementSegment{};
			}
			prev = statementList[list_idx];
		}


		//获取运算符优先级
		std::sort(propList.begin(), propList.end(), std::less<int>());
		propList.erase(std::unique(propList.begin(), propList.end()), propList.end());

		if (propList.empty()) {
			minProp = 0;
			maxProp = 16;
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
		else if (statementList.size() == 2 && recursionDepth == 0 && (statementList[1].Token.mDesc == ETokenDesc::SELFADD || statementList[1].Token.mDesc == ETokenDesc::SELFSUB)) { //自增自减运算符

			return statementList[0];
		}
		else if (hasAssignmentOperator) {

			//有赋值语句
			SStatementSegment B = ExprSolverCall(minProp, maxProp, minProp, statementList, 2, statementList.size() - 1);
			SStatementSegment A = VariableSolver(statementList[0]);
			if (!mErrorMachine->Success()) {
				return SStatementSegment{};
			}
			SStatementSegment res = SovlerAsmAssignmentOperator(statementList[1], A, B);
			if (!mErrorMachine->Success()) {
				return SStatementSegment{};
			}
			else {
				return res;
			}
		}
		else {
			SStatementSegment res = ExprSolverCall(minProp, maxProp, minProp, statementList, 0, statementList.size() - 1);
			if (!mErrorMachine->Success()) {
				return SStatementSegment{};
			}
			else {
				return res;
			}
		}
	}

	SStatementSegment SourceCodeCompile::ExprSolverCall(size_t minprop, size_t maxprop, size_t currentprop, std::vector<SStatementSegment>& items, size_t start, size_t end) {

		if (!mErrorMachine->Success()) {
			return SStatementSegment{};
		}

		if (currentprop > maxprop + 1) { return SStatementSegment{}; }

		if (end == start) {
			return SingleStatementSegmentSolver(items[start]);
		}

		if (currentprop == 14 && items[start].BasicType == 5 && items[start].Token.IsUnaryOperator()) { //一元运算符
			size_t next = start + 1;

			SStatementSegment val = SingleStatementSegmentSolver(items[next]);
			if (!mErrorMachine->Success()) {
				return SStatementSegment{};
			}
			return SovlerAsmUnaryOperator(items[start], val);
		}
		else {
			//0是变量， 1是int， 2是float, 3是string, 4是subStatement, 5是运算符, 6是栈空间，

			// type， 0-错误， 1-属性， 2-属性成员变量， 3-属性成员函数， 4-变量， 5-变量的成员变量, 6-变量的成员函数, 7-普通函数
			// MName 所在模组的名字
			// VName - 变量 / 属性 / 函数 名字 
			// FName - 成员变量名字 / 成员函数名字 
			// argsBegin, argsEnd 如果有函数， 函数参数的范围， 包含 "(" 和 ")", argsEnd也是分析结束的地址

			std::vector<SStatementSegment> varlayer;
			std::vector<SStatementSegment> oplayer;

			int nearlestPropDelta = 999;
			for (size_t i = start; i <= end; i++) {
				if (items[i].BasicType == 5) {
					int propDelta = items[i].Token.GetOperatorProperity() - currentprop;
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
				if (items[i].BasicType == 5) {

					if (items[i].Token.GetOperatorProperity() == currentprop) {
						oplayer.push_back(items[i]);
						SStatementSegment newVar = ExprSolverCall(minprop, maxprop, currentprop + nearlestPropDelta, items, s_mask, i - 1);
						if (!mErrorMachine->Success()) {
							return SStatementSegment{};
						}
						varlayer.push_back(newVar);
						s_mask = i + 1;
					}
				}
			}

			SStatementSegment newVar = ExprSolverCall(minprop, maxprop, currentprop + nearlestPropDelta, items, s_mask, end);
			if (!mErrorMachine->Success()) {
				return SStatementSegment{};
			}
			varlayer.push_back(newVar);

			if (oplayer.size() + 1 != varlayer.size()) {
				mErrorMachine->AddError("Opeartor list is null.", mTokens[items[start].Idx].mLineNumber, mTokens[items[start].Idx].mColNumber);
				return SStatementSegment{};
			}

			if (oplayer.size() == 0) {
				return newVar;
			}

			for (size_t i = 0; i < varlayer.size(); i++) {
				if (varlayer[i].VarTypeDesc == "void") {
					mErrorMachine->AddError("Function which doesn't have a return value can't used in operation statement.", mTokens[items[0].Idx].mLineNumber, mTokens[items[0].Idx].mColNumber);
					return SStatementSegment{};
				}
			}

			SStatementSegment A = varlayer[0];
			for (size_t i = 0; i < oplayer.size(); i++) {
				SStatementSegment B = varlayer[i + 1];
				A = SovlerAsmBinaryOperator(oplayer[i], A, B);
				if (!mErrorMachine->Success()) {
					return SStatementSegment{};
				}
			}
			DescGenerator(A);
			return A;
		}
	}

	SStatementSegment SourceCodeCompile::SingleStatementSegmentSolver(SStatementSegment& item) {
		if (!mErrorMachine->Success()) {
			return SStatementSegment{};
		}
		if (item.BasicType == 0) {
			if (item.VariableType == 1 || item.VariableType == 2 || item.VariableType == 4 || item.VariableType == 5) {
				return VariableSolver(item);
			}
			else if (item.VariableType == 3 || item.VariableType == 6 || item.VariableType == 7) {
				return FunctionSolver(item);
			}
			else {
				mErrorMachine->AddError("Unknow Token whitch type = error.", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
				return SStatementSegment{};
			}
		}
		else if (item.BasicType == 1 || item.BasicType == 2 || item.BasicType == 3) {
			return ConstSolver(item);
		}
		else if (item.BasicType == 4) {
			return ExprSolver(item.Idx + 1, item.EndPos - 1, 1);
		}
		else {
			mErrorMachine->AddError("Unknow Token whitch itype = error.", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
			return SStatementSegment{};
		}
	}

	SStatementSegment SourceCodeCompile::FunctionSolver(SStatementSegment& item) {
		//函数参数分离

		std::vector<std::string> args{};
		std::string retType{};

		bool NeedCheck = true;

		//获取函数信息部分
		if (item.VariableType == 7) { // 如果是普通函数
			if (item.ModuleName != "") {  //如果是外部函数
				NeedCheck = false;
			}
			else {
				auto it = mCompilerConfig->InlineFunction.find(item.MainName);
				if (it != mCompilerConfig->InlineFunction.end()) {
					retType = it->second.returnType;
					args = it->second.arguments;
				}
				else {
					mErrorMachine->AddError("Function: \"" + item.MainName + "\" is not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
					return SStatementSegment{};
				}
			}
		}
		else {
			if (item.VariableType == 6) { // 如果是成员函数
				auto it = mCompilerConfig->InlineMemberFunction.find(item.VarTypeDesc);
				if (it != mCompilerConfig->InlineMemberFunction.end() && it->second.find(item.MemberName) != it->second.end()) {
					auto& func = it->second[item.MemberName];
					retType = func.returnType;
					args = func.arguments;
				}
				else {
					mErrorMachine->AddError("Local Variable: \"" + item.MainName + " \",Type: \"" + item.VarTypeDesc + "\" doesn't have Member Function \"" + item.MemberName + "\" not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
					return SStatementSegment{};
				}
			}
			else if (item.VariableType == 3) {

				auto it = mCompilerConfig->InlineMemberFunction.find(item.VarTypeDesc);
				if (it != mCompilerConfig->InlineMemberFunction.end() && it->second.find(item.MemberName) != it->second.end()) {
					auto& func = it->second[item.MemberName];
					retType = func.returnType;
					args = func.arguments;
				}
				else {
					mErrorMachine->AddError("Attribute: \"" + item.MainName + " \",Type: \"" + item.VarTypeDesc + "\" doesn't have Member Function \"" + item.MemberName + "\" not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
					return SStatementSegment{};
				}
			}
		}

		size_t begin = item.ArgBegin + 1;
		size_t end = item.EndPos - 1;

		std::vector<std::pair<size_t, size_t>> cArgs{};

		//获取现有函数的参数
		if (end - begin >= 0) {

			size_t bracketMask = 0;
			size_t argStart = begin;
			size_t idx = begin;
			for (; idx <= end; idx++) {
				if (mTokens[idx].mDesc == ETokenDesc::BRACKET_ROUND_START) {
					bracketMask++;
				}
				else if (mTokens[idx].mDesc == ETokenDesc::BRACKET_ROUND_END) {
					bracketMask--;
				}
				else if (mTokens[idx].mDesc == ETokenDesc::COMMA && bracketMask == 0) {
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
				if (item.VariableType == 3 || item.VariableType == 6) {
					mErrorMachine->AddError("Member Function: \"" + item.MemberName + " \" in Type: \"" + item.VarTypeDesc + "\" need the same number of arguments (need " + std::to_string(args.size()) + " arguments, but now have " + std::to_string(cArgs.size()) + ").", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
					return SStatementSegment{};
				}
				else {
					mErrorMachine->AddError("Function: \"" + item.MainName + "\" need the same number of arguments (need " + std::to_string(args.size()) + " arguments, but now have " + std::to_string(cArgs.size()) + ").", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
					return SStatementSegment{};
				}
			}
		}

		//函数执行
		for (size_t i = 0; i < cArgs.size(); ++i) {
			SStatementSegment temp = ExprSolver(cArgs[i].first, cArgs[i].second, 1);
			if (!mErrorMachine->Success()) {
				return SStatementSegment{};
			}

			if (NeedCheck) {
				if (temp.VarTypeDesc != args[i]) {
					if (temp.VarTypeDesc == "float" && args[i] == "int") {
						if (temp.BasicType == 2) {
							temp.BasicType = 1;
							temp.Token.mValue = temp.Token.GetValue<RuleTable::SlgINT>();
						}
						temp.VarTypeDesc = "int";
					}
					else if (temp.VarTypeDesc == "int" && args[i] == "float") {
						if (temp.BasicType == 1) {
							temp.BasicType = 2;
							temp.Token.mValue = temp.Token.GetValue<RuleTable::SlgFLOAT>();
						}
						temp.VarTypeDesc = "float";
					}
					else {
						if (item.VariableType == 3 || item.VariableType == 6) {
							mErrorMachine->AddError("Member Function: \"" + item.MemberName + " \" in Type: \"" + item.VarTypeDesc + "\" argument " + std::to_string(i) + "," + args[i] + ")is not match with \"" + temp.VarTypeDesc + "\"", mTokens[cArgs[i].first].mLineNumber, mTokens[cArgs[i].first].mColNumber);
							return SStatementSegment{};
						}
						else {
							mErrorMachine->AddError("Function: \"" + item.MainName + "\" argument (" + std::to_string(i) + "," + args[i] + ") is not match with \"" + temp.VarTypeDesc + "\"", mTokens[cArgs[i].first].mLineNumber, mTokens[cArgs[i].first].mColNumber);
							return SStatementSegment{};
						}
					}
				}
			}
			DescGenerator(temp);
			printf("PUSH %s\n", temp.VarNameDesc.c_str());
		}

		if (item.VariableType == 3)
		{
			if (item.ModuleName != "")
			{
				item.VarNameDesc = item.ModuleName + "::";
			}
			item.VarNameDesc += item.MainName + "." + item.MemberName;
			printf("CALLA %s\n", item.VarNameDesc.c_str());
		}
		else if (item.VariableType == 6)
		{
			item.VarNameDesc += item.MainName + "." + item.MemberName;
			printf("CALLV %s\n", item.VarNameDesc.c_str());
		}
		else if (item.VariableType == 7)
		{
			if (item.ModuleName != "")
			{
				item.VarNameDesc = item.ModuleName + "::";
			}
			item.VarNameDesc += item.MainName;
			printf("CALL %s\n", item.VarNameDesc.c_str());
		}

		SStatementSegment vtemp;
		vtemp.BasicType = 7;
		vtemp.RegisterID = GetFreeRegister();
		vtemp.VarTypeDesc = retType;
		DescGenerator(vtemp);
		if (retType != "void") {
			std::cout << "MOV " << DescGenerator(vtemp).VarNameDesc << ", RET\n";
		}
		return vtemp;
	}

	SStatementSegment SourceCodeCompile::VariableSolver(SStatementSegment& item) {

		if (item.VariableType == 1 || item.VariableType == 2) {
			if (item.ModuleName == "") {
				if (mModuleInfo->mRunTimeAttributeTable.find(item.MainName) == mModuleInfo->mRunTimeAttributeTable.end()) {
					mErrorMachine->AddError("Attribute \"" + item.MainName + "\" not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
					return SStatementSegment{};
				}
				else {
					item.VarTypeDesc = mModuleInfo->mRunTimeAttributeTable[item.MainName].mType;
				}
			}
			else {
				auto it = mNetWork->FindAttribute(item.ModuleName + "::" + item.ModuleName);
				if (it.has_value()) {
					item.VarTypeDesc = it.value().mType;
				}
				else {
					mErrorMachine->AddError("Attribute \"" + item.MainName + "\" not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
					return SStatementSegment{};
				}
			}

			if (item.VariableType == 2) {

				auto mFind = mCompilerConfig->InlineMemberVariable.find(item.VarTypeDesc);
				if (mFind == mCompilerConfig->InlineMemberVariable.end()) {
					mErrorMachine->AddError("Attribute: \"" + item.MainName + " \",Type: \"" + item.VarTypeDesc + "\" doesn't have Member Variable \"" + item.MemberName + "\" not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
					return SStatementSegment{};
				}

				auto mFind2 = mFind->second.find(item.MemberName);
				if (mFind2 == mFind->second.end()) {
					mErrorMachine->AddError("Attribute: \"" + item.MainName + " \",Type: \"" + item.VarTypeDesc + "\" doesn't have Member Variable \"" + item.MemberName + "\" not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
					return SStatementSegment{};
				}

				item.VarTypeDesc = mFind2->second.Type;

			}
		}
		else if (item.VariableType == 4 || item.VariableType == 5) {
			for (auto& i : mLocalVariable) {
				auto itor = i.mLocalVariable.find(item.MainName);
				if (itor == i.mLocalVariable.end()) {
					break;
				}
				else {
					item.VarTypeDesc = itor->second.mTypeStr;

					if (item.VariableType == 5) {
						auto mFind = mCompilerConfig->InlineMemberVariable.find(item.VarTypeDesc);
						if (mFind == mCompilerConfig->InlineMemberVariable.end()) {
							mErrorMachine->AddError("Local varaible: \"" + item.MainName + " \",Type: \"" + item.VarTypeDesc + "\" doesn't have Member Variable \"" + item.MemberName + "\" not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
							return SStatementSegment{};
						}

						auto mFind2 = mFind->second.find(item.MemberName);
						if (mFind2 == mFind->second.end()) {
							mErrorMachine->AddError("Local varaible: \"" + item.MainName + " \",Type: \"" + item.VarTypeDesc + "\" doesn't have Member Variable \"" + item.MemberName + "\" not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
							return SStatementSegment{};
						}

						item.VarTypeDesc = mFind2->second.Type;
					}
					return DescGenerator(item);
				}
			}
			mErrorMachine->AddError("Local varaible \"" + item.MainName + "\" not defined", mTokens[item.Idx].mLineNumber, mTokens[item.Idx].mColNumber);
			return SStatementSegment{};
		}

		return DescGenerator(item);
	}

	//如果有，返回 返回类型 和汇编
	std::optional<std::pair<std::string, std::string>> SourceCodeCompile::GetTypeCompatibleAsmForBinaryOperator(ETokenDesc op, std::string A, std::string B) {

		auto itor = TypeRuler::TypeBinaryOperatorCompatible.find(op);
		if (itor == TypeRuler::TypeBinaryOperatorCompatible.end()) {
			return std::nullopt;
		}
		else {
			auto& compatible = itor->second;
			auto itor2 = compatible.find(std::make_pair(A, B));
			if (itor2 == compatible.end()) {
				return std::nullopt;
			}
			else {
				return itor2->second;
			}
		}
	}

	std::optional<std::string> SourceCodeCompile::GetTypeCompatibleAsmForAssignmentOperator(ETokenDesc op, std::string A, std::string B) {

		auto itor = TypeRuler::TypeAssignmentOperatorCompatible.find(op);
		if (itor == TypeRuler::TypeAssignmentOperatorCompatible.end()) {
			return std::nullopt;
		}
		else {
			auto& compatible = itor->second;
			auto itor2 = compatible.find(std::make_pair(A, B));
			if (itor2 == compatible.end()) {
				return std::nullopt;
			}
			else {
				return itor2->second;
			}
		}
	}

	std::optional<std::string> SourceCodeCompile::GetTypeCompatibleAsmForUnaryOperator(ETokenDesc op, std::string A) {


		auto itor = TypeRuler::TypeUnaryOperatorCompatible.find(op);
		if (itor == TypeRuler::TypeUnaryOperatorCompatible.end()) {
			return std::nullopt;
		}
		else {
			auto& compatible = itor->second;
			auto itor2 = compatible.find(A);
			if (itor2 == compatible.end()) {
				return std::nullopt;
			}
			else {
				return itor2->second;
			}
		}
	}

	bool SourceCodeCompile::GetIdentifierSegment(size_t start_idx, size_t limited_end_idx, int& type, std::string& MName, std::string& VName, std::string& FName, size_t& argsBegin, size_t& argsEnd) {
		size_t r_idx = start_idx;
		size_t bracketMask = 0;

		type = 0;
		MName = "";
		VName = "";
		FName = "";
		argsBegin = 0;
		argsEnd = 0;

		if (mTokens[start_idx].IsNumber()) {
			return false;
		}

		//GetModule
		bool hasModuleAccess = false;
		size_t temp_idx = r_idx;
		while (temp_idx <= limited_end_idx) {
			if (mTokens[temp_idx].mDesc == ETokenDesc::MODULEACCESS) {
				mErrorMachine->AddError("Missing module name part before \"::\".", mTokens[temp_idx].mLineNumber, mTokens[temp_idx].mColNumber);
				return false;
			}
			else if (mTokens[temp_idx].mDesc == ETokenDesc::IDENTIFIER) {
				if (mTokens[temp_idx + 1].mDesc == ETokenDesc::MODULEACCESS) {
					MName += mTokens[temp_idx].GetValue<std::string>();
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
		if (mTokens[temp_idx].mDesc == ETokenDesc::ATTRIBUTE) {
			temp_idx++;
			if (mTokens[temp_idx].mDesc == ETokenDesc::IDENTIFIER) {
				isAttribute = true;
				VName = "@" + mTokens[temp_idx].GetValue<std::string>();
			}
			else {
				mErrorMachine->AddError("Missing attribute name after \"@\".", mTokens[temp_idx].mLineNumber, mTokens[temp_idx].mColNumber);
				return false;
			}
		}
		else {
			if (mTokens[temp_idx].mDesc == ETokenDesc::IDENTIFIER) {

				VName = mTokens[temp_idx].GetValue<std::string>();
			}
			else {
				if (hasModuleAccess) {
					mErrorMachine->AddError("Missing identifier after \"::\".", mTokens[temp_idx].mLineNumber, mTokens[temp_idx].mColNumber);
				}
				else {
					mErrorMachine->AddError("Unknown opeartion in statement.", mTokens[temp_idx].mLineNumber, mTokens[temp_idx].mColNumber);
				}
				return false;
			}
		}

		temp_idx++;

		//是否是函数
		if (!isAttribute) {
			if (mTokens[temp_idx].mDesc == ETokenDesc::BRACKET_ROUND_START) {

				argsBegin = temp_idx;
				bracketMask++;
				temp_idx++;
				while (temp_idx <= limited_end_idx) {
					if (mTokens[temp_idx].mDesc == ETokenDesc::BRACKET_ROUND_START) {
						bracketMask++;
					}
					else if (mTokens[temp_idx].mDesc == ETokenDesc::BRACKET_ROUND_END) {
						bracketMask--;
						if (bracketMask == 0) {

							if (mTokens[temp_idx + 1].mDesc == ETokenDesc::MEMBERACCESS) {
								mErrorMachine->AddError("Member Access can't used for function.", mTokens[temp_idx + 1].mLineNumber, mTokens[temp_idx + 1].mColNumber);
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

		if (mTokens[temp_idx].mDesc == ETokenDesc::MEMBERACCESS) {
			temp_idx++;
			if (mTokens[temp_idx].mDesc == ETokenDesc::IDENTIFIER) {

				if (!isAttribute && hasModuleAccess) {
					type = 0;
					argsEnd = temp_idx;
					mErrorMachine->AddError("Access to normal variables of other modules is not allowed.", mTokens[temp_idx].mLineNumber, mTokens[temp_idx].mColNumber);
					return false;
				}

				FName = mTokens[temp_idx].GetValue<std::string>();
				if (mTokens[temp_idx + 1].mDesc == ETokenDesc::BRACKET_ROUND_START) {
					temp_idx++;
					argsBegin = temp_idx;
					bracketMask++;
					temp_idx++;
					while (temp_idx <= limited_end_idx) {
						if (mTokens[temp_idx].mDesc == ETokenDesc::BRACKET_ROUND_START) {
							bracketMask++;
						}
						else if (mTokens[temp_idx].mDesc == ETokenDesc::BRACKET_ROUND_END) {
							bracketMask--;
							if (bracketMask == 0) {

								if (mTokens[temp_idx + 1].mDesc == ETokenDesc::MEMBERACCESS) {
									mErrorMachine->AddError("Member Access can't used for function.", mTokens[temp_idx + 1].mLineNumber, mTokens[temp_idx + 1].mColNumber);
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
							mErrorMachine->AddError("Access to normal variables of other modules is not allowed.", mTokens[temp_idx].mLineNumber, mTokens[temp_idx].mColNumber);
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
					mErrorMachine->AddError("Access to normal variables of other modules is not allowed.", mTokens[temp_idx].mLineNumber, mTokens[temp_idx].mColNumber);
					return false;
				}

			}
		}

		return false;
	}

	SStatementSegment SourceCodeCompile::DescGenerator(SStatementSegment& item) {

		if (item.BasicType == 1) {
			item.VarNameDesc = std::to_string(item.Token.GetValue<RuleTable::SlgINT>());
		}
		else if (item.BasicType == 2) {
			item.VarNameDesc = std::to_string(item.Token.GetValue<RuleTable::SlgFLOAT>());
		}
		else if (item.BasicType == 4) {
			item.VarNameDesc = item.Token.GetValue<std::string>();
		}
		else if (item.BasicType == 0) {
			if (item.VariableType == 1) {
				if (item.ModuleName != "")
				{
					item.VarNameDesc = item.ModuleName + "::";
				}
				item.VarNameDesc += item.MainName;
			}
			else if (item.VariableType == 2) {
				if (item.ModuleName != "")
				{
					item.VarNameDesc = item.ModuleName + "::";
				}
				item.VarNameDesc += item.MainName + "." + item.MemberName;
			}
			else if (item.VariableType == 4) {
				item.VarNameDesc = item.MainName;
			}
			else if (item.VariableType == 5) {
				item.VarNameDesc = item.MainName + "." + item.MemberName;
			}
		}
		else if (item.BasicType == 7) {
			item.VarNameDesc = "R" + std::to_string(item.RegisterID);
		}
		return item;
	}

	SStatementSegment SourceCodeCompile::SovlerAsmUnaryOperator(SStatementSegment& op, SStatementSegment& A) {
		if (!mErrorMachine->Success()) {
			return SStatementSegment{};
		}
		SStatementSegment reg{};

		if (A.BasicType == 1) {
			reg = A;
			reg.Token.mValue = IntConstexprCalcValueUnary(op.Token.mDesc, A.Token.GetValue<RuleTable::SlgINT>());
			reg.BasicType = 1;
			reg.VarTypeDesc = "int";
			DescGenerator(reg);
			return reg;
		}
		else if (A.BasicType == 2) {

			reg = A;
			reg.Token.mValue = FloatConstexprCalcValueUnary(op.Token.mDesc, A.Token.GetValue<RuleTable::SlgFLOAT>());
			reg.BasicType = 2;
			reg.VarTypeDesc = "float";
			DescGenerator(reg);
			return reg;
		}
		else if (A.BasicType != 7) {
			reg.BasicType = 7;
			reg.VarTypeDesc = A.VarTypeDesc;
			reg.RegisterID = GetFreeRegister();
			DescGenerator(reg);
			auto movRes = GetTypeCompatibleAsmForAssignmentOperator(ETokenDesc::EQUAL, reg.VarTypeDesc, reg.VarTypeDesc);
			if (movRes.has_value()) {
				printf("%s %s, %s\n", movRes.value().c_str(), reg.VarNameDesc.c_str(), A.VarNameDesc.c_str());
			}
			else {
				//std::cout << "Error:" << reg.VarNameDesc << "\n";
				mErrorMachine->AddError("Error type : " + reg.VarTypeDesc, op.Token.mColNumber, op.Token.mColNumber);
				return SStatementSegment{};
			}
		}
		else {
			reg = A;
			DescGenerator(reg);
		}

		auto result = GetTypeCompatibleAsmForUnaryOperator(op.Token.mDesc, reg.VarTypeDesc);
		if (result.has_value()) {
			printf("%s %s\n", result.value().c_str(), reg.VarNameDesc.c_str());
			return reg;
		}
		else {
			mErrorMachine->AddError("Operator " + op.Token.GetValue<std::string>() + " can't used for type " + reg.VarTypeDesc, mTokens[op.Idx].mColNumber, mTokens[op.Idx].mColNumber);
			return SStatementSegment{};
		}
	}

	SStatementSegment SourceCodeCompile::SovlerAsmBinaryOperator(SStatementSegment& op, SStatementSegment& A, SStatementSegment& B) {

		if (!mErrorMachine->Success()) {
			return SStatementSegment{};
		}
		SStatementSegment reg{};

		if (A.BasicType == 1 && B.BasicType == 1) {
			reg = A;
			reg.Token.mValue = IntConstexprCalcValueBinary(op.Token.mDesc, A.Token.GetValue<RuleTable::SlgINT>(), B.Token.GetValue<RuleTable::SlgINT>());
			reg.BasicType = 1;
			reg.VarTypeDesc = "int";
			DescGenerator(reg);
			return reg;
		}
		else if (A.BasicType == 2 && B.BasicType == 2 || A.BasicType == 2 && B.BasicType == 1 || A.BasicType == 1 && B.BasicType == 2) {
			reg = A;
			reg.Token.mValue = FloatConstexprCalcValueBinary(op.Token.mDesc, A.Token.GetValue<RuleTable::SlgFLOAT>(), B.Token.GetValue<RuleTable::SlgFLOAT>());
			reg.BasicType = 2;
			reg.VarTypeDesc = "float";
			DescGenerator(reg);
			return reg;
		}
		else if (A.BasicType != 7) {
			reg.BasicType = 7;
			reg.VarTypeDesc = A.VarTypeDesc;
			reg.RegisterID = GetFreeRegister();
			DescGenerator(reg);

			auto movRes = GetTypeCompatibleAsmForAssignmentOperator(ETokenDesc::EQUAL, reg.VarTypeDesc, reg.VarTypeDesc);
			if (movRes.has_value()) {
				printf("%s %s, %s\n", movRes.value().c_str(), reg.VarNameDesc.c_str(), A.VarNameDesc.c_str());
			}
			else {
				std::cout << "Error:" << reg.VarNameDesc << B.VarNameDesc << "\n";
				mErrorMachine->AddError("Error type : " + reg.VarTypeDesc, op.Token.mColNumber, op.Token.mColNumber);
				return SStatementSegment{};
			}
		}
		else {
			reg = A;
			DescGenerator(reg);
		}

		auto result = GetTypeCompatibleAsmForBinaryOperator(op.Token.mDesc, reg.VarTypeDesc, B.VarTypeDesc);

		if (result.has_value()) {
			reg.VarTypeDesc = result.value().first.c_str();
			printf("%s %s, %s\n", result.value().second.c_str(), reg.VarNameDesc.c_str(), B.VarNameDesc.c_str());
			return reg;
		}
		else {
			mErrorMachine->AddError("Operator " + op.Token.GetValue<std::string>() + " can't used for type " + reg.VarTypeDesc + " and " + B.VarTypeDesc + ".", mTokens[op.Idx].mColNumber, mTokens[op.Idx].mColNumber);
			return SStatementSegment{};
		}
	}
	SStatementSegment SourceCodeCompile::SovlerAsmAssignmentOperator(SStatementSegment& op, SStatementSegment& A, SStatementSegment& B) {
		if (!mErrorMachine->Success()) {
			return SStatementSegment{};
		}
		auto result = GetTypeCompatibleAsmForAssignmentOperator(op.Token.mDesc, A.VarTypeDesc, B.VarTypeDesc);

		if (result.has_value()) {
			printf("%s %s, %s\n", result.value().c_str(), A.VarNameDesc.c_str(), B.VarNameDesc.c_str());
			return A;
		}
		else {
			mErrorMachine->AddError("Operator " + op.Token.GetValue<std::string>() + " can't used for type " + A.VarTypeDesc + " and " + B.VarTypeDesc + ".", mTokens[op.Idx].mColNumber, mTokens[op.Idx].mColNumber);
			return SStatementSegment{};
		}
	}
	RuleTable::SlgINT SourceCodeCompile::IntConstexprCalcValueUnary(ETokenDesc desc, RuleTable::SlgINT value) {
		switch (desc) {
		case ETokenDesc::NOT:
			return value == 0 ? 1 : 0;
		case ETokenDesc::INVERT:
			return ~value;
		}
		return 0;
	}
	RuleTable::SlgINT SourceCodeCompile::IntConstexprCalcValueBinary(ETokenDesc desc, RuleTable::SlgINT A, RuleTable::SlgINT B) {
		switch (desc) {
		case ETokenDesc::ADD:
			return A + B;
		case ETokenDesc::SUB:
			return A - B;
		case ETokenDesc::MUL:
			return A * B;
		case ETokenDesc::DIV:
			if (B == 0) {
				return 0;
			}
			return A / B;
		case ETokenDesc::MOD:
			if (A == 0 || B == 0) {
				return 0;
			}
			return A % B;
		case ETokenDesc::AND:
			return A & B;
		case ETokenDesc::OR:
			return A | B;
		case ETokenDesc::SMALLER:
			return  A < B ? 1 : 0;
		case ETokenDesc::LARGER:
			return  A > B ? 1 : 0;
		case ETokenDesc::LSHIFT:
			return A << B;
		case ETokenDesc::RSHIFT:
			return A >> B;
		case ETokenDesc::XOR:
			return A ^ B;
		case ETokenDesc::LOGICAND:
			return A && B ? 1 : 0;
		case ETokenDesc::LOGICOR:
			return  A || B ? 1 : 0;
		case ETokenDesc::NOTEQUAL:
			return  A != B ? 1 : 0;
		case ETokenDesc::ISEQUAL:
			return  A == B ? 1 : 0;
		case ETokenDesc::SMALLEREQUAL:
			return  A <= B ? 1 : 0;
		case ETokenDesc::LARGEREQUAL:
			return  A >= B ? 1 : 0;
		default:
			return 0;
		}
	}
	RuleTable::SlgFLOAT SourceCodeCompile::FloatConstexprCalcValueUnary(ETokenDesc desc, RuleTable::SlgFLOAT value) {
		switch (desc) {
		case ETokenDesc::NOT:
		case ETokenDesc::INVERT:
			return std::abs(value) < 0.0001 ? 1.0 : 0.0;
		}
		return 0.0;
	}
	RuleTable::SlgFLOAT SourceCodeCompile::FloatConstexprCalcValueBinary(ETokenDesc desc, RuleTable::SlgFLOAT A, RuleTable::SlgFLOAT B) {
		switch (desc) {
		case ETokenDesc::ADD:
			return A + B;
		case ETokenDesc::SUB:
			return A - B;
		case ETokenDesc::MUL:
			return A * B;
		case ETokenDesc::DIV:
			if (std::abs(B) < 0.0001) {
				return 0;
			}
			return A / B;
		case ETokenDesc::SMALLER:
			return  A < B ? 1 : 0;
		case ETokenDesc::LARGER:
			return  A > B ? 1 : 0;
		case ETokenDesc::LOGICAND:
			return A && B ? 1 : 0;
		case ETokenDesc::LOGICOR:
			return  A || B ? 1 : 0;
		case ETokenDesc::NOTEQUAL:
			return  A != B ? 1 : 0;
		case ETokenDesc::ISEQUAL:
			return  A == B ? 1 : 0;
		case ETokenDesc::SMALLEREQUAL:
			return  A <= B ? 1 : 0;
		case ETokenDesc::LARGEREQUAL:
			return  A >= B ? 1 : 0;

			//浮点数不支持的操作
		case ETokenDesc::LSHIFT:
		case ETokenDesc::RSHIFT:
		case ETokenDesc::XOR:
		case ETokenDesc::MOD:
		case ETokenDesc::AND:
		case ETokenDesc::OR:
		default:
			return 0.0;
		}
	}
	Tokenizer::Tokenizer(std::string codes, ErrorMachine* errorMachine) : mCode(codes), mPointer(0), mErrorMachine(errorMachine) {
		StartToken();
	}
	void Tokenizer::StartToken() {
		while (mCode.length() > mPointer) {
			mCurrentToken = GetNextToken();
			if (!Literal()) { return; }
		}
	}
	bool Tokenizer::Literal() {

		decltype(RuleTable::KeyWord)::iterator itor;
		switch ((ETokenType)((int)mCurrentToken.mType & 0xFF00))
		{
		case ETokenType::ERROR:
			mErrorMachine->AddError(mCurrentToken.GetValue<std::string>(), mCurrentToken.mLineNumber, 0);
			return false;

		case ETokenType::IDENTIFIER:
			itor = RuleTable::KeyWord.find(mCurrentToken.GetValue<std::string>());
			if (itor != std::end(RuleTable::KeyWord)) {
				mCurrentToken.mType = ETokenType::KEYWORD;
				mCurrentToken.mDesc = itor->second;
			}
			else {
				mCurrentToken.mType = ETokenType::IDENTIFIER;
				mCurrentToken.mDesc = ETokenDesc::IDENTIFIER;
			}
		case ETokenType::NUMBER:
		case ETokenType::BRACKET:
		case ETokenType::OPERATOR:
		case ETokenType::STATEMENTOPERATOR:
			if (mCurrentToken.mDesc == ETokenDesc::ATTRIBUTE)
			{
				mCurrentToken.mType = ETokenType::IDENTIFIER;
			}
			mTokens.emplace_back(mCurrentToken);
			break;
		}
		return true;
	}
	SlgTokenItem Tokenizer::GetNextToken() {
		std::string token = "";

		if (mCode[mPointer] == '\n') {
			mLine++;
			mPassCharacterCount = mPointer;
			mCol = 0;
		}
		else {
			mCol = mPointer - mPassCharacterCount;
		}

		//去注释
		if (mCode[mPointer] == '/') {
			if (mCode.length() > mPointer + 1) {
				if (mCode[mPointer + 1] == '/') {
					while (mCode.length() > mPointer && mCode[mPointer] != '\n') {
						mPointer++;
					}
					return SlgTokenItem(ETokenType::NULLVALUE, ETokenDesc::UNKNOWN, "", -1, -1);
				}
				else if (mCode[mPointer + 1] == '*') {

					while (mCode.length() > mPointer + 1 && (mCode[mPointer] != '*' || mCode[mPointer + 1] != '/')) {
						if (mCode[mPointer] == '\n') {
							mLine++;
							mPassCharacterCount = mPointer;
						}
						mPointer++;
					}
					mPointer += 2;
					return SlgTokenItem(ETokenType::NULLVALUE, ETokenDesc::UNKNOWN, "", -1, -1);
				}
			}
		}

		if (mCode[mPointer] == '-') {
			mPointer++;
			if (IsNumber(mCode[mPointer])) {
				do {
					token += mCode[mPointer];
					mPointer++;
				} while (mCode.length() > mPointer && IsNumber(mCode[mPointer]));

				if (mCode[mPointer] == '.') {
					mPointer++;
					std::string Decimal = "";
					while (mCode.length() > mPointer && IsNumber(mCode[mPointer])) {
						Decimal += mCode[mPointer];
						mPointer++;
					}
					return SlgTokenItem(ETokenType::FLOAT, ETokenDesc::FLOAT, -std::stof(token + "." + Decimal), mLine, mCol);
				}
				else {
					return SlgTokenItem(ETokenType::INT, ETokenDesc::INT, -std::stoll(token), mLine, mCol);
				}
			}
			else {
				mPointer--;
			}
		}

		//数字
		if (IsNumber(mCode[mPointer])) {
			do {
				token += mCode[mPointer];
				mPointer++;
			} while (mCode.length() > mPointer && IsNumber(mCode[mPointer]));

			if (mCode[mPointer] == '.') {
				mPointer++;
				std::string Decimal = "";
				while (mCode.length() > mPointer && IsNumber(mCode[mPointer])) {
					Decimal += mCode[mPointer];
					mPointer++;
				}
				return SlgTokenItem(ETokenType::FLOAT, ETokenDesc::FLOAT, std::stof(token + "." + Decimal), mLine, mCol);
			}
			else {
				return SlgTokenItem(ETokenType::INT, ETokenDesc::INT, std::stoll(token), mLine, mCol);
			}
		}

		//字段 标识符
		if (Isalpha(mCode[mPointer]) || mCode[mPointer] == '_') {
			do {
				token += mCode[mPointer];
				mPointer++;
			} while (mCode.length() > mPointer && (Isalnum(mCode[mPointer]) || mCode[mPointer] == '_'));
			return SlgTokenItem(ETokenType::IDENTIFIER, ETokenDesc::IDENTIFIER, token, mLine, mCol);
		}

		//字符串
		if (mCode[mPointer] == '\"') {
			mPointer++;
			while (mCode.length() > mPointer && (mCode[mPointer] != '\"' || mCode[mPointer - 1] == '\\')) {
				if (mCode[mPointer] == '\n') {
					return SlgTokenItem(ETokenType::ERROR, ETokenDesc::STRING, "The string must be fully written on one line", mLine, mCol);
				}
				else {
					token += mCode[mPointer];
					mPointer++;
				}
			}
			mPointer++;
			return SlgTokenItem(ETokenType::STRING, ETokenDesc::STRING, token, mLine, mCol);
		}

		if (mCode[mPointer] == ':') {
			mPointer++;
			if (mPointer < mCode.length() && mCode[mPointer] == ':') {
				token = "::";
				mPointer++;
				return SlgTokenItem(ETokenType::STATEMENTOPERATOR, ETokenDesc::MODULEACCESS, token, mLine, mCol);
			}
			else {
				return SlgTokenItem(ETokenType::ERROR, ETokenDesc::STRING, "Unknown operator \":\", maybe you wanna \"::\"?", mLine, mCol);
			}
		}

		//括号
		auto Bitor = RuleTable::Brackets.find(mCode[mPointer]);
		if (Bitor != std::end(RuleTable::Brackets)) {
			token += mCode[mPointer];
			mPointer++;
			return SlgTokenItem(ETokenType::BRACKET, Bitor->second, token, mLine, mCol);
		}

		//运算符
		auto itor1 = RuleTable::OperatorsSingle.find((char)mCode[mPointer]);
		if (itor1 != RuleTable::OperatorsSingle.end()) {

			token += mCode[mPointer];

			if (mPointer + 2 < mCode.length()) {
				auto itor2 = RuleTable::OperatorsMulti.find(token + mCode[mPointer + 1] + mCode[mPointer + 2]);
				if (itor2 != RuleTable::OperatorsMulti.end()) {
					mPointer += 3;
					return SlgTokenItem(ETokenType::OPERATOR, itor2->second, itor2->first, mLine, mCol);
				}
			}

			if (mPointer + 1 < mCode.length()) {
				auto itor2 = RuleTable::OperatorsMulti.find(token + mCode[mPointer + 1]);
				if (itor2 != RuleTable::OperatorsMulti.end()) {
					mPointer += 2;
					return SlgTokenItem(ETokenType::OPERATOR, itor2->second, itor2->first, mLine, mCol);
				}
			}

			mPointer++;
			return SlgTokenItem(ETokenType::OPERATOR, itor1->second, token, mLine, mCol);
		}

		auto Sitor = RuleTable::StatementOpeartor.find((char)mCode[mPointer]);
		if (Sitor != RuleTable::StatementOpeartor.end()) {
			token += Sitor->first;
			mPointer++;
			return SlgTokenItem(ETokenType::STATEMENTOPERATOR, Sitor->second, token, mLine, mCol);
		}

		mPointer++;
		return SlgTokenItem(ETokenType::NULLVALUE, ETokenDesc::UNKNOWN, "", -1, -1);
	}
};