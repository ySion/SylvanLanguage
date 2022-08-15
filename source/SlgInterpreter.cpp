#include "SlgInterpreter.hpp"

//CompileModule
namespace SylvanLanguage {

	RunTimeNetWork::RunTimeNetWork(CompilerConfig* config) : mConfig(config) {

	}

	void RunTimeNetWork::CompileModule(std::string codes) {
		SModuleInfo moduleInfo;
		SourceCodeCompile codeCompile(codes, mConfig, this, &moduleInfo);

		if (codeCompile.GetResult()) {
			codeCompile.ShowProgramTable();
			codeCompile.ShowAsm();
			auto itor = mModuleTable.find(moduleInfo.mCurrentModuleName);
			if (itor != mModuleTable.end()) {

				mModuleTable.erase(moduleInfo.mCurrentModuleName);
				mModuleTable[moduleInfo.mCurrentModuleName] = std::move(moduleInfo);
			}
			else {
				mModuleTable[moduleInfo.mCurrentModuleName] = std::move(moduleInfo);
			}
		}
	}

	std::optional<SModuleFuntionDesc> RunTimeNetWork::FindFuntion(const std::string& moduleName, const std::string& funtionName) {

		auto itor = mModuleTable.find(moduleName);
		if (itor != mModuleTable.end()) {
			auto itor2 = itor->second.mRunTimeFuntionTable.find(funtionName);
			if (itor2 != itor->second.mRunTimeFuntionTable.end()) {
				return itor2->second;
			}
		}

		return std::nullopt;
	}

	std::optional<SModuleAttributeDesc> RunTimeNetWork::FindAttribute(const std::string& moduleName, const std::string& attrName) {
		std::string attPath;

		if (moduleName == "") {
			attPath = moduleName + "::" + attrName;
		}
		else {
			attPath = attrName;
		}

		auto itor = mRunTimeAttributeTable.find(attPath);
		if (itor != mRunTimeAttributeTable.end()) {
			return SModuleAttributeDesc{ itor->second.mType, itor->second.mIsPrivate };
		}
		return std::nullopt;
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

	bool RunTimeEnvironment::AddModuleSourceCode(std::string netWorkName, std::string sourceCode) {
		if (mRunTimeNetWorkTable.find(netWorkName) == mRunTimeNetWorkTable.end()) {
			return false;
		}
		else {
			mRunTimeNetWorkTable.at(netWorkName).get()->CompileModule(sourceCode);
			return true;
		}
	}
};