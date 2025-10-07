#pragma once
#include "../List.h"
#include "Student.h"
namespace PROJECT {
	class StudentList : public List<Student> {
	public:
		using List::List;

		enum StudentVariable {
			name,
			uniqueNumber,
			username,
			password,
			assignedSubjects_subject_uniqueNumber,
			assignedSubjects_assignedTasks_task_uniqueNumber,
			assignedSubjects_assignedTasks_mark
		};

		int findByValue(string stringReference, StudentVariable referencedVariable) {
			Node* currentlySelected = this->tail;
			int i = this->nodeCount - 1;
			switch (referencedVariable) {
			case name:
			{
				for (; i > -1; i--) {
					if (currentlySelected->data.name == stringReference) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			case uniqueNumber:
			{
				int intReference = stoi(stringReference);
				for (; i > -1; i--) {
					if (currentlySelected->data.uniqueNumber == intReference) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			case username:
			{
				for (; i > -1; i--) {
					if (currentlySelected->data.username == stringReference) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			case password:
			{
				for (; i > -1; i--) {
					if (currentlySelected->data.password == stringReference) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			case assignedSubjects_subject_uniqueNumber:
			{
				for (; i > -1; i--) {
					if (currentlySelected->data.assignedSubjects.findByValue(stringReference, AssignedSubjectList::subject_uniqueNumber) >= 0) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			case assignedSubjects_assignedTasks_task_uniqueNumber:
			{
				for (; i > -1; i--) {
					if (currentlySelected->data.assignedSubjects.findByValue(stringReference, AssignedSubjectList::assignedTasks_task_uniqueNumber) >= 0) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			case assignedSubjects_assignedTasks_mark:
			{
				for (; i > -1; i--) {
					if (currentlySelected->data.assignedSubjects.findByValue(stringReference, AssignedSubjectList::assignedTasks_mark) >= 0) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			}
			return i;
		}

		tuple<bool, string> toFile(const string& fileName, ofstream& out) {
			Node* currentlySelected = this->head;
			try {
				for (int i = 0; i < this->length(); i++) {
					out.write(reinterpret_cast<char*>(&currentlySelected->data.name), sizeof(char[30]));
					out.write(reinterpret_cast<char*>(&currentlySelected->data.uniqueNumber), sizeof(int));
					out.write(reinterpret_cast<char*>(&currentlySelected->data.username), sizeof(char[30]));
					out.write(reinterpret_cast<char*>(&currentlySelected->data.password), sizeof(char[30]));
					int length_temp = currentlySelected->data.assignedSubjects.length();
					out.write(reinterpret_cast<char*>(&length_temp), sizeof(int));
					currentlySelected->data.assignedSubjects.toFile(fileName, out);
					currentlySelected = currentlySelected->next;
				}
			}
			catch (int errorNumber) {
				out.close();
				return make_tuple(false, "Error while saving data to file \"" + fileName + "\",  error number: " + to_string(errorNumber) + ".");
			}
			return make_tuple(true, "");
		}
		tuple<bool, string> fromFile(const string& fileName, ifstream& in, int numberOfIterations = -1) {
			this->clear();
			try {
				Student data;

				////In the code below we are writing directly into variables of 'data', which may be the cause of any problems which may arise in the future

				auto fromFile_ItemReadingCycle = [&]() {
					try {
						in.read(reinterpret_cast<char*>(&data.name), sizeof(char[30]));
						in.read(reinterpret_cast<char*>(&data.uniqueNumber), sizeof(int));
						in.read(reinterpret_cast<char*>(&data.username), sizeof(char[30]));
						in.read(reinterpret_cast<char*>(&data.password), sizeof(char[30]));
						this->add(data);
						int length_temp;
						in.read(reinterpret_cast<char*>(&length_temp), sizeof(int));
						this->getReference(-1)->assignedSubjects.fromFile(fileName, in, length_temp);
						return true;
					}
					catch (int errorNumber) { return false; }
					};

				if (numberOfIterations < 0) while (in.peek() != EOF) { if (!fromFile_ItemReadingCycle()) break; }
				else {
					for (int currentIteration = 0; currentIteration < numberOfIterations; currentIteration++) { if (!fromFile_ItemReadingCycle()) break; }
				}
			}
			catch (int errorNumber) {
				in.close();
				return make_tuple(false, "Error while loading data from file \"" + fileName + "\",  error number: " + to_string(errorNumber) + ".");
			}
			return make_tuple(true, "");
		}

		tuple<bool, string> toFile(const string& fileName) {
			ofstream out;
			try { out.open(fileName, ios::binary); }
			catch (int errorNumber) {
				return make_tuple(false, "Can't open file \"" + fileName + "\",  error number: " + to_string(errorNumber) + ".");
			}

			tuple<bool, string> fileSaveResult;
			fileSaveResult = toFile(fileName, out);

			out.close();
			return fileSaveResult;
		}
		tuple<bool, string> fromFile(const string& fileName, int numberOfIterations = -1) {
			ifstream in;
			try { in.open(fileName, ios::binary); }
			catch (int errorNumber) {
				return make_tuple(false, "Can't open file \"" + fileName + "\",  error number: " + to_string(errorNumber) + ".");
			}
			if (!in.good()) {
				in.close();
				return make_tuple(false, "File \"" + fileName + "\" doesn't exist.");
			}
			if (in.peek() == ifstream::traits_type::eof()) {
				in.close();
				return make_tuple(false, "File \"" + fileName + "\" is empty.");
			}


			tuple<bool, string> fileLoadResult;
			fileLoadResult = fromFile(fileName, in, numberOfIterations);

			in.close();
			return fileLoadResult;
		}
	};
}