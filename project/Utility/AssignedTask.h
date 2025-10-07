#pragma once
namespace PROJECT {
	struct AssignedTask {
		int task_uniqueNumber = -1;
		int mark = -1;
		AssignedTask() {

		}
		AssignedTask(unsigned int task_uniqueNumber) {
			this->task_uniqueNumber = task_uniqueNumber;
		}
	};
}