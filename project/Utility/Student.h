#pragma once
#include "../char30.h"
#include <string>
namespace PROJECT {
	using namespace std;
	struct Student {
		char name[30];
		int uniqueNumber = -1;
		char username[30];
		char password[30];
		AssignedSubjectList assignedSubjects;
		Student() {
			memset(this->name, 0, 30);
			memset(this->username, 0, 30);
			memset(this->password, 0, 30);
		}
		Student(char name[30], int uniqueNumber, char username[30], char password[30]) : Student() {
			strcpy(this->name, name);
			strcpy(this->username, username);
			strcpy(this->password, password);

			setNullTerminator(this->name, 30);
			setNullTerminator(this->username, 30);
			setNullTerminator(this->password, 30);

			this->uniqueNumber = uniqueNumber;
		}
		Student(string name, int uniqueNumber, string username, string password) : Student() {
			name.copy(this->name, 30);
			username.copy(this->username, 30);
			password.copy(this->password, 30);

			setNullTerminator(this->name, 30);
			setNullTerminator(this->username, 30);
			setNullTerminator(this->password, 30);

			this->uniqueNumber = uniqueNumber;
		}
	};
}