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
		int CodeOffest{};
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
		std::string mCurrentModuleName{};

		inline SModuleInfo() : mFunctionTable{}, mGlobalVariableTable{}, mAssemblyData{}, mCurrentModuleName{} {

		}

		inline SModuleInfo(const SModuleInfo& item)
			: mFunctionTable{ item.mFunctionTable }, mGlobalVariableTable{ item.mGlobalVariableTable },
			mAssemblyData{ new AssemblyData(*item.mAssemblyData.get()) }, mCurrentModuleName{ item.mCurrentModuleName } {
		}

		inline void operator = (const SModuleInfo& item) {
			mFunctionTable = item.mFunctionTable;
			mGlobalVariableTable = item.mGlobalVariableTable;
			mAssemblyData.reset(new AssemblyData(*item.mAssemblyData.get()));
			mCurrentModuleName = item.mCurrentModuleName;
		}
	};

	//同步器
	class EnvironmentSyncServer {

		//MakeNetWorkBridge
	};

	class RunTimeNetWork {
		friend class RunTimeInstance;
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

		inline std::optional<long long>   GetIntAttribute(std::string moduleName, std::string name) { return std::nullopt; }
		inline std::optional<double>	   GetFloatAttribute(std::string moduleName, std::string name) { return std::nullopt; }
		inline std::optional<std::string> GetStringAttribute(std::string moduleName, std::string name) { return std::nullopt; }

		//CreateRunTimeInstanceFromFunction0
		//CreateRunTimeInstanceFromCode
	};


	//运行代码片段
	//使用方法:
	//创建 RunTimeInstance， 需要传递当前RunTimeNetWork的指针
	//执行 Init, 如果初始化失败，会返回false
	//执行 SetNewTarget 来指定将要执行哪个函数， 如果函数不存在或某种原因执行失败， 会返回false
	//前置函数都执行成功的时候， 执行Execute来运行字节码
	class RunTimeInstance {

		int CS{};
		int BP{};
		int SP{};
		AssemblyData* AsmRef = nullptr;
		unsigned char* AsmData = nullptr;
		std::stack<int> BPStack{};
		std::stack<int> CSStack{};
		std::stack<AssemblyData*> AsmRefStack;

		RunTimeNetWork* pNetWork{};
		std::string mCurrentModuleSpace{};
		//stack

		std::unordered_map <std::string, SModuleInfo> mModuleTable{};
		SModuleInfo* pCurrentModuleRef = nullptr;


		std::unique_ptr<char> RegisterMem;
		std::unique_ptr<char> StackMem;

		//消耗的算力, 每一条指令就会增加算力
		int Consumption = 0;

		inline void RunTimeFuntionAddressSwitching(std::string moduleName, std::string function) {
			pCurrentModuleRef = &mModuleTable[moduleName];
			CS = pCurrentModuleRef->mFunctionTable[function].CodeOffest;
		}

	public:

		inline RunTimeInstance(RunTimeNetWork* network) : pNetWork(network), mModuleTable(network->mModuleTable) {

		}

		inline bool Init() {
			return CreateRegisterAndStack();
		}

		inline bool SetNewTarget(std::string moduleName, std::string function) {

			if (CreateRegisterAndStack()) {
				auto itor = mModuleTable.find(moduleName);
				if (itor == mModuleTable.end()) {
					return false;
				}

				auto itor2 = itor->second.mFunctionTable.find(function);
				if (itor2 == itor->second.mFunctionTable.end()) {
					return false;
				}

				pCurrentModuleRef = &mModuleTable[moduleName];
				CS = pCurrentModuleRef->mFunctionTable[function].CodeOffest;
				AsmRef = pCurrentModuleRef->mAssemblyData.get();
				BP = 0;
				SP = 0;
				memset(RegisterMem.get(), 0, 4 * 1024 * 1024);
				memset(StackMem.get(), 0, 128 * 128);
				Consumption = 0;
			}
			else {
				return false;
			}
			return true;
		}

		//4M 栈和一个 128 * 128, 16KB的寄存器组
		inline bool CreateRegisterAndStack() {
			void* reg = malloc(4 * 1024 * 1024);
			void* stack = malloc(128 * 128);
			if (reg == nullptr || stack == nullptr) {
				return false;
			}

			memset(reg, 0, 4 * 1024 * 1024);
			memset(stack, 0, 128 * 128);
			RegisterMem.reset((char*)reg);
			StackMem.reset((char*)stack);

			return true;
		}


		template<class _Ty>
		inline void Push(_Ty val) {
			void* mem = (char*)StackMem.get() + SP;
			*(_Ty*)mem = val;
			SP += sizeof(_Ty);
		}

		template<class _Ty>
		inline _Ty POP() {
			SP -= sizeof(_Ty);
			void* mem = (char*)StackMem.get() + SP;
			return *(_Ty*)mem;
		}

		inline double GetFloat(int& CS) {

		}

		inline long long GetInt(int& CS) {
			*(unsigned char*)(AsmData + CS);
			CS += 8;
		}

		inline long long GetIntFromConst(int& CS) {
			long long res = *(long long*)(AsmData + CS);
			CS += sizeof(long long);
			return res;
		}

		inline long long GetIntFromReg(int& CS) {
			*(unsigned char*)(AsmData + CS);
			CS += 8;
		}

		inline long long GetIntFromLVar(int& CS) {
			*(unsigned char*)(AsmData + CS);
			CS += 8;
		}

		inline long long GetIntFromGVar(int& CS) {
			*(unsigned char*)(AsmData + CS);
			CS += 8;
		}



		//0 局部变量(栈)， 1全局变量， 2寄存器， 4常量
		inline void GetVal(int CS) {
			//return *(unsigned char*)(AsmData + CS);
		}


		inline bool Execute() {
			if (RegisterMem.get() == nullptr || StackMem.get() == nullptr || pCurrentModuleRef == nullptr || AsmRef == nullptr) {
				return false;
			}
			else {
				AsmData = (unsigned char*)AsmRef->GetData();
				int Depth = 0;
				while (true) {
					switch ((ESlgd87Asm)(*(unsigned short*)(AsmData + CS))) {
					case SylvanLanguage::ESlgd87Asm::CALL:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET2:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET3:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET4:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET_ONE2:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET_ONE3:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET_ONE4:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET_LINE2:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET_LINE3:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET_LINE4:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET_COLUMN2:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET_COLUMN3:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_SET_COLUMN4:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_GET_ONE2:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_GET_ONE3:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_GET_ONE4:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_GET_LINE2:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_GET_LINE3:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_GET_LINE4:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_GET_COLUMN2:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_GET_COLUMN3:
						break;
					case SylvanLanguage::ESlgd87Asm::M_MATRIAX_GET_COLUMN4:
						break;
					case SylvanLanguage::ESlgd87Asm::SIN:
						break;
					case SylvanLanguage::ESlgd87Asm::SIN2:
						break;
					case SylvanLanguage::ESlgd87Asm::SIN3:
						break;
					case SylvanLanguage::ESlgd87Asm::SIN4:
						break;
					case SylvanLanguage::ESlgd87Asm::COS:
						break;
					case SylvanLanguage::ESlgd87Asm::COS2:
						break;
					case SylvanLanguage::ESlgd87Asm::COS3:
						break;
					case SylvanLanguage::ESlgd87Asm::COS4:
						break;
					case SylvanLanguage::ESlgd87Asm::TAN:
						break;
					case SylvanLanguage::ESlgd87Asm::TAN2:
						break;
					case SylvanLanguage::ESlgd87Asm::TAN3:
						break;
					case SylvanLanguage::ESlgd87Asm::TAN4:
						break;
					case SylvanLanguage::ESlgd87Asm::ASIN:
						break;
					case SylvanLanguage::ESlgd87Asm::ASIN2:
						break;
					case SylvanLanguage::ESlgd87Asm::ASIN3:
						break;
					case SylvanLanguage::ESlgd87Asm::ASIN4:
						break;
					case SylvanLanguage::ESlgd87Asm::ACOS:
						break;
					case SylvanLanguage::ESlgd87Asm::ACOS2:
						break;
					case SylvanLanguage::ESlgd87Asm::ACOS3:
						break;
					case SylvanLanguage::ESlgd87Asm::ACOS4:
						break;
					case SylvanLanguage::ESlgd87Asm::ATAN:
						break;
					case SylvanLanguage::ESlgd87Asm::ATAN2:
						break;
					case SylvanLanguage::ESlgd87Asm::ATAN3:
						break;
					case SylvanLanguage::ESlgd87Asm::ATAN4:
						break;
					case SylvanLanguage::ESlgd87Asm::CROSS:
						break;
					case SylvanLanguage::ESlgd87Asm::DOT2:
						break;
					case SylvanLanguage::ESlgd87Asm::DOT3:
						break;
					case SylvanLanguage::ESlgd87Asm::DOT4:
						break;
					case SylvanLanguage::ESlgd87Asm::LENGTH2:
						break;
					case SylvanLanguage::ESlgd87Asm::LENGTH3:
						break;
					case SylvanLanguage::ESlgd87Asm::DEG2RAD:
						break;
					case SylvanLanguage::ESlgd87Asm::RAD2DEG:
						break;
					case SylvanLanguage::ESlgd87Asm::NORMALIZE2:
						break;
					case SylvanLanguage::ESlgd87Asm::NORMALIZE3:
						break;
					case SylvanLanguage::ESlgd87Asm::DISTANCE2:
						break;
					case SylvanLanguage::ESlgd87Asm::DISTANCE3:
						break;
					case SylvanLanguage::ESlgd87Asm::MAX:
						break;
					case SylvanLanguage::ESlgd87Asm::MIN:
						break;
					case SylvanLanguage::ESlgd87Asm::POW:
						break;
					case SylvanLanguage::ESlgd87Asm::POW2:
						break;
					case SylvanLanguage::ESlgd87Asm::POW3:
						break;
					case SylvanLanguage::ESlgd87Asm::POW4:
						break;
					case SylvanLanguage::ESlgd87Asm::SQRT:
						break;
					case SylvanLanguage::ESlgd87Asm::SQRT2:
						break;
					case SylvanLanguage::ESlgd87Asm::SQRT3:
						break;
					case SylvanLanguage::ESlgd87Asm::SQRT4:
						break;
					case SylvanLanguage::ESlgd87Asm::CLAMP:
						break;
					case SylvanLanguage::ESlgd87Asm::LERP:
						break;
					case SylvanLanguage::ESlgd87Asm::LERP2:
						break;
					case SylvanLanguage::ESlgd87Asm::LERP3:
						break;
					case SylvanLanguage::ESlgd87Asm::LERP4:
						break;
					case SylvanLanguage::ESlgd87Asm::ABS:
						break;
					case SylvanLanguage::ESlgd87Asm::ABS2:
						break;
					case SylvanLanguage::ESlgd87Asm::ABS3:
						break;
					case SylvanLanguage::ESlgd87Asm::ABS4:
						break;
					case SylvanLanguage::ESlgd87Asm::FLOOR:
						break;
					case SylvanLanguage::ESlgd87Asm::CEIL:
						break;
					case SylvanLanguage::ESlgd87Asm::FRACT:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_L_C:

						break;
					case SylvanLanguage::ESlgd87Asm::MOV_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOV_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTR_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVSTRV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOVMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTR_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOSTRV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDTOMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBTOMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV2MAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV3MAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOV4MAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULTOMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV2I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV3I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOV4I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVTOMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MODTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ANDTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ORTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::XORTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::INVTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::RSHTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LSHTO_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADD_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTR_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDSTRV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ADDMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUB_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SUBMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MUL_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULIMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULFMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV2I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV3I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULV4I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2V2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3V3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4V4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT2F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT3F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MULMAT4F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIV_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4F_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV2I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV3I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVV4I_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::DIVMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::MOD_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LSH_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::RSH_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMP_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SCMPF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMP_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LCMPF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMP_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::SECMPF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMP_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LECMPF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMP_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPSTR_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::ICMPMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMP_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPIF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPSTR_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPFI_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPF_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPV4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT2_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT3_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NCMPMAT4_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::AND_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::OR_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::XOR_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICAND_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_L_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_L_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_L_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_L_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_G_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_G_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_G_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_G_L:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_R_C:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_R_G:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_R_R:
						break;
					case SylvanLanguage::ESlgd87Asm::LOGICOR_R_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NOT_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NOT_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NOT_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NOT_R:
						break;
					case SylvanLanguage::ESlgd87Asm::NOTF_C:
						break;
					case SylvanLanguage::ESlgd87Asm::NOTF_G:
						break;
					case SylvanLanguage::ESlgd87Asm::NOTF_L:
						break;
					case SylvanLanguage::ESlgd87Asm::NOTF_R:
						break;
					case SylvanLanguage::ESlgd87Asm::INV_C:
						break;
					case SylvanLanguage::ESlgd87Asm::INV_G:
						break;
					case SylvanLanguage::ESlgd87Asm::INV_L:
						break;
					case SylvanLanguage::ESlgd87Asm::INV_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSH_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSH_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSH_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSH_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHIF_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHIF_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHIF_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHIF_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHFI_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHFI_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHFI_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHFI_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHF_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHF_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHF_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHF_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV2_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV2_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV2_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV2_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV3_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV3_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV3_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV3_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV4_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV4_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV4_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHV4_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT2_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT2_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT2_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT2_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT3_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT3_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT3_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT3_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT4_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT4_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT4_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHMAT4_R:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHSTR_C:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHSTR_G:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHSTR_L:
						break;
					case SylvanLanguage::ESlgd87Asm::PUSHSTR_R:
						break;
					case SylvanLanguage::ESlgd87Asm::POP:
						break;
					case SylvanLanguage::ESlgd87Asm::MARKBP:
						break;
					case SylvanLanguage::ESlgd87Asm::FUNCTIONRET:
						break;
					case SylvanLanguage::ESlgd87Asm::POPSTR:
						break;
					default:
						break;
					}

					Consumption++;
				}
			}
		}
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
};