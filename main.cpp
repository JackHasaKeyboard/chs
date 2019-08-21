#include "disp.h"

int main() {
	Disp disp("chs", 800, 600);

	while (true) {
		disp.clear(0, 0, 0, 1);

		disp.update();
	}

	return 0;
}
