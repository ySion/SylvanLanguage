#include "pch.hpp"
#include "SlgCompiler.hpp"
#include "SlgInterpreter.hpp"


int main() {

	std::ifstream file("D:/hello.txt");

	if (file.is_open()) {
		std::string codes = "";
		std::string temp = "";
		while (std::getline(file, temp)) {
			codes += temp + "\n";
		}
		file.close();

		SylvanLanguage::CompilerConfig config;
		SylvanLanguage::RunTimeEnvironment env(&config);


		env.CreateNetWork("Hello");
		env.AddModuleSourceCode("Hello", codes);

		return  0;
	}
}

