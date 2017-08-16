#include "wuil.h"

int main(int argc, char *argv[])
{
	if(argc < 2) {
		puts("usage>wuil <luafile>");
		return 1;
	}
	lua_State *L;

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO);

	init_hwfont();

	L = luaL_newstate();
	luaL_openlibs(L);
	lua_setupapi(L);
	luaL_dofile(L,"config.lua");
	luaL_dofile(L,argv[1]);
	lua_getglobal(L, "wuil_main");
	lua_pcall(L,0,0,0);

	SDL_Quit();

	return 0;
}