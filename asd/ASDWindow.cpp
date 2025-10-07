#include <iostream>
#include "ASDWindow.h"
#include "fun_console.h"

ASD::Window::Window() {
	x = y = 0;
	width = GetCols();
	height = GetRows();
	SetBgColor(Blue);
	SetColor(White);
	SetBorderColor(LightBlue);
}

ASD::Window & ASD::Window::HCenter() {
	x = (GetCols() - width) / 2;
	return *this;
}
ASD::Window & ASD::Window::VCenter() {
	y = (GetRows() - height) / 2;
	return *this;
}

ASD::Window & ASD::Window::SetColor(int c) {
	color = c;
	return *this;
}

ASD::Window & ASD::Window::SetBgColor(int c) {
	bgColor = c;
	return *this;
}

ASD::Window & ASD::Window::SetBorderColor(int c) {
	borderColor = c;
	return *this;
}

ASD::Window & ASD::Window::Move(int _x, int _y) {
	if (_x + width <= GetCols()) {
		x = _x;
	}
	else {
		width -= (_x - x);
		x = _x;
	}
	
	if (_y + height <= GetRows()) {
		y = _y;
	}
	else {
		height -= (_y - y);
		y = _y;
	}
	return *this;
}

ASD::Window & ASD::Window::SetWidth(int w) {
	if (w >= 3 && x + w <= GetCols()) {
		width = w;
	}
	return *this;
}

ASD::Window & ASD::Window::SetHeight(int h) {
	if (h >= 3 && y + h <= GetRows()) {
		height = h;
	}
	return *this;
}

//ASD::Window& ASD::Window::SetLink_top(Window& link) {
//
//}
//ASD::Window& ASD::Window::SetLink_bottom(Window& link) {
//
//}
//ASD::Window& ASD::Window::SetLink_left(Window& link) {
//
//}
//ASD::Window& ASD::Window::SetLink_right(Window& link) {
//
//}

ASD::Window & ASD::Window::Write(int _x, int _y, const char * str) {
	if (_x < 0 || _x >= GetClientWidth() || _y < 0 || _y >= GetClientHeight()) return *this;
	//::SetColor(color, bgColor);
	GotoXY(x + _x + 1, y + _y + 1);
	for (int i = 0; str[i]; i++) {
		if (_x + i >= GetClientWidth()) {
			break;
		}
		std::cout << str[i];
	}
	return *this;
}

ASD::Window& ASD::Window::Write(int _x, int _y, char str) {
	if (_x < 0 || _x >= GetClientWidth() || _y < 0 || _y >= GetClientHeight()) return *this;
	GotoXY(x + _x + 1, y + _y + 1);
	if (_x >= GetClientWidth()) {
		return *this;
	}
	std::cout << str;
	return *this;
}

//ASD::Window &  ASD::Window::Show() {
//	::SetColor(borderColor, bgColor);
//	GotoXY(x, y);
//	std::cout << *graphics.corner_top_left.current;
//	for (int j = 0; j < width - 2; j++) {
//		std::cout << graphics.line_horizontal;
//	}
//	std::cout << *graphics.corner_top_right.current;
//	for (int i = 1; i < height - 1; i++) {
//		GotoXY(x, y + i);
//		std::cout << graphics.line_vertical;
//		GotoXY(x + width - 1, y + i);
//		std::cout << graphics.line_vertical;
//	}
//	GotoXY(x, y + height - 1);
//	std::cout << *graphics.corner_bottom_left.current;
//	for (int j = 0; j < width - 2; j++) {
//		std::cout << graphics.line_horizontal;
//	}
//	std::cout << *graphics.corner_bottom_right.current;
//	::SetColor(color, bgColor);
//	for (int i = 1; i < height - 1; i++) {
//		GotoXY(x + 1, y + i);
//		for (int j = 0; j < width-2; j++) {
//			std::cout << " ";
//		}
//	}
//	GotoXY(x + 1, y + 1);
//	Draw();
//	return *this;
//}


/*
int ASD::Menu::Select() {
	if (items.GetCount() == 0) {
		active = -1;
		return active;
	}
	ShowCaret(false);
	active = 0;
	first = 0;
	while (1) {
		Show();

		if (active > first + GetClientHeight() - 1) {
			first = active - GetClientHeight() + 1;
		}
		else if (active < first) {
			first = active;
		}

		for (int i = 0, j = first; i < GetClientHeight() && j < items.GetCount(); i++, j++) {
			if (j == active) {
				::SetColor(selectColor, selectBgColor);
				::GotoXY(x + 1, y + 1 + i);
				for (int k = 0; k < GetClientWidth(); k++) {
					std::cout << " ";
				}
			}
			else {
				::SetColor(color, bgColor);
			}
			this->Write(0, i, items[j]);
		}
		int key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 80 && active + 1 < items.GetCount()) { //down
				active++;
			}
			if (key == 72 && active > 0) { //down
				active--;
			}
			continue;
		}
		if (key == 27) {
			active = -1;
			break;
		}
		if (key == 13) {
			break;
		}

	}
	ShowCaret(true);
	return active;
}

*/

ASD::Menu & ASD::Menu::SetSelectColor(int c)
{
	selectColor = c;
	return *this;
}

ASD::Menu & ASD::Menu::SetSelectBgColor(int c)
{
	selectBgColor = c;
	return *this;
}

int ASD::Menu::Select(unsigned int activeDefault) {
	if (GetCount() == 0) {
		active = -1;
		return active;
	}
	ShowCaret(false);
	active = activeDefault;
	first = 0;
	//Show();
	MakeLinkRequest_NoArgs<ASD::Window&>(defaultCoverageParameters, ASD::Window::Options::linkRequestType_NoArgs::Show);////
	//Update();////
	//MakeLinkRequest_InvokeFunction<bool**********, Window&>(defaultUpdateParameters, ASD::Array<string>().Add(""), ASD::Window::Options::linkRequest_invokeableFunctions::Show);
	//Show();
	while (1) {

		if (active > first + GetClientHeight() - 1) {
			first = active - GetClientHeight() + 1;
		}
		else if (active < first) {
			first = active;
		}

		for (int i = 0, j = first; i < GetClientHeight() && j < GetCount(); i++, j++) {
			if (j == active) {
				::SetColor(selectColor, selectBgColor);
			}
			else {
				::SetColor(color, bgColor);
			}
			::GotoXY(x + 1, y + 1 + i);
			for (int k = 0; k < GetClientWidth(); k++) {
				std::cout << " ";
			}
			::GotoXY(x + 1, y + 1 + i);
			DrawItem(i, j);
		}
		int key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 80) {//down
				if (active + 1 < GetCount()) {
					active++;
				}
				else {
					active = 0;
				}
			}
			else if (key == 72) {//up
				if (active > 0) {
					active--;
				}
				else {
					active = GetCount() - 1;
				}
			}
			continue;
		}
		if (key == 27) {//esc
			active = -1;
			break;
		}
		if (key == 13 || key == 32) {//enter or space
			break;
		}

	}
	ShowCaret(true);
	return active;
}
