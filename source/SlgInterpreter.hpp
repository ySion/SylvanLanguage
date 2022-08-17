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

	struct SModuleFunctionDesc {
		std::vector<std::string> mFunctionArgsTypeDesc;
		std::string mFunctionReturnTypeDesc;
		unsigned int CodeOffest;
		bool isExport = false;
	};

	struct SModuleGlobalVariableDesc {
		std::string mType;
	};

	// 0 [RR]public		|  R/W	|   R/W 
	// 1 [RN]protect	|  R/W	|   R   
	// 2 [NN]private	|  R/W	|   N   

	// 内网读写, 友好网络读写, 外部网络读写(来宾网络) 这些可以在主服务器中运行时设置, 因为只有本地网络变量才可以直接通过@Att访问， 跨网络访问需要使用 GetIntValue("名字") GetFloatValue ...  以及 SetIntValue("名字", INT值) ...等函数

	struct SRunTimeGlobalVariableMemory {
		std::string mType;
		std::unique_ptr<char> mValue;
	};

	struct SModuleInfo {
		std::unordered_map <std::string, SModuleFunctionDesc> mFunctionTable;
		std::unordered_map <std::string, SModuleGlobalVariableDesc> mGlobalVariableTable;
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

		std::unordered_map <std::string, SModuleInfo> mModuleTable{};
		std::unordered_map <std::string, SRunTimeGlobalVariableMemory> mRunTimeGlobalVariableMemory{};
		
		std::unordered_map <std::string, RunTimeInstance> mRunTimeInstanceTable{};

		CompilerConfig* mConfig;
	public:
		RunTimeNetWork(CompilerConfig* config);

		//CompileModule
		void CompileModule(std::string codes);

		//CompileCode

		std::optional<SModuleFunctionDesc> FindFunction(const std::string& moduleName, const std::string& functionName);

		//AddModule
		//RemoveModule
		//CreateRunTimeInstanceFromFunction0
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
		//CreateRunTimeInstanceFromModuleFunction
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