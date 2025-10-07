#pragma once
#include <string>
#include "..\List.h"
#include "AssignedTask.h"
namespace PROJECT {
	class AssignedTaskList : public List<AssignedTask> {
	public:
		using List::List;

		enum assignedTaskVariable {
			task_uniqueNumber,
			mark
		};

		int findByValue(string stringReference, assignedTaskVariable referencedVariable) {
			Node* currentlySelected = this->tail;
			int i = this->nodeCount - 1;
			switch (referencedVariable) {
			case task_uniqueNumber:
			{
				int intReference = stoi(stringReference);
				for (; i > -1; i--) {
					if (currentlySelected->data.task_uniqueNumber == intReference) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			case mark:
			{
				int intReference = stoi(stringReference);
				for (; i > -1; i--) {
					if (currentlySelected->data.mark == intReference) break;
					currentlySelected = currentlySelected->prev;
				}
			}
			break;
			}
			return i;
		}
	};
}