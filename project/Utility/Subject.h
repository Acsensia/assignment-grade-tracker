#pragma once
#include <string>
namespace PROJECT {
	using namespace std;
	struct Subject {
		char name[30];
		////By the way, is this (taskCount) really needed inside the project runtime? 
		////We should be able to just write it to file from tasks.length() and store
		//// it inside a temporary variable while reading, shouldn't we?
		int uniqueNumber = -1;//// 
		int taskCount = 0;
		//List<char30> tasks;
		TaskList tasks;
		Subject() {
			memset(this->name, 0, 30);
		}
		Subject(char name[30], int uniqueNumber) : Subject() {////
			strcpy(this->name, name);
			setNullTerminator(this->name, 30);
			this->uniqueNumber = uniqueNumber;////
		}
		Subject(string name, int uniqueNumber) : Subject() {////
			name.copy(this->name, 30);
			setNullTerminator(this->name, 30);
			this->uniqueNumber = uniqueNumber;////
		}
		void add(char task[30], int uniqueTaskNumber = -1, int index = -1) {
			tasks.add(Task(task, uniqueTaskNumber), index);
			updateTaskCount();
		}
		void add(string& task, int uniqueTaskNumber = -1, int index = -1) {
			tasks.add(Task(task, uniqueTaskNumber), index);
			updateTaskCount();
		}
		void add(char30& task, int uniqueTaskNumber = -1, int index = -1) {
			tasks.add(Task(task.data, uniqueTaskNumber), index);
			updateTaskCount();
		}
		void remove(int index) {
			tasks.remove(index);
			updateTaskCount();
		}
	private:
		void updateTaskCount() { taskCount = tasks.length(); }
	};
}