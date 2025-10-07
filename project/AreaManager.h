#pragma once
#include "List.h"
#include "dimensions.h"
#include "../asd/ASDWindow.h"
namespace PROJECT {
	class AreaManager {
		int areaWidth, areaHeight;
		int columns, rows;
		int gap_Columns, gap_Rows, gap_Windows;
		int x_Offset, y_Offset;
		List<coords> areasCoordinates;
		int curArea = 0;
		List<dimensions> DefaultWindowSizeAdjustments;
	public:
		struct Options {
			enum Directions {
				horizontal,
				vertical,
			} windowDirection = vertical;
			bool applyDefaultWindowSizeAdjustments = true;
			bool use_last_DWSA_if_no_DWSA_found = true;
			Options& operator=(const Options& other) {
				windowDirection = other.windowDirection;
				applyDefaultWindowSizeAdjustments = other.applyDefaultWindowSizeAdjustments;
				use_last_DWSA_if_no_DWSA_found = other.use_last_DWSA_if_no_DWSA_found;
				return *this;
			}
		} options;
		AreaManager(int areaWidth, int areaHeight, int columns, int rows, int gap_Columns = 0, int gap_Rows = 0, int gap_Windows = 0, int x_Offset = 0, int y_Offset = 0, Options::Directions windowDirection = Options::Directions::vertical, int curArea = 0) {
			this->areaWidth = abs(areaWidth);
			this->areaHeight = abs(areaHeight);
			this->columns = abs(columns);
			this->rows = abs(rows);
			this->gap_Columns = gap_Columns;
			this->gap_Rows = gap_Rows;
			this->gap_Windows = gap_Windows;
			this->x_Offset = abs(x_Offset);
			this->y_Offset = abs(y_Offset);
			SetCurrentArea(curArea);
			RecalculateCoordinates();
			options.windowDirection = windowDirection;
		}
		coords GetAreaCoordinates(int areaIndex) { return *areasCoordinates.getReference(areaIndex); }
		int GetAreas() { return areasCoordinates.length(); }
		int GetAreaWidth() { return areaWidth; }
		int GetAreaHeight() { return areaHeight; }
		int GetColumns() { return columns; }
		int GetRows() { return rows; }
		int GetGap_Columns() { return gap_Columns; }
		int GetGap_Rows() { return gap_Rows; }
		int GetGap_Windows() { return gap_Windows; }
		int GetX_Offset() { return x_Offset; }
		int GetY_Offset() { return y_Offset; }
		int GetCurrentArea() { return curArea; }
		dimensions getDefaultWindowSizeAdjustment(int windowIndex) { return *DefaultWindowSizeAdjustments.getReference(windowIndex); }
		AreaManager& SetAreaWidth(int newAreaWidth) {
			areaWidth = abs(newAreaWidth);
			RecalculateCoordinates();
			return *this;
		}
		AreaManager& SetAreaHeight(int newAreaHeight) {
			areaHeight = abs(newAreaHeight);
			RecalculateCoordinates();
			return *this;
		}
		AreaManager& SetColumns(int newColumns) {
			columns = abs(newColumns);
			RecalculateCoordinates();
			return *this;
		}
		AreaManager& SetRows(int newRows) {
			rows = abs(newRows);
			RecalculateCoordinates();
			return *this;
		}
		AreaManager& SetGap_Columns(int newGap_Columns) {
			gap_Columns = newGap_Columns;
			RecalculateCoordinates();
			return *this;
		}
		AreaManager& SetGap_Rows(int newGap_Rows) {
			gap_Rows = newGap_Rows;
			RecalculateCoordinates();
			return *this;
		}
		AreaManager& SetGap_Windows(int newGap_Windows) {
			gap_Windows = newGap_Windows;
			return *this;
		}
		AreaManager& SetX_Offset(int newX_Offset) {
			x_Offset = abs(newX_Offset);
			return *this;
		}
		AreaManager& SetY_Offset(int newY_Offset) {
			y_Offset = abs(newY_Offset);
			return *this;
		}
		AreaManager& CheckCurrentArea() {
			int maxCurArea = GetAreas() - 1;
			if (curArea < 0) curArea = maxCurArea;
			else if (curArea > maxCurArea) curArea = 0;
			return *this;
		}
		AreaManager& SetCurrentArea(int newCurrentArea = 0) {
			curArea = newCurrentArea;
			CheckCurrentArea();
			return *this;
		}
		AreaManager& SetDefaultWindowSizeAdjustment(dimensions adjustment, int windowIndex) {
			*DefaultWindowSizeAdjustments.getReference(windowIndex) = adjustment;////potentially doesn't work
			return *this;
		}
		AreaManager& SetDefaultWindowSizeAdjustments(List<dimensions>& adjustments) {
			DefaultWindowSizeAdjustments = adjustments;
			return *this;
		}
		/*int dummy(int& value) {
			if (value != 0) return 1;
			else return 0;
		}*/
		AreaManager& RecalculateCoordinates() {
			areasCoordinates.clear();
			for (int curRow = 0; curRow < rows; curRow++) {
				int curY = y_Offset + (areaHeight * curRow) + (curRow * gap_Rows);
				for (int curColumn = 0; curColumn < columns; curColumn++) {
					int curX = x_Offset + (areaWidth * curColumn) + (curColumn * gap_Columns);
					areasCoordinates.add({ curX, curY });
				}
			}
			/*for (int i = 0; i < areasCoordinates.length(); i++) {
				coords* curCoords = areasCoordinates.getReference(i);
				cout << " " << i << "| X=" << curCoords->x << " Y=" << curCoords->y << '\n'; _getch();
			}*/
			CheckCurrentArea();
			return *this;
		}
		//Returns -1 if the given coordinates do not equal to any of registered areas' coordinates
		int ReverseEngineerCoordsIntoAreaNumber(coords coordinates) {
			int curAreaIndex;
			for (curAreaIndex = areasCoordinates.length() - 1; curAreaIndex > -1; curAreaIndex--) {
				coords* curAreaCoords = areasCoordinates.getReference(curAreaIndex);
				if (coordinates.x == curAreaCoords->x && coordinates.y == curAreaCoords->y) break;
			}
			return curAreaIndex;
		}
		AreaManager& AssignArea(List<ASD::Window*>* windows, bool incrementCurrentArea = true) {
			auto WindowDimensionsSetter = [this](dimensions dms, ASD::Window*& curWindow, dimensions*& defAdjustment) {
				if (!options.applyDefaultWindowSizeAdjustments) {
					curWindow->Move(dms.x, dms.y);
					curWindow->SetDimensions(dms.width, dms.height);
				}
				else {
					if (defAdjustment->x) {
						if (defAdjustment->y) curWindow->Move(dms.x, dms.y);
						else curWindow->Move(dms.x, curWindow->GetY());
					}
					else if (defAdjustment->y) curWindow->Move(curWindow->GetX(), dms.y);
					if (defAdjustment->width) {
						if (defAdjustment->height) curWindow->SetDimensions(dms.width, dms.height);
						else curWindow->SetWidth(dms.width);
					}
					else if (defAdjustment->height) curWindow->SetHeight(dms.height);
				}
				};
			auto WindowDimensionsProcessor_horizontal = [](int& spareSpace, List<ASD::Window*>& windows, ASD::Window*& curWindow,
				dimensions*& defAdjustment, coords*& areaCoords, int& iteration, AreaManager& areaManager,
				decltype(WindowDimensionsSetter) dimensionSetter) {
					int gap_Window_Adjustment = (int(bool(iteration)) * (areaManager.gap_Windows));
					int x_prevWindowRightSide = areaManager.areaWidth - spareSpace;
					int x_adjustment = areaCoords->x + x_prevWindowRightSide + gap_Window_Adjustment;
					int windowsLeft = windows.length() - iteration;
					int adjustedWidth = (spareSpace / windowsLeft) - gap_Window_Adjustment;
					dimensionSetter({ x_adjustment, areaCoords->y, adjustedWidth, areaManager.areaHeight }, curWindow, defAdjustment);
					spareSpace -= curWindow->GetWidth();
				};
			auto WindowDimensionsProcessor_vertical = [](int& spareSpace, List<ASD::Window*>& windows, ASD::Window*& curWindow,
				dimensions*& defAdjustment, coords*& areaCoords, int& iteration, AreaManager& areaManager,
				decltype(WindowDimensionsSetter) dimensionSetter) {
					int gap_Window_Adjustment = (int(bool(iteration)) * (areaManager.gap_Windows));
					int y_prevWindowBottom = areaManager.areaHeight - spareSpace;
					int y_adjustment = areaCoords->y + y_prevWindowBottom + gap_Window_Adjustment;
					int windowsLeft = windows.length() - iteration;
					int adjustedHeight = (spareSpace / windowsLeft) - gap_Window_Adjustment;
					dimensionSetter({ areaCoords->x, y_adjustment, areaManager.areaWidth, adjustedHeight }, curWindow, defAdjustment);
					spareSpace -= curWindow->GetHeight();
				};

			void (*dimensionsProcessor)(int& spareSpace, List<ASD::Window*>&windows, ASD::Window * &curWindow,
				dimensions * &defAdjustment, coords * &areaCoords, int& iteration, AreaManager & areaManager,
				decltype(WindowDimensionsSetter) dimensionSetter) = [](
					int& spareSpace, List<ASD::Window*>& windows, ASD::Window*& curWindow,
					dimensions*& defAdjustment, coords*& areaCoords, int& iteration, AreaManager& areaManager,
					decltype(WindowDimensionsSetter) dimensionSetter
					) {};
			int spareSpace;
			switch (options.windowDirection) {
				using enum Options::Directions;
			case horizontal: spareSpace = areaWidth; dimensionsProcessor = WindowDimensionsProcessor_horizontal; break;
			case vertical: spareSpace = areaHeight; dimensionsProcessor = WindowDimensionsProcessor_vertical; break;
			}

			coords* areaCoords = areasCoordinates.getReference(curArea);
			dimensions* defAdjustment;
			dimensions defAdjustment_missing = { 1, 1, 1, 1 };
			bool keepCheckingForAdjustments = options.applyDefaultWindowSizeAdjustments;
			for (int i = 0; i < windows->length(); i++) {
				if (keepCheckingForAdjustments) {
					defAdjustment = &defAdjustment_missing;
					if (i < DefaultWindowSizeAdjustments.length()) defAdjustment = DefaultWindowSizeAdjustments.getReference(i);
					else {
						if (options.use_last_DWSA_if_no_DWSA_found && DefaultWindowSizeAdjustments.length() != 0) defAdjustment = DefaultWindowSizeAdjustments.getReference(-1);
						keepCheckingForAdjustments = false;
					}
				}
				ASD::Window* curWindow = *windows->getReference(i);
				dimensionsProcessor(spareSpace, *windows, curWindow, defAdjustment, areaCoords, i, *this, WindowDimensionsSetter);
			}
			if (incrementCurrentArea) {
				curArea++;
				CheckCurrentArea();
			}
			return *this;
		}
		AreaManager& AssignAreasInAreas(List<List<List<ASD::Window*>>>* areasInAreas) {
			for (int areasIndex = 0; areasIndex < areasInAreas->length(); areasIndex++) {
				List<List<ASD::Window*>>* curAreas = areasInAreas->getReference(areasIndex);
				for (int areaIndex = 0; areaIndex < curAreas->length(); areaIndex++) {
					List<ASD::Window*>* curArea = curAreas->getReference(areaIndex);
					AssignArea(curArea, false);
				}
				SetCurrentArea(GetCurrentArea() + 1);
			}
			return *this;
		}
		class AreasInAreasGrouper {
			List<List<List<ASD::Window*>>> areaGroups;
			int curAreaGroup = 0;
			int curArea = 0;
			AreasInAreasGrouper& AddAreaGroup() { areaGroups.addBlank(); return *this; }
			AreasInAreasGrouper& AddArea() { areaGroups.getReference(curAreaGroup)->addBlank(); return *this; }
		public:
			AreasInAreasGrouper() {
				Reset();
			}
			List<List<ASD::Window*>>* GetCurrentAreaGroupContents() { return areaGroups.getReference(curAreaGroup); }
			List<ASD::Window*>* GetCurrentAreaContents() { return GetCurrentAreaGroupContents()->getReference(curArea); }
			AreasInAreasGrouper& NextArea() { AddArea();  curArea++; return *this; }
			AreasInAreasGrouper& NextAreaGroup() { AddAreaGroup();  curAreaGroup++; AddArea(); curArea = 0; return *this; }
			AreasInAreasGrouper& AddWindow(ASD::Window* windowPtr) { GetCurrentAreaContents()->add(windowPtr); return *this; }
			List<List<List<ASD::Window*>>>* Get() { return &areaGroups; }
			AreasInAreasGrouper& Reset() {
				areaGroups.clear();
				AddAreaGroup().AddArea();
				return *this;
			}
		};
	};
}