# assignment-grade-tracker
A tracker for grades, subjects and assignments developed on C++ with the goal of getting a high mark for my uni assignment

- A console application using console graphics, created to help track subjects and graded tasks in relation to a group of students
- Student and Teacher sign-up and log-in
- Dynamic creation / deletion of subjects and tasks (assignments)
- Dynamic assignment / unassignment of subjects and tasks to individual students
- Report generation for individual subjects and tasks (number of students with the subject / task assigned, how many have their task graded and how many - do not)
- Teachers can change students’ names, logins and passwords.
- Students can view subjects and tasks assigned to them, as well if any of the tasks is graded and what that grade is
- All the data is serialized (stored locally on disk every time the program is closed and loaded into the program every time it is opened)
- Menus
- Limited text input fields (i. e. those that allow only certain symbols of certain predefined minimal and maximum lengths to be entered)
- Binary serialization of dynamic arrays containing objects that contain more dynamic arrays (arrays without predefined length)
- Object-oriented approach with every class being stored inside its own header
- Custom linked list with access by index and search functionality used
