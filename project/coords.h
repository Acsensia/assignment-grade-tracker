#pragma once
namespace PROJECT {
	struct coords {
		int x;
		int y;
		void operator=(const coords other) {/*'this' is the leftChar30*/
			x = other.x;
			y = other.y;
		}
	};
}