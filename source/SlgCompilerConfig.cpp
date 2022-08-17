#include "SlgCompilerConfig.hpp"
namespace SylvanLanguage {

	bool AssemblyData::ReSize(size_t newSize) {
		void* newMem = realloc(mRoot, newSize);

		if (mRoot) {
			mRoot = newMem;
			mDataAllSize = newSize;
			return true;
		}
		else {
			return false;
		}
	}

	AssemblyData::AssemblyData(size_t size) : mDataAllSize(size) {
		mRoot = malloc(size);
	}

	bool AssemblyData::ReadData(const void* p, size_t size) {

		if (!ReSize(size)) {
			return false;
		}

		memcpy((unsigned char*)mRoot, p, size);
		mDataUsingSize = size;
		return true;
	}

	bool AssemblyData::AddData(const void* p, size_t size) {
		if (mDataUsingSize + size > mDataAllSize) {
			if (!ReSize(mDataUsingSize + size + 2048)) {
				return false;
			}
		}
		memcpy((unsigned char*)mRoot + mDataUsingSize, p, size);
		mDataUsingSize += size;
		return true;
	}

	bool AssemblyData::AddString(const std::string& strPtr) {
		return AddData(strPtr.c_str(), strPtr.size() + 1);
	}

	AssemblyData::~AssemblyData() {

		if (mRoot != nullptr) {
			free(mRoot);
		}
	}

	void AssemblyData::Show() {

		std::cout << "Total Size(Byte):" << this->mDataAllSize << "\n";
		std::cout << "Using Size(Byte):" << this->mDataUsingSize << "\n";

		for (size_t i = 0; i < mDataUsingSize; i++) {

			printf("%02X  ", ((unsigned char*)mRoot)[i]);

			if ((i + 1) % 16 == 0) {
				printf("      ");
				for (size_t j = i - 15; j <= i; j++) {
					if (isprint(((unsigned char*)mRoot)[j])) {
						printf("%c ", ((unsigned char*)mRoot)[j]);
					}
					else {
						printf(". ");
					}

				}
				printf("\n");
			}
		}
		printf("      ");
		for (size_t j = mDataUsingSize - mDataUsingSize % 16; j < mDataUsingSize; j++) {

			if (isprint(((unsigned char*)mRoot)[j])) {
				printf("%c ", ((unsigned char*)mRoot)[j]);
			}
			else {
				printf(". ");
			}
		}

		std::cout << std::endl;
	}
}