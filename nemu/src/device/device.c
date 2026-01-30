#include "common.h"
#ifdef HAS_DEVICE

void init_serial();
void init_timer();
void init_ide();
#ifdef HAS_SDL
void init_vga();
void init_i8042();
#endif

void init_device() {
	init_serial();
	init_timer();
	init_ide();
#ifdef HAS_SDL
	init_vga();
	init_i8042();
#endif
}

#ifndef HAS_SDL
void device_update() {
	/* SDL is unavailable; no device updates */
}

void sdl_clear_event_queue(void) {
	/* SDL is unavailable; no event queue */
}
#endif

#endif
