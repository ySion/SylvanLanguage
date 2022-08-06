#include "pch.hpp"
#include "SlgCompiler.hpp"


int main() {

	std::ifstream file("D:/hello.txt");

	if (file.is_open()) {
		std::string codes = "";
		std::string temp = "";
		while (std::getline(file, temp)) {
			codes += temp + "\n";
		}
		file.close();
		std::string bb = "abc";

		SlgErrorMachine mach;
		SlgTokenizer token(codes, &mach);
		if (mach.Success()) {
			SlgStatementCompile SA(&token, &mach);
			if (mach.Success()) {
				std::cout << "Success" << "\n";
				SA.ShowProgramTable();
				SA.ShowAsm();
			}
			else {
				mach.ShowErrors();
			}
		}
		else {
			mach.ShowErrors();
		}
	}
	return  0;
}


