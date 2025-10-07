#pragma once
#include "AssignedTaskList.h"
namespace PROJECT {
	struct AssignedSubject {
		unsigned int subject_uniqueNumber = -1;
		AssignedTaskList assignedTasks;
		AssignedSubject() {

		}
		AssignedSubject(int subject_uniqueNumber) {
			this->subject_uniqueNumber = subject_uniqueNumber;
		}
	};
}