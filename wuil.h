#include "lua.hpp"
#include "SDL.h"

void lua_setupapi(lua_State *L);
int poll_event(SDL_Event *sdl_event);

void init_hwfont(void);
void boxfill8(uint32_t *vram, int xsize, int c, int x0, int y0, int x1, int y1, int w, int h);
void putfonts_ttf(uint32_t *vram, int xsize, int x, int y, int c, char *u, int w, int h);
void drawline(uint32_t *vram, int xsize, int x0, int y0, int x1, int y1, int col, int w, int h);
void bitblt8(struct SHEET *sht, uint32_t *graph, int bx, int by, int x, int y, int w, int h);
int alphamix(int c1, int c2, int alpha);

int ulib_utf8_to_ucs4(char *utf8, uint16_t *ucs4);
int ulib_strlen(char *s);

int lib_iniread_n(char *name);
char *lib_iniread_s(char *name);

SDL_Surface *stbi_createimage(const char *file);
uint32_t *stbi_createimage2(char *fname, int *ix, int *iy);