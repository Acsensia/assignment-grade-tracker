#pragma once
#include <conio.h>
#include "ASDString.h"
#include "ASDArray.h"
#include "ASDFunctor.h"
#include "fun_console.h"
#include <windows.h>
#include <string>

using std::string, std::tuple;

namespace ASD {
	class Window
	{
	protected:
		int x, y, width, height;
		int color, bgColor, borderColor;
		Window *link_top = 0, * link_bottom = 0, * link_left = 0, * link_right = 0;
		struct Graphics{
			struct Corner {
				char standart;
				char& verticalIntersection;
				char& horizontalIntersection;
				char& cross;
				char* current = &standart;
			};
			char line_horizontal  = 205;
			char line_vertical    = 186;
			char T_junction       = 203;
			char T_down_junction  = 202;
			char T_left_junction  = 204;
			char T_right_junction = 185;
			char cross = 206;
			Corner corner_top_left     = { (char)201, T_left_junction, T_junction, cross };
			Corner corner_top_right    = { (char)187, T_right_junction, T_junction, cross };
			Corner corner_bottom_left  = { (char)200, T_left_junction, T_down_junction, cross };
			Corner corner_bottom_right = { (char)188, T_right_junction, T_down_junction, cross };
		}graphics;
		struct _coverageParameters {
			int  top{ 0 };
			int  bottom{ 0 };
			int  left{ 0 };
			int  right{ 0 };
			bool updateSelf{ true };
		} defaultCoverageParameters;
		void adjustCornersToLinks() {
			if (link_top) {
				if (link_left) graphics.corner_top_left.current   = &graphics.corner_top_left.cross;
				else graphics.corner_top_left.current             = &graphics.corner_top_left.verticalIntersection;
				if (link_right) graphics.corner_top_right.current = &graphics.corner_top_right.cross;
				else graphics.corner_top_right.current            = &graphics.corner_top_right.verticalIntersection;
			}
			else {
				if (link_left) graphics.corner_top_left.current   = &graphics.corner_top_left.horizontalIntersection;
				else graphics.corner_top_left.current             = &graphics.corner_top_left.standart;
				if (link_right) graphics.corner_top_right.current = &graphics.corner_top_right.horizontalIntersection;
				else graphics.corner_top_right.current            = &graphics.corner_top_right.standart;
			}
			if (link_bottom) {
				if (link_left) graphics.corner_bottom_left.current   = &graphics.corner_bottom_left.cross;
				else graphics.corner_bottom_left.current             = &graphics.corner_bottom_left.verticalIntersection;
				if (link_right) graphics.corner_bottom_right.current = &graphics.corner_bottom_right.cross;
				else graphics.corner_bottom_right.current            = &graphics.corner_bottom_right.verticalIntersection;
			}
			else {
				if (link_left) graphics.corner_bottom_left.current   = &graphics.corner_bottom_left.horizontalIntersection;
				else graphics.corner_bottom_left.current             = &graphics.corner_bottom_left.standart;
				if (link_right) graphics.corner_bottom_right.current = &graphics.corner_bottom_right.horizontalIntersection;
				else graphics.corner_bottom_right.current            = &graphics.corner_bottom_right.standart;
			}
		}
	public:
		Window();
		Window & Move(int _x, int _y);
		int GetColor() { return color; }
		int GetBgColor() { return bgColor; }
		int GetBorderColor() { return borderColor; }
		int GetX() { return x; }
		int GetY() { return y; }
		int GetWidth() { return width; }
		int GetClientWidth() { return width - 2; }
		int GetHeight() { return height; }
		int GetClientHeight() { return height - 2; }
		Window* GetLink_top() { return link_top; }
		Window* GetLink_bottom() { return link_bottom; }
		Window* GetLink_left() { return link_left; }
		Window* GetLink_right() { return link_right; }
		_coverageParameters GetDefaultCoverageParameters() { return defaultCoverageParameters; }
		Window & HCenter();
		Window & VCenter();
		Window & SetColor(int c);
		Window & SetBorderColor(int c);
		Window & SetBgColor(int c);
		Window & SetWidth(int w);
		Window & SetHeight(int h);
		Window& SetDimensions(int width, int height) {
			SetWidth(width);
			SetHeight(height);
			return *this;
		}
		Window& SetLink_top(Window& link) {
			link_top = &link;
			link.link_bottom = this;
			link.adjustCornersToLinks();
			adjustCornersToLinks();
			return *this;
		}
		Window& SetLink_bottom(Window& link) {
			link_bottom = &link;
			link.link_top = this;
			link.adjustCornersToLinks();
			adjustCornersToLinks();
			return *this;
		}
		Window& SetLink_left(Window& link) {
			link_left = &link;
			link.link_right = this;
			link.adjustCornersToLinks();
			adjustCornersToLinks();
			return *this;
		}
		Window& SetLink_right(Window& link) {
			link_right = &link;
			link.link_left = this;
			link.adjustCornersToLinks();
			adjustCornersToLinks();
			return *this;
		}
		Window& SetDefaultCoverageParameters(int top, int bottom, int left, int right, bool updateSelf) {
			defaultCoverageParameters = { top, bottom, left, right, updateSelf };
			return *this;
		}
		Window& ResetLink_top() { link_top = 0; };
		Window& ResetLink_bottom() { link_bottom = 0; };
		Window& ResetLink_left() { link_left = 0; };
		Window& ResetLink_right() { link_right = 0; };
		Window& ClearSelf() { ::SetColor(borderColor, bgColor); cls(GetX(), GetY(), GetWidth(), GetHeight()); return *this; }
		Window& Show() {
			::SetColor(borderColor, bgColor);
			ClearSelf();
			GotoXY(x, y);
			std::cout << *graphics.corner_top_left.current;
			for (int j = 0; j < width - 2; j++) {
				std::cout << graphics.line_horizontal;
			}
			std::cout << *graphics.corner_top_right.current;
			for (int i = 1; i < height - 1; i++) {
				GotoXY(x, y + i);
				std::cout << graphics.line_vertical;
				GotoXY(x + width - 1, y + i);
				std::cout << graphics.line_vertical;
			}
			GotoXY(x, y + height - 1);
			std::cout << *graphics.corner_bottom_left.current;
			for (int j = 0; j < width - 2; j++) {
				std::cout << graphics.line_horizontal;
			}
			std::cout << *graphics.corner_bottom_right.current;
			::SetColor(color, bgColor);
			for (int i = 1; i < height - 1; i++) {
				GotoXY(x + 1, y + i);
				for (int j = 0; j < width - 2; j++) {
					std::cout << " ";
				}
			}
			GotoXY(x + 1, y + 1);
			Draw();
			return *this;
		}

		struct Options {
			static enum linkRequestType {
				SetWidth,
				SetHeight,
			};
			static enum linkRequestType_NoArgs {
				Show,
				ClearSelf
			};
			bool accept_linkRequest_width = true;
			bool accept_linkRequest_height = true;

			/*static enum linkRequest_invokeableFunctions {
				Show,
			};
			bool accept_linkRequest_Show = true;*/
		}options;

		void coveredWindowsArrayGenerator(_coverageParameters& coverageOptions, Array<Window*>& coveredWindows) {
			if (link_top) {
				Window* curLink = link_top;
				for (int i = 0; i < coverageOptions.top; i++) {
					coveredWindows.Add(curLink);
					if (curLink->link_top) curLink = curLink->link_top;
					else break;
				}
			}
			if (link_bottom) {
				Window* curLink = link_bottom;
				for (int i = 0; i < coverageOptions.bottom; i++) {
					coveredWindows.Add(curLink);
					if (curLink->link_bottom) curLink = curLink->link_bottom;
					else break;
				}
			}
			if (link_left) {
				Window* curLink = link_left;
				for (int i = 0; i < coverageOptions.left; i++) {
					coveredWindows.Add(curLink);
					if (curLink->link_left) curLink = curLink->link_left;
					else break;
				}
			}
			if (link_right) {
				Window* curLink = link_right;
				for (int i = 0; i < coverageOptions.right; i++) {
					coveredWindows.Add(curLink);
					if (curLink->link_right) curLink = curLink->link_right;
					else break;
				}
			}
			if (coverageOptions.updateSelf) { coveredWindows.Add(this); }
		}
		template<typename ARG_TYPE, typename RETURN_TYPE> Window& MakeLinkRequest(_coverageParameters coverageOptions, Array<string> stringReferences, Options::linkRequestType variable) {
			
			Functor<Window, RETURN_TYPE, ARG_TYPE> changing_function;
			switch (variable) {
			using enum Options::linkRequestType;
			case SetWidth:  changing_function.memberFunction = &Window::SetWidth; break;
			case SetHeight: changing_function.memberFunction = &Window::SetHeight; break;
			}

			ARG_TYPE argument;
			if (typeid(ARG_TYPE) == typeid(int)) argument = std::stoi(stringReferences[0]);
			
			Array<Window*> coveredWindows;
			coveredWindowsArrayGenerator(coverageOptions, coveredWindows);

			for (int i = 0; i < coveredWindows.GetCount(); i++) {
				changing_function.objectPtr = coveredWindows[i];
				changing_function(argument);
			}
			return *this;
		}
		template<typename RETURN_TYPE> Window& MakeLinkRequest_NoArgs(_coverageParameters coverageOptions, Options::linkRequestType_NoArgs variable) {

			//Functor<Window, RETURN_TYPE, ARG_TYPE> changing_function;
			Functor_no_args<Window, RETURN_TYPE> changing_function;
			switch (variable) {
			using enum Options::linkRequestType_NoArgs;
			case Show:  changing_function.memberFunction = &Window::Show; break;
			case ClearSelf: changing_function.memberFunction = &Window::ClearSelf; break;
			}

			Array<Window*> coveredWindows;
			coveredWindowsArrayGenerator(coverageOptions, coveredWindows);
			
			for (int i = 0; i < coveredWindows.GetCount(); i++) {
				changing_function.objectPtr = coveredWindows[i];
				changing_function();
			}
			return *this;
		}

		/*Window& UpdateClear(_updateParameters top_bottom_left_right_updateSelf_STRUCT) {
			_updateParameters& params = top_bottom_left_right_updateSelf_STRUCT;
			if (link_top) {
				Window* curLink = link_top;
				for (int i = 0; i < params.top; i++) {
					curLink->ClearSelf();
					if (curLink->link_top) curLink = curLink->link_top;
					else break;
				}
			}
			if (link_bottom) {
				Window* curLink = link_bottom;
				for (int i = 0; i < params.bottom; i++) {
					curLink->ClearSelf();
					if (curLink->link_bottom) curLink = curLink->link_bottom;
					else break;
				}
			}
			if (link_left) {
				Window* curLink = link_left;
				for (int i = 0; i < params.left; i++) {
					curLink->ClearSelf();
					if (curLink->link_left) curLink = curLink->link_left;
					else break;
				}
			}
			if (link_right) {
				Window* curLink = link_right;
				for (int i = 0; i < params.left; i++) {
					curLink->ClearSelf();
					if (curLink->link_right) curLink = curLink->link_right;
					else break;
				}
			}
			if (params.updateSelf) ClearSelf();
			return *this;
		}
		Window& UpdateClear() { return UpdateClear(defaultUpdateParameters); }

		Window& Update(_updateParameters top_bottom_left_right_updateSelf_STRUCT) {
			_updateParameters& params = top_bottom_left_right_updateSelf_STRUCT;
			if (link_top) {
				Window* curLink = link_top;
				for (int i = 0; i < params.top; i++) {
					curLink->Show();
					if (curLink->link_top) curLink = curLink->link_top;
					else break;
				}
			}
			if (link_bottom) {
				Window* curLink = link_bottom;
				for (int i = 0; i < params.bottom; i++) {
					curLink->Show();
					if (curLink->link_bottom) curLink = curLink->link_bottom;
					else break;
				}
			}
			if (link_left) {
				Window* curLink = link_left;
				for (int i = 0; i < params.left; i++) {
					curLink->Show();
					if (curLink->link_left) curLink = curLink->link_left;
					else break;
				}
			}
			if (link_right) {
				Window* curLink = link_right;
				for (int i = 0; i < params.left; i++) {
					curLink->Show();
					if (curLink->link_right) curLink = curLink->link_right;
					else break;
				}
			}
			if (params.updateSelf) Show();
			return *this;
		}
		Window& Update() { return Update(defaultUpdateParameters); }*/
		Window& Write(int, int, const char *);
		Window& Write(int, int, char);
		virtual void Draw() {}
	};


	class Message: public Window {
		public:
			enum LineCenteringOption {
				left,
				center,
				right
			};
			
			struct Line {
				string line;
				LineCenteringOption centering_option;
			};
			struct Options : public Window::Options {////
				enum VerticalCenteringOption {
					top,
					center,
					bottom
				};

				//Height auto adjustment
				bool adjustHeight_IfInsufficient = true;
				bool adjustHeight_Always = false;
				bool adjustHeight_IfAddedWhileEmpty = true;

				//Width auto adjustment
				bool adjustWidth_IfInsufficient = true;
				bool adjustLinksWidth_IfInsufficient = true;

				//Ask for key press before the end of Show() function?
				bool do_getch = false;

				//Divide a line that is longer than (width - horizontal margin) into several smaller lines?
				bool adjustLinesForWidth = true;

				VerticalCenteringOption verticalCentering = center;

				int textVerticalMargin = 1;
				int textHorizontalMargin = 1;
			} options;
		protected:
			bool Draw_keepAdjustingLinesForWidth = true;
			Array<Line> lines;
			Message& AdjustLinesForWidth() {
				//bool increase_height = false;
				//int increase_height_amount = 0;
				//if (check_Height()) increase_height = true;
				int max_allowed_lineLength = GetWidth() - options.textHorizontalMargin * 2;
				int max_allowed_lineIndex = max_allowed_lineLength - 1;
				for (int i = 0; i < lines.GetCount(); i++) {
					if (lines[i].line.length() <= max_allowed_lineLength) continue;
					for (int divisionIndex = max_allowed_lineIndex; divisionIndex > -1; divisionIndex--) {
						if (lines[i].line[divisionIndex] == ' ') {
							string division_rightSide = lines[i].line.substr(divisionIndex + 1);
							lines[i].line.erase(divisionIndex);
							//options.adjustLinesForWidth = false;
							//AddLine(division_rightSide, lines[i].centering_option, i + 1);
							lines.AddAt({ division_rightSide, lines[i].centering_option }, i + 1);
							//options.adjustLinesForWidth = true;
							break;
						}
					}
					//increase_height_amount++;
				}
				//if (increase_height) SetHeight(GetHeight() + increase_height_amount);
				check_Adjust_Height();
				return *this;
			}
			Message& checkLine_Width(string& line) {
				if (options.adjustLinesForWidth) { AdjustLinesForWidth(); }
				else if (GetWidth() <= line.length()) {
					int minimalExpectedWidth = line.length() + 2 * options.textHorizontalMargin;
					if (options.adjustWidth_IfInsufficient) SetWidth(minimalExpectedWidth);
					if (options.adjustLinksWidth_IfInsufficient) {
						MakeLinkRequest<int, Window&>(defaultCoverageParameters,
							Array<string>().Add(std::to_string(minimalExpectedWidth)), Options::linkRequestType::SetWidth);
					}
				}
				return *this;
			}
			bool check_Height() {
				int minimalExpectedHeight = GetLinesAmount() + 2 * options.textVerticalMargin;
				return (
					(options.adjustHeight_IfInsufficient && GetHeight() < minimalExpectedHeight) ||
					(options.adjustHeight_Always) ||
					(options.adjustHeight_IfAddedWhileEmpty && lines.GetCount() == 1)
					);
			}
			Message& check_Adjust_Height() {
				int minimalExpectedHeight = GetLinesAmount() + 2 * options.textVerticalMargin;
				if (check_Height()) SetHeight(minimalExpectedHeight);
				return *this;
			}
		public:
			Message() {};
			Message(string line, LineCenteringOption option = center) {
				AddLine(line, option);
			}
			Message(Array<Line> lines) : lines(lines) {}
			Message& SetWidth(int w) {
				Window::SetWidth(w);
				if (options.adjustLinesForWidth) AdjustLinesForWidth();
				return *this;
			}
			void Draw() {
				int (*subAligner_Start)(int totalLength, int usedLength, int margin) = [](int totalLength, int usedLength, int margin) { return margin; };
				int (*subAligner_Center)(int totalLength, int usedLength, int margin) = [](int totalLength, int usedLength, int margin) { return (totalLength - usedLength) / 2; };
				int (*subAligner_End)(int totalLength, int usedLength, int margin) = [](int totalLength, int usedLength, int margin) { return totalLength - usedLength - margin; };

				auto verticalAligner = [&](decltype(subAligner_Start) subAligner) {return subAligner(GetHeight(), GetLinesAmount(), options.textVerticalMargin); };
				auto horizontalAligner = [&](decltype(subAligner_Start) subAligner, int& index) {return subAligner(GetWidth(), lines[index].line.length(), options.textHorizontalMargin); };

				int number_of_blank_lines_added = 0;
				if ((GetHeight() - GetLinesAmount() - (2 * options.textVerticalMargin)) < 0) number_of_blank_lines_added = verticalAligner(subAligner_Start);
				else switch (options.verticalCentering) {
				using enum Options::VerticalCenteringOption;
				case top: number_of_blank_lines_added = verticalAligner(subAligner_Start); break;
				case center: number_of_blank_lines_added = verticalAligner(subAligner_Center); break;
				case bottom: number_of_blank_lines_added = verticalAligner(subAligner_End); break;
				}
				
				for (int index = 0; index < lines.GetCount(); index++) {
					int number_of_spaces_added = 0;
					if ((GetWidth() - lines[index].line.length() - (2 * options.textHorizontalMargin)) < 0) {
						if (options.adjustLinesForWidth && Draw_keepAdjustingLinesForWidth) {
							Draw_keepAdjustingLinesForWidth = false;
							AdjustLinesForWidth();
							ClearSelf();
							return Draw();
						}
						else number_of_spaces_added = horizontalAligner(subAligner_Start, index);
					}
					else switch (lines[index].centering_option) {
					using enum LineCenteringOption;
					case left: number_of_spaces_added = horizontalAligner(subAligner_Start, index); break;
					case center: number_of_spaces_added = horizontalAligner(subAligner_Center, index); break;
					case right: number_of_spaces_added = horizontalAligner(subAligner_End, index); break;
					}
					
					GotoXY(x + number_of_spaces_added, y + index + number_of_blank_lines_added);
					std::cout << lines[index].line;
				}
				if (options.do_getch) _getch();
				Draw_keepAdjustingLinesForWidth = true;
			}
			Line GetLine(int index) { return lines[index]; }
			int GetLinesAmount() { return lines.GetCount(); }
			Message& AddLine(string line, LineCenteringOption option = left, int index = -1) {
				lines.AddAt({ line, option }, index);

				checkLine_Width(line);

				check_Adjust_Height();

				return *this;
			}
			Message& SetLine(int index, string line, LineCenteringOption option = left) {
				lines[index] = { line, option };
				return *this;
			}
			Message& Clear() {
				lines.RemoveAll();
				return *this;
			}
	};

	
	/*
	class Menu: public Window {
		protected:
			Array<String> items;
			int active;
			int selectColor, selectBgColor;
			int first;
		public:
			Menu(): Window() {
				active = -1;
				selectColor = Blue;
				selectBgColor = White;
			}
			Menu & Add(String m) { items.Add(m); return *this; }
			int GetSelect() { return active;  }
			int Select();		
	};
	*/
	
	class Menu : public Window {
	protected:
		int active;
		int selectColor, selectBgColor;
		int first;
	public:
		Menu() : Window() {
			active = -1;
			selectColor = Blue;
			selectBgColor = White;
		}
		int GetSelect() { return active; }
		int Select(unsigned int activeDefault = 0);
		Menu& SetSelectColor(int c);
		Menu& SetSelectBgColor(int c);
		virtual int GetCount() = 0;
		virtual void DrawItem(int row, int index) = 0;		
	};

	class ArrayMenu: public Menu{
	protected:
		Array<String> items;
	public:
		ArrayMenu() : Menu() {

		}
		void Add(const char * m) { items.Add(m); }
		void Set(const char* m, unsigned int i) { items[i] = m; }
		void Remove(unsigned int i) { items.RemoveAt(i); }
		void Clear() { items.RemoveAll(); }
		String Get(unsigned int index) { return items[index]; }
		ArrayMenu & AutoSize(int marginLeft = 1, int marginRight = 1, int marginTop = 1, int marginBottom = 1) {
			SetHeight(GetCount() + marginTop + marginBottom);
			int itemMaxSymbolWidth = 1;
			for (int itemsIndex = 0; itemsIndex < items.GetCount(); itemsIndex++) {
				if (items[itemsIndex].length() > itemMaxSymbolWidth) {
					itemMaxSymbolWidth = items[itemsIndex].length();
				}
			}
			SetWidth(itemMaxSymbolWidth + marginLeft + marginRight);
			return *this;
		}

		virtual int GetCount() {
			return items.GetCount();
		}
		virtual void DrawItem(int row, int index) {
			Write(0, row, items[index]);
		}
	};

	class CallbackMenu : public ArrayMenu {
	protected:
		Array<tuple<int, string>(*)()> callbacks;
		tuple<int, string>(*cancelCallback)() = []()->tuple<int, string> { return { -1000, "Cancel callback undefined" }; };
		int lastChosenOptionIndex = 0;
		void adaptToChangeInSize() {
			if (items.GetCount()) {
				int maxAllowedIndex = items.GetCount() - 1;
				if (lastChosenOptionIndex > maxAllowedIndex) lastChosenOptionIndex = maxAllowedIndex;
				else if (lastChosenOptionIndex < 0) {
					lastChosenOptionIndex = lastChosenOptionIndex + maxAllowedIndex;
					if (lastChosenOptionIndex < 0) lastChosenOptionIndex = 0;
				}
			}
		}
	public:
		using ArrayMenu::ArrayMenu;

		void Add(const char* option, tuple<int, string>(*callback)()) { items.Add(option); callbacks.Add(callback); }
		void Set(const char* option, tuple<int, string>(*callback)(), unsigned int i) { items[i] = option; callbacks[i] = callback; }
		int GetLastChosenOptionIndex() { return lastChosenOptionIndex; }
		void SetCancelCallback(tuple<int, string>(*callback)()) { cancelCallback = callback; }
		CallbackMenu& SetLastChosenOptionIndex(int newIndex) {
			lastChosenOptionIndex = newIndex;
			adaptToChangeInSize();
			return *this;
		}
		void ResetCancelCallback() { cancelCallback = []()->tuple<int, string> { return { -1000, "Cancel callback undefined" }; }; }
		void Remove(unsigned int i) { items.RemoveAt(i); callbacks.RemoveAt(i); adaptToChangeInSize(); }
		void Clear(bool clear_cancelCallback = false) {
			ArrayMenu::Clear();
			callbacks.RemoveAll();
			lastChosenOptionIndex = 0;
			if (clear_cancelCallback) ResetCancelCallback();
		}
		tuple<int, string> Select(bool useLastChosenOptionIndex = true) {
			lastChosenOptionIndex = ArrayMenu::Select(useLastChosenOptionIndex*lastChosenOptionIndex);
			if (lastChosenOptionIndex != -1) return callbacks[lastChosenOptionIndex]();
			else { lastChosenOptionIndex = 0; return cancelCallback(); }
		}
	};

	class UniversalCallbackMenu : public ArrayMenu {
	protected:
		tuple<int, string>(*universalCallback)(int) = [](int chosenOptionIndex)->tuple<int, string> { return { -1000, "Universal callback undefined" }; };
		tuple<int, string>(*cancelCallback)() = []()->tuple<int, string> { return { -1000, "Cancel callback undefined" }; };
		int lastChosenOptionIndex = 0;
		void adaptToChangeInSize() {
			if (items.GetCount()) {
				int maxAllowedIndex = items.GetCount() - 1;
				if (lastChosenOptionIndex > maxAllowedIndex) lastChosenOptionIndex = maxAllowedIndex;
				else if (lastChosenOptionIndex < 0) {
					lastChosenOptionIndex = lastChosenOptionIndex + maxAllowedIndex;
					if (lastChosenOptionIndex < 0) lastChosenOptionIndex = 0;
				}
			}
		}
	public:
		using ArrayMenu::ArrayMenu;

		int GetLastChosenOptionIndex() { return lastChosenOptionIndex; }
		void SetCancelCallback(tuple<int, string>(*callback)()) { cancelCallback = callback; }
		void SetUniversalCallback(tuple<int, string>(*callback)(int)) { universalCallback = callback; }
		UniversalCallbackMenu& SetLastChosenOptionIndex(int newIndex) {
			lastChosenOptionIndex = newIndex;
			adaptToChangeInSize();
			return *this;
		}
		void ResetCancelCallback() { cancelCallback = []()->tuple<int, string> { return { -1000, "Cancel callback undefined" }; }; }
		void ResetUniversalCallback() { universalCallback = [](int chosenOptionIndex)->tuple<int, string> { return { -1000, "Universal callback undefined" }; }; }
		void Remove(unsigned int i) { ArrayMenu::Remove(i); adaptToChangeInSize(); }
		void Clear(bool clear_cancelCallback = false, bool clear_universalCallback = false) {
			ArrayMenu::Clear();
			lastChosenOptionIndex = 0;
			if (clear_cancelCallback) ResetCancelCallback();
			if (clear_universalCallback) ResetUniversalCallback();
		}
		tuple<int, string> Select(bool useLastChosenOptionIndex = true) {
			lastChosenOptionIndex = ArrayMenu::Select(useLastChosenOptionIndex * lastChosenOptionIndex);
			if (lastChosenOptionIndex != -1) return universalCallback(lastChosenOptionIndex);
			else { lastChosenOptionIndex = 0; return cancelCallback(); }
		}
	};


	class FileMenu : public ArrayMenu {
	private:
		String path;
		String curPath;
		int countDir, countFile;
		int colorDir;
	public:
		FileMenu() : ArrayMenu() {
			colorDir = LightCyan;
		}

		String GetFile() { 
			String s = curPath;
			s += "\\";
			s += items[active];
			return s; 
		}

		FileMenu & SetPath(String p) { path = p; return *this; }
		int Select() {
			curPath = path;
			while (1) {
				Fill();
				int c = ArrayMenu::Select();
				if (c < 0) return c;
				if (c < countDir) {
					/*
					if (items[c] == "..") {
						
					}
					else {
						curPath += "\\";
						curPath += items[c];
					}
					*/
					curPath += "\\";
					curPath += items[c];
					continue;
				}
				return c;
			}
		}
		virtual void DrawItem(int row, int index) {
			if (index < countDir && index != active) {
				::SetColor(colorDir, bgColor);
			}
			ArrayMenu::DrawItem(row, index);
		}
	protected:
		void Fill() {
			countDir = 0; countFile = 0;
			items.RemoveAll();
			WIN32_FIND_DATAA data;
			String p = curPath;
			p += "\\*.*";
			HANDLE h = ::FindFirstFileA((const char *)p, &data);
			if (!h) {
				return;
			}
			while (FindNextFileA(h, &data)) {
				if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
				items.Add(data.cFileName);
				countDir++;
			}
			FindClose(h);
			p = curPath;
			p += "\\*.*";
			h = ::FindFirstFileA((const char *)p, &data);
			while (FindNextFileA(h, &data)) {
				if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
				items.Add(data.cFileName);
				countFile++;
			}
			FindClose(h);
		}

	};


	class ColorsMenu : public Menu {
	public:
		ColorsMenu() : Menu() {
		}

		int GetCount() { return 16; }

		void DrawItem(int row, int index) {
			if (index == active) Write(0, row, ">");
			::SetColor(index, index);
			for (int i = 1; i < GetClientWidth(); i++) {
				Write(i, row, " ");
			}
		}

	};
}

