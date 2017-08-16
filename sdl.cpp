#include "wuil.h"
#include "keytable.h"

typedef struct {
	char shift;
	char caps;
	char num;
	char ctrl;
	char alt;
} key_mode;

Uint8 keycode = 0x00;
key_mode key;

int poll_event(SDL_Event *sdl_event)
{
	if(SDL_PollEvent(sdl_event)) {
		switch (sdl_event->type) {
		case SDL_KEYDOWN:
			if(sdl_event->key.keysym.sym == SDLK_LSHIFT || sdl_event->key.keysym.sym == SDLK_RSHIFT) {
				key.shift = 1;
			} else if(sdl_event->key.keysym.sym == SDLK_CAPSLOCK && key.shift == 1) {
				key.caps ^= ~0;
			} else if(sdl_event->key.keysym.sym == SDLK_NUMLOCKCLEAR) {
				key.num ^= ~0;
			} else if(sdl_event->key.keysym.sym == SDLK_LALT || sdl_event->key.keysym.sym == SDLK_RALT) {
				key.alt = 1;
			} else if(sdl_event->key.keysym.sym == SDLK_LCTRL || sdl_event->key.keysym.sym == SDLK_RCTRL) {
				key.ctrl = 1;
			}
			if(key.shift == 0 && key.caps == 0 && key.num == 0) keycode = keytable0_jp106[sdl_event->key.keysym.scancode] + 0x20;

			if(key.shift == 1 && key.caps == 0 && key.num == 0) keycode = keytable1_jp106[sdl_event->key.keysym.scancode];
			if(key.shift == 0 && key.caps == 1 && key.num == 0) keycode = keytable1_jp106[sdl_event->key.keysym.scancode];
			if(key.shift == 1 && key.caps == 1 && key.num == 0) keycode = keytable1_jp106[sdl_event->key.keysym.scancode];
			


			if(key.shift == 0 && key.caps == 0 && key.num == 1) keycode = keytable2_jp106[sdl_event->key.keysym.scancode] + 0x20;

			if(key.shift == 1 && key.caps == 0 && key.num == 1) keycode = keytable3_jp106[sdl_event->key.keysym.scancode];
			if(key.shift == 0 && key.caps == 1 && key.num == 1) keycode = keytable3_jp106[sdl_event->key.keysym.scancode];
			if(key.shift == 1 && key.caps == 1 && key.num == 1) keycode = keytable3_jp106[sdl_event->key.keysym.scancode];
			
			break;
		case SDL_KEYUP:
			if(sdl_event->key.keysym.sym == SDLK_LSHIFT || sdl_event->key.keysym.sym == SDLK_RSHIFT) {
				key.shift = 0;
			} else if(sdl_event->key.keysym.sym == SDLK_LALT || sdl_event->key.keysym.sym == SDLK_RALT) {
				key.alt = 0;
			} else if(sdl_event->key.keysym.sym == SDLK_LCTRL || sdl_event->key.keysym.sym == SDLK_RCTRL) {
				key.ctrl = 0;
			}
			keycode = 0xff;
			break;
		case SDL_QUIT:
			return 1;
		}
	}

	return 0;
}