#pragma once
#include <string>
namespace PROJECT {
	using namespace std;
	void setNullTerminator(char* charArray, int charArrayLength) {
		for (int charArrayIndex = 0; charArrayIndex < charArrayLength; charArrayIndex++) {
			if (charArray[charArrayIndex] == -52) {
				charArray[charArrayIndex] = '\0';
				return;
			}
			else if (charArray[charArrayIndex] == '\0') return;
		}

		charArray[charArrayLength - 1] = '\0';//Potential problem in the future?
	}

	struct char30 {
		char data[30];
		char30() {
			memset(this->data, 0, 30);
		}
		char30(char data[30]) : char30() {
			strcpy(this->data, data);
			setNullTerminator(this->data, 30);
		}
		char30(string data) : char30() {
			data.copy(this->data, 30);
			setNullTerminator(this->data, 30);
		}
		bool operator==(const char30 rightChar30) {/*'this' is the leftChar30*/
			return !strcmp(this->data, rightChar30.data);
		}
		void operator=(const char30 rightChar30) {/*'this' is the leftChar30*/
			strcpy(this->data, rightChar30.data);
		}
	};
}