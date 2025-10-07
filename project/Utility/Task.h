#pragma once
#include <string>
namespace PROJECT {
	using namespace std;
	struct Task {
		char name[30];
		int uniqueNumber = -1;
		Task() {
			memset(this->name, 0, 30);
		}
		Task(char name[30], int uniqueNumber = -1) : Task() {
			strcpy(this->name, name);
			setNullTerminator(this->name, 30);
			this->uniqueNumber = uniqueNumber;
		}
		Task(string name, int uniqueNumber = -1) : Task() {
			name.copy(this->name, 30);
			setNullTerminator(this->name, 30);
			this->uniqueNumber = uniqueNumber;
		}
	};
}