#pragma once
#include "../../asd/ASDWindow.h"
#include "../List.h"
namespace PROJECT {
	class MessageContents {
	public:
		List <ASD::Message::Line> data;
		MessageContents& addLine(string line, ASD::Message::LineCenteringOption centeringOption = ASD::Message::LineCenteringOption::center) {
			data.add({ line, centeringOption });
			return *this;
		}
	};
}