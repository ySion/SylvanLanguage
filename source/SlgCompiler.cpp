#include "SlgCompiler.hpp"

namespace SylvanLanguage {

	SourceCodeCompile::SourceCodeCompile(std::string codes, CompilerConfig* config, RunTimeNetWork* netWork, SModuleInfo* moduleInfo) : mCompilerConfig(config), mNetWork(netWork), mModuleInfo(moduleInfo) {
		mErrorMachine = std::make_unique<ErrorMachine>();
		mToken = std::make_unique<Tokenizer>(codes, mErrorMachine.get());
		if (mErrorMachine->Success()) {
			mToken->PrintTokens();
			mTokens = std::move(mToken->GetTokens());
			mModuleInfo->mAssemblyData = std::unique_ptr<AssemblyData>(new AssemblyData(1024));
			mAsmGen = std::make_unique<D87AssemblyWriter>(mModuleInfo->mAssemblyData.get(), mCompilerConfig);
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

		for (auto i : mModuleInfo->mGlobalVariableTable) {
			std::cout << i.first << "\t\tType:" << i.second.mType << std::endl;
		}
	}

	void SourceCodeCompile::Splitstatement() {

		int tokenSize = mTokens.size();
		int bracketDepth = 0;

		for (int i = 0; i < tokenSize; ++i) {
			switch (mTokens.at(i).mType)
			{
			case ETokenType::KEYWORD: {

				switch (mTokens.at(i).mDesc)
				{
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
					if (!D_Declare(i, bracketDepth, false)) {
						return;
					}
				}
				else if (!D_IntentifierAnalysis(i, bracketDepth))
					return;
				break;

			case ETokenType::BRACKET: {
				if (mTokens.at(i).mDesc == ETokenDesc::BRACKET_CURLY_START) {
					bracketDepth++;
				}
				if (mTokens.at(i).mDesc == ETokenDesc::BRACKET_CURLY_END) {
					bracketDepth--;
					if (bracketDepth < 0) {
						mErrorMachine->AddError("Mismatched parentheses.", mTokens.at(i).mLineNumber, mTokens.at(i).mColNumber);
						return;
					}
				}
				break;
			}
			default:
				mErrorMachine->AddError("Meaningless Statement.", mTokens.at(i).mLineNumber, mTokens.at(i).mColNumber);
				return;
			}


			if (!mErrorMachine->Success()) {
				return;
			}
		}

		if (bracketDepth != 0) {
			mErrorMachine->AddError("Meaningless Statement.", mTokens.at(tokenSize - 1).mLineNumber, mTokens.at(tokenSize - 1).mColNumber);
			return;
		}
		printf("==================================================================================\n");
		for (auto& i : mFunctionDefineTable) {
			std::cout << i.mFunctionName << ":   " << i.IsExport << "  ";
			for (auto& i2 : i.mArgs) {
				std::cout << "(" << i2.first << "," << i2.second << ")  ";
			}
			std::cout << "RET " << i.mRetType << "   ";
			std::cout << "body " << i.mFunctionBody.first << " to " << i.mFunctionBody.second << "\n";

		}

		for (auto& i : mModuleInfo->mFunctionTable) {
			std::cout << i.first << ":   " << i.second.isExport << "  ";
			for (auto& i2 : i.second.mFunctionArgsTypeDesc) {
				std::cout << i2 << " ";
			}
			std::cout << "RET " << i.second.mFunctionReturnTypeDesc << "\n";
		}
		printf("==================================================================================\n");

		for (auto& i : mFunctionDefineTable) {
			D_FunctionBodySolver(i);
		}
	}

	bool SourceCodeCompile::D_FunctionBodySolver(SFunctionDefineInfo& info) {
		//在调用函数之前需要markBP

		int FunctionStartAddress = mAsmGen->data->GetCurrentSize();

		int tokenSize = info.mFunctionBody.second;
		int depth = 2;

		int argSize = 0;
		for (auto& i : info.mArgs) {
			std::string& type = i.first;
			std::string& name = i.second;

			if (LocalVariableDefaultDefineDetail(type, name, depth)) {
				int vsize = GetTypeSize(type);
				argSize += vsize;
			}
			else {
				return false;
			}
		}
		//BP MARK argSize;
		ASM_MARK_BP(argSize);

		int bodyStart = info.mFunctionBody.first + 1;
		int bodyEnd = info.mFunctionBody.second - 1;

		if (!D_CodeBlockSolver(bodyStart, bodyEnd, depth)) {
			return false;
		}

		ASM_FUNTCION_RET();
		mLocalVariable.clear();
		std::string funName = info.mFunctionName;
		std::cout << "FUNCTION " << funName << ": " << FunctionStartAddress << " to " << mAsmGen->data->GetCurrentSize() << std::endl;
		mModuleInfo->mFunctionTable[funName].CodeOffest = FunctionStartAddress;
		return true;
	}

	void SourceCodeCompile::ASM_FUNTCION_RET() {
		mAsmGen->FUNTCION_RET();
	}

	bool SourceCodeCompile::D_CodeBlockSolver(int idxStart, int idxEnd, int depth) {
		if (idxEnd - idxStart < 2) {
			return true;
		}


		int tokenSize = mTokens.size();
		int idx = idxStart;

		while (true) {
			if (idx >= idxEnd) {
				LocalVariableBlockPop();
				break;
			}
			switch (mTokens.at(idx).mDesc) {
			case ETokenDesc::BRACKET_CURLY_START: {
				int bracketDepth = 1;
				idx++;

				int subStart = idx;
				while (bracketDepth == 0) {
					if (mTokens.at(idx).mDesc == ETokenDesc::BRACKET_CURLY_START) {
						bracketDepth++;
					}
					else if (mTokens.at(idx).mDesc == ETokenDesc::BRACKET_CURLY_END) {
						bracketDepth--;
						if (bracketDepth == 0) {
							D_CodeBlockSolver(subStart, idx - 1, depth + 1);
							break;
						}
					}
					idx++;
				}
			}break;
			case ETokenDesc::IDENTIFIER:
				std::cout << "====================================Depth " << depth << '\n';
				if (IsDeclare(idx)) {
					if (!D_Declare(idx, depth, true)) {
						return false;
					}
				}
				else if (!D_IntentifierAnalysis(idx, depth))
					return false;
				break;

			case ETokenDesc::KEYWORLD_return: break;
			case ETokenDesc::KEYWORLD_break: break;

			}
			idx++;
		}
		return true;
	}


	bool SourceCodeCompile::D_module(int& idx, int& bDepth) {
		if (idx + 2 > mTokens.size()) {
			mErrorMachine->AddError("Missing content after module.. Format: module \"module Path\"{ your code }", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
			return false;
		}

		if (bDepth != 0) {
			mErrorMachine->AddError("module statement is not allowed in bracket.", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
			return false;
		}

		if (mModuleInfo->mCurrentModuleName != "") {
			mErrorMachine->AddError("Only 1 module is allowed per source file", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
			return false;
		}
		idx++;
		if (mTokens.at(idx).mDesc != ETokenDesc::STRING) {
			mErrorMachine->AddError("module stament need a const string as a argument.", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
			return false;
		}

		std::string moduleName = mTokens.at(idx).GetValue<std::string>();
		if (moduleName == "") {
			mErrorMachine->AddError("Module name can't be empty! Character you can use: [0-9 a-z A-Z _]. ", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
			return false;
		}

		mModuleInfo->mCurrentModuleName = moduleName;

		idx++;
		if (mTokens.at(idx).mDesc != ETokenDesc::BRACKET_CURLY_START) {
			mErrorMachine->AddError("Can't declar empty module, Maybe you Losing '{' after statement.", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
			return false;
		}
		bDepth++;
		return true;
	}

	bool SourceCodeCompile::D_Declare(int& idx, int bDepth, bool ignoreDepth) {
		if (ignoreDepth != true) {
			if (bDepth == 0) {
				mErrorMachine->AddError("Declare statement must be in code block.", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
				return false;
			}
		}
		bool function = false;

		std::string typeName;
		std::string varName;

		typeName = mTokens.at(idx).GetValue<std::string>(); 	//获取类型名字
		idx++;

		if (mTokens.at(idx).mDesc != ETokenDesc::IDENTIFIER) {
			mErrorMachine->AddError("Declaration identifier must be followed by an identifier(a-z, A-Z, _, 0-9) and start with no-number letter .", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
			return false;
		}

		varName = mTokens.at(idx).GetValue<std::string>(); //获取字段名
		idx++;

		//检查默认定义, 赋值定义, 函数定义
		if (mTokens.at(idx).mDesc == ETokenDesc::END) {
			//默认定义
			if (bDepth == 1) {
				return GlobalVariableDefaultDefine(idx, typeName, varName);
			}
			else {
				if (bDepth < 2) {
					mErrorMachine->AddError("Local variables can only be defined in functions", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
					return false;
				}
				else {
					return LocalVariableDefaultDefine(idx, typeName, varName, bDepth);
				}
			}
		}
		else if (mTokens.at(idx).mDesc == ETokenDesc::EQUAL) {
			//赋值定义
			if (bDepth == 1) {
				return GlobalVariableAssignmentDefine(typeName, varName, idx);
			}
			else {
				return LocalVariableAssignmentDefine(typeName, varName, idx, bDepth);
			}
		}
		else if (mTokens.at(idx).mDesc == ETokenDesc::BRACKET_ROUND_START) {
			//函数定义
			return FunctionDeclare(typeName, varName, idx);
		}
		else {
			//错误
			mErrorMachine->AddError("Invalid statement found in the Declaration statement.", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
			return false;
		}
	}

	int SourceCodeCompile::GetTypeSize(std::string tp) {
		auto itor = TypeRuler::SlgTypeSize.find(tp);
		if (itor != TypeRuler::SlgTypeSize.end()) {
			return itor->second;
		}
		return 0;
	}

	bool SourceCodeCompile::IsDeclare(int idx) {
		std::string name = mTokens.at(idx).GetValue<std::string>();
		auto itor = TypeRuler::SlgTypeSize.find(name);
		if (itor != TypeRuler::SlgTypeSize.end()) {
			return true;
		}
		else {
			return false;
		}
	}

	bool SourceCodeCompile::FunctionDeclare(std::string returnType, std::string functionName, int& idx) {
		//定义头
		SFunctionDefineInfo functionInfo{};
		functionInfo.mRetType = returnType;
		functionInfo.mFunctionName = functionName;

		int r_idx = idx + 1;

		while (true) {

			if (r_idx >= mTokens.size()) {
				mErrorMachine->AddError("Missing \")\" in Function defintion statement.", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
				return false;
			}
			else if (mTokens.at(r_idx).mDesc == ETokenDesc::IDENTIFIER) {
				std::string type = mTokens.at(r_idx).GetValue<std::string>();
				if (GetTypeSize(type) != 0) {
					r_idx++;
					if (mTokens.at(r_idx).mDesc == ETokenDesc::IDENTIFIER) {
						std::string vname = mTokens.at(r_idx).GetValue<std::string>();
						functionInfo.mArgs.push_back(std::make_pair(type, vname));
					}
					else {
						mErrorMachine->AddError("Need a Identifier after the type " + type + ".", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
						return false;
					}

				}
				else {
					mErrorMachine->AddError("Function Define unknown type :" + type + ".", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
					return false;
				}
			}
			else if (mTokens.at(r_idx).mDesc == ETokenDesc::COMMA) {
			}
			else if (mTokens.at(r_idx).mDesc == ETokenDesc::BRACKET_ROUND_END) {
				r_idx++;
				break;
			}
			else {
				mErrorMachine->AddError("Wrong character. In Function arguments list, you must use identifiers or commas.", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
				return false;
			}
			r_idx++;
		}

		if (mTokens.at(r_idx).mDesc == ETokenDesc::KEYWORLD_export) {
			functionInfo.IsExport = true;
			r_idx++;
		}

		if (mTokens.at(r_idx).mDesc == ETokenDesc::BRACKET_CURLY_START) {
			int bodyStart = r_idx;
			int bodyEnd = r_idx;
			r_idx++;
			int bracketMask = 1;
			while (true) {
				if (r_idx >= mTokens.size()) {
					mErrorMachine->AddError("Missing end of Function body.", mTokens.at(bodyStart).mLineNumber, mTokens.at(bodyStart).mColNumber);
					return false;
				}
				else if (mTokens.at(r_idx).mDesc == ETokenDesc::BRACKET_CURLY_END) {
					bracketMask--;
					if (bracketMask == 0) {
						bodyEnd = r_idx;
						functionInfo.mFunctionBody = std::make_pair(bodyStart, bodyEnd);

						//printf("\nFunction Name : %s\nReturn Type: %s\nIsExport : %s\nArgs: %d \n", functionInfo.mFunctionName.c_str(), functionInfo.mRetType.c_str(), (functionInfo.IsExport ? "yes" : "no"), functionInfo.mArgs.size());
						//for (auto& i : functionInfo.mArgs) {
						//	printf("| Type: %s    Name: %s\n", i.first.c_str(), i.second.c_str());
						//}
						//printf("FunctionBody: From %d to %d\n", functionInfo.mFunctionBody.first, functionInfo.mFunctionBody.second);

						idx = r_idx;
						break;
					}
				}
				else if (mTokens.at(r_idx).mDesc == ETokenDesc::BRACKET_CURLY_START) {
					bracketMask++;
				}

				r_idx++;
			}

		}
		else {
			mErrorMachine->AddError("Missing Function body.", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
			return false;
		}

		for (int i = 0; i < functionInfo.mArgs.size(); ++i) {
			for (int j = i + 1; j < functionInfo.mArgs.size(); ++j) {
				if (functionInfo.mArgs[i].second == functionInfo.mArgs[j].second) {
					mErrorMachine->AddError("Duplicate argument name: " + functionInfo.mArgs[i].second + ".", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
					return false;
				}
			}
		}

		auto itor = mModuleInfo->mFunctionTable.find(functionInfo.mFunctionName);
		if (itor != mModuleInfo->mFunctionTable.end()) {
			mErrorMachine->AddError("Function " + functionInfo.mFunctionName + " is already defined.", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
			return false;
		}
		else {
			mFunctionDefineTable.push_back(functionInfo);

			std::vector<std::string> types{};
			for (auto& i : functionInfo.mArgs) {
				types.push_back(i.first);
			}

			mModuleInfo->mFunctionTable[functionInfo.mFunctionName] = SModuleFunctionDesc{
				types,
				functionInfo.mRetType,
				0,
				functionInfo.IsExport
			};
		}

		return true;
	}



	bool SourceCodeCompile::GlobalVariableDefaultDefine(int r_idx, std::string varType, std::string varName) {

		auto it = FindGlobalVariable(varName);
		if (it.has_value()) {
			mErrorMachine->AddError("Global Variable \"" + varName + "\" has been defined.", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
			return false;
		}
		else {
			mModuleInfo->mGlobalVariableTable[varName] = std::move(SModuleGlobalVariableDesc{ varType });
			return true;
		}
	}

	bool SourceCodeCompile::LocalVariableDefaultDefine(int r_idx, std::string varType, std::string varName, int depth) {

		if (mLocalVariable.size() < depth - 1) {
			mLocalVariable.push_back(SLocalVariableCodeBlock{});
		}

		auto it = FindLocalVariable(varName);
		if (it.has_value()) {

			mErrorMachine->AddError("Local variable \"" + varName + "\"" + "has been defined.", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
			return false;
		}
		else {
			int size = GetTypeSize(varType);

			mLocalVariable[depth - 2].mLocalVariable[varName].mTypeStr = varType;
			mLocalVariable[depth - 2].mLocalVariable[varName].mVariableSize = size;
			mLocalVariable[depth - 2].mLocalVariable[varName].mVariableOffest = mLocalVariableStackSize;
			mLocalVariable[depth - 2].mSize += size;
			mLocalVariableStackSize += size;
			std::cout << "Local Variable Layer Create : " << depth - 2 << " Total Size: " << mLocalVariableStackSize << " Layer Count: " << mLocalVariable.size() << "\n";

			return true;
		}
	}

	bool SourceCodeCompile::LocalVariableDefaultDefineDetail(std::string varType, std::string varName, int depth) {

		if (mLocalVariable.size() < depth - 1) {
			mLocalVariable.push_back(SLocalVariableCodeBlock{});
		}

		int size = GetTypeSize(varType);

		mLocalVariable[depth - 2].mLocalVariable[varName].mTypeStr = varType;
		mLocalVariable[depth - 2].mLocalVariable[varName].mVariableSize = size;
		mLocalVariable[depth - 2].mLocalVariable[varName].mVariableOffest = mLocalVariableStackSize;
		mLocalVariable[depth - 2].mSize += size;
		mLocalVariableStackSize += size;

		std::cout << "Local Variable Layer Create : " << depth - 2 << " Total Size: " << mLocalVariableStackSize << " Layer Count: " << mLocalVariable.size() << "\n";

		return true;
	}
	void SourceCodeCompile::ASM_MARK_BP(int offest) {
		mAsmGen->MARK_BP(offest);
	}

	bool SourceCodeCompile::LocalVariableBlockPop() {
		if (mLocalVariable.size() < 1) return false;
		mLocalVariableStackSize -= mLocalVariable[mLocalVariable.size() - 1].mSize;

		std::cout << "Local Variable Layer Pop : " << mLocalVariable.size() - 1 << " Total Size: " << mLocalVariableStackSize << " Layer Count: " << mLocalVariable.size() - 1 << "\n";

		size_t idx = mLocalVariable.size() - 1;
		for (auto& i : mLocalVariable[idx].mLocalVariable) {
			if (i.second.mTypeStr == "string") {
				mAsmGen->POP_STR(i.second.mVariableOffest);
			}
		}
		mLocalVariable.erase(mLocalVariable.begin() + mLocalVariable.size() - 1);
		return true;
	}

	bool SourceCodeCompile::GlobalVariableAssignmentDefine(std::string varType, std::string varName, int& idx) {
		if (GlobalVariableDefaultDefine(idx, varType, varName)) {
			int r_idx = idx - 1;
			int Bdepth = 0;
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

	bool SourceCodeCompile::LocalVariableAssignmentDefine(std::string varType, std::string varName, int& idx, int depth) {
		if (LocalVariableDefaultDefine(idx, varType, varName, depth)) {
			int r_idx = idx - 1;

			if (D_IntentifierAnalysis(r_idx, depth, false)) {
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

	bool SourceCodeCompile::D_IntentifierAnalysis(int& idx, int bDepth, bool ignoreDepth) {

		if (ignoreDepth == false) {
			if (bDepth == 0) {
				mErrorMachine->AddError("Identifier operation only write in code block.", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
				return false;
			}
		}
		int r_idx = idx;

		int bracketMask = 0;
		int end_idx = 0;

		while (r_idx < mTokens.size()) {

			if (r_idx >= mTokens.size()) {
				mErrorMachine->AddError("Statement missing end \";\".", mTokens.at(idx).mLineNumber, mTokens.at(idx).mColNumber);
				return false;
			}
			else if (mTokens.at(r_idx).IsKeyWord()) {
				mErrorMachine->AddError("Missing end \";\" before keyword.", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
				return false;
			}
			else if (mTokens.at(r_idx).mDesc == ETokenDesc::BRACKET_ROUND_START) {
				bracketMask++;
			}
			else if (mTokens.at(r_idx).mDesc == ETokenDesc::BRACKET_ROUND_END) {
				bracketMask--;
			}
			else if (mTokens.at(r_idx).IsBracket()) {
				mErrorMachine->AddError("Only round bracket is allowed to used in statement.", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
				return false;
			}
			else if (mTokens.at(r_idx).mDesc == ETokenDesc::END) {
				end_idx = r_idx - 1;
				break;
			}
			r_idx++;
		}

		if (bracketMask != 0) {
			mErrorMachine->AddError("The number of parentheses does not match.", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
			return false;
		}

		ExprSolver(idx, end_idx);
		FreeAllRegister();
		idx = r_idx;
		return mErrorMachine->Success();
	}

	unsigned char SourceCodeCompile::GetFreeRegister() {
		for (int i = 0; i < 127; i++) {
			if (mRegister[i] == 0) {
				mRegister[i] = 1;
				return i;
			}
		}
		return 126;
	}

	void SourceCodeCompile::FreeAllRegister() {
		for (int i = 0; i < 127; i++) {
			mRegister[i] = 0;
		}
	}

	SStatementSegment SourceCodeCompile::ExprSolver(int start_idx, int end_idx, int recursionDepth) {

		std::vector<SStatementSegment> statementList;

		int r_idx = start_idx;
		EStatementVarType type;
		std::string moduleName;
		std::string mainName;
		std::string memberName;
		int argbegin;
		int end;
		int bracketMask = 0;

		int bracketStart = 0;
		while (r_idx <= end_idx) {

			if (mTokens.at(r_idx).mDesc == ETokenDesc::BRACKET_ROUND_START) {
				if (bracketMask == 0) {
					bracketStart = r_idx;
				}
				bracketMask++;

			}
			else if (mTokens.at(r_idx).mDesc == ETokenDesc::BRACKET_ROUND_END) {
				bracketMask--;
				if (bracketMask == 0) {
					if (r_idx - bracketStart < 4) {
						mErrorMachine->AddError("Meaningless sub statement.", mTokens.at(bracketStart).mLineNumber, mTokens.at(bracketStart).mColNumber);
						return SStatementSegment{};
					}
					SStatementSegment temp{};
					temp.Idx = bracketStart;
					temp.BasicType = EStatementBasicType::SUBEXPR;
					temp.EndPos = r_idx;
					statementList.push_back(temp);
				}
			}
			else if (bracketMask == 0 && mTokens.at(r_idx).IsOperator()) {
				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = EStatementBasicType::OPERATOR;
				temp.Token = mTokens.at(r_idx);
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && mTokens.at(r_idx).mDesc == ETokenDesc::INT) {
				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = EStatementBasicType::CONST_INT;
				temp.Token = mTokens.at(r_idx);
				temp.VarTypeDesc = "int";
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && mTokens.at(r_idx).mDesc == ETokenDesc::FLOAT) {
				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = EStatementBasicType::CONST_FLOAT;
				temp.Token = mTokens.at(r_idx);
				temp.VarTypeDesc = "float";
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && mTokens.at(r_idx).mDesc == ETokenDesc::STRING) {
				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = EStatementBasicType::CONST_STRING;
				temp.Token = mTokens.at(r_idx);
				temp.VarTypeDesc = "string";
				statementList.push_back(temp);
			}
			else if (bracketMask == 0 && GetIdentifierSegment(r_idx, end_idx, type, moduleName, mainName, memberName, argbegin, end)) {

				SStatementSegment temp{};
				temp.Idx = r_idx;
				temp.BasicType = EStatementBasicType::VARTYPE;
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
					mErrorMachine->AddError("Statement error.", mTokens.at(r_idx).mLineNumber, mTokens.at(r_idx).mColNumber);
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

		int list_idx = 0;
		SStatementSegment prev = statementList[list_idx];
		list_idx++;

		if (prev.BasicType == EStatementBasicType::OPERATOR) {
			propList.push_back(prev.Token.GetOperatorProperity());
		}

		bool hasAssignmentOperator = false;
		for (; list_idx < statementList.size(); ++list_idx) {
			if (statementList[list_idx].BasicType == EStatementBasicType::ERROR_BASICTYPE) {
				mErrorMachine->AddError("Unknown error, type = error.", mTokens.at(statementList[list_idx].Idx).mColNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
				return SStatementSegment{};
			}

			if (statementList[list_idx].BasicType < EStatementBasicType::OPERATOR) {
				if (prev.BasicType != EStatementBasicType::OPERATOR) {
					mErrorMachine->AddError("Missing operator before identifier.", mTokens.at(statementList[list_idx].Idx).mColNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
					return SStatementSegment{};
				}
			}
			else if (statementList[list_idx].BasicType == EStatementBasicType::OPERATOR) {
				if (statementList[list_idx].Token.mDesc == ETokenDesc::SELFADD || statementList[list_idx].Token.mDesc == ETokenDesc::SELFSUB) {
					if (list_idx != 1) {
						mErrorMachine->AddError("Self Add/Sub operator is only allowed in the second location.", mTokens.at(statementList[list_idx].Idx).mColNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
						return SStatementSegment{};
					}
					else if (!(prev.BasicType == EStatementBasicType::VARTYPE && (prev.VariableType == EStatementVarType::NORMAL_VAR || prev.VariableType == EStatementVarType::MEMBER_VAR))) {
						mErrorMachine->AddError("Self Add/Sub operator can only be used with variables, not with subexpressions and functions.", mTokens.at(statementList[list_idx].Idx).mColNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
						return SStatementSegment{};
					}
				}
				else if (statementList[list_idx].Token.IsUnaryOperator()) {
					if (prev.BasicType == EStatementBasicType::OPERATOR && prev.Token.IsUnaryOperator()) {
						mErrorMachine->AddError("Can't using double unary operator.", mTokens.at(statementList[list_idx].Idx).mLineNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
						return SStatementSegment{};

					}if (prev.BasicType == EStatementBasicType::OPERATOR && prev.Token.IsBinaryOperator()) {
						//correct
					}
					else {
						if (list_idx != 2) {
							mErrorMachine->AddError("Missing operator before unary operator.", mTokens.at(statementList[list_idx].Idx).mLineNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
							return SStatementSegment{};
						}
					}
				}
				else if (statementList[list_idx].Token.IsBinaryOperator()) {
					if (prev.BasicType < EStatementBasicType::OPERATOR) {
						//correct
					}
					else {
						mErrorMachine->AddError("Missing number before binary operator.", mTokens.at(statementList[list_idx].Idx).mLineNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
						return SStatementSegment{};
					}
				}
				else if (statementList[list_idx].Token.IsAssignmentOperator()) {
					if (recursionDepth != 0) {
						mErrorMachine->AddError("Assignment operator is not allowed in the subexpressions.", mTokens.at(statementList[list_idx].Idx).mColNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
						return SStatementSegment{};
					}
					if (list_idx != 1) {
						mErrorMachine->AddError("Assignment operator is only allowed in the second location.", mTokens.at(statementList[list_idx].Idx).mColNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
						return SStatementSegment{};
					}
					else if (!(prev.BasicType == EStatementBasicType::VARTYPE && (prev.VariableType == EStatementVarType::NORMAL_VAR || prev.VariableType == EStatementVarType::MEMBER_VAR))) {
						mErrorMachine->AddError("Assignment operator can only be used with variables, not with subexpressions and functions.", mTokens.at(statementList[list_idx].Idx).mColNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
						return SStatementSegment{};
					}
					hasAssignmentOperator = true;
				}

				propList.push_back(statementList[list_idx].Token.GetOperatorProperity());
			}
			else {
				mErrorMachine->AddError("Expressions unkown error.", mTokens.at(statementList[list_idx].Idx).mColNumber, mTokens.at(statementList[list_idx].Idx).mColNumber);
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

	SStatementSegment SourceCodeCompile::ExprSolverCall(int minprop, int maxprop, int currentprop, std::vector<SStatementSegment>& items, int start, int end) {

		if (!mErrorMachine->Success()) {
			return SStatementSegment{};
		}

		if (currentprop > maxprop + 1) { return SStatementSegment{}; }

		if (end == start) {
			return SingleStatementSegmentSolver(items[start]);
		}

		if (currentprop == 14 && items[start].BasicType == EStatementBasicType::OPERATOR && items[start].Token.IsUnaryOperator()) { //一元运算符
			int next = start + 1;

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
			for (int i = start; i <= end; i++) {
				if (items[i].BasicType == EStatementBasicType::OPERATOR) {
					int propDelta = items[i].Token.GetOperatorProperity() - currentprop;
					if (propDelta > 0) {
						nearlestPropDelta = std::min(nearlestPropDelta, propDelta);
					}
				}
			}
			int s_mask = start;

			if (nearlestPropDelta > 20) {
				nearlestPropDelta = 0;
			}

			for (int i = start; i <= end; i++) {
				if (items[i].BasicType == EStatementBasicType::OPERATOR) {

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
				mErrorMachine->AddError("Opeartor list is null.", mTokens.at(items[start].Idx).mLineNumber, mTokens.at(items[start].Idx).mColNumber);
				return SStatementSegment{};
			}

			if (oplayer.size() == 0) {
				return newVar;
			}

			for (int i = 0; i < varlayer.size(); i++) {
				if (varlayer[i].VarTypeDesc == "void") {
					mErrorMachine->AddError("Function which doesn't have a return value can't used in operation statement.", mTokens.at(items[0].Idx).mLineNumber, mTokens.at(items[0].Idx).mColNumber);
					return SStatementSegment{};
				}
			}

			SStatementSegment A = varlayer[0];
			for (int i = 0; i < oplayer.size(); i++) {
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
		if (item.BasicType == EStatementBasicType::VARTYPE) {
			if (item.VariableType == EStatementVarType::MEMBER_VAR || item.VariableType == EStatementVarType::NORMAL_VAR) {
				return VariableSolver(item);
			}
			else if (item.VariableType == EStatementVarType::NORMAL_FUNC || item.VariableType == EStatementVarType::MEMBER_FUNC) {
				return FunctionSolver(item);
			}
			else {
				mErrorMachine->AddError("Unknow Token whitch type = error.", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
				return SStatementSegment{};
			}
		}
		else if (item.BasicType == EStatementBasicType::CONST_INT || item.BasicType == EStatementBasicType::CONST_FLOAT || item.BasicType == EStatementBasicType::CONST_STRING) {
			return ConstSolver(item);
		}
		else if (item.BasicType == EStatementBasicType::SUBEXPR) {
			return ExprSolver(item.Idx + 1, item.EndPos - 1, 1);
		}
		else {
			mErrorMachine->AddError("Unknow Token whitch itype = error.", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
			return SStatementSegment{};
		}
	}

	SStatementSegment SourceCodeCompile::FunctionSolver(SStatementSegment& item) {

		std::vector<std::string> targetArgs{};
		std::string retType{};
		std::string inlineFunctionASM;

		if (item.VariableType == EStatementVarType::NORMAL_FUNC) { // 普通函数
			if (item.ModuleName != "") {
				auto res = FindCustomFunction(item.ModuleName, item.MainName);
				if (res.has_value()) {
					retType = res.value().mFunctionReturnTypeDesc;
					targetArgs = res.value().mFunctionArgsTypeDesc;
				}
				else {
					mErrorMachine->AddError("Function: \"" + item.MainName + "\" is not defined", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
					return SStatementSegment{};
				}
			}
			else {
				auto res = FindInlineFunction(item.MainName);
				auto res2 = FindCustomFunction(item.ModuleName, item.MainName);
				auto res3 = FindBindingFunction(item.MainName);
				if (res.has_value()) {
					retType = res.value().returnType;
					targetArgs = res.value().arguments;
					inlineFunctionASM = res.value().assemblyStr;
				}
				else {
					if (res2.has_value()) {
						retType = res2.value().mFunctionReturnTypeDesc;
						targetArgs = res2.value().mFunctionArgsTypeDesc;
					}
					else {
						if (res3.has_value()) {
							retType = res3.value().returnType;
							targetArgs = res3.value().arguments;
						}
						else {
							mErrorMachine->AddError("Function: \"" + item.MainName + "\" is not defined", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
							return SStatementSegment{};
						}
					}
				}
			}
		}
		else {
			if (item.VariableType == EStatementVarType::MEMBER_FUNC) { // 成员函数

				auto res = FindLocalVariable(item.MainName);
				auto resg = FindGlobalVariable(item.MainName);

				std::string varType = "";
				if (res.has_value()) { varType = res.value().second.mTypeStr; }
				else if (resg.has_value()) { varType = resg.value().mType; }

				if (varType != "") {
					auto res2 = FindMemberFunction(varType, item.MemberName);
					if (res2.has_value()) {
						retType = res2.value().returnType;
						targetArgs = res2.value().arguments;
						inlineFunctionASM = res2.value().assemblyStr;
					}
					else {
						if (res.has_value()) {
							mErrorMachine->AddError("Local Variable: \"" + item.MainName + " \",Type: \"" + varType + "\" doesn't have Member Function \"" + item.MemberName + "\" not defined", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
						}
						else {
							mErrorMachine->AddError("Global Variable: \"" + item.MainName + " \",Type: \"" + varType + "\" doesn't have Member Function \"" + item.MemberName + "\" not defined", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
						}
						return SStatementSegment{};
					}
				}
				else {
					mErrorMachine->AddError("Variable \"" + item.MainName + "\" not defined", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
					return SStatementSegment{};
				}
			}
		}

		int begin = item.ArgBegin + 1;
		int end = item.EndPos - 1;

		std::vector<std::pair<int, int>> crueentArgs{};

		//获取现有函数的参数
		if (end - begin >= 0) {

			int bracketMask = 0;
			int argStart = begin;
			int idx = begin;
			for (; idx <= end; idx++) {
				if (mTokens.at(idx).mDesc == ETokenDesc::BRACKET_ROUND_START) {
					bracketMask++;
				}
				else if (mTokens.at(idx).mDesc == ETokenDesc::BRACKET_ROUND_END) {
					bracketMask--;
				}
				else if (mTokens.at(idx).mDesc == ETokenDesc::COMMA && bracketMask == 0) {
					crueentArgs.push_back({ argStart, idx - 1 });
					argStart = idx + 1;
				}
			}
			if (argStart < idx) {
				crueentArgs.push_back({ argStart, idx - 1 });
			}
		}

		if (crueentArgs.size() != targetArgs.size()) {
			if (item.VariableType == EStatementVarType::MEMBER_FUNC) {
				mErrorMachine->AddError("Member Function: \"" + item.MemberName + " \" in Type: \"" + item.VarTypeDesc + "\" need the same number of arguments (need " + std::to_string(targetArgs.size()) + " arguments, but now have " + std::to_string(crueentArgs.size()) + ").", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
				return SStatementSegment{};
			}
			else {
				mErrorMachine->AddError("Function: \"" + item.MainName + "\" need the same number of arguments (need " + std::to_string(targetArgs.size()) + " arguments, but now have " + std::to_string(crueentArgs.size()) + ").", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
				return SStatementSegment{};
			}
		}


		//函数执行
		for (int i = 0; i < crueentArgs.size(); ++i) {
			SStatementSegment temp = ExprSolver(crueentArgs[i].first, crueentArgs[i].second, 1);
			if (!mErrorMachine->Success()) {
				return SStatementSegment{};
			}

			//push
			auto res = GetTypeCompatibleAsmForPush(temp.VarTypeDesc, targetArgs[i]);
			if (res.has_value()) {
				DescGenerator(temp);
				ASM_PUSH(res.value(), temp);
			}
			else {
				if (item.VariableType == EStatementVarType::MEMBER_FUNC) {
					mErrorMachine->AddError("Member Function: \"" + item.MemberName + " \" in Type: \"" + item.VarTypeDesc + "\" argument " + std::to_string(i) + "," + targetArgs[i] + ")is not match with \"" + temp.VarTypeDesc + "\"", mTokens.at(crueentArgs[i].first).mLineNumber, mTokens.at(crueentArgs[i].first).mColNumber);
					return SStatementSegment{};
				}
				else {
					mErrorMachine->AddError("Function: \"" + item.MainName + "\" argument (" + std::to_string(i) + "," + targetArgs[i] + ") is not match with \"" + temp.VarTypeDesc + "\"", mTokens.at(crueentArgs[i].first).mLineNumber, mTokens.at(crueentArgs[i].first).mColNumber);
					return SStatementSegment{};
				}
			}
		}

		if (item.VariableType == EStatementVarType::MEMBER_FUNC) {
			if (inlineFunctionASM != "") {
				ASM_CALL_MEMBER_FUNC(inlineFunctionASM, item);
			}
		}
		else if (item.VariableType == EStatementVarType::NORMAL_FUNC) {

			if (inlineFunctionASM != "") {
				ASM_CALL_INLINE(inlineFunctionASM);
				printf("%s\n\n", inlineFunctionASM.c_str());
			}
			else {
				item.VarNameDesc = item.ModuleName + "::" + item.MainName;
				ASM_CALL(item.ModuleName, item.MainName);
				printf("CALL %s\n\n", item.VarNameDesc.c_str());
			}
		}

		SStatementSegment vtemp;
		vtemp.BasicType = EStatementBasicType::REG;
		vtemp.RegisterID = GetFreeRegister();
		vtemp.VarTypeDesc = retType;
		DescGenerator(vtemp);

		SStatementSegment RET;
		RET.BasicType = EStatementBasicType::REG;
		RET.RegisterID = 127;
		RET.VarTypeDesc = retType;
		DescGenerator(RET);

		if (retType != "void") {

			auto movRes = GetTypeCompatibleAsmForAssignmentOperator(ETokenDesc::EQUAL, vtemp.VarTypeDesc, vtemp.VarTypeDesc);
			if (movRes.has_value()) {
				ASM_BINARY(movRes.value(), vtemp, RET);
				//printf("%s %s, RET\n", movRes.value().c_str(), vtemp.VarNameDesc.c_str());
			}
			else {
				mErrorMachine->AddError("Unkown Function : " + item.VarNameDesc, mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
				return SStatementSegment{};
			}
		}
		return vtemp;
	}

	SStatementSegment SourceCodeCompile::VariableSolver(SStatementSegment& item) {

		auto res = FindLocalVariable(item.MainName);
		auto resg = FindGlobalVariable(item.MainName);

		std::string varType = "";
		if (res.has_value()) { varType = res.value().second.mTypeStr; }
		else if (resg.has_value()) { varType = resg.value().mType; }

		if (varType != "") {
			item.VarTypeDesc = varType;
		}
		else {
			mErrorMachine->AddError("Variable \"" + item.MainName + "\" not defined", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
			return SStatementSegment{};
		}

		if (item.VariableType == EStatementVarType::MEMBER_VAR) {

			auto res2 = FindMemberVariable(varType, item.MemberName);
			if (res2.has_value()) {
				item.VarTypeDesc = res2.value().Type;
			}
			else {
				mErrorMachine->AddError("Local variable: \"" + item.MainName + " \",Type: \"" + varType + "\" doesn't have Member Variable \"" + item.MemberName + "\" not defined", mTokens.at(item.Idx).mLineNumber, mTokens.at(item.Idx).mColNumber);
				return SStatementSegment{};
			}
		}

		return DescGenerator(item);
	}

	inline SStatementSegment SourceCodeCompile::ConstSolver(SStatementSegment& item) {
		DescGenerator(item);

		if (item.BasicType == EStatementBasicType::CONST_INT) {
			item.VarTypeDesc = "int";
		}if (item.BasicType == EStatementBasicType::CONST_FLOAT) {
			item.VarTypeDesc = "float";
		}if (item.BasicType == EStatementBasicType::CONST_STRING) {
			item.VarTypeDesc = "string";
		}
		return item;
	}

	std::optional<std::string> SourceCodeCompile::GetTypeCompatibleAsmForPush(std::string A, std::string B)
	{
		auto itor = TypeRuler::TypePushRulerCompatible.find(A);
		if (itor != TypeRuler::TypePushRulerCompatible.end()) {
			auto itor2 = itor->second.find(B);
			if (itor2 != itor->second.end()) {
				return itor2->second;
			}
		}

		return std::nullopt;
	}

	//如果有，返回 返回类型 和汇编
	std::optional<std::pair<std::string, std::string>> SourceCodeCompile::GetTypeCompatibleAsmForBinaryOperator(ETokenDesc op, std::string A, std::string B) {

		auto itor = TypeRuler::TypeBinaryOperatorCompatible.find(op);
		if (itor != TypeRuler::TypeBinaryOperatorCompatible.end()) {
			auto& compatible = itor->second;
			auto itor2 = compatible.find(std::make_pair(A, B));
			if (itor2 != compatible.end()) {
				return itor2->second;
			}
		}
		return std::nullopt;
	}

	std::optional<std::string> SourceCodeCompile::GetTypeCompatibleAsmForAssignmentOperator(ETokenDesc op, std::string A, std::string B) {

		auto itor = TypeRuler::TypeAssignmentOperatorCompatible.find(op);
		if (itor != TypeRuler::TypeAssignmentOperatorCompatible.end()) {
			auto& compatible = itor->second;
			auto itor2 = compatible.find(std::make_pair(A, B));
			if (itor2 != compatible.end()) {
				return itor2->second;
			}
		}
		return std::nullopt;
	}

	std::optional<std::string> SourceCodeCompile::GetTypeCompatibleAsmForUnaryOperator(ETokenDesc op, std::string A) {

		auto itor = TypeRuler::TypeUnaryOperatorCompatible.find(op);
		if (itor != TypeRuler::TypeUnaryOperatorCompatible.end()) {
			auto& compatible = itor->second;
			auto itor2 = compatible.find(A);
			if (itor2 != compatible.end()) {
				return itor2->second;
			}
		}

		return std::nullopt;
	}

	bool SourceCodeCompile::GetIdentifierSegment(int start_idx, int limited_end_idx, EStatementVarType& type, std::string& moduleName, std::string& varName, std::string& memberName, int& argsBegin, int& argsEnd) {
		int r_idx = start_idx;
		int bracketMask = 0;

		type = EStatementVarType::ERROR_VARTYPE;
		moduleName = "";
		varName = "";
		memberName = "";
		argsBegin = 0;
		argsEnd = 0;

		if (mTokens.at(start_idx).IsNumber()) {
			return false;
		}

		bool hasModuleAccess = false;
		int temp_idx = r_idx;
		while (temp_idx <= limited_end_idx) {
			if (mTokens.at(temp_idx).mDesc == ETokenDesc::MODULEACCESS) {
				mErrorMachine->AddError("Missing module name part before \"::\".", mTokens.at(temp_idx).mLineNumber, mTokens.at(temp_idx).mColNumber);
				return false;
			}
			else if (mTokens.at(temp_idx).mDesc == ETokenDesc::IDENTIFIER) {
				if (mTokens.at(temp_idx + 1).mDesc == ETokenDesc::MODULEACCESS) {
					moduleName += mTokens.at(temp_idx).GetValue<std::string>();
					moduleName += "::";
					hasModuleAccess = true;
					temp_idx++;
				}
				else {
					if (!hasModuleAccess) {
						moduleName = "";
					}
					else {
						moduleName.pop_back();
						moduleName.pop_back();
					}
					break;
				}
			}
			else {
				if (!hasModuleAccess) {
					moduleName = "";
				}
				else {
					moduleName.pop_back();
					moduleName.pop_back();
				}
				break;
			}
			temp_idx++;
		}

		if (mTokens.at(temp_idx).mDesc == ETokenDesc::IDENTIFIER) {

			varName = mTokens.at(temp_idx).GetValue<std::string>();
		}
		else {
			if (hasModuleAccess) {
				mErrorMachine->AddError("Missing identifier after \"::\".", mTokens.at(temp_idx).mLineNumber, mTokens.at(temp_idx).mColNumber);
			}
			else {
				mErrorMachine->AddError("Unknown opeartion in statement.", mTokens.at(temp_idx).mLineNumber, mTokens.at(temp_idx).mColNumber);
			}
			return false;
		}


		temp_idx++;

		//是否是函数
		if (mTokens.at(temp_idx).mDesc == ETokenDesc::BRACKET_ROUND_START) {

			argsBegin = temp_idx;
			bracketMask++;
			temp_idx++;
			while (temp_idx <= limited_end_idx) {
				if (mTokens.at(temp_idx).mDesc == ETokenDesc::BRACKET_ROUND_START) {
					bracketMask++;
				}
				else if (mTokens.at(temp_idx).mDesc == ETokenDesc::BRACKET_ROUND_END) {
					bracketMask--;
					if (bracketMask == 0) {

						if (mTokens.at(temp_idx + 1).mDesc == ETokenDesc::MEMBERACCESS) {
							mErrorMachine->AddError("Member Access can't used for function.", mTokens.at(temp_idx + 1).mLineNumber, mTokens.at(temp_idx + 1).mColNumber);
							return false;
						}
						else {
							type = EStatementVarType::NORMAL_FUNC;
							argsEnd = temp_idx;
							return true;
						}
					}
				}
				temp_idx++;
			}
		}

		if (hasModuleAccess) {
			mErrorMachine->AddError("Access to variables of other modules is not allowed.", mTokens.at(temp_idx).mLineNumber, mTokens.at(temp_idx).mColNumber);
			return false;
		}

		if (mTokens.at(temp_idx).mDesc == ETokenDesc::MEMBERACCESS) {
			temp_idx++;
			if (mTokens.at(temp_idx).mDesc == ETokenDesc::IDENTIFIER) {

				memberName = mTokens.at(temp_idx).GetValue<std::string>();
				if (mTokens.at(temp_idx + 1).mDesc == ETokenDesc::BRACKET_ROUND_START) {
					temp_idx++;
					argsBegin = temp_idx;
					bracketMask++;
					temp_idx++;
					while (temp_idx <= limited_end_idx) {
						if (mTokens.at(temp_idx).mDesc == ETokenDesc::BRACKET_ROUND_START) {
							bracketMask++;
						}
						else if (mTokens.at(temp_idx).mDesc == ETokenDesc::BRACKET_ROUND_END) {
							bracketMask--;
							if (bracketMask == 0) {

								if (mTokens.at(temp_idx + 1).mDesc == ETokenDesc::MEMBERACCESS) {
									mErrorMachine->AddError("Member Access can't used for function.", mTokens.at(temp_idx + 1).mLineNumber, mTokens.at(temp_idx + 1).mColNumber);
									return false;
								}
								else {
									type = EStatementVarType::MEMBER_FUNC;
									argsEnd = temp_idx;
									return true;
								}
							}
						}
						temp_idx++;
					}
				}
				else {
					type = EStatementVarType::MEMBER_VAR;
					argsEnd = temp_idx;
					return true;
				}
			}
		}
		else {
			//这个时候指针指向了 ; 或者其他符号
			type = EStatementVarType::NORMAL_VAR;
			argsEnd = temp_idx - 1;
			return true;
		}

		return false;
	}

	SStatementSegment SourceCodeCompile::DescGenerator(SStatementSegment& item) {

		if (item.BasicType == EStatementBasicType::CONST_INT) {
			item.VarNameDesc = std::to_string(item.Token.GetValue<RuleTable::SlgINT>());
		}
		else if (item.BasicType == EStatementBasicType::CONST_FLOAT) {
			item.VarNameDesc = std::to_string(item.Token.GetValue<RuleTable::SlgFLOAT>());
		}
		else if (item.BasicType == EStatementBasicType::CONST_STRING) {
			item.VarNameDesc = item.Token.GetValue<std::string>();
		}
		else if (item.BasicType == EStatementBasicType::VARTYPE) {
			if (item.VariableType == EStatementVarType::NORMAL_FUNC) {
				if (item.ModuleName != "")
				{
					item.VarNameDesc = item.ModuleName + "::";
				}
				item.VarNameDesc += item.MainName;
			}
			else if (item.VariableType == EStatementVarType::MEMBER_FUNC) {
				item.VarNameDesc += item.MainName + "." + item.MemberName;
			}
			else if (item.VariableType == EStatementVarType::MEMBER_VAR) {
				item.VarNameDesc = item.MainName + "." + item.MemberName;

			}
			else if (item.VariableType == EStatementVarType::NORMAL_VAR) {
				item.VarNameDesc = item.MainName;
			}
		}
		else if (item.BasicType == EStatementBasicType::REG) {
			item.VarNameDesc = "R" + std::to_string(item.RegisterID);
		}
		return item;
	}

	std::optional<CompilerConfig::SBindingFunctionDesc> SourceCodeCompile::FindBindingFunction(const std::string& memberFunctionName) {

		return mCompilerConfig->FindBindingFunction(memberFunctionName);
	}

	std::optional<CompilerConfig::SInlineFunctionDesc> SourceCodeCompile::FindMemberFunction(const std::string& variableType, const std::string& memberFunctionName) {

		return mCompilerConfig->FindMemberFunction(variableType, memberFunctionName);
	}

	std::optional<CompilerConfig::SMemberVaribleDesc> SourceCodeCompile::FindMemberVariable(const std::string& variableType, const std::string& memberName) {

		return mCompilerConfig->FindMemberVariable(variableType, memberName);
	}

	std::optional<CompilerConfig::SInlineFunctionDesc> SourceCodeCompile::FindInlineFunction(const std::string& functionName) {

		return mCompilerConfig->FindInlineFunction(functionName);
	}

	std::optional<std::pair<int, SLocalVariableDesc>> SourceCodeCompile::FindLocalVariable(const std::string& varName) {

		for (int i = 0; i < mLocalVariable.size(); ++i) {
			auto itor = mLocalVariable[i].mLocalVariable.find(varName);
			if (itor != mLocalVariable[i].mLocalVariable.end()) {
				return std::make_pair(i, itor->second);
			}
		}

		return std::nullopt;
	}

	std::optional<SModuleGlobalVariableDesc> SourceCodeCompile::FindGlobalVariable(const std::string& varName) {

		auto itor = mModuleInfo->mGlobalVariableTable.find(varName);
		if (itor != mModuleInfo->mGlobalVariableTable.end()) {
			return itor->second;
		}

		return std::nullopt;
	}

	std::optional<SModuleFunctionDesc> SourceCodeCompile::FindCustomFunction(const std::string& ModuleName, const std::string& varName) {

		if (ModuleName == "" || ModuleName == mModuleInfo->mCurrentModuleName) {

			auto itor = mModuleInfo->mFunctionTable.find(varName);
			if (itor != mModuleInfo->mFunctionTable.end()) {
				return itor->second;
			}
			else {
				auto it = mNetWork->FindFunction(ModuleName, varName);
				if (it.has_value()) {
					return it.value();
				}
			}
		}
		else {

			auto it = mNetWork->FindFunction(ModuleName, varName);
			if (it.has_value()) {
				return it.value();
			}
		}
		return std::nullopt;
	}

	SStatementSegment SourceCodeCompile::SovlerAsmUnaryOperator(SStatementSegment& op, SStatementSegment& A) {
		if (!mErrorMachine->Success()) {
			return SStatementSegment{};
		}
		SStatementSegment reg{};

		if (A.BasicType == EStatementBasicType::CONST_INT) {
			reg = A;
			reg.Token.mValue = IntConstexprCalcValueUnary(op.Token.mDesc, A.Token.GetValue<RuleTable::SlgINT>());
			reg.BasicType = EStatementBasicType::CONST_INT;
			reg.VarTypeDesc = "int";
			DescGenerator(reg);
			return reg;
		}
		else if (A.BasicType == EStatementBasicType::CONST_FLOAT) {

			reg = A;
			reg.Token.mValue = FloatConstexprCalcValueUnary(op.Token.mDesc, A.Token.GetValue<RuleTable::SlgFLOAT>());
			reg.BasicType = EStatementBasicType::CONST_FLOAT;
			reg.VarTypeDesc = "float";
			DescGenerator(reg);
			return reg;
		}
		else if (A.BasicType == EStatementBasicType::CONST_STRING) {
			mErrorMachine->AddError("Strings cannot use unit operators, only 'addition'+' is allowed ", op.Token.mColNumber, op.Token.mColNumber);
			return SStatementSegment{};
		}
		else if (A.BasicType != EStatementBasicType::REG) {
			reg.BasicType = EStatementBasicType::REG;
			reg.VarTypeDesc = A.VarTypeDesc;
			reg.RegisterID = GetFreeRegister();
			DescGenerator(reg);
			auto movRes = GetTypeCompatibleAsmForAssignmentOperator(ETokenDesc::EQUAL, reg.VarTypeDesc, reg.VarTypeDesc);
			if (movRes.has_value()) {
				ASM_BINARY(movRes.value(), reg, A);
			}
			else {
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
			ASM_UNARY(result.value(), reg);
			return reg;
		}
		else {
			mErrorMachine->AddError("Operator " + op.Token.GetValue<std::string>() + " can't used for type " + reg.VarTypeDesc, mTokens.at(op.Idx).mColNumber, mTokens.at(op.Idx).mColNumber);
			return SStatementSegment{};
		}
	}

	std::tuple<std::string, std::string, int, unsigned short, short, int, long long, double, std::string> SourceCodeCompile::GetVarAsmType(SStatementSegment& item) {
		if (item.BasicType == EStatementBasicType::REG) {
			return { "", "", 0, 0, item.RegisterID, 0, 0, 0.0, "" };
		}
		else if (item.BasicType == EStatementBasicType::VARTYPE) {
			auto resL = FindLocalVariable(item.MainName);
			auto resG = FindGlobalVariable(item.MainName);

			std::string vType = "";
			std::string mModule = "";
			int offest = 0;
			int memOffest = 0;

			if (resL.has_value()) {
				vType = resL.value().second.mTypeStr;
				offest = resL.value().second.mVariableOffest;
			}
			else if (resG.has_value()) {
				vType = resG.value().mType;
				mModule = mModuleInfo->mCurrentModuleName;
			}

			if (item.VariableType == EStatementVarType::MEMBER_VAR) {
				auto res = FindMemberVariable(vType, item.MemberName);
				memOffest = res.value().AddressOffest;
			}

			return { item.MainName, mModule, offest, memOffest, -1, 0, 0, 0.0, "" };
		}
		else if (item.BasicType == EStatementBasicType::CONST_INT) {
			return { "", "", 0, 0, -1, 1, item.Token.GetValue<long long>(), 0.0, "" };
		}
		else if (item.BasicType == EStatementBasicType::CONST_FLOAT) {
			return { "", "", 0, 0, -1, 2, 0, item.Token.GetValue<double>(), "" };
		}
		else if (item.BasicType == EStatementBasicType::CONST_STRING) {
			return { "", "", 0, 0, -1, 3, 0, 0.0, item.Token.GetValue<std::string>() };
		}
		printf("Error: Can't Pass The Variable : %s============\n", item.MainName.c_str());
		return { "", "", 0, 0, -1, 0, 0, 0.0, "" };
	}

	SStatementSegment SourceCodeCompile::SovlerAsmBinaryOperator(SStatementSegment& op, SStatementSegment& A, SStatementSegment& B) {

		if (!mErrorMachine->Success()) {
			return SStatementSegment{};
		}
		SStatementSegment reg{};

		if (A.BasicType == EStatementBasicType::CONST_INT && B.BasicType == EStatementBasicType::CONST_INT) {
			reg = A;
			reg.Token.mValue = IntConstexprCalcValueBinary(op.Token.mDesc, A.Token.GetValue<RuleTable::SlgINT>(), B.Token.GetValue<RuleTable::SlgINT>());
			reg.BasicType = EStatementBasicType::CONST_INT;
			reg.VarTypeDesc = "int";
			DescGenerator(reg);
			return reg;
		}
		else if (A.BasicType == EStatementBasicType::CONST_FLOAT && B.BasicType == EStatementBasicType::CONST_FLOAT ||
			A.BasicType == EStatementBasicType::CONST_FLOAT && B.BasicType == EStatementBasicType::CONST_INT ||
			A.BasicType == EStatementBasicType::CONST_INT && B.BasicType == EStatementBasicType::CONST_FLOAT) {

			reg = A;
			reg.Token.mValue = FloatConstexprCalcValueBinary(op.Token.mDesc, A.Token.GetValue<RuleTable::SlgFLOAT>(), B.Token.GetValue<RuleTable::SlgFLOAT>());
			reg.BasicType = EStatementBasicType::CONST_FLOAT;
			reg.VarTypeDesc = "float";
			DescGenerator(reg);
			return reg;
		}
		else if (A.BasicType == EStatementBasicType::CONST_STRING) {


			reg = A;
			//TODO :

			reg.BasicType = EStatementBasicType::CONST_STRING;
			reg.VarTypeDesc = "string";
			DescGenerator(reg);
			return reg;
		}
		else if (A.BasicType != EStatementBasicType::REG) {
			reg.BasicType = EStatementBasicType::REG;
			reg.VarTypeDesc = A.VarTypeDesc;
			reg.RegisterID = GetFreeRegister();
			DescGenerator(reg);

			auto movRes = GetTypeCompatibleAsmForAssignmentOperator(ETokenDesc::EQUAL, reg.VarTypeDesc, reg.VarTypeDesc);

			if (movRes.has_value()) {
				ASM_BINARY(movRes.value(), reg, A);
			}
			else {
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
			ASM_BINARY(result.value().second, reg, B);
			return reg;
		}
		else {
			mErrorMachine->AddError("Operator " + op.Token.GetValue<std::string>() + " can't used for type " + reg.VarTypeDesc + " and " + B.VarTypeDesc + ".", mTokens.at(op.Idx).mColNumber, mTokens.at(op.Idx).mColNumber);
			return SStatementSegment{};
		}
	}
	SStatementSegment SourceCodeCompile::SovlerAsmAssignmentOperator(SStatementSegment& op, SStatementSegment& A, SStatementSegment& B) {
		if (!mErrorMachine->Success()) {
			return SStatementSegment{};
		}
		auto result = GetTypeCompatibleAsmForAssignmentOperator(op.Token.mDesc, A.VarTypeDesc, B.VarTypeDesc);

		if (result.has_value()) {
			ASM_BINARY(result.value(), A, B);
			return A;
		}
		else {
			mErrorMachine->AddError("Operator " + op.Token.GetValue<std::string>() + " can't used for type " + A.VarTypeDesc + " and " + B.VarTypeDesc + ".", mTokens.at(op.Idx).mColNumber, mTokens.at(op.Idx).mColNumber);
			return SStatementSegment{};
		}
	}

	bool SourceCodeCompile::ASM_UNARY(const std::string& asmStr, SStatementSegment& A) {
		auto [name, moduleName, address, offest, regidx, constType, LL, DD, SS] = GetVarAsmType(A);
		bool t = mAsmGen->UNARY_OPERATE(asmStr, name, moduleName, address, offest, regidx, constType, LL, DD, SS);

		printf("%s %s\n\n", asmStr.c_str(), A.VarNameDesc.c_str());
		return t;
	}

	bool SourceCodeCompile::ASM_BINARY(const std::string& asmStr, SStatementSegment& A, SStatementSegment& B) {
		auto [name, moduleName, address, offest, regidx, constType, LL, DD, SS] = GetVarAsmType(A);
		auto [nameB, moduleNameB, addressB, offestB, regidxB, constTypeB, LLB, DDB, SSB] = GetVarAsmType(B);

		bool t = mAsmGen->BIN_OPERATE(asmStr, name, nameB,
			moduleName, moduleNameB,
			address, addressB,
			offest, offestB,
			regidx, regidxB,
			constType, constTypeB,
			LL, LLB,
			DD, DDB,
			SS, SSB);

		printf("%s %s, %s\n\n", asmStr.c_str(), A.VarNameDesc.c_str(), B.VarNameDesc.c_str());

		return t;
	}

	bool SourceCodeCompile::ASM_PUSH(const std::string& asmStr, SStatementSegment& A) {
		return ASM_UNARY(asmStr, A);
	}

	bool SourceCodeCompile::ASM_CALL(const std::string& moduleName, const std::string& FunctionName) {
		return mAsmGen->CALL(moduleName, FunctionName);
	}
	bool SourceCodeCompile::ASM_CALL_INLINE(const std::string& FunctionName) {

		return mAsmGen->CALL_INLINE(FunctionName);
	}

	bool SourceCodeCompile::ASM_CALL_MEMBER_FUNC(const std::string& FunctionName, SStatementSegment& A) {
		auto it = FindGlobalVariable(A.MainName);
		auto it2 = FindLocalVariable(A.MainName);
		std::string modulename = "";
		std::string globalName = "";
		int offest = 0;
		if (it.has_value()) {
			globalName = A.MainName;
			modulename = mModuleInfo->mCurrentModuleName;

		}
		else if (it2.has_value()) {
			offest = it2.value().second.mVariableOffest;
		}
		else {
			return false;
		}

		bool t = mAsmGen->CALL_MEMBER_FUNC(FunctionName, modulename, globalName, offest);
		std::string temp = "";
		if (A.ModuleName != "") {
			temp += A.ModuleName + "::";
		}
		temp += A.MainName;
		printf("CALL %s\n\n", temp.c_str());
		return t;

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
		case ETokenType::ERRORTOKEN:
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
			mTokens.emplace_back(mCurrentToken);
			break;
		}
		return true;
	}
	SlgTokenItem Tokenizer::GetNextToken() {
		std::string token = "";

		if (mCode.at(mPointer) == '\n') {
			mLine++;
			mPassCharacterCount = mPointer;
			mCol = 0;
		}
		else {
			mCol = mPointer - mPassCharacterCount;
		}

		//去注释
		if (mCode.at(mPointer) == '/') {
			if (mCode.length() > mPointer + 1) {
				if (mCode.at(mPointer + 1) == '/') {
					while (mCode.length() > mPointer && mCode.at(mPointer) != '\n') {
						mPointer++;
					}
					return SlgTokenItem(ETokenType::NULLVALUE, ETokenDesc::UNKNOWN, "", -1, -1);
				}
				else if (mCode.at(mPointer + 1) == '*') {

					while (mCode.length() > mPointer + 1 && (mCode.at(mPointer) != '*' || mCode.at(mPointer + 1) != '/')) {
						if (mCode.at(mPointer) == '\n') {
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

		if (mCode.at(mPointer) == '-') {
			mPointer++;
			if (IsNumber(mCode.at(mPointer))) {
				do {
					token += mCode.at(mPointer);
					mPointer++;
				} while (mCode.length() > mPointer && IsNumber(mCode.at(mPointer)));

				if (mCode.at(mPointer) == '.') {
					mPointer++;
					std::string Decimal = "";
					while (mCode.length() > mPointer && IsNumber(mCode.at(mPointer))) {
						Decimal += mCode.at(mPointer);
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
		if (IsNumber(mCode.at(mPointer))) {
			do {
				token += mCode.at(mPointer);
				mPointer++;
			} while (mCode.length() > mPointer && IsNumber(mCode.at(mPointer)));

			if (mCode.at(mPointer) == '.') {
				mPointer++;
				std::string Decimal = "";
				while (mCode.length() > mPointer && IsNumber(mCode.at(mPointer))) {
					Decimal += mCode.at(mPointer);
					mPointer++;
				}
				return SlgTokenItem(ETokenType::FLOAT, ETokenDesc::FLOAT, std::stof(token + "." + Decimal), mLine, mCol);
			}
			else {
				return SlgTokenItem(ETokenType::INT, ETokenDesc::INT, std::stoll(token), mLine, mCol);
			}
		}

		//字段 标识符
		if (Isalpha(mCode.at(mPointer)) || mCode.at(mPointer) == '_') {
			do {
				token += mCode.at(mPointer);
				mPointer++;
			} while (mCode.length() > mPointer && (Isalnum(mCode.at(mPointer)) || mCode.at(mPointer) == '_'));
			return SlgTokenItem(ETokenType::IDENTIFIER, ETokenDesc::IDENTIFIER, token, mLine, mCol);
		}

		//字符串
		if (mCode.at(mPointer) == '\"') {
			mPointer++;
			while (mCode.length() > mPointer && (mCode.at(mPointer) != '\"' || mCode.at(mPointer - 1) == '\\')) {
				if (mCode.at(mPointer) == '\n') {
					return SlgTokenItem(ETokenType::ERRORTOKEN, ETokenDesc::STRING, "The string must be fully written on one line", mLine, mCol);
				}
				else {
					token += mCode.at(mPointer);
					mPointer++;
				}
			}
			mPointer++;
			return SlgTokenItem(ETokenType::STRING, ETokenDesc::STRING, token, mLine, mCol);
		}

		if (mCode.at(mPointer) == ':') {
			mPointer++;
			if (mPointer < mCode.length() && mCode.at(mPointer) == ':') {
				token = "::";
				mPointer++;
				return SlgTokenItem(ETokenType::STATEMENTOPERATOR, ETokenDesc::MODULEACCESS, token, mLine, mCol);
			}
			else {
				return SlgTokenItem(ETokenType::ERRORTOKEN, ETokenDesc::STRING, "Unknown operator \":\", maybe you wanna \"::\"?", mLine, mCol);
			}
		}

		//括号
		auto Bitor = RuleTable::Brackets.find(mCode.at(mPointer));
		if (Bitor != std::end(RuleTable::Brackets)) {
			token += mCode.at(mPointer);
			mPointer++;
			return SlgTokenItem(ETokenType::BRACKET, Bitor->second, token, mLine, mCol);
		}

		//运算符
		auto itor1 = RuleTable::OperatorsSingle.find((char)mCode.at(mPointer));
		if (itor1 != RuleTable::OperatorsSingle.end()) {

			token += mCode.at(mPointer);

			if (mPointer + 2 < mCode.length()) {
				auto itor2 = RuleTable::OperatorsMulti.find(token + mCode.at(mPointer + 1) + mCode.at(mPointer + 2));
				if (itor2 != RuleTable::OperatorsMulti.end()) {
					mPointer += 3;
					return SlgTokenItem(ETokenType::OPERATOR, itor2->second, itor2->first, mLine, mCol);
				}
			}

			if (mPointer + 1 < mCode.length()) {
				auto itor2 = RuleTable::OperatorsMulti.find(token + mCode.at(mPointer + 1));
				if (itor2 != RuleTable::OperatorsMulti.end()) {
					mPointer += 2;
					return SlgTokenItem(ETokenType::OPERATOR, itor2->second, itor2->first, mLine, mCol);
				}
			}

			mPointer++;
			return SlgTokenItem(ETokenType::OPERATOR, itor1->second, token, mLine, mCol);
		}

		auto Sitor = RuleTable::StatementOpeartor.find((char)mCode.at(mPointer));
		if (Sitor != RuleTable::StatementOpeartor.end()) {
			token += Sitor->first;
			mPointer++;
			return SlgTokenItem(ETokenType::STATEMENTOPERATOR, Sitor->second, token, mLine, mCol);
		}

		mPointer++;
		return SlgTokenItem(ETokenType::NULLVALUE, ETokenDesc::UNKNOWN, "", -1, -1);
	}
};