#include "SlgInterpreter.hpp"

//CompileModule
namespace SylvanLanguage {

	RunTimeNetWork::RunTimeNetWork(CompilerConfig* config) : mConfig(config) {

	}

	void RunTimeNetWork::AddOrReplaceModule(std::string moduleName, std::string sourceCode)
	{
		mCodes[moduleName] = sourceCode;
	}

	void RunTimeNetWork::RemoveModule(std::string moduleName)
	{
		auto itor = mCodes.find(moduleName);
		if (itor != mCodes.end()) {
			mCodes.erase(itor);
			auto itor2 = mModuleTable.find(moduleName);
			if (itor2 != mModuleTable.end()) {
				mModuleTable.erase(itor2);
			}
		}
	}

	bool RunTimeNetWork::FindModule(std::string moduleName)
	{
		auto itor = mCodes.find(moduleName);
		if (itor != mCodes.end()) {
			return true;
		}
		else {
			return false;
		}
	}

	std::optional<std::string> RunTimeNetWork::GetModuleSourceCode(std::string moduleName)
	{
		auto itor = mCodes.find(moduleName);
		if (itor != mCodes.end()) {
			return itor->second;
		}
		else {
			return std::nullopt;
		}
	}

	bool RunTimeNetWork::CompileModule(std::string moduleName) {

		auto itor = mCodes.find(moduleName);
		if (itor == mCodes.end()) {
			return false;
		}

		SModuleInfo moduleInfo;
		SourceCodeCompile codeCompile(itor->second, mConfig, this, &moduleInfo);

		if (codeCompile.GetResult()) {
			std::cout << "Module Compiler Info : " << moduleInfo.mCurrentModuleName << "\n";
			codeCompile.ShowProgramTable();
			codeCompile.ShowAsm();

			if (moduleInfo.mCurrentModuleName != moduleName) {
				//文件名和模组名字不一样
				return false;
			}

			mModuleTable[moduleInfo.mCurrentModuleName] = std::move(moduleInfo);
		}
		return true;
	}

	std::optional<SModuleFunctionDesc> RunTimeNetWork::FindFunction(const std::string& moduleName, const std::string& FunctionName) {

		auto itor = mModuleTable.find(moduleName);
		if (itor != mModuleTable.end()) {
			auto itor2 = itor->second.mFunctionTable.find(FunctionName);
			if (itor2 != itor->second.mFunctionTable.end()) {
				if (itor2->second.isExport) {
					return itor2->second;
				}
			}
		}

		return std::nullopt;
	}

	RunTimeEnvironment::RunTimeEnvironment(CompilerConfig* config) : mConfig(config) {

	}

	bool RunTimeEnvironment::CreateNetWork(std::string netWorkName) {
		if (mRunTimeNetWorkTable.find(netWorkName) == mRunTimeNetWorkTable.end()) {
			mRunTimeNetWorkTable.insert({ netWorkName, std::make_unique<RunTimeNetWork>(mConfig) });
			return true;
		}
		else {
			return false;
		}
	}

	bool RunTimeEnvironment::AddOrReplaceModule(std::string netWorkName, std::string moduleName, std::string sourceCode) {

		auto itor = mRunTimeNetWorkTable.find(netWorkName);
		if (mRunTimeNetWorkTable.find(netWorkName) == mRunTimeNetWorkTable.end()) {
			return false;
		}
		else {
			itor->second.get()->AddOrReplaceModule(moduleName, sourceCode);
			return true;
		}
	}
	bool RunTimeEnvironment::CompileModule(std::string netWorkName, std::string moduleName)
	{
		auto itor = mRunTimeNetWorkTable.find(netWorkName);
		if (itor == mRunTimeNetWorkTable.end()) {
			return false;
		}
		else {
			return itor->second.get()->CompileModule(moduleName);
		}
	}
	std::optional<std::string> RunTimeEnvironment::GetModuleSourceCode(std::string netWorkName, std::string moduleName)
	{
		auto itor = mRunTimeNetWorkTable.find(netWorkName);
		if (itor == mRunTimeNetWorkTable.end()) {
			return std::nullopt;
		}
		else {
			return itor->second.get()->GetModuleSourceCode(moduleName);
		}
	}
};