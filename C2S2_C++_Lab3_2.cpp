#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <windows.h>
#include "asd/ASDInput.h"
#include "asd/ASDWindow.h"
#include "asd/fun_console.h"
#include "asd/ASDFunctor.h"
#include "asd/ASDInputWIndow.h"
#include "project/List.h"
#include "project/char30.h"
#include "project/coords.h"
#include "project/dimensions.h"
#include "project/AreaManager.h"
#include "project/Utility/Task.h"
#include "project/Utility/TaskList.h"
#include "project/Utility/Subject.h"
#include "project/Utility/SubjectList.h"
#include "project/Utility/AssignedTask.h"
#include "project/Utility/AssignedTaskList.h"
#include "project/Utility/AssignedSubject.h"
#include "project/Utility/AssignedSubjectList.h"
#include "project/Utility/Student.h"
#include "project/Utility/StudentList.h"
#include "project/Utility/Teacher.h"
#include "project/Utility/TeacherList.h"
#include "project/Utility/MessageContents.h"
using namespace PROJECT;
namespace PROJECT {
	namespace LIMITS {
		const int usernameMinLength = 5;
		const int passwordMinLength = 6;

		const int uniqueNumberMaxLength = 3;
		const int usernameMaxLength = 29;
		const int passwordMaxLength = 29;

		const int markMinValue = 0;
		const int markMaxValue = 100;
		const int markMaxLength = 3;

		const int x_Offset = 0;// != 0 IS DRAWN BADLY 
		const int y_Offset = 0;// != 0 IS DRAWN BADLY

		const int windowWidth = 192;
		const int windowHeight = 50;

		const int screenAreasColumns = 4;
		const int screenAreasRows = 2;

		const int screenAreasColumnGap = 1;
		const int screenAreasRowGap = 0;
		const int screenAreasWindowsGap = -1;

		const int areaWidth = (windowWidth / screenAreasColumns) - abs(screenAreasColumnGap * (screenAreasColumns - 1));
		const int areaHeight = (windowHeight / screenAreasRows) - abs(screenAreasRowGap * (screenAreasRows - 1));

		ConsoleColor backgroundColor = Black;
		ConsoleColor foregroundColor = White;
		ConsoleColor windowBorderColor = Yellow;
		
		ConsoleColor menuSelectedOption_backgroundColor = Brown;
		ConsoleColor menuSelectedOption_foregroundColor = Black;
	}

	namespace STYLES {
		namespace ArrayMenu {
			/*private:*/namespace {
				void universal(ASD::ArrayMenu& arrayMenu) {
					arrayMenu.SetSelectBgColor(LIMITS::menuSelectedOption_backgroundColor).SetSelectColor(LIMITS::menuSelectedOption_foregroundColor);
					arrayMenu.SetBgColor(LIMITS::backgroundColor).SetColor(LIMITS::foregroundColor).SetBorderColor(LIMITS::windowBorderColor);
					arrayMenu.SetDefaultCoverageParameters(1, 0, 0, 0, true);
				}
			}

			void regular(ASD::ArrayMenu& arrayMenu) { universal(arrayMenu); }
			void arrayGenerated(ASD::ArrayMenu& arrayMenu) { universal(arrayMenu); }
		}
		namespace Message {
			/*private:*/namespace {
				void popup(ASD::Message& message) {
					message.options.adjustHeight_IfAddedWhileEmpty = false;
					message.options.adjustHeight_IfInsufficient = false;
					message.options.adjustWidth_IfInsufficient = false;
					message.options.adjustLinksWidth_IfInsufficient = false;
					message.options.do_getch = true;
				}
			}
			void title(ASD::Message& message) {
				message.SetBgColor(LIMITS::backgroundColor).SetColor(LIMITS::foregroundColor).SetBorderColor(LIMITS::windowBorderColor);
				message.options.adjustHeight_Always = true;
				message.SetWidth(LIMITS::areaWidth);
			}
			void error(ASD::Message& message) {
				message.SetBgColor(LIMITS::backgroundColor).SetColor(LIMITS::foregroundColor).SetBorderColor(Red);
				popup(message);
			}
			void success(ASD::Message& message) {
				message.SetBgColor(LIMITS::backgroundColor).SetColor(LIMITS::foregroundColor).SetBorderColor(Green);
				popup(message);
			}
			void neutral(ASD::Message& message) {
				message.SetBgColor(LIMITS::backgroundColor).SetColor(LIMITS::foregroundColor).SetBorderColor(LightGray);
				popup(message);
			}
		}
		namespace InputWindow {
			void regular(ASD::InputWindow& inputWindow) {
				SetColor(LIMITS::foregroundColor, LIMITS::backgroundColor);
				inputWindow.input->SetColor(LIMITS::menuSelectedOption_foregroundColor).SetBgColor(LIMITS::menuSelectedOption_backgroundColor);
				inputWindow.SetBgColor(LIMITS::backgroundColor).SetColor(LIMITS::foregroundColor).SetBorderColor(LIMITS::windowBorderColor);
				inputWindow.SetDefaultCoverageParameters(1, 0, 0, 0, false);
			}
		}
	}

	namespace GLOBAL {
		SubjectList subjects;
		StudentList students;
		TeacherList teachers;

		//Classes used to assign windows to one of four quarters of the console window
		AreaManager::AreasInAreasGrouper menuGroups;
		AreaManager screenArea = AreaManager(LIMITS::areaWidth, LIMITS::areaHeight, LIMITS::screenAreasColumns,
			LIMITS::screenAreasRows, LIMITS::screenAreasColumnGap, LIMITS::screenAreasRowGap,
			LIMITS::screenAreasWindowsGap, LIMITS::x_Offset, LIMITS::y_Offset, AreaManager::Options::vertical);

		//Shared message line list for shared Message windows
		MessageContents messageContents;

		//Last window which was used as a snapTarget by 'snapInputWindow()' function (meaning, last 'window_SnapTarget' parameter) 
		ASD::Window* currentSnappedWindow;

		//Shared Message windows; used for all the quick pop-up messages across the project
		ASD::Message message_error;
		ASD::Message message_success;
		ASD::Message message_neutral;

		//InputWindow windows
		ASD::IntegerInput __input_uniqueNumber = ASD::IntegerInput(LIMITS::uniqueNumberMaxLength);
		ASD::InputWindow inputWindow_uniqueNumber = ASD::InputWindow(&__input_uniqueNumber, STYLES::InputWindow::regular);
		ASD::UsernameInput __input_username = ASD::UsernameInput(LIMITS::usernameMaxLength);
		ASD::InputWindow inputWindow_username = ASD::InputWindow(&__input_username, STYLES::InputWindow::regular);
		ASD::FilenameInput __input_password = ASD::FilenameInput(LIMITS::passwordMaxLength);
		ASD::InputWindow inputWindow_password = ASD::InputWindow(&__input_password, STYLES::InputWindow::regular);

		//Input windows' shared title
		ASD::Message inputsTitle;
	}

	string fileStringClarify(string fileString) {
		return fileString.substr(0, fileString.find(-52));
	}

	string stoi_leadingZeros(int number, int numberOfZeros) {
		bool numberIsNegative = false;
		if (number < 0) { numberIsNegative = true; number *= -1; }
		string result = to_string(number);
		int calculatedNumberOfZeros = numberOfZeros - result.length();
		if (calculatedNumberOfZeros > 0) result.insert(0, calculatedNumberOfZeros, '0');
		if (numberIsNegative) result.insert(0, 1, '-');
		return result;
	}

	string to_string_specific(double floatingPointNumber, int precision) {
		string result = to_string(floatingPointNumber);
		if (precision >= 0) {
			int pointIndex = result.find('.');
			if (pointIndex != string::npos && (pointIndex + 1 + precision) < result.length()) result.erase(pointIndex + bool(precision) + precision);
		}
		return result;
	}

	string to_string_specific(Task* task, bool useUniqueNumber = true) {
		string result;
		if (useUniqueNumber) result += "[ID:" + stoi_leadingZeros(task->uniqueNumber, LIMITS::uniqueNumberMaxLength) + "] ";
		result += task->name;
		return result;
	}
	string to_string_specific(Subject* subject, bool useUniqueNumber = true) {
		string result;
		if (useUniqueNumber) result += "[ID:" + stoi_leadingZeros(subject->uniqueNumber, LIMITS::uniqueNumberMaxLength) + "] ";
		result += subject->name;
		return result;
	}
	string to_string_specific(Student* student, bool useUniqueNumber = true, bool useUserName = false) {
		string result;
		if (useUniqueNumber) result += "[ID:" + stoi_leadingZeros(student->uniqueNumber, LIMITS::uniqueNumberMaxLength) + "] ";
		result += student->name;
		if (useUserName) result += "(Username: " + string(student->username) + string(")");
		return result;
	}

	string to_string_specific(AssignedSubject* assignedSubject, SubjectList& subjectList, bool useUniqueNumber = true) {
		Subject* referencedSubject = subjectList.getReference(
			subjectList.findByValue(
				to_string(assignedSubject->subject_uniqueNumber),
				SubjectList::uniqueNumber
			)
		);
		return to_string_specific(referencedSubject, useUniqueNumber);
	}
	string to_string_specific(AssignedTask* assignedTask, TaskList& taskList, bool useUniqueNumber = true, bool useMark = true) {
		Task* referencedTask = taskList.getReference(
			taskList.findByValue(
				to_string(assignedTask->task_uniqueNumber),
				TaskList::uniqueNumber
			)
		);
		string result;
		if (useMark) {
			string markConverted = (assignedTask->mark >= 0) ? to_string(assignedTask->mark) : "X";
			result += "[MARK:" + markConverted + "] ";
		}
		result += to_string_specific(referencedTask, useUniqueNumber);
		return result;
	}

	void generateUniMenuFromList(ASD::UniversalCallbackMenu& menu, TaskList& taskList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int taskIndex = 0; taskIndex < taskList.length(); taskIndex++) {
			Task* referencedTask = taskList.getReference(taskIndex);
			menu.Add(to_string_specific(referencedTask).c_str());
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}
	void generateUniMenuFromList(ASD::UniversalCallbackMenu& menu, SubjectList& subjectList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int subjectIndex = 0; subjectIndex < subjectList.length(); subjectIndex++) {
			Subject* referencedSubject = subjectList.getReference(subjectIndex);
			menu.Add(to_string_specific(referencedSubject).c_str());
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}
	void generateUniMenuFromList(ASD::UniversalCallbackMenu& menu, StudentList& studentList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int studentIndex = 0; studentIndex < studentList.length(); studentIndex++) {
			Student* referencedStudent = studentList.getReference(studentIndex);
			menu.Add(to_string_specific(referencedStudent).c_str());
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}
	void generateUniMenuFromList(ASD::UniversalCallbackMenu& menu, List<char30>& char30list) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int char30Index = 0; char30Index < char30list.length(); char30Index++) {
			menu.Add(char30list.getReference(char30Index)->data);
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}

	void generateUniMenuFromLists(ASD::UniversalCallbackMenu& menu, AssignedSubjectList& assignedSubjectList, SubjectList& subjectList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int assignedSubjectIndex = 0; assignedSubjectIndex < assignedSubjectList.length(); assignedSubjectIndex++) {
			AssignedSubject* referencedAssignedSubject = assignedSubjectList.getReference(assignedSubjectIndex);
			menu.Add(to_string_specific(referencedAssignedSubject, subjectList).c_str());
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}
	void generateUniMenuFromLists(ASD::UniversalCallbackMenu& menu, AssignedTaskList& assignedTaskList, TaskList& taskList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int assignedTaskIndex = 0; assignedTaskIndex < assignedTaskList.length(); assignedTaskIndex++) {
			AssignedTask* referencedAssignedTask = assignedTaskList.getReference(assignedTaskIndex);
			menu.Add(to_string_specific(referencedAssignedTask, taskList).c_str());
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}

	void generateUniMenuFromSubjectLists_markNegative(ASD::UniversalCallbackMenu& menu, AssignedSubjectList& assignedSubjectList, SubjectList& subjectList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int assignedSubjectIndex = 0; assignedSubjectIndex < assignedSubjectList.length(); assignedSubjectIndex++) {
			AssignedSubject* referencedAssignedSubject = assignedSubjectList.getReference(assignedSubjectIndex);
			if (referencedAssignedSubject->assignedTasks.findByValue("-1", AssignedTaskList::mark) != -1) menu.Add(to_string_specific(referencedAssignedSubject, subjectList).c_str());
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}
	void generateUniMenuFromSubjectLists_markPositive(ASD::UniversalCallbackMenu& menu, AssignedSubjectList& assignedSubjectList, SubjectList& subjectList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int assignedSubjectIndex = 0; assignedSubjectIndex < assignedSubjectList.length(); assignedSubjectIndex++) {
			AssignedSubject* referencedAssignedSubject = assignedSubjectList.getReference(assignedSubjectIndex);
			int i;
			for (i = referencedAssignedSubject->assignedTasks.length() - 1; i > -1; i--) {
				AssignedTask* curAssignedTask = referencedAssignedSubject->assignedTasks.getReference(i);
				if (curAssignedTask->mark >= 0) break;
			}
			if (i != -1) menu.Add(to_string_specific(referencedAssignedSubject, subjectList).c_str());
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}

	void generateUniMenuFromTaskLists_markNegative(ASD::UniversalCallbackMenu& menu, AssignedTaskList& assignedTaskList, TaskList& taskList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int assignedTaskIndex = 0; assignedTaskIndex < assignedTaskList.length(); assignedTaskIndex++) {
			AssignedTask* referencedAssignedTask = assignedTaskList.getReference(assignedTaskIndex);
			if (referencedAssignedTask->mark < 0) menu.Add(to_string_specific(referencedAssignedTask, taskList).c_str());
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}
	void generateUniMenuFromTaskLists_markPositive(ASD::UniversalCallbackMenu& menu, AssignedTaskList& assignedTaskList, TaskList& taskList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int assignedTaskIndex = 0; assignedTaskIndex < assignedTaskList.length(); assignedTaskIndex++) {
			AssignedTask* referencedAssignedTask = assignedTaskList.getReference(assignedTaskIndex);
			if (referencedAssignedTask->mark >= 0) menu.Add(to_string_specific(referencedAssignedTask, taskList).c_str());
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}

	void generateUniMenuForAssignment(ASD::UniversalCallbackMenu& menu, AssignedSubjectList& assignedSubjectList, SubjectList& subjectList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int subjectIndex = 0; subjectIndex < subjectList.length(); subjectIndex++) {
			Subject* referencedSubject = subjectList.getReference(subjectIndex);
			if (assignedSubjectList.findByValue(to_string(referencedSubject->uniqueNumber), AssignedSubjectList::subject_uniqueNumber) < 0) {
				menu.Add(to_string_specific(referencedSubject).c_str());
			}
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}
	void generateUniMenuForAssignment(ASD::UniversalCallbackMenu& menu, AssignedTaskList& assignedTaskList, TaskList& taskList) {
		int LastChosenOptionIndex = menu.GetLastChosenOptionIndex();
		menu.Clear();
		for (int taskIndex = 0; taskIndex < taskList.length(); taskIndex++) {
			Task* referencedTask = taskList.getReference(taskIndex);
			if (assignedTaskList.findByValue(to_string(referencedTask->uniqueNumber), AssignedTaskList::task_uniqueNumber) < 0) {
				menu.Add(to_string_specific(referencedTask).c_str());
			}
		}
		menu.SetLastChosenOptionIndex(LastChosenOptionIndex);
	}

	//Universal function that handles the event of ESC button being pressed
	tuple<int, string>(*universalEscPressed)() = []()->tuple<int, string> {
		return { -1, "ESC pressed" };
		};

	//Functions used to simplify copying a Window object's 'x', 'y', 'width' and 'height' attributes
	ASD::Window& copyWindowCoords(ASD::Window& from, ASD::Window& to) { to.Move(from.GetX(), from.GetY()); return to; }
	ASD::Window& copyWindowDimensions(ASD::Window& from, ASD::Window& to) { to.SetDimensions(from.GetWidth(), from.GetHeight()); return to; }
	ASD::Window& copyWindowCoordsDimensions(ASD::Window& from, ASD::Window& to) { copyWindowCoords(from, to); copyWindowDimensions(from, to); return to; }

	//Function used to customize the shared Message windows
	//Clears the incoming List object after use
	ASD::Message snapMessage(ASD::Window& window_SnapTarget, ASD::Message& message, MessageContents* messageContents) {
		//Clearing the message to prevent unneeded checks during width changes
		message.Clear();

		//Getting a pointer to a window's title window 
		ASD::Window* snapTarget_TopLink = window_SnapTarget.GetLink_top();

		//If the window_SnapTarget doesn't have a title, then most probably the user passed the title itself as an argument
		//(cause the title is not supposed to have a 'Link_top')
		//Then we'll use that as we need the Title window either way
		if (!snapTarget_TopLink) snapTarget_TopLink = &window_SnapTarget;

		//Recovering the window_SnapTarget's area index
		int snapTarget_AreaIndex = GLOBAL::screenArea.ReverseEngineerCoordsIntoAreaNumber({ snapTarget_TopLink->GetX(), snapTarget_TopLink->GetY() });
		if (snapTarget_AreaIndex >= 0) {

			//Saving old values to reverse the changes later on
			int screenArea_oldCurrentArea = GLOBAL::screenArea.GetCurrentArea();
			bool screenArea_oldAdjustmentRule = GLOBAL::screenArea.options.applyDefaultWindowSizeAdjustments;

			//Temporary adjustments
			GLOBAL::screenArea.SetCurrentArea(snapTarget_AreaIndex);
			GLOBAL::screenArea.options.applyDefaultWindowSizeAdjustments = false;

			//Assigning size and coordinates according to window_SnapTarget's area (there are 8 areas total in the project)
			GLOBAL::screenArea.AssignArea(&List<ASD::Window*>().add(&message), false);

			//Reversing temporary adjustments
			GLOBAL::screenArea.SetCurrentArea(screenArea_oldCurrentArea);
			GLOBAL::screenArea.options.applyDefaultWindowSizeAdjustments = screenArea_oldAdjustmentRule;
		}
		else {//Fallback
			auto functor = [](ASD::Window* message, ASD::Window* window_SnapTarget, ASD::Window* snapTarget_TopLink) {
				copyWindowCoords(*snapTarget_TopLink, *message);
				int adjustedWidth = snapTarget_TopLink->GetWidth();
				int adjustedHeight = snapTarget_TopLink->GetHeight();
				if (snapTarget_TopLink->GetWidth() != window_SnapTarget->GetWidth()) adjustedWidth += window_SnapTarget->GetWidth();
				if (snapTarget_TopLink->GetHeight() != window_SnapTarget->GetHeight()) adjustedHeight += window_SnapTarget->GetHeight();
				message->SetDimensions(adjustedWidth, adjustedHeight);
				};
			if (snapTarget_TopLink != &window_SnapTarget) { functor(&message, &window_SnapTarget, snapTarget_TopLink); }
			else {
				ASD::Window* topLink_BottomLink = snapTarget_TopLink->GetLink_bottom();
				functor(&message, topLink_BottomLink, snapTarget_TopLink);
			}
		}

		for (int i = 0; i < messageContents->data.length(); i++) {
			ASD::Message::Line* currentLine = messageContents->data.getReference(i);
			message.AddLine(currentLine->line, currentLine->centering_option);
		}

		messageContents->data.clear();
		return message;
	}

	//Function used to customize Window objects and and their 'title' Message objects 'on the go'
	ASD::Window& snapWindowWithTitle(ASD::Window& window_SnapTarget, ASD::Window& window, ASD::Message& windowTitle, string windowTitle_Title,
		ASD::Message::LineCenteringOption centeringOption = ASD::Message::LineCenteringOption::center) {
		windowTitle.Clear().AddLine(windowTitle_Title, centeringOption);
		ASD::Window* snapTarget_TopLink = window_SnapTarget.GetLink_top();

		window.SetLink_top(windowTitle);
		int snapTarget_AreaIndex = GLOBAL::screenArea.ReverseEngineerCoordsIntoAreaNumber({ snapTarget_TopLink->GetX(), snapTarget_TopLink->GetY() });
		int screenArea_oldCurrentArea = GLOBAL::screenArea.GetCurrentArea();
		GLOBAL::screenArea.SetCurrentArea(snapTarget_AreaIndex);
		GLOBAL::screenArea.AssignArea(&List<ASD::Window*>().add(&windowTitle).add(&window), false);
		GLOBAL::screenArea.SetCurrentArea(screenArea_oldCurrentArea);

		GLOBAL::currentSnappedWindow = &window_SnapTarget;
		return window;
	}

	//Function used as a shorthand for ordering a window and its links (most often its title window) to erase themselves from the console screen
	//Dependent on the 'defaultCoverageParameters' variable of the passed Window object. If the values of the said variable are {0, 0, 0, 0, false}, nothing will happen
	void RequestWindowGroupErasure(ASD::Window& window) {
		window.MakeLinkRequest_NoArgs<ASD::Window&>(window.GetDefaultCoverageParameters(), ASD::Window::Options::linkRequestType_NoArgs::ClearSelf);
	}

	//Function used as a shorthand for creating, styling and linking a Window title to a Window object
	void CreateBindMessageTitle(ASD::Window& bindTarget, ASD::Message& messageTitle, string messageContents,
		void (*stylingFunction)(ASD::Message&) = STYLES::Message::title)
	{
		stylingFunction(messageTitle);
		messageTitle.AddLine(messageContents, ASD::Message::center);
		messageTitle.SetLink_bottom(bindTarget);
	}

	//Function used as a shorthand for dynamically changing a Window object's title
	void changeWindowTitle(ASD::Window& window, string newTitleContents) {
		/*ASD::Message* windowTitle = dynamic_cast<ASD::Message*>(window.GetLink_top());
		windowTitle->Clear().AddLine(newTitleContents);*/
		ASD::Message* windowTitle = dynamic_cast<ASD::Message*>(window.GetLink_top());
		snapWindowWithTitle(window, window, *windowTitle, newTitleContents);
	}

	namespace WORKCYCLE {
		//Pointers needed to transfer results of calculations between functions (lambdas)
		Task* selectedTask;
		Subject* selectedSubject;
		AssignedTask* selectedAssignedTask;
		AssignedSubject* selectedAssignedSubject;
		Student* selectedStudent;
		Teacher* selectedTeacher;

		//Universal function which contains operations which have to be executed at the start of a -WorkCycle() function
		void workCycleStart() {
			SetColor(LIMITS::foregroundColor, LIMITS::backgroundColor);
			cls();
		}

		//Universal function which contains operations which have to be executed at the end of a -WorkCycle() function
		void workCycleShutdown() {

		}

		namespace STUDENT {
			//Various menus and their titles sorted by depth
			//Depth 0 - menu that first appears on screen
			ASD::Message menuOptions_title;                        ASD::CallbackMenu menuOptions;

			//Depth 1 - menus that are submenus of the menu above 
			ASD::Message menuViewDoneUndoneAssignedSubjects_title; ASD::UniversalCallbackMenu menuViewDoneUndoneAssignedSubjects;
			ASD::Message menuViewDoneAssignedSubjects_title;       ASD::UniversalCallbackMenu menuViewDoneAssignedSubjects;
			ASD::Message menuViewUndoneAssignedSubjects_title;     ASD::UniversalCallbackMenu menuViewUndoneAssignedSubjects;
			ASD::Message menuAccountManagement_title;              ASD::CallbackMenu          menuAccountManagement;

			//Depth 2 - menus that are submenus of the menus above
			ASD::Message menuViewEditDoneUndoneTasks_title;        ASD::UniversalCallbackMenu menuViewEditDoneUndoneTasks;
			ASD::Message menuViewEditDoneTasks_title;              ASD::UniversalCallbackMenu menuViewEditDoneTasks;
			ASD::Message menuViewEditUndoneTasks_title;            ASD::UniversalCallbackMenu menuViewEditUndoneTasks;

			//--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

			//Initialization (pre-Main) function
			void studentWorkCycle_Init() {

				menuViewEditDoneUndoneTasks.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuViewEditDoneUndoneTasks);
				CreateBindMessageTitle(menuViewEditDoneUndoneTasks, menuViewEditDoneUndoneTasks_title, "_Selected subject name and number_ + : Currently Assigned Tasks");
				menuViewEditDoneUndoneTasks.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					return { 0, "Success" };
					});

				menuViewDoneUndoneAssignedSubjects.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuViewDoneUndoneAssignedSubjects);
				CreateBindMessageTitle(menuViewDoneUndoneAssignedSubjects, menuViewDoneUndoneAssignedSubjects_title, "_Selected student username and number_ + : Currently Assigned subjects");
				menuViewDoneUndoneAssignedSubjects.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					selectedAssignedSubject = selectedStudent->assignedSubjects.getReference(chosenOptionIndex);
					selectedSubject = GLOBAL::subjects.getReference(
						GLOBAL::subjects.findByValue(
							to_string(selectedAssignedSubject->subject_uniqueNumber),
							SubjectList::uniqueNumber
						)
					);
					if (selectedAssignedSubject->assignedTasks.length() == 0) {
						string errorStr = "No tasks from " + to_string_specific(selectedSubject) + " are assigned to you at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuViewDoneUndoneAssignedSubjects, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					changeWindowTitle(menuViewEditDoneUndoneTasks, to_string_specific(selectedSubject) + " : Currently Assigned Tasks");
					do { generateUniMenuFromLists(menuViewEditDoneUndoneTasks, selectedAssignedSubject->assignedTasks, selectedSubject->tasks); } while (get<0>(menuViewEditDoneUndoneTasks.Select()) != -1);
					RequestWindowGroupErasure(menuViewEditDoneUndoneTasks);
					return { 0, "Success" };
					});

				menuViewEditDoneTasks.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuViewEditDoneTasks);
				CreateBindMessageTitle(menuViewEditDoneTasks, menuViewEditDoneTasks_title, "_Selected subject name and number_ + : Currently Done/Marked Tasks");
				menuViewEditDoneTasks.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					return { 0, "Success" };
					});

				menuViewDoneAssignedSubjects.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuViewDoneAssignedSubjects);
				CreateBindMessageTitle(menuViewDoneAssignedSubjects, menuViewDoneAssignedSubjects_title, "_Selected student username and number_ + : Subjects With Done/Marked Tasks");
				menuViewDoneAssignedSubjects.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					string chosenOption = menuViewDoneAssignedSubjects.Get(chosenOptionIndex).get();
					selectedAssignedSubject = selectedStudent->assignedSubjects.getReference(
						selectedStudent->assignedSubjects.findByValue(
							chosenOption.substr(chosenOption.find('D') + 2, LIMITS::uniqueNumberMaxLength),
							AssignedSubjectList::assignedSubjectVariable::subject_uniqueNumber
						)
					);
					selectedSubject = GLOBAL::subjects.getReference(
						GLOBAL::subjects.findByValue(
							to_string(selectedAssignedSubject->subject_uniqueNumber),
							SubjectList::uniqueNumber
						)
					);
					changeWindowTitle(menuViewEditDoneTasks, to_string_specific(selectedSubject) + " : Currently Done/Marked Tasks");
					do { generateUniMenuFromTaskLists_markPositive(menuViewEditDoneTasks, selectedAssignedSubject->assignedTasks, selectedSubject->tasks); } while (get<0>(menuViewEditDoneTasks.Select()) != -1);
					RequestWindowGroupErasure(menuViewEditDoneTasks);
					return { 0, "Success" };
					});

				menuViewEditUndoneTasks.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuViewEditUndoneTasks);
				CreateBindMessageTitle(menuViewEditUndoneTasks, menuViewEditUndoneTasks_title, "_Selected subject name and number_ + : Currently Undone/Unmarked Tasks");
				menuViewEditUndoneTasks.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					return { 0, "Success" };
					});

				menuViewUndoneAssignedSubjects.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuViewUndoneAssignedSubjects);
				CreateBindMessageTitle(menuViewUndoneAssignedSubjects, menuViewUndoneAssignedSubjects_title, "_Selected student username and number_ + : Subjects With Undone/Unmarked Tasks");
				menuViewUndoneAssignedSubjects.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					string chosenOption = menuViewUndoneAssignedSubjects.Get(chosenOptionIndex).get();
					selectedAssignedSubject = selectedStudent->assignedSubjects.getReference(
						selectedStudent->assignedSubjects.findByValue(
							chosenOption.substr(chosenOption.find('D') + 2, LIMITS::uniqueNumberMaxLength),
							AssignedSubjectList::assignedSubjectVariable::subject_uniqueNumber
						)
					);
					selectedSubject = GLOBAL::subjects.getReference(
						GLOBAL::subjects.findByValue(
							to_string(selectedAssignedSubject->subject_uniqueNumber),
							SubjectList::uniqueNumber
						)
					);
					changeWindowTitle(menuViewEditUndoneTasks, to_string_specific(selectedSubject) + " : Currently Undone/Unmarked Tasks");
					do { generateUniMenuFromTaskLists_markNegative(menuViewEditUndoneTasks, selectedAssignedSubject->assignedTasks, selectedSubject->tasks); } while (get<0>(menuViewEditUndoneTasks.Select()) != -1);
					RequestWindowGroupErasure(menuViewEditUndoneTasks);
					return { 0, "Success" };
					});

				menuAccountManagement.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuAccountManagement);
				CreateBindMessageTitle(menuAccountManagement, menuAccountManagement_title, "_Selected student username and number_ : Account Management");
				menuAccountManagement.Add("Change username", []()->tuple<int, string> {
					string newUsername;
					snapWindowWithTitle(menuAccountManagement, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Changing username. Enter your new username:");
					while (true) {
						newUsername = GLOBAL::inputWindow_username.Process();
						if (!newUsername[0]) return { -600, "Operation cancelled." };
						if (newUsername.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a username: " + newUsername + "!");
							snapMessage(menuAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						if (GLOBAL::teachers.findByValue(newUsername, TeacherList::username) >= 0) {
							GLOBAL::messageContents.addLine("The username provided is already taken: " + newUsername + "!");
							snapMessage(menuAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Changed username successfully! Your new username is: " + newUsername + ".");
					snapMessage(menuAccountManagement, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					strcpy_s(selectedStudent->username, 30, newUsername.c_str());
					return { 0, "Success" };
					});
				menuAccountManagement.Add("Change name", []()->tuple<int, string> {
					string newName;
					snapWindowWithTitle(menuAccountManagement, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Changing name. Enter your new name:");
					while (true) {
						newName = GLOBAL::inputWindow_username.Process();
						if (!newName[0]) return { -600, "Operation cancelled." };
						if (newName.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a name: " + newName + "!");
							snapMessage(menuAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Changed name successfully! Your new name is: " + newName + ".");
					snapMessage(menuAccountManagement, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					strcpy_s(selectedStudent->name, 30, newName.c_str());
					return { 0, "Success" };
					});
				menuAccountManagement.Add("Change password", []()->tuple<int, string> {
					string newPassword;
					snapWindowWithTitle(menuAccountManagement, GLOBAL::inputWindow_password, GLOBAL::inputsTitle, "Changing password. Enter your new password:");
					while (true) {
						newPassword = GLOBAL::inputWindow_password.Process();
						if (!newPassword[0]) return { -600, "Operation cancelled." };
						if (newPassword.length() < LIMITS::passwordMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a password: " + newPassword + "!");
							snapMessage(menuAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Changed password successfully! Your new password is: " + newPassword + ".");
					snapMessage(menuAccountManagement, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					strcpy_s(selectedStudent->password, 30, newPassword.c_str());
					return { 0, "Success" };
					});
				menuAccountManagement.Add("<-Back", universalEscPressed);

				menuOptions.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuOptions);
				CreateBindMessageTitle(menuOptions, menuOptions_title, "_Selected student username and number_");
				menuOptions.Add("View all assigned tasks/marks/subjects", []()->tuple<int, string> {
					if (selectedStudent->assignedSubjects.length() == 0) {
						string errorStr = "You have no assigned subjects at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuOptions, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					changeWindowTitle(menuViewDoneUndoneAssignedSubjects, to_string_specific(selectedStudent, true, true) + " : Currently Assigned Subjects");
					do { generateUniMenuFromLists(menuViewDoneUndoneAssignedSubjects, selectedStudent->assignedSubjects, GLOBAL::subjects); } while (get<0>(menuViewDoneUndoneAssignedSubjects.Select()) != -1);
					RequestWindowGroupErasure(menuViewDoneUndoneAssignedSubjects);
					return { 0, "Success" };
					});
				menuOptions.Add("View all done/marked tasks/marks", []()->tuple<int, string> {
					if (selectedStudent->assignedSubjects.length() == 0) {
						string errorStr = "You have no assigned subjects with done/marked tasks/marks at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuOptions, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}

					changeWindowTitle(menuViewDoneAssignedSubjects, to_string_specific(selectedStudent, true, true) + " : Subjects With Done/Marked Tasks");
					do {
						generateUniMenuFromSubjectLists_markPositive(menuViewDoneAssignedSubjects, selectedStudent->assignedSubjects, GLOBAL::subjects);
						if (menuViewDoneAssignedSubjects.GetCount() == 0) {
							string errorStr = "You have no assigned subjects with done/marked tasks/marks at the moment!";
							GLOBAL::messageContents.addLine(errorStr);
							snapMessage(menuOptions, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							return { -500, errorStr };
						}
					} while (get<0>(menuViewDoneAssignedSubjects.Select()) != -1);
					RequestWindowGroupErasure(menuViewDoneAssignedSubjects);
					return { 0, "Success" };
					});
				menuOptions.Add("View all undone/unmarked tasks/marks", []()->tuple<int, string> {
					if (selectedStudent->assignedSubjects.length() == 0) {
						string errorStr = "You have no assigned subjects with undone/unmarked tasks/marks at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuOptions, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}

					changeWindowTitle(menuViewUndoneAssignedSubjects, to_string_specific(selectedStudent, true, true) + " : Subjects With Undone/Unmarked Tasks");
					do {
						generateUniMenuFromSubjectLists_markNegative(menuViewUndoneAssignedSubjects, selectedStudent->assignedSubjects, GLOBAL::subjects);
						if (menuViewUndoneAssignedSubjects.GetCount() == 0) {
							string errorStr = "You have no subjects with undone/unmarked tasks at the moment!";
							GLOBAL::messageContents.addLine(errorStr);
							snapMessage(menuOptions, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							return { -500, errorStr };
						}
					} while (get<0>(menuViewUndoneAssignedSubjects.Select()) != -1);
					RequestWindowGroupErasure(menuViewUndoneAssignedSubjects);
					return { 0, "Success" };
					});
				menuOptions.Add("Account Management", []()->tuple<int, string> {
					do { changeWindowTitle(menuAccountManagement, to_string_specific(selectedStudent, true, true) + " : Account Management"); } while (get<0>(menuAccountManagement.Select()) != -1);
					RequestWindowGroupErasure(menuAccountManagement);
					return { 0, "Success" };
					});
				menuOptions.Add("Log off", universalEscPressed);

				GLOBAL::menuGroups.Reset();

				GLOBAL::menuGroups.AddWindow(menuOptions.GetLink_top()).AddWindow(&menuOptions).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuViewDoneUndoneAssignedSubjects.GetLink_top()).AddWindow(&menuViewDoneUndoneAssignedSubjects).NextArea();
				GLOBAL::menuGroups.AddWindow(menuViewDoneAssignedSubjects.GetLink_top()).AddWindow(&menuViewDoneAssignedSubjects).NextArea();
				GLOBAL::menuGroups.AddWindow(menuViewUndoneAssignedSubjects.GetLink_top()).AddWindow(&menuViewUndoneAssignedSubjects).NextArea();
				GLOBAL::menuGroups.AddWindow(menuAccountManagement.GetLink_top()).AddWindow(&menuAccountManagement).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuViewEditDoneUndoneTasks.GetLink_top()).AddWindow(&menuViewEditDoneUndoneTasks).NextArea();
				GLOBAL::menuGroups.AddWindow(menuViewEditDoneTasks.GetLink_top()).AddWindow(&menuViewEditDoneTasks).NextArea();
				GLOBAL::menuGroups.AddWindow(menuViewEditUndoneTasks.GetLink_top()).AddWindow(&menuViewEditUndoneTasks).NextAreaGroup();

				GLOBAL::screenArea.SetCurrentArea(0).AssignAreasInAreas(GLOBAL::menuGroups.Get());
			}

			//Main function
			void studentWorkCycle() {
				workCycleStart();
				do { changeWindowTitle(menuOptions, to_string_specific(selectedStudent, true, true)); } while (get<0>(menuOptions.Select()) != -1);
				RequestWindowGroupErasure(menuOptions);
				workCycleShutdown();
			}
		}
		namespace TEACHER {
			//Various menus and their titles sorted by depth
			//Depth 0 - menu that first appears on screen
			ASD::Message menuOptions_title;                  ASD::CallbackMenu          menuOptions;

			//Depth 1 - menus that are submenus of the menu above 
			ASD::Message menuConfigureStudents_title;        ASD::CallbackMenu          menuConfigureStudents;
			ASD::Message menuConfigureSubjects_title;        ASD::CallbackMenu          menuConfigureSubjects;
			ASD::Message menuAccountManagement_title;        ASD::CallbackMenu          menuAccountManagement;

			//Depth 2 - menus that are submenus of the menus above
			ASD::Message menuViewSubjects_title;             ASD::UniversalCallbackMenu menuViewSubjects;
			ASD::Message menuRemoveSubjects_title;           ASD::UniversalCallbackMenu menuRemoveSubjects;
			ASD::Message menuViewStudents_title;             ASD::UniversalCallbackMenu menuViewStudents;
			ASD::Message menuRemoveStudents_title;           ASD::UniversalCallbackMenu menuRemoveStudents;

			//Depth 3
			ASD::Message menuYesNo_title;                    ASD::ArrayMenu             menuYesNo;
			ASD::Message menuConfigureTasks_title;           ASD::CallbackMenu          menuConfigureTasks;
			ASD::Message menuInspectStudent_title;           ASD::CallbackMenu          menuInspectStudent;

			//Depth 4
			ASD::Message menuViewAssignedSubjects_title;     ASD::UniversalCallbackMenu menuViewAssignedSubjects;
			ASD::Message menuAssignSubject_title;            ASD::UniversalCallbackMenu menuAssignSubject;
			ASD::Message menuRemoveTasks_title;              ASD::UniversalCallbackMenu menuRemoveTasks;
			ASD::Message menuViewEditTasks_title;            ASD::UniversalCallbackMenu menuViewEditTasks;
			ASD::Message menuStudentAccountManagement_title; ASD::CallbackMenu          menuStudentAccountManagement;

			//Depth 5
			//YESNO
			ASD::Message menuInspectAssignedSubject_title;   ASD::CallbackMenu          menuInspectAssignedSubject;
			ASD::Message menuEditTask_title;                 ASD::CallbackMenu          menuEditTask;

			//Depth 6
			//YESNO
			ASD::Message menuViewAssignedTasks_title;        ASD::UniversalCallbackMenu menuViewAssignedTasks;
			ASD::Message menuAssignTask_title;               ASD::UniversalCallbackMenu menuAssignTask;

			//Depth 7
			//YESNO
			ASD::Message menuInspectAssignedTask_title;      ASD::CallbackMenu          menuInspectAssignedTask;

			//--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

			//Initialization (pre-Main) function
			void teacherWorkCycle_Init() {

				menuEditTask.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuEditTask);
				CreateBindMessageTitle(menuEditTask, menuEditTask_title, "Task Edit Menu");
				menuEditTask.Add("Pull up Statistics", []()->tuple<int, string> {
					if (GLOBAL::students.length() == 0) {
						string errorStr = "No students to pull up statistics from are registered at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuEditTask, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						RequestWindowGroupErasure(GLOBAL::message_error);
						return { -500, errorStr };
					}
					struct StudentCount {
						int overall = 0;

						int withMark = 0;
						int withNoMark = 0;

						double percentage(int overall_subset) {
							double result = (double(overall_subset) / double(overall)) * 100;
							if (result == INFINITY) result = 100;
							return result;
						}
					} studentsAssignedWithTask;
					for (int i = 0; i < GLOBAL::students.length(); i++) {
						Student* currentStudent = GLOBAL::students.getReference(i);
						int assignedSubjectOfTheTask_Index = currentStudent->assignedSubjects.findByValue(to_string(selectedSubject->uniqueNumber)
							, AssignedSubjectList::assignedSubjectVariable::subject_uniqueNumber);
						if (assignedSubjectOfTheTask_Index < 0) continue;

						AssignedTaskList* assignedSubjectOfTheTask_TaskList = &currentStudent->assignedSubjects.getReference(assignedSubjectOfTheTask_Index)->assignedTasks;
						int assignedTaskItself_Index = assignedSubjectOfTheTask_TaskList->findByValue(to_string(selectedTask->uniqueNumber)
							, AssignedTaskList::assignedTaskVariable::task_uniqueNumber);
						if (assignedTaskItself_Index < 0) continue;

						studentsAssignedWithTask.overall++;
						AssignedTask* assignedTaskItself_TaskPtr = assignedSubjectOfTheTask_TaskList->getReference(assignedTaskItself_Index);
						if (assignedTaskItself_TaskPtr->mark < 0) studentsAssignedWithTask.withNoMark++;
						else studentsAssignedWithTask.withMark++;

					}
					if (!studentsAssignedWithTask.overall) {
						string errorStr = "No students to pull up statistics from are assigned this task at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuEditTask, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						RequestWindowGroupErasure(GLOBAL::message_error);
						return { -500, errorStr };
					}

					GLOBAL::messageContents.addLine(
						to_string_specific(selectedTask)
					);
					GLOBAL::messageContents.addLine(
						"Statistics"
					);
					GLOBAL::messageContents.addLine(
						""
					);

					GLOBAL::messageContents.addLine(
						"All students: " + to_string(GLOBAL::students.length()),
						ASD::Message::left
					);
					GLOBAL::messageContents.addLine(
						""
					);
					GLOBAL::messageContents.addLine(
						"All students who are assigned this task : " + to_string(studentsAssignedWithTask.overall),
						ASD::Message::left
					);
					GLOBAL::messageContents.addLine(
						" With a mark    : " + to_string(studentsAssignedWithTask.withMark) + "(" + to_string_specific(studentsAssignedWithTask.percentage(studentsAssignedWithTask.withMark), 0) + "%)",
						ASD::Message::left
					);
					GLOBAL::messageContents.addLine(
						" Without a mark : " + to_string(studentsAssignedWithTask.withNoMark) + "(" + to_string_specific(studentsAssignedWithTask.percentage(studentsAssignedWithTask.withMark), 0) + "%)",
						ASD::Message::left
					);
					snapMessage(menuEditTask, GLOBAL::message_neutral, &GLOBAL::messageContents).Show();

					return { 0, "Success" };
					});
				menuEditTask.Add("Rename the chosen task", []()->tuple<int, string> {
					string inputResult;
					GLOBAL::messageContents.addLine("Renaming task" + to_string_specific(selectedTask) + ". Make sure to keep the new name at least " + to_string(LIMITS::usernameMinLength) + " characters long");
					snapMessage(menuEditTask, GLOBAL::message_neutral, &GLOBAL::messageContents).Show();

					snapWindowWithTitle(menuEditTask, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Enter the new name:");
					while (true) {
						inputResult = GLOBAL::inputWindow_username.Process();
						if (!inputResult[0]) return { -600, "Operation cancelled." };
						if (inputResult.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a name: " + inputResult + "!");
							snapMessage(menuEditTask, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						if (selectedSubject->tasks.findByValue(inputResult, TaskList::name) >= 0) {
							GLOBAL::messageContents.addLine("The task name provided is already taken: " + inputResult + "!");
							snapMessage(menuEditTask, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					//the constructor of char30 makes sure all unused symbols are 0 and 
					// not some negative ASCII bullshit like -51
					char30 formattedInputResult = char30(inputResult);
					strcpy_s(selectedTask->name, 30, formattedInputResult.data);
					return { 0, "Success" };
					});
				menuEditTask.Add("<-Back", universalEscPressed);

				STYLES::ArrayMenu::arrayGenerated(menuViewEditTasks);////
				CreateBindMessageTitle(menuViewEditTasks, menuViewEditTasks_title, "View/Edit Tasks");
				menuViewEditTasks.SetCancelCallback(universalEscPressed);
				menuViewEditTasks.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					selectedTask = selectedSubject->tasks.getReference(chosenOptionIndex);
					do { changeWindowTitle(menuEditTask, "Editing task: " + to_string_specific(selectedTask)); } while (get<0>(menuEditTask.Select()) != -1);
					RequestWindowGroupErasure(menuEditTask);
					return { 0, "Success" };
					});

				STYLES::ArrayMenu::regular(menuYesNo);
				CreateBindMessageTitle(menuYesNo, menuYesNo_title, "Are you sure?");
				menuYesNo.Add("Yes");
				menuYesNo.Add("No");

				STYLES::ArrayMenu::arrayGenerated(menuRemoveTasks);////
				CreateBindMessageTitle(menuRemoveTasks, menuRemoveTasks_title, "Task Batch Removal Menu");
				menuRemoveTasks.SetCancelCallback(universalEscPressed);
				menuRemoveTasks.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					string chosenOption = (string)(menuRemoveTasks.Get(chosenOptionIndex).get());
					snapWindowWithTitle(menuRemoveTasks, menuYesNo, GLOBAL::inputsTitle, "Really remove " + chosenOption + "?");
					menuYesNo.Select();
					if (menuYesNo.GetSelect() == 0) {// "Yes"
						for (int studentIndex = 0; studentIndex < GLOBAL::students.length(); studentIndex++) {
							Student* referencedStudent = GLOBAL::students.getReference(studentIndex);
							AssignedSubject* referencedAssignedSubject = referencedStudent->assignedSubjects.getReference(
								referencedStudent->assignedSubjects.findByValue(
									to_string(selectedSubject->uniqueNumber),
									AssignedSubjectList::subject_uniqueNumber
								)
							);
							int assignedTaskIndex = referencedAssignedSubject->assignedTasks.findByValue(
								chosenOption.substr(chosenOption.find('D') + 2, LIMITS::uniqueNumberMaxLength),
								AssignedTaskList::task_uniqueNumber
							);
							if (assignedTaskIndex >= 0) referencedAssignedSubject->assignedTasks.remove(assignedTaskIndex);
						}
						selectedSubject->remove(chosenOptionIndex);
						if (selectedSubject->taskCount == 0) {
							string errorStr = "No tasks left after removal!";
							GLOBAL::messageContents.addLine(errorStr);
							snapMessage(menuRemoveTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							RequestWindowGroupErasure(GLOBAL::message_error);
							return { -1, errorStr };
						}
						RequestWindowGroupErasure(menuYesNo);
						return { 0, "Success, some tasks left after removal." };
					}
					RequestWindowGroupErasure(menuYesNo);
					return { -600, "Operation cancelled." };
					});

				STYLES::ArrayMenu::regular(menuConfigureTasks);
				CreateBindMessageTitle(menuConfigureTasks, menuConfigureTasks_title, "Subject Configuration Menu");
				menuConfigureTasks.SetCancelCallback(universalEscPressed);
				menuConfigureTasks.Add("Pull up Statistics", []()->tuple<int, string> {
					if (GLOBAL::students.length() == 0) {
						string errorStr = "No students to pull up statistics from are registered at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuConfigureTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						RequestWindowGroupErasure(GLOBAL::message_error);
						return { -500, errorStr };
					}
					struct StudentCount {
						int overall = 0;

						int allTasksDone = 0;
						int someTasksDone = 0;
						int noTasksDone = 0;
						int noTasksAssigned = 0;

						double percentage(int overall_subset) {
							double result = (double(overall_subset) / double(overall)) * 100;
							if (result == INFINITY) result = 100;
							return result;
						}
					} studentsAssignedWithSubject;
					for (int i = 0; i < GLOBAL::students.length(); i++) {
						Student* currentStudent = GLOBAL::students.getReference(i);
						
						int assignedSubject_Index = currentStudent->assignedSubjects.findByValue(to_string(selectedSubject->uniqueNumber), AssignedSubjectList::assignedSubjectVariable::subject_uniqueNumber);
						if (assignedSubject_Index < 0) continue;

						AssignedSubject* assignedSubject_Itself = currentStudent->assignedSubjects.getReference(assignedSubject_Index);
						studentsAssignedWithSubject.overall++;
						int tasksOverall = assignedSubject_Itself->assignedTasks.length();
						int tasksWithMarks = 0;
						for (int m = 0; m < tasksOverall; m++) {
							AssignedTask* currentAssignedTask = assignedSubject_Itself->assignedTasks.getReference(m);
							if (currentAssignedTask->mark > 0)  tasksWithMarks++;
						}
						if (tasksOverall) {
							if (tasksWithMarks) {
								if (tasksWithMarks == tasksOverall) studentsAssignedWithSubject.allTasksDone++;
								else studentsAssignedWithSubject.someTasksDone++;
							}
							else studentsAssignedWithSubject.noTasksDone++;
						}
						else studentsAssignedWithSubject.noTasksAssigned++;
					}
					if (!studentsAssignedWithSubject.overall) {
						string errorStr = "No students to pull up statistics from are assigned this subject at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuConfigureTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						RequestWindowGroupErasure(GLOBAL::message_error);
						return { -500, errorStr };
					}

					GLOBAL::messageContents.addLine(
						to_string_specific(selectedSubject)
					);
					GLOBAL::messageContents.addLine(
						"Statistics"
					);
					GLOBAL::messageContents.addLine(
						""
					);

					GLOBAL::messageContents.addLine(
						"All students: " + to_string(GLOBAL::students.length()),
						ASD::Message::left
					);
					GLOBAL::messageContents.addLine(
						""
					);

					GLOBAL::messageContents.addLine(
						" All students who are assigned this subject : " + to_string(studentsAssignedWithSubject.overall),
						ASD::Message::left
					);
					GLOBAL::messageContents.addLine(
						" With all tasks marked  : " + to_string(studentsAssignedWithSubject.allTasksDone) + "(" + to_string_specific(studentsAssignedWithSubject.percentage(studentsAssignedWithSubject.allTasksDone), 0) + "%)",
						ASD::Message::left
					);
					GLOBAL::messageContents.addLine(
						" With some tasks marked : " + to_string(studentsAssignedWithSubject.someTasksDone) + "(" + to_string_specific(studentsAssignedWithSubject.percentage(studentsAssignedWithSubject.someTasksDone), 0) + "%)",
						ASD::Message::left
					);
					GLOBAL::messageContents.addLine(
						" With no tasks marked   : " + to_string(studentsAssignedWithSubject.noTasksDone) + "(" + to_string_specific(studentsAssignedWithSubject.percentage(studentsAssignedWithSubject.noTasksDone), 0) + "%)",
						ASD::Message::left
					);
					GLOBAL::messageContents.addLine(
						""
					);

					GLOBAL::messageContents.addLine(
						" With no tasks assigned : " + to_string(studentsAssignedWithSubject.noTasksAssigned) + "(" + to_string_specific(studentsAssignedWithSubject.percentage(studentsAssignedWithSubject.noTasksAssigned), 0) + "%)",
						ASD::Message::left
					);
					snapMessage(menuConfigureTasks, GLOBAL::message_neutral, &GLOBAL::messageContents).Show();

					return { 0, "Success" };
					});
				menuConfigureTasks.Add("View/Edit Tasks", []()->tuple<int, string> {
					if (selectedSubject->taskCount == 0) {
						string errorStr = "No tasks registered for " + (string)(selectedSubject->name) + "!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuConfigureTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					changeWindowTitle(menuViewEditTasks, to_string_specific(selectedSubject) + ": Tasks");
					do { generateUniMenuFromList(menuViewEditTasks, selectedSubject->tasks); } while (get<0>(menuViewEditTasks.Select()) != -1);
					RequestWindowGroupErasure(menuViewEditTasks);
					return { 0, "Success" };
					});
				menuConfigureTasks.Add("Add a Task", []()->tuple<int, string> {
					struct InputResults {
						string name, uniqueNumber;
						char30 name_formatted;////TODO Check if this formatting is really needed here
					}inputResults;
					snapWindowWithTitle(menuConfigureTasks, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Adding a new task. Enter the new task's title:");
					while (true) {
						inputResults.name = GLOBAL::inputWindow_username.Process();
						if (!inputResults.name[0]) return { -600, "Operation cancelled." };
						if (inputResults.name.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a task title: " + inputResults.name + "!");
							snapMessage(menuConfigureTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						inputResults.name_formatted = char30(inputResults.name);
						if (selectedSubject->tasks.findByValue(inputResults.name_formatted.data, TaskList::name) >= 0) {
							GLOBAL::messageContents.addLine("The task title provided is already taken: " + inputResults.name + "!");
							snapMessage(menuConfigureTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					snapWindowWithTitle(menuConfigureTasks, GLOBAL::inputWindow_uniqueNumber, GLOBAL::inputsTitle, "Adding a new task. Enter the new task's unique number:");
					while (true) {
						inputResults.uniqueNumber = GLOBAL::inputWindow_uniqueNumber.Process();
						if (!inputResults.uniqueNumber[0]) return { -600, "Operation cancelled." };
						if (selectedSubject->tasks.findByValue(inputResults.uniqueNumber, TaskList::uniqueNumber) >= 0) {
							GLOBAL::messageContents.addLine("The unique task number provided is already taken: " + inputResults.uniqueNumber + "!");
							snapMessage(menuConfigureTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					selectedSubject->add(inputResults.name_formatted, stoi(inputResults.uniqueNumber));
					return { 0, "Success" };
					});
				menuConfigureTasks.Add("Remove Tasks in Batch", []()->tuple<int, string> {
					if (selectedSubject->taskCount == 0) {
						string errorStr = "No tasks are registered at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuConfigureTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					GLOBAL::messageContents.addLine("Removing tasks in batch. Remove tasks with ENTER or hit ESC to exit.");
					snapMessage(menuConfigureTasks, GLOBAL::message_neutral, &GLOBAL::messageContents).Show();
					do { generateUniMenuFromList(menuRemoveTasks, selectedSubject->tasks); } while (get<0>(menuRemoveTasks.Select()) != -1);
					RequestWindowGroupErasure(menuRemoveTasks);
					return { 0, "Success" };
					});
				menuConfigureTasks.Add("Rename The Chosen Subject", []()->tuple<int, string> {
					string inputResult;
					GLOBAL::messageContents.addLine("Renaming the chosen subject. Make sure to keep the new name at least " + to_string(LIMITS::usernameMinLength) + " characters long");
					snapMessage(menuConfigureTasks, GLOBAL::message_neutral, &GLOBAL::messageContents).Show();
					snapWindowWithTitle(menuConfigureTasks, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Enter the new name:");
					while (true) {
						inputResult = GLOBAL::inputWindow_username.Process();
						if (!inputResult[0]) return { -600, "Operation cancelled." };
						if (inputResult.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a subject name: " + inputResult + "!");
							snapMessage(menuConfigureTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						if (GLOBAL::subjects.findByValue(inputResult, SubjectList::name) >= 0) {
							GLOBAL::messageContents.addLine("The subject name provided is already taken: " + inputResult + "!");
							snapMessage(menuConfigureTasks, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					//the constructor of char30 makes sure all unused symbols are 0 and 
					// not some negative ASCII bullshit like -51
					char30 formattedInputResult = char30(inputResult);
					strcpy_s(selectedSubject->name, 30, formattedInputResult.data);
					return { 0, "Success" };
					});
				menuConfigureTasks.Add("<-Back", universalEscPressed);

				STYLES::ArrayMenu::arrayGenerated(menuViewSubjects);////
				CreateBindMessageTitle(menuViewSubjects, menuViewSubjects_title, "View/Edit Existing Subjects");
				menuViewSubjects.SetCancelCallback(universalEscPressed);
				menuViewSubjects.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					selectedSubject = GLOBAL::subjects.getReference(chosenOptionIndex);
					do { changeWindowTitle(menuConfigureTasks, "Editing Subject: " + to_string_specific(selectedSubject)); } while (get<0>(menuConfigureTasks.Select()) != -1);
					RequestWindowGroupErasure(menuConfigureTasks);
					return { 0, "Success" };
					});

				STYLES::ArrayMenu::arrayGenerated(menuRemoveSubjects);////
				CreateBindMessageTitle(menuRemoveSubjects, menuRemoveSubjects_title, "Subject Batch Removal Menu");
				menuRemoveSubjects.SetCancelCallback(universalEscPressed);
				menuRemoveSubjects.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					string chosenOption = (string)(menuRemoveSubjects.Get(chosenOptionIndex).get());
					snapWindowWithTitle(menuRemoveSubjects, menuYesNo, GLOBAL::inputsTitle, "Really remove " + chosenOption + "?");
					menuYesNo.Select();
					if (menuYesNo.GetSelect() == 0) {// "Yes"
						for (int studentIndex = 0; studentIndex < GLOBAL::students.length(); studentIndex++) {
							Student* referencedStudent = GLOBAL::students.getReference(studentIndex);
							int assignedSubjectIndex = referencedStudent->assignedSubjects.findByValue(
								chosenOption.substr(chosenOption.find('D') + 2, LIMITS::uniqueNumberMaxLength),
								AssignedSubjectList::subject_uniqueNumber
							);
							if (assignedSubjectIndex >= 0) referencedStudent->assignedSubjects.remove(assignedSubjectIndex);
						}
						GLOBAL::subjects.remove(chosenOptionIndex);
						if (GLOBAL::subjects.length() == 0) {
							string errorStr = "No subjects left after removal!";
							GLOBAL::messageContents.addLine(errorStr);
							snapMessage(menuRemoveSubjects, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							RequestWindowGroupErasure(GLOBAL::message_error);
							return { -1, errorStr };
						}
						RequestWindowGroupErasure(menuYesNo);
						return { 0, "Success, some subjects left after removal." };
					}
					RequestWindowGroupErasure(menuYesNo);
					return { -600, "Operation cancelled." };
					});

				STYLES::ArrayMenu::regular(menuConfigureSubjects);
				CreateBindMessageTitle(menuConfigureSubjects, menuConfigureSubjects_title, "Subjects Configuration Menu");
				menuConfigureSubjects.SetCancelCallback(universalEscPressed);
				menuConfigureSubjects.Add("View/Edit Subjects", []()->tuple<int, string> {
					if (GLOBAL::subjects.length() == 0) {
						string errorStr = "No subjects are registered at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuConfigureSubjects, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					do { generateUniMenuFromList(menuViewSubjects, GLOBAL::subjects); } while (get<0>(menuViewSubjects.Select()) != -1);
					RequestWindowGroupErasure(menuViewSubjects);
					return { 0, "Success" };
					});
				menuConfigureSubjects.Add("Add a Subject", []()->tuple<int, string> {
					struct InputResults {
						string title, number;
					}inputResults;
					snapWindowWithTitle(menuConfigureSubjects, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Adding a new subject. Enter the new subject's name:");
					while (true) {
						inputResults.title = GLOBAL::inputWindow_username.Process();
						if (!inputResults.title[0]) return { -600, "Operation cancelled." };
						if (inputResults.title.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a subject name: " + inputResults.title + "!");
							snapMessage(menuConfigureSubjects, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						if (GLOBAL::subjects.findByValue(inputResults.title, SubjectList::name) >= 0) {
							GLOBAL::messageContents.addLine("The subject name provided is already taken: " + inputResults.title + "!");
							snapMessage(menuConfigureSubjects, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					snapWindowWithTitle(menuConfigureSubjects, GLOBAL::inputWindow_uniqueNumber, GLOBAL::inputsTitle, "Adding a new subject. Enter the new subject's unique number:");
					while (true) {
						inputResults.number = GLOBAL::inputWindow_uniqueNumber.Process();
						if (!inputResults.number[0]) return { -600, "Operation cancelled." };
						if (GLOBAL::subjects.findByValue(inputResults.number, SubjectList::uniqueNumber) >= 0) {
							GLOBAL::messageContents.addLine("The unique subject number provided is already taken: " + inputResults.number + "!");
							snapMessage(menuConfigureSubjects, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::subjects.add(Subject(inputResults.title, stoi(inputResults.number)));
					return { 0, "Success" };
					});
				menuConfigureSubjects.Add("Remove Subjects in Batch", []()->tuple<int, string> {
					if (GLOBAL::subjects.length() == 0) {
						string errorStr = "No subjects are registered at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuConfigureSubjects, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					GLOBAL::messageContents.addLine("Removing subjects in batch. Remove subjects with ENTER or hit ESC to exit.");
					snapMessage(menuConfigureSubjects, GLOBAL::message_neutral, &GLOBAL::messageContents).Show();
					do { generateUniMenuFromList(menuRemoveSubjects, GLOBAL::subjects); } while (get<0>(menuRemoveSubjects.Select()) != -1);
					RequestWindowGroupErasure(menuRemoveSubjects);
					return { 0, "Success" };
					});
				menuConfigureSubjects.Add("<-Back", universalEscPressed);

				STYLES::ArrayMenu::regular(menuAssignSubject);
				CreateBindMessageTitle(menuAssignSubject, menuAssignSubject_title, "Assign New Subject");
				menuAssignSubject.SetCancelCallback(universalEscPressed);
				menuAssignSubject.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					string selectedOption = (string)(menuAssignSubject.Get(chosenOptionIndex).get());
					snapWindowWithTitle(menuAssignSubject, menuYesNo, GLOBAL::inputsTitle, "Really assign " + selectedOption + "?");
					menuYesNo.Select();
					if (menuYesNo.GetSelect() == 0) {// "Yes"
						selectedStudent->assignedSubjects.add(AssignedSubject(stoi(selectedOption.substr(selectedOption.find('D') + 2, LIMITS::uniqueNumberMaxLength))));
						if (GLOBAL::subjects.length() == selectedStudent->assignedSubjects.length()) {
							string errorStr = "All available subjets have been assigned to this student!";
							GLOBAL::messageContents.addLine(errorStr);
							snapMessage(menuYesNo, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							RequestWindowGroupErasure(GLOBAL::message_error);
							return { -1, errorStr };
						}
						RequestWindowGroupErasure(menuYesNo);
						return { 0, "Success, some subjects left to assign after the operation." };
					}
					RequestWindowGroupErasure(menuYesNo);
					return { -600, "Operation cancelled." };
					});

				STYLES::ArrayMenu::regular(menuInspectAssignedTask);
				CreateBindMessageTitle(menuInspectAssignedTask, menuInspectAssignedTask_title, "Inspecting Assigned Task");
				menuInspectAssignedTask.SetCancelCallback(universalEscPressed);
				menuInspectAssignedTask.Add("Change the mark", []()->tuple<int, string> {
					string inputResult;
					int stoiConversionResult;
					GLOBAL::messageContents.addLine(
						"Changing the mark. Make sure to keep the new mark between the values of " +
						to_string(LIMITS::markMinValue) + " and " + to_string(LIMITS::markMaxValue) + "."
					);
					snapMessage(menuInspectAssignedTask, GLOBAL::message_neutral, &GLOBAL::messageContents).Show();
					snapWindowWithTitle(menuInspectAssignedTask, GLOBAL::inputWindow_uniqueNumber, GLOBAL::inputsTitle, "Enter the new mark:");
					while (true) {
						inputResult = GLOBAL::inputWindow_uniqueNumber.Process();
						if (!inputResult[0]) return { -600, "Operation cancelled." };
						stoiConversionResult = stoi(inputResult);
						if (stoiConversionResult > LIMITS::markMaxValue || stoiConversionResult < LIMITS::markMinValue) {
							GLOBAL::messageContents.addLine(
								"Provided value exceeds the allowed minimal or maximal mark values: " + inputResult +
								" is either less than " + to_string(LIMITS::markMinValue) + " or larger than " +
								to_string(LIMITS::markMaxValue) + "."
							);
							snapMessage(menuInspectAssignedTask, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					selectedAssignedTask->mark = stoiConversionResult;
					return { 0, "Success" };
					});
				menuInspectAssignedTask.Add("Unassign this task", []()->tuple<int, string> {
					snapWindowWithTitle(menuInspectAssignedTask, menuYesNo, GLOBAL::inputsTitle, "Really unassign " + to_string_specific(selectedAssignedTask, selectedSubject->tasks) + "?");
					menuYesNo.Select();
					if (menuYesNo.GetSelect() == 0) {// "Yes"
						selectedAssignedSubject->assignedTasks.remove(
							selectedAssignedSubject->assignedTasks.findByValue(
								to_string(selectedAssignedTask->task_uniqueNumber),
								AssignedTaskList::task_uniqueNumber
							)
						);
						RequestWindowGroupErasure(menuYesNo);
						return { -1, "Success" };
					}
					RequestWindowGroupErasure(menuYesNo);
					return { -600, "Operation cancelled." };
					});
				menuInspectAssignedTask.Add("<-Back", universalEscPressed);

				STYLES::ArrayMenu::regular(menuViewAssignedTasks);
				CreateBindMessageTitle(menuViewAssignedTasks, menuViewAssignedTasks_title, "View/Edit Assigned Tasks");
				menuViewAssignedTasks.SetCancelCallback(universalEscPressed);
				menuViewAssignedTasks.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					selectedAssignedTask = selectedAssignedSubject->assignedTasks.getReference(chosenOptionIndex);
					selectedTask = selectedSubject->tasks.getReference(
						selectedSubject->tasks.findByValue(
							to_string(selectedAssignedTask->task_uniqueNumber),
							TaskList::uniqueNumber
						)
					);
					do { changeWindowTitle(menuInspectAssignedTask, "Editing Assigned Task: " + to_string_specific(selectedTask)); }
					while (get<0>(menuInspectAssignedTask.Select()) != -1);
					RequestWindowGroupErasure(menuInspectAssignedTask);
					return { 0, "Success" };
					});

				STYLES::ArrayMenu::regular(menuAssignTask);
				CreateBindMessageTitle(menuAssignTask, menuAssignTask_title, "Task Assignation Menu");
				menuAssignTask.SetCancelCallback(universalEscPressed);
				menuAssignTask.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					string selectedOption = (string)(menuAssignTask.Get(chosenOptionIndex).get());
					snapWindowWithTitle(menuAssignTask, menuYesNo, GLOBAL::inputsTitle, "Really assign " + selectedOption + "?");
					menuYesNo.Select();
					if (menuYesNo.GetSelect() == 0) {// "Yes"
						selectedAssignedSubject->assignedTasks.add(
							AssignedTask(stoi(selectedOption.substr(selectedOption.find('D') + 2, LIMITS::uniqueNumberMaxLength)))
						);
						if (selectedSubject->tasks.length() == selectedAssignedSubject->assignedTasks.length()) {
							string errorStr = "All tasks available for this subject have been assigned to this student!";
							GLOBAL::messageContents.addLine(errorStr);
							snapMessage(menuAssignTask, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							RequestWindowGroupErasure(GLOBAL::message_error);
							return { -1, errorStr };
						}
						RequestWindowGroupErasure(menuYesNo);
						return { 0, "Success, some tasks left to assign after the operation." };
					}
					RequestWindowGroupErasure(menuYesNo);
					return { -600, "Operation cancelled." };
					});

				STYLES::ArrayMenu::regular(menuInspectAssignedSubject);
				CreateBindMessageTitle(menuInspectAssignedSubject, menuInspectAssignedSubject_title, "Inspecting Assigned Subject");
				menuInspectAssignedSubject.SetCancelCallback(universalEscPressed);
				menuInspectAssignedSubject.Add("View/edit assigned tasks/marks", []()->tuple<int, string> {
					if (selectedAssignedSubject->assignedTasks.length() == 0) {
						string errorStr = "No tasks from this subject are assigned to this student at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuInspectAssignedSubject, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					changeWindowTitle(menuViewAssignedTasks, "Viewing/Editing Assigned Tasks For " + to_string_specific(selectedSubject));
					do { generateUniMenuFromLists(menuViewAssignedTasks, selectedAssignedSubject->assignedTasks, selectedSubject->tasks); } while (get<0>(menuViewAssignedTasks.Select()) != -1);
					RequestWindowGroupErasure(menuViewAssignedTasks);
					return { 0, "Success" };
					});
				menuInspectAssignedSubject.Add("Assign a task", []()->tuple<int, string> {
					if (selectedSubject->tasks.length() == 0) {
						string errorStr = "No tasks are registered for this subject at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuInspectAssignedSubject, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					if (selectedSubject->tasks.length() == selectedAssignedSubject->assignedTasks.length()) {
						string errorStr = "All tasks available for this subject have already been assigned to this student!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuInspectAssignedSubject, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					changeWindowTitle(menuAssignTask, "Assigning a task from subject: " + to_string_specific(selectedSubject) + " to student: " + to_string_specific(selectedStudent));
					do { generateUniMenuForAssignment(menuAssignTask, selectedAssignedSubject->assignedTasks, selectedSubject->tasks); } while (get<0>(menuAssignTask.Select()) != -1);
					RequestWindowGroupErasure(menuAssignTask);
					return { 0, "Success" };
					});
				menuInspectAssignedSubject.Add("Unassign this subject", []()->tuple<int, string> {
					snapWindowWithTitle(menuInspectAssignedSubject, menuYesNo, GLOBAL::inputsTitle, "Really unassign " + to_string_specific(selectedAssignedSubject, GLOBAL::subjects) + "?");
					menuYesNo.Select();
					if (menuYesNo.GetSelect() == 0) {// "Yes"
						selectedStudent->assignedSubjects.remove(
							selectedStudent->assignedSubjects.findByValue(
								to_string(selectedAssignedSubject->subject_uniqueNumber),
								AssignedSubjectList::subject_uniqueNumber
							)
						);
						RequestWindowGroupErasure(menuYesNo);
						return { -1, "Success" };
					}
					RequestWindowGroupErasure(menuYesNo);
					return { -600, "Operation cancelled." };
					});
				menuInspectAssignedSubject.Add("<-Back", universalEscPressed);

				STYLES::ArrayMenu::regular(menuViewAssignedSubjects);
				CreateBindMessageTitle(menuViewAssignedSubjects, menuViewAssignedSubjects_title, "View/Edit Assigned Subjects");
				menuViewAssignedSubjects.SetCancelCallback(universalEscPressed);
				menuViewAssignedSubjects.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					selectedAssignedSubject = selectedStudent->assignedSubjects.getReference(chosenOptionIndex);
					selectedSubject = GLOBAL::subjects.getReference(
						GLOBAL::subjects.findByValue(
							to_string(selectedAssignedSubject->subject_uniqueNumber),
							SubjectList::uniqueNumber
						)
					);
					do { changeWindowTitle(menuInspectAssignedSubject, "Edit Assigned Subject: " + to_string_specific(selectedSubject)); } while (get<0>(menuInspectAssignedSubject.Select()) != -1);
					RequestWindowGroupErasure(menuInspectAssignedSubject);
					return { 0, "Success" };
					});

				STYLES::ArrayMenu::regular(menuStudentAccountManagement);
				CreateBindMessageTitle(menuStudentAccountManagement, menuStudentAccountManagement_title, "Student Account Management");
				menuStudentAccountManagement.SetCancelCallback(universalEscPressed);
				menuStudentAccountManagement.Add("Change password", []()->tuple<int, string> {
					string newPassword;
					snapWindowWithTitle(menuStudentAccountManagement, GLOBAL::inputWindow_password, GLOBAL::inputsTitle, "Changing password. Enter " + to_string_specific(selectedStudent, false, true) + "'s new password:");
					while (true) {
						newPassword = GLOBAL::inputWindow_password.Process();
						if (!newPassword[0]) return { -600, "Operation cancelled." };
						if (newPassword.length() < LIMITS::passwordMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a password: " + newPassword + "!");
							snapMessage(menuStudentAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Changed password successfully!" + to_string_specific(selectedStudent, false, true) + " new password is: " + newPassword + ".");
					snapMessage(menuStudentAccountManagement, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					strcpy_s(selectedStudent->password, 30, newPassword.c_str());
					return { 0, "Success" };
					});
				menuStudentAccountManagement.Add("Change username", []()->tuple<int, string> {
					string newUsername;
					snapWindowWithTitle(menuStudentAccountManagement, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Changing username. Enter " + to_string_specific(selectedStudent, false, true) + "'s new username:");
					while (true) {
						newUsername = GLOBAL::inputWindow_username.Process();
						if (!newUsername[0]) return { -600, "Operation cancelled." };
						if (newUsername.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a username: " + newUsername + "!");
							snapMessage(menuStudentAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						if (GLOBAL::teachers.findByValue(newUsername, TeacherList::username) >= 0) {
							GLOBAL::messageContents.addLine("The username provided is already taken: " + newUsername + "!");
							snapMessage(menuStudentAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Changed username successfully!" + to_string_specific(selectedStudent, false, true) + " new username is: " + newUsername + ".");
					snapMessage(menuStudentAccountManagement, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					strcpy_s(selectedStudent->username, 30, newUsername.c_str());
					return { 0, "Success" };
					});
				menuStudentAccountManagement.Add("Change name", []()->tuple<int, string> {
					string newName;
					snapWindowWithTitle(menuStudentAccountManagement, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Changing name. Enter " + to_string_specific(selectedStudent, false, true) + "'s new name:");
					while (true) {
						newName = GLOBAL::inputWindow_username.Process();
						if (!newName[0]) return { -600, "Operation cancelled." };
						if (newName.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a name: " + newName + "!");
							snapMessage(menuStudentAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Changed name successfully!" + to_string_specific(selectedStudent, false, true) + " new name is: " + newName + ".");
					snapMessage(menuStudentAccountManagement, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					strcpy_s(selectedStudent->name, 30, newName.c_str());
					return { 0, "Success" };
					});
				menuStudentAccountManagement.Add("Delete account", []()->tuple<int, string> {
					snapWindowWithTitle(menuStudentAccountManagement, menuYesNo, GLOBAL::inputsTitle, "Deleting" + to_string_specific(selectedStudent, false, true) + "'s account. Are you sure?");
					menuYesNo.Select();
					if (menuYesNo.GetSelect() == 0) {// "Yes"
						GLOBAL::students.remove(GLOBAL::students.findByValue(to_string(selectedStudent->uniqueNumber), StudentList::StudentVariable::uniqueNumber));
						selectedStudent = 0;
						return { -3, "Student account deleted, goto menuViewStudents.Select()" };
					}
					RequestWindowGroupErasure(menuYesNo);
					return { -600, "Operation cancelled." };
					});

				STYLES::ArrayMenu::regular(menuInspectStudent);
				CreateBindMessageTitle(menuInspectStudent, menuInspectStudent_title, "Student Configuration Menu");
				menuInspectStudent.SetCancelCallback(universalEscPressed);
				menuInspectStudent.Add("View/edit assigned subjects", []()->tuple<int, string> {
					if (selectedStudent->assignedSubjects.length() == 0) {
						string errorStr = "No subjects are assigned to this student at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuInspectStudent, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					do { generateUniMenuFromLists(menuViewAssignedSubjects, selectedStudent->assignedSubjects, GLOBAL::subjects); } while (get<0>(menuViewAssignedSubjects.Select()) != -1);
					RequestWindowGroupErasure(menuViewAssignedSubjects);
					return { 0, "Success" };
					});
				menuInspectStudent.Add("Assign a subject", []()->tuple<int, string> {
					if (GLOBAL::subjects.length() == 0) {
						string errorStr = "No subjects are registered at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuInspectStudent, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					if (GLOBAL::subjects.length() == selectedStudent->assignedSubjects.length()) {
						string errorStr = "All available subjects have already been assigned to this student!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuInspectStudent, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					do { generateUniMenuForAssignment(menuAssignSubject, selectedStudent->assignedSubjects, GLOBAL::subjects); } while (get<0>(menuAssignSubject.Select()) != -1);
					RequestWindowGroupErasure(menuAssignSubject);
					return { 0, "Success" };
					});
				menuInspectStudent.Add("Account management", []()->tuple<int, string> {
					while (true) {
						changeWindowTitle(menuInspectStudent, to_string_specific(selectedStudent) + ": Student Account Management");
						tuple<int, string> selectResult = menuStudentAccountManagement.Select();
						if (get<0>(selectResult) == -1) break;
						else if (get<0>(selectResult) == -3) {
							RequestWindowGroupErasure(menuStudentAccountManagement);
							return selectResult;
						}
					}
					RequestWindowGroupErasure(menuStudentAccountManagement);
					return { 0, "Success" };
					});
				menuInspectStudent.Add("<-Back", universalEscPressed);

				STYLES::ArrayMenu::arrayGenerated(menuViewStudents);////
				CreateBindMessageTitle(menuViewStudents, menuViewStudents_title, "View/Edit Students");
				menuViewStudents.SetCancelCallback(universalEscPressed);
				menuViewStudents.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					selectedStudent = GLOBAL::students.getReference(chosenOptionIndex);
					while (true) {
						changeWindowTitle(menuInspectStudent, "Edit student: " + to_string_specific(selectedStudent));
						int selectResult = get<0>(menuInspectStudent.Select());
						if (selectResult == -1 || selectResult == -3) break;
					}
					RequestWindowGroupErasure(menuInspectStudent);
					return { 0, "Success" };
					});

				STYLES::ArrayMenu::arrayGenerated(menuRemoveStudents);////
				CreateBindMessageTitle(menuRemoveStudents, menuRemoveStudents_title, "Student Batch Removal Menu");
				menuRemoveStudents.SetCancelCallback(universalEscPressed);
				menuRemoveStudents.SetUniversalCallback([](int chosenOptionIndex)->tuple<int, string> {
					snapWindowWithTitle(menuRemoveStudents, menuYesNo, GLOBAL::inputsTitle, "Really remove " + (string)(menuRemoveStudents.Get(chosenOptionIndex).get()) + "?");
					menuYesNo.Select();
					if (menuYesNo.GetSelect() == 0) {// "Yes"
						GLOBAL::students.remove(chosenOptionIndex);
						if (GLOBAL::students.length() == 0) {
							string errorStr = "No students left after removal!";
							GLOBAL::messageContents.addLine(errorStr);
							snapMessage(menuRemoveStudents, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							RequestWindowGroupErasure(GLOBAL::message_error);
							return { -1, errorStr };
						}
						RequestWindowGroupErasure(menuYesNo);
						return { 0, "Success, some students left after removal." };
					}
					RequestWindowGroupErasure(menuYesNo);
					return { -600, "Operation cancelled." };
					});

				STYLES::ArrayMenu::regular(menuConfigureStudents);
				CreateBindMessageTitle(menuConfigureStudents, menuConfigureStudents_title, "Students Configuration Menu");
				menuConfigureStudents.SetCancelCallback(universalEscPressed);
				menuConfigureStudents.Add("View/edit students", []()->tuple<int, string> {
					if (GLOBAL::students.length() == 0) {
						string errorStr = "No students are registered at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuConfigureStudents, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					do { generateUniMenuFromList(menuViewStudents, GLOBAL::students); } while (get<0>(menuViewStudents.Select()) != -1);
					RequestWindowGroupErasure(menuViewStudents);
					return { 0, "Success" };
					});
				menuConfigureStudents.Add("Add a student", []()->tuple<int, string> {
					struct InputResults {
						string name, number, username, password;
					}inputResults;
					snapWindowWithTitle(menuConfigureStudents, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Adding a new student. Enter the new student's name:");
					while (true) {
						inputResults.name = GLOBAL::inputWindow_username.Process();
						if (!inputResults.name[0]) return { -600, "Operation cancelled." };
						if (inputResults.name.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a student's name: " + inputResults.name + "!");
							snapMessage(menuConfigureStudents, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					snapWindowWithTitle(menuConfigureStudents, GLOBAL::inputWindow_uniqueNumber, GLOBAL::inputsTitle, "Adding a new student. Enter the new student's unique number:");
					while (true) {
						inputResults.number = GLOBAL::inputWindow_uniqueNumber.Process();
						if (!inputResults.number[0]) return { -600, "Operation cancelled." };
						if (GLOBAL::students.findByValue(inputResults.number, StudentList::uniqueNumber) >= 0) {
							GLOBAL::messageContents.addLine("The unique student number provided is already taken: " + inputResults.number + "!");
							snapMessage(menuConfigureStudents, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					snapWindowWithTitle(menuConfigureStudents, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Adding a new student. Enter the new student's username:");
					while (true) {
						inputResults.username = GLOBAL::inputWindow_username.Process();
						if (!inputResults.username[0]) return { -600, "Operation cancelled." };
						if (GLOBAL::students.findByValue(inputResults.username, StudentList::username) >= 0) {
							GLOBAL::messageContents.addLine("The student username provided is already taken: " + inputResults.username + "!");
							snapMessage(menuConfigureStudents, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					snapWindowWithTitle(menuConfigureStudents, GLOBAL::inputWindow_password, GLOBAL::inputsTitle, "Adding a new student. Enter the new student's password:");
					inputResults.password = GLOBAL::inputWindow_password.Process();
					if (!inputResults.password[0]) return { -600, "Operation cancelled." };

					GLOBAL::students.add(Student(inputResults.name, stoi(inputResults.number), inputResults.username, inputResults.password));
					return { 0, "Success" };
					});
				menuConfigureStudents.Add("Remove Students in Batch", []()->tuple<int, string> {
					if (GLOBAL::students.length() == 0) {
						string errorStr = "No students are registered at the moment!";
						GLOBAL::messageContents.addLine(errorStr);
						snapMessage(menuConfigureStudents, GLOBAL::message_error, &GLOBAL::messageContents).Show();
						return { -500, errorStr };
					}
					GLOBAL::messageContents.addLine("Removing students in batch. Remove students with ENTER or hit ESC to exit.");
					snapMessage(menuConfigureStudents, GLOBAL::message_neutral, &GLOBAL::messageContents).Show();
					do { generateUniMenuFromList(menuRemoveStudents, GLOBAL::students); } while (get<0>(menuRemoveStudents.Select()) != -1);
					RequestWindowGroupErasure(menuRemoveStudents);
					return { 0, "Success" };
					});
				menuConfigureStudents.Add("<-Back", universalEscPressed);

				menuAccountManagement.SetCancelCallback(universalEscPressed);
				STYLES::ArrayMenu::regular(menuAccountManagement);
				CreateBindMessageTitle(menuAccountManagement, menuAccountManagement_title, "Account Management");
				menuAccountManagement.Add("Change username", []()->tuple<int, string> {
					string newUsername;
					snapWindowWithTitle(menuAccountManagement, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Changing username. Enter your new username:");
					while (true) {
						newUsername = GLOBAL::inputWindow_username.Process();
						if (!newUsername[0]) return { -600, "Operation cancelled." };
						if (newUsername.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a username: " + newUsername + "!");
							snapMessage(menuAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						if (GLOBAL::teachers.findByValue(newUsername, TeacherList::username) >= 0) {
							GLOBAL::messageContents.addLine("The teacher username provided is already taken: " + newUsername + "!");
							snapMessage(menuAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Changed username successfully! Your new username is: " + newUsername + ".");
					snapMessage(menuAccountManagement, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					strcpy_s(selectedTeacher->username, 30, newUsername.c_str());
					return { 0, "Success" };
					});
				menuAccountManagement.Add("Change password", []()->tuple<int, string> {
					string newPassword;
					snapWindowWithTitle(menuAccountManagement, GLOBAL::inputWindow_password, GLOBAL::inputsTitle, "Changing password. Enter your new password:");
					while (true) {
						newPassword = GLOBAL::inputWindow_password.Process();
						if (!newPassword[0]) return { -600, "Operation cancelled." };
						if (newPassword.length() < LIMITS::passwordMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for a password: " + newPassword + "!");
							snapMessage(menuAccountManagement, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Changed password successfully! Your new password is: " + newPassword + ".");
					snapMessage(menuAccountManagement, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					strcpy_s(selectedTeacher->password, 30, newPassword.c_str());
					return { 0, "Success" };
					});
				menuAccountManagement.Add("Delete account", []()->tuple<int, string> {
					snapWindowWithTitle(menuAccountManagement, menuYesNo, GLOBAL::inputsTitle, "Deleting account. Are you sure?");
					menuYesNo.Select();
					if (menuYesNo.GetSelect() == 0) {// "Yes"
						GLOBAL::teachers.remove(GLOBAL::teachers.findByValue(to_string(selectedTeacher->uniqueNumber), TeacherList::StudentVariable::uniqueNumber));
						selectedTeacher = 0;
						return { -3, "Account deleted, goto auth()" };
					}
					RequestWindowGroupErasure(menuYesNo);
					return { -600, "Operation cancelled." };
					});
				menuAccountManagement.Add("<-Back", universalEscPressed);

				STYLES::ArrayMenu::regular(menuOptions);
				CreateBindMessageTitle(menuOptions, menuOptions_title, "Teacher Menu");
				menuOptions.SetCancelCallback(universalEscPressed);
				menuOptions.Add("Configure subjects", []()->tuple<int, string> {
					while (get<0>(menuConfigureSubjects.Select()) != -1);
					RequestWindowGroupErasure(menuConfigureSubjects);
					return { 0, "Success" };
					});
				menuOptions.Add("Configure students", []()->tuple<int, string> {
					while (get<0>(menuConfigureStudents.Select()) != -1);
					RequestWindowGroupErasure(menuConfigureStudents);
					return { 0, "Success" };
					});
				menuOptions.Add("Account management", []()->tuple<int, string> {
					while (true) {
						tuple<int, string> selectResult = menuAccountManagement.Select();
						if (get<0>(selectResult) == -1) break;
						else if (get<0>(selectResult) == -3) {
							return selectResult;
						}
					}
					RequestWindowGroupErasure(menuAccountManagement);
					return { 0, "Success" };
					});
				menuOptions.Add("<-Log off and exit", universalEscPressed);

				GLOBAL::menuGroups.Reset();

				GLOBAL::menuGroups.AddWindow(menuOptions.GetLink_top()).AddWindow(&menuOptions).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuConfigureSubjects.GetLink_top()).AddWindow(&menuConfigureSubjects).NextArea();
				GLOBAL::menuGroups.AddWindow(menuConfigureStudents.GetLink_top()).AddWindow(&menuConfigureStudents).NextArea();
				GLOBAL::menuGroups.AddWindow(menuAccountManagement.GetLink_top()).AddWindow(&menuAccountManagement).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuViewSubjects.GetLink_top()).AddWindow(&menuViewSubjects).NextArea();
				GLOBAL::menuGroups.AddWindow(menuRemoveSubjects.GetLink_top()).AddWindow(&menuRemoveSubjects).NextArea();
				GLOBAL::menuGroups.AddWindow(menuViewStudents.GetLink_top()).AddWindow(&menuViewStudents).NextArea();
				GLOBAL::menuGroups.AddWindow(menuRemoveStudents.GetLink_top()).AddWindow(&menuRemoveStudents).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuYesNo.GetLink_top()).AddWindow(&menuYesNo).NextArea();
				GLOBAL::menuGroups.AddWindow(menuConfigureTasks.GetLink_top()).AddWindow(&menuConfigureTasks).NextArea();
				GLOBAL::menuGroups.AddWindow(menuInspectStudent.GetLink_top()).AddWindow(&menuInspectStudent).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuViewAssignedSubjects.GetLink_top()).AddWindow(&menuViewAssignedSubjects).NextArea();
				GLOBAL::menuGroups.AddWindow(menuAssignSubject.GetLink_top()).AddWindow(&menuAssignSubject).NextArea();
				GLOBAL::menuGroups.AddWindow(menuRemoveTasks.GetLink_top()).AddWindow(&menuRemoveTasks).NextArea();
				GLOBAL::menuGroups.AddWindow(menuViewEditTasks.GetLink_top()).AddWindow(&menuViewEditTasks).NextArea();
				GLOBAL::menuGroups.AddWindow(menuStudentAccountManagement.GetLink_top()).AddWindow(&menuStudentAccountManagement).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuYesNo.GetLink_top()).AddWindow(&menuYesNo).NextArea();
				GLOBAL::menuGroups.AddWindow(menuInspectAssignedSubject.GetLink_top()).AddWindow(&menuInspectAssignedSubject).NextArea();
				GLOBAL::menuGroups.AddWindow(menuEditTask.GetLink_top()).AddWindow(&menuEditTask).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuYesNo.GetLink_top()).AddWindow(&menuYesNo).NextArea();
				GLOBAL::menuGroups.AddWindow(menuViewAssignedTasks.GetLink_top()).AddWindow(&menuViewAssignedTasks).NextArea();
				GLOBAL::menuGroups.AddWindow(menuAssignTask.GetLink_top()).AddWindow(&menuAssignTask).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuYesNo.GetLink_top()).AddWindow(&menuYesNo).NextArea();
				GLOBAL::menuGroups.AddWindow(menuInspectAssignedTask.GetLink_top()).AddWindow(&menuInspectAssignedTask).NextAreaGroup();

				GLOBAL::screenArea.SetCurrentArea(0).AssignAreasInAreas(GLOBAL::menuGroups.Get());
			}

			//Main function
			void teacherWorkCycle() {
				workCycleStart();
				while (true) {
					int selectResult = get<0>(menuOptions.Select());
					if (selectResult == -1 || selectResult == -3) break;
				}
				RequestWindowGroupErasure(menuOptions);
				workCycleShutdown();
			}
		}
		namespace AUTH {
			//Various menus and their titles sorted by depth
			//Depth 0 - menu that first appears on screen
			ASD::Message menuUserType_title; ASD::CallbackMenu menuUserType;

			//Depth 1 - menus that are submenus of the menu above
			ASD::Message menuM_title;        ASD::CallbackMenu menuM;

			//--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

			//Initialization (pre-Main) function
			void authWorkCycle_Init() {

				STYLES::ArrayMenu::regular(menuUserType);
				CreateBindMessageTitle(menuUserType, menuUserType_title, "Who are you?");
				menuUserType.SetCancelCallback(universalEscPressed);
				menuUserType.Add("I'm a student", []()->tuple<int, string> {
					struct InputResults {
						string username, password;
						int usernameSearchResult;
					}inputResults;
					snapWindowWithTitle(menuUserType, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Logging in as a student. Enter your username:");
					while (true) {
						inputResults.username = GLOBAL::inputWindow_username.Process();
						if (!inputResults.username[0]) return { -600, "Operation cancelled." };
						if (inputResults.username.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for username: " + inputResults.username + "!");
							snapMessage(menuUserType, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						inputResults.usernameSearchResult = GLOBAL::students.findByValue(inputResults.username, StudentList::username);
						if (inputResults.usernameSearchResult < 0) {
							GLOBAL::messageContents.addLine("No student accounts with such username found: " + inputResults.username + "!");
							snapMessage(menuUserType, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					snapWindowWithTitle(menuUserType, GLOBAL::inputWindow_password, GLOBAL::inputsTitle, "Logging in as a student. Enter your password:");
					while (true) {
						inputResults.password = GLOBAL::inputWindow_password.Process();
						if (!inputResults.password[0]) return { -600, "Operation cancelled." };
						if (GLOBAL::students.getReference(inputResults.usernameSearchResult)->password != inputResults.password) {
							GLOBAL::messageContents.addLine("Wrong password! Username: " + inputResults.username + ", Password: " + inputResults.password + ".");
							snapMessage(menuUserType, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Logged in successfully! Username: " + inputResults.username + ", Password: " + inputResults.password + ".");
					snapMessage(menuUserType, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					selectedStudent = GLOBAL::students.getReference(inputResults.usernameSearchResult);
					WORKCYCLE::STUDENT::studentWorkCycle();
					return { -1, "Success" };
					});
				menuUserType.Add("I'm a teacher", []()->tuple<int, string> {
					struct InputResults {
						string username, password;
						int usernameSearchResult;
					}inputResults;
					snapWindowWithTitle(menuUserType, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Logging in as a teacher. Enter your username:");
					while (true) {
						inputResults.username = GLOBAL::inputWindow_username.Process();
						if (!inputResults.username[0]) return { -600, "Operation cancelled." };
						if (inputResults.username.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for username: " + inputResults.username + "!");
							snapMessage(menuUserType, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						inputResults.usernameSearchResult = GLOBAL::teachers.findByValue(inputResults.username, TeacherList::username);
						if (inputResults.usernameSearchResult < 0) {
							GLOBAL::messageContents.addLine("No teacher accounts with such username found: " + inputResults.username + "!");
							snapMessage(menuUserType, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					snapWindowWithTitle(menuUserType, GLOBAL::inputWindow_password, GLOBAL::inputsTitle, "Logging in as a teacher. Enter your password:");
					while (true) {
						inputResults.password = GLOBAL::inputWindow_password.Process();
						if (!inputResults.password[0]) return { -600, "Operation cancelled." };
						if (GLOBAL::teachers.getReference(inputResults.usernameSearchResult)->password != inputResults.password) {
							GLOBAL::messageContents.addLine("Wrong password! Username: " + inputResults.username + ", Password: " + inputResults.password + ".");
							snapMessage(menuUserType, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Logged in successfully! Username: " + inputResults.username + ", Password: " + inputResults.password + ".");
					snapMessage(menuUserType, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					selectedTeacher = GLOBAL::teachers.getReference(inputResults.usernameSearchResult);
					WORKCYCLE::TEACHER::teacherWorkCycle();
					return { -1, "Success" };
					});

				STYLES::ArrayMenu::regular(menuM);
				CreateBindMessageTitle(menuM, menuM_title, "Authentification menu");
				menuM.SetCancelCallback(universalEscPressed);
				menuM.Add("Sign up", []()->tuple<int, string> {
					struct InputResults {
						string username, password, uniqueNumber;
					}inputResults;
					snapWindowWithTitle(menuM, GLOBAL::inputWindow_username, GLOBAL::inputsTitle, "Signing up. Enter your username:");
					while (true) {
						inputResults.username = GLOBAL::inputWindow_username.Process();
						if (!inputResults.username[0]) return { -600, "Operation cancelled." };
						if (inputResults.username.length() < LIMITS::usernameMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for username: " + inputResults.username + "!");
							snapMessage(menuM, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						if (GLOBAL::teachers.findByValue(inputResults.username, TeacherList::username) >= 0) {
							GLOBAL::messageContents.addLine("The username provided is already taken: " + inputResults.username + "!");
							snapMessage(menuM, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					snapWindowWithTitle(menuM, GLOBAL::inputWindow_uniqueNumber, GLOBAL::inputsTitle, "Signing up. Enter your desired unique number:");
					while (true) {
						inputResults.uniqueNumber = GLOBAL::inputWindow_uniqueNumber.Process();
						if (!inputResults.uniqueNumber[0]) return { -600, "Operation cancelled." };
						if (GLOBAL::teachers.findByValue(inputResults.uniqueNumber, TeacherList::uniqueNumber) >= 0) {
							GLOBAL::messageContents.addLine("The chosen unique number is already taken: " + inputResults.uniqueNumber + "!");
							snapMessage(menuM, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					snapWindowWithTitle(menuM, GLOBAL::inputWindow_password, GLOBAL::inputsTitle, "Signing up. Enter your password:");
					while (true) {
						inputResults.password = GLOBAL::inputWindow_password.Process();
						if (!inputResults.password[0]) return { -600, "Operation cancelled." };
						if (inputResults.password.length() < LIMITS::passwordMinLength) {
							GLOBAL::messageContents.addLine("Insufficient length for password: " + inputResults.password + "!");
							snapMessage(menuM, GLOBAL::message_error, &GLOBAL::messageContents).Show();
							continue;
						}
						break;
					}
					GLOBAL::messageContents.addLine("Signed up successfully! Username: " + inputResults.username + ", Password: " + inputResults.password + ".");
					snapMessage(menuM, GLOBAL::message_success, &GLOBAL::messageContents).Show();
					GLOBAL::teachers.add(Teacher(stoi(inputResults.uniqueNumber), inputResults.username, inputResults.password));
					return { 0, "Success" };
					});
				menuM.Add("Log in", []()->tuple<int, string> {
					while (get<0>(menuUserType.Select()) != -1);
					RequestWindowGroupErasure(menuUserType);
					return { 0, "Success" };
					});
				menuM.Add("End the program", universalEscPressed);

				GLOBAL::menuGroups.Reset();

				GLOBAL::menuGroups.AddWindow(menuM.GetLink_top()).AddWindow(&menuM).NextAreaGroup();

				GLOBAL::menuGroups.AddWindow(menuUserType.GetLink_top()).AddWindow(&menuUserType);

				GLOBAL::screenArea.SetCurrentArea(0).AssignAreasInAreas(GLOBAL::menuGroups.Get());
			}

			//Main function
			void authWorkCycle() {
				workCycleStart();
				while (get<0>(menuM.Select()) != -1);
				RequestWindowGroupErasure(menuM);
			}
		}
	}

	namespace CORE {
		void init() {
			GLOBAL::teachers.fromFile("__DATA__/teachers.txt");
			GLOBAL::subjects.fromFile("__DATA__/subjects.txt");
			GLOBAL::students.fromFile("__DATA__/students.txt");

			GLOBAL::screenArea.SetDefaultWindowSizeAdjustments(List<dimensions>().add({ 1, 1, 1, 0 }).add({ 1, 1, 1, 1 }));

			GLOBAL::inputWindow_uniqueNumber.resultChecker = [](char*& inputResult)->int {
				if (!inputResult[0] || (inputResult[0] == '-' && strlen(inputResult) == 1)) {
					GLOBAL::messageContents.addLine("Input is empty!");
					//Beware, no checks for if 'currentSnappedWindow' is NULL can result in error
					//But in this project this particular function will only ever be used AFTER that pointer is initialized
					snapMessage(*GLOBAL::currentSnappedWindow, GLOBAL::message_error, &GLOBAL::messageContents).Show();
					return false;
				}
				else return true;
				};

			STYLES::Message::title(GLOBAL::inputsTitle);

			STYLES::Message::error(GLOBAL::message_error);
			STYLES::Message::success(GLOBAL::message_success);
			STYLES::Message::neutral(GLOBAL::message_neutral);

			WORKCYCLE::AUTH::authWorkCycle_Init();
			WORKCYCLE::STUDENT::studentWorkCycle_Init();
			WORKCYCLE::TEACHER::teacherWorkCycle_Init();

			SetSize(LIMITS::windowWidth, LIMITS::windowHeight);
			ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
			SetColor(LIMITS::foregroundColor, LIMITS::backgroundColor);
			cls();
		}

		void workcycle() {
			WORKCYCLE::AUTH::authWorkCycle();
		}

		void shutdown() {
			GLOBAL::teachers.toFile("__DATA__/teachers.txt");

			GLOBAL::subjects.toFile("__DATA__/subjects.txt");

			GLOBAL::students.toFile("__DATA__/students.txt");
		}
	}
}
int main() {
	CORE::init();
	CORE::workcycle();
	CORE::shutdown();
	return 0;
}