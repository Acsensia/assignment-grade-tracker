#pragma once
#include <iostream>
#include <conio.h>
#include "fun_console.h"

namespace ASD {
	class Input {
	protected:
		int color, bgColor;
		int x, y;
		size_t length;
		char *str;
	public:
		Input(size_t len) {
			SetBgColor(Red);
			SetColor(White);
			length = len;
			str = new char[length + 1];
			str[0] = 0;
			x = y = 0;
		}

		~Input() {
			delete str;
		}
		int GetColor() { return color; }
		int GetBgColor() { return bgColor; }
		size_t GetLength() { return length; }
		Input& SetColor(int c) { color = c; return *this; }
		Input& SetBgColor(int c) { bgColor = c; return *this; }

		Input& Move(int _x, int _y) { x = _x; y = _y; return *this; }

		virtual bool process() {

			int k = 0;

			while (true) {
				::SetColor(color, bgColor);
				GotoXY(x, y);
				for (int i = 0; i < length; i++) std::cout << " ";
				GotoXY(x, y);
				std::cout << str;
				int key = _getch();
				if (key == 27) {//ESC
					memset(this->str, 0, this->length + 1);
					return false;
				}
				if (key == 8) {
					if (k == 0) continue;
					GotoXY(x, y);
					k--;
					str[k] = 0;
					continue;
				}
				if (key == 13) {
					break;
				}
				if (key == 32) continue;
				if (k == length) continue;
				str[k] = key;
				k++;
				str[k] = 0;
			}
			GotoXY(x, y);
			std::cout << str;
			return true;
		}

		char *get() { return str; }
		Input& clear() { memset(str, 0, length + 1); return *this; }

	};
	class IntegerInput : public Input {
	public:
		using Input::Input;
		bool process() {

			int k = 0;

			while (true) {
				::SetColor(color, bgColor);
				GotoXY(x, y);
				for (int i = 0; i < length; i++) std::cout << " ";
				GotoXY(x, y);
				std::cout << str;
				int key = _getch();
				if (key == 27) {//ESC
					memset(this->str, 0, this->length + 1);
					return false;
				}
				if (key == 8) {
					if (k == 0) continue;
					GotoXY(x, y);
					k--;
					str[k] = 0;
					continue;
				}
				if (key == 13) {
					break;
				}
				if (key == 32) continue;
				if (key < '0' || key > '9') {
					if (key != '-') continue;
					else {
						if (k != 0)continue;
					}
				}
				if (k == length) continue;
				str[k] = key;
				k++;
				str[k] = 0;
			}
			GotoXY(x, y);
			std::cout << str;
			return true;
		}
	};
	class FilenameInput : public Input {
	public:
		using Input::Input;
		bool process() {

			int k = 0;

			while (true) {
				::SetColor(color, bgColor);
				GotoXY(x, y);
				for (int i = 0; i < length; i++) std::cout << " ";
				GotoXY(x, y);
				std::cout << str;
				int key = _getch();
				if (key == 27) {//ESC
					memset(this->str, 0, this->length + 1);
					return false;
				}
				if (key == 8) {
					if (k == 0) continue;
					GotoXY(x, y);
					k--;
					str[k] = 0;
					continue;
				}
				if (key == 13) {
					break;
				}
				if (key == 32 || key == '\t' || key == '<' || key == '>' || key == ':' || key == '"' || key == '/'
					|| key == '\\' || key == '|' || key == '?' || key == '*' || key == '.'
					) continue;
				if (k == length) continue;

				str[k] = key;
				k++;
				str[k] = 0;
			}
			GotoXY(x, y);
			std::cout << str;
			return true;
		}
	};
	class UsernameInput : public Input {
	public:
		using Input::Input;
		bool process() {

			int k = 0;

			while (true) {
				::SetColor(color, bgColor);
				GotoXY(x, y);
				for (int i = 0; i < length; i++) std::cout << " ";
				GotoXY(x, y);
				std::cout << str;
				int key = _getch();
				if (key == 27) {//ESC
					memset(this->str, 0, this->length+1);
					return false;
				}
				if (key == 8) {
					if (k == 0) continue;
					GotoXY(x, y);
					k--;
					str[k] = 0;
					continue;
				}
				if (key == 13) {
					break;
				}
				//block lower from there is for A-Z a-z checking
				if (key >= 'A') {//if A>
					if (key > 'Z') {//if Z>
						if (key >= 'a') {//if a>
							if (key > 'z') continue;//if z>
						}
						else continue;
					}
				}
				else {
					continue;
				}
				if (k == length) continue;

				str[k] = key;
				k++;
				str[k] = 0;
			}
			GotoXY(x, y);
			std::cout << str;
			return true;
		}
	};
}
