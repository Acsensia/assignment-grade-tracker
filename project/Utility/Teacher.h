#pragma once
#include <string>
#include "../char30.h"
namespace PROJECT {
	using namespace std;
	struct Teacher {
		char username[30];
		char password[30];
		int uniqueNumber = -1;
		Teacher() {
			memset(this->username, 0, 30);
			memset(this->password, 0, 30);
		}
		Teacher(int uniqueNumber, char username[30], char password[30]) : Teacher() {
			strcpy(this->username, username);
			strcpy(this->password, password);

			setNullTerminator(this->username, 30);
			setNullTerminator(this->password, 30);

			this->uniqueNumber = uniqueNumber;
		}
		Teacher(int uniqueNumber, string username, string password) : Teacher() {
			username.copy(this->username, 30);
			password.copy(this->password, 30);

			setNullTerminator(this->username, 30);
			setNullTerminator(this->password, 30);

			this->uniqueNumber = uniqueNumber;
		}
	};
}