#include<d3d9.h>
#include<d3dx9.h>
#include"debug_font.h"
#include"input.h"

static bool log_use = false;

void debug_logDraw(void) {
#if defined(_DEBUG) || defined(DEBUG)

	if (Keyboard_IsTrigger(DIK_0)) {
		if (log_use) {
			log_use = false;
		}
		else {
			log_use = true;
		}
	}

	if (log_use) {
		int y = 24;
	}

#endif
}