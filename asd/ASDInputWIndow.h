#pragma once
#include <string>
#include "fun_console.h"
#include "ASDInput.h"
#include "ASDWindow.h"

namespace ASD {
	class InputWindow : public Window {
		void scls() {
			::SetColor(Black, White);
			cls();
		}
	public:
		enum CenteringOption {
			left,
			center,
			right
		};
		struct Options : public Window::Options {
			int textVerticalMargin = 1;
			int textHorizontalMargin = 1;

			CenteringOption centering_option = center;
		} options;
		Input* input;
		//resultChecker returns int instead of bool in order to allow for more customizability along the line
		int (*resultChecker)(char*& result);
		InputWindow(Input* input, void (*styleApplier)(InputWindow& window) = [](InputWindow&){}, int (*resultChecker)(char*& result) = [](char*& result)->int {
			if (!result[0]) return 0;
			else return 1;
			}) : Window::Window() {
			this->input = input;
			this->resultChecker = resultChecker;
			SetWidth(input->GetLength() + 2 * options.textHorizontalMargin);
			SetHeight(1 + 2 * options.textVerticalMargin);
			styleApplier(*this);
		}
		InputWindow& Show() {
			//scls();
			Window::Show();
			int centeringAdjustment = 0;
			switch (options.centering_option) {
				using enum CenteringOption;
			case center: centeringAdjustment = (GetWidth() - input->GetLength()) / 2 - options.textHorizontalMargin; break;
			case right: centeringAdjustment = GetWidth() - input->GetLength() - 1 - 2 * options.textHorizontalMargin; break;
			}
			input->Move(GetX() + centeringAdjustment + options.textHorizontalMargin, GetY() + options.textVerticalMargin);
			return *this;
		}
		string Process() {
			while (true) {
				input->clear();
				Show();
				MakeLinkRequest_NoArgs<ASD::Window&>(defaultCoverageParameters, ASD::Window::Options::linkRequestType_NoArgs::Show);////
				if (!input->process()) break;//if ESC pressed
				char* result = input->get();
				if (resultChecker(result)) break;//if result passes the checks
			}
			string result = input->get();
			input->clear();
			return result;
		}
	};
}