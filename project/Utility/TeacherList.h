#pragma once
#include "../List.h"
#include "Teacher.h"
namespace PROJECT {
	class TeacherList : public List<Teacher> {
	public:
		using List::List;

		enum StudentVariable {
			username,
			password,
			uniqueNumber,
		};

		int findByValue(string stringReference, StudentVariable referencedVariable) {
			Node* currentlySelected = this->tail;
			int i = this->nodeCount - 1;
			switch (referencedVariable) {
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
			case uniqueNumber:
			{
				int intReference = stoi(stringReference);
				for (; i > -1; i--) {
					if (currentlySelected->data.uniqueNumber == intReference) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			}
			return i;
		}
	};
}