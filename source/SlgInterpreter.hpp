#pragma once 

#include "pch.hpp"
#include "SlgCompilerConfig.hpp"
#include "SlgCompiler.hpp"

namespace SylvanLanguage {

	class CompilerConfig;
	class SourceCodeCompile;
	class RunTimeEnvironment;
	class RunTimeNetWork;
	class RunTimeInstance;

	struct SModuleFuntionTable {
		std::vector<std::string> mFunctionArgsTypeDesc;
		std::string mFunctionReturnTypeDesc;
		unsigned int CodeOffest;
	};

	struct SModuleAttributeTable {
		std::string mType;
		bool mIsPrivate = false;
	};

	struct SRunTimeAttributeMemory {
		std::string mType;
		bool mIsPrivate = false;
		std::unique_ptr<char> mValue;
	};

	struct SModuleInfo {
		std::unordered_map <std::string, SModuleFuntionTable> mRunTimeFuntionTable;
		std::unordered_map <std::string, SModuleAttributeTable> mRunTimeAttributeTable;
		std::unique_ptr<AssemblyData> mAssemblyData;
		std::vector<std::string> mDependenceModules;
		std::string mCurrentModuleName;
	};

	//同步器
	class EnvironmentSyncServer {

		//MakeNetWorkBridge
	};

	class RunTimeNetWork {
		friend class RunTimeEnvironment;

		std::unordered_map <std::string, SModuleInfo> mModuleTable{}; //模块表, 运行时从Environment拷贝源代码后编译

		std::unordered_map <std::string, SRunTimeAttributeMemory> mRunTimeAttributeTable{};
		std::unordered_map <std::string, RunTimeInstance> mRunTimeInstanceTable{};

		CompilerConfig* mConfig;
	public:
		RunTimeNetWork(CompilerConfig* config);

		//CompileModule
		void CompileModule(std::string codes);

		//CompileCode

		std::optional<SModuleFuntionTable> FindFuntion(std::string funtionName);

		std::optional<SModuleAttributeTable> FindAttribute(std::string attrName);

		//AddModule
		//RemoveModule
		//CreateRunTimeInstanceFromFuntion0
		//CreateRunTimeInstanceFromCode
	};

	class RunTimeEnvironment {
		std::unordered_map <std::string, std::unique_ptr<RunTimeNetWork>> mRunTimeNetWorkTable;
		CompilerConfig* mConfig;

	public:

		RunTimeEnvironment(CompilerConfig* config) : mConfig(config) {

		}

		bool CreateNetWork(std::string netWorkName);

		bool AddModuleSourceCode(std::string netWorkName, std::string sourceCode);

		//AddModuleSourceCode
		//DestoryModuleSourceCode
		//CreateNetWork
		//DestoryNetWork
		//LinkModuleToNetWork
		//UnlinkModuleFromNetWork
		//CreateRunTimeInstanceFromModuleFuntion
		//CreateRunTimeInstanceFromCode
		//DestoryRunTimeInstance
	};


	//运行代码片段
	class RunTimeInstance {
		//CS : IP
		//STACK MEMORY
		//
	};
}