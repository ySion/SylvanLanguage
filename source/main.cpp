#include "pch.hpp"
#include "SlgCompiler.hpp"
#include "SlgInterpreter.hpp"


int main() {

	std::ifstream file("../../../hello.txt");


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
		env.AddOrReplaceModule("Hello", "Dom", codes);
		bool t = env.CompileModule("Hello", "Dom");

		std::cout << "\n\n" << t << "\n";
	}

	return 0;
}
