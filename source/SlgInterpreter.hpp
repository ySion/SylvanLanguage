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
		std::vector<std::string> mFunctionArgsTypeDesc{};
		std::string mFunctionReturnTypeDesc{};
		unsigned int CodeOffest{};
		bool isExport = false;
	};

	struct SModuleGlobalVariableDesc {
		std::string mType{};
	};

	// 内网读写, 友好网络读写, 外部网络读写(来宾网络) 这些可以在主服务器中运行时设置, 因为只有本地网络变量才可以直接通过@Att访问， 跨网络访问需要使用 GetIntValue("名字") GetFloatValue ...  以及 SetIntValue("名字", INT值) ...等函数
	struct SRunTimeGlobalVariableMemory {
		std::string mType{};
		std::unique_ptr<char> mValue{};
	};

	struct SModuleInfo {
		std::unordered_map <std::string, SModuleFunctionDesc> mFunctionTable{};
		std::unordered_map <std::string, SModuleGlobalVariableDesc> mGlobalVariableTable{};
		std::unique_ptr<AssemblyData> mAssemblyData{};
		std::vector<std::string> mDependenceModules{};
		std::string mCurrentModuleName{};
	};

	//同步器
	class EnvironmentSyncServer {

		//MakeNetWorkBridge
	};

	class RunTimeNetWork {
		friend class RunTimeEnvironment;
		std::unordered_map <std::string, std::string> mCodes{};
		std::unordered_map <std::string, SModuleInfo> mModuleTable{};
		std::unordered_map <std::string, SRunTimeGlobalVariableMemory> mRunTimeGlobalVariableMemory{};
		std::unordered_map <std::string, RunTimeInstance> mRunTimeInstanceTable{};

		CompilerConfig* mConfig;
	public:
		//创建一个NetWork， 需要一个编译设置.
		RunTimeNetWork(CompilerConfig* config);

		// 编译 / 重新编译模块源代码
		bool CompileModule(std::string moduleName);

		// 添加/替换已有的模块源代码 但是不会重新编译模组
		void AddOrReplaceModule(std::string moduleName, std::string sourceCode);

		// 移除模块源代码和模块编译数据
		void RemoveModule(std::string moduleName);

		// 查找模块是否存在
		bool FindModule(std::string moduleName);

		// 获取模块的源代码	
		std::optional<std::string> GetModuleSourceCode(std::string moduleName);

		//查找模块内的函数
		std::optional<SModuleFunctionDesc> FindFunction(const std::string& moduleName, const std::string& functionName);


		//CreateRunTimeInstanceFromFunction0
		//CreateRunTimeInstanceFromCode
	};

	class RunTimeEnvironment {
		std::unordered_map <std::string, std::unique_ptr<RunTimeNetWork>> mRunTimeNetWorkTable;
		CompilerConfig* mConfig;

	public:
		//创建运行环境， 需要一个编译设置.
		RunTimeEnvironment(CompilerConfig* config);

		//创建网络
		bool CreateNetWork(std::string netWorkName);

		//添加或者替换模组, (替换不会删除和修改编译数据)
		bool AddOrReplaceModule(std::string netWorkName, std::string moduleName, std::string sourceCode);
		bool CompileModule(std::string netWorkName, std::string moduleName);

		std::optional<std::string> GetModuleSourceCode(std::string netWorkName, std::string moduleName);
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

		long long CS{};
		long long BP{};
		long long SP{};
		std::stack<long long> BP_BACKEND;
		std::stack<long long> CS_BACKEND;
		std::unique_ptr<char*> Stack;

	};
}