#pragma once
#include "coords.h"
namespace PROJECT {
	struct dimensions : public coords {
		int width;
		int height;
		void operator=(const dimensions other) {/*'this' is the leftChar30*/
			coords::operator=(other);
			width = other.width;
			height = other.height;
		}
	};
}