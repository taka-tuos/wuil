#include "wuil.h"
#include "sheet.h"
#include <time.h>

struct SDL_SW {
	SDL_Window *window;
	SDL_Surface *surface;
	SDL_Renderer *renderer;
	struct SHTCTL *shtctl;
	struct SHEET *sheet;
};

struct W_OBJECT {
	int x,y,w,h;
	void *data;
	struct SHEET *sht;
	int flag;
};

int wx = 16,wy = 16;
SDL_Event sdl_event;

int mx = 0,my = 0;

int createwin(lua_State *L) {
	size_t len;
	int w = (int)lua_tonumber(L, 1);
    int h = (int)lua_tonumber(L, 2);
    char *title = (char *)lua_tolstring(L, 3, &len);
	struct SDL_SW *sdlsw = (struct SDL_SW *)malloc(sizeof(struct SDL_SW));

	sdlsw->window = SDL_CreateWindow(title,wx,wy,w,h,SDL_WINDOW_OPENGL);
	sdlsw->surface = SDL_GetWindowSurface(sdlsw->window);
	sdlsw->shtctl = shtctl_init((uint32_t *)sdlsw->surface->pixels,sdlsw->surface->w,sdlsw->surface->h);
	sdlsw->sheet = sheet_alloc(sdlsw->shtctl);
	sdlsw->renderer = SDL_CreateRenderer(sdlsw->window,-1,SDL_RENDERER_ACCELERATED);
	sheet_setbuf(sdlsw->sheet,(uint32_t *)malloc(sdlsw->surface->w*sdlsw->surface->h*4),sdlsw->surface->w,sdlsw->surface->h,-1);
	sheet_updown(sdlsw->sheet,0);
	sheet_slide(sdlsw->sheet,0,0);

	boxfill8(sdlsw->sheet->buf,sdlsw->surface->w,0xc6c6c6,0,0,w,h,sdlsw->surface->w,sdlsw->surface->h);
	//boxfill8((uint32_t *)sdlsw->surface->pixels,sdlsw->surface->w,0xc6c6c6,0,0,w,h);
	SDL_UpdateWindowSurface(sdlsw->window);

	SDL_SetWindowIcon(sdlsw->window,stbi_createimage("wuil.png"));

	//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,title,title,sdlsw->window);

	lua_pop(L, lua_gettop(L));
	lua_pushlightuserdata(L,sdlsw);

	return 1;
}

int pollevent(lua_State *L) {
	if(poll_event(&sdl_event)) exit(0);

	lua_pop(L, lua_gettop(L));
	return 0;
}

int reflesh(lua_State *L) {
	struct SDL_SW *sdlsw = (struct SDL_SW *)lua_touserdata(L, 1);
	sheet_refresh(sdlsw->sheet,0,0,sdlsw->surface->w,sdlsw->surface->h);
	SDL_UpdateWindowSurface(sdlsw->window);

	lua_pop(L, lua_gettop(L));

	return 0;
}

int destroywin(lua_State *L) {
	struct SDL_SW *sdlsw = (struct SDL_SW *)lua_touserdata(L, 1);
	SDL_DestroyWindow(sdlsw->window);

	lua_pop(L, lua_gettop(L));

	return 0;
}

int gdi_drawstring(lua_State *L) {
	size_t len;
	struct SDL_SW *sdlsw = (struct SDL_SW *)lua_touserdata(L, 1);
	int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
	int c = (int)lua_tonumber(L, 4);
    char *title = (char *)lua_tolstring(L, 5, &len);

	putfonts_ttf(sdlsw->sheet->buf,sdlsw->surface->w,x,y,c,title, sdlsw->surface->w, sdlsw->surface->h);
	//putfonts_ttf((uint32_t *)sdlsw->surface->pixels,sdlsw->surface->w,x,y,c,title);

	lua_pop(L, lua_gettop(L));

	return 0;
}

int gdi_drawline(lua_State *L) {
	struct SDL_SW *sdlsw = (struct SDL_SW *)lua_touserdata(L, 1);
	int x0 = (int)lua_tonumber(L, 2);
    int y0 = (int)lua_tonumber(L, 3);
	int x1 = (int)lua_tonumber(L, 4);
    int y1 = (int)lua_tonumber(L, 5);
	int c = (int)lua_tonumber(L, 6);

	drawline(sdlsw->sheet->buf,sdlsw->surface->w,x0,y0,x1,y1,c,sdlsw->surface->w,sdlsw->surface->h);
	//drawline((uint32_t *)sdlsw->surface->pixels,sdlsw->surface->w,x0,y0,x1,y1,c);

	lua_pop(L, lua_gettop(L));

	return 0;
}

int gdi_drawbox(lua_State *L) {
	struct SDL_SW *sdlsw = (struct SDL_SW *)lua_touserdata(L, 1);
	int x0 = (int)lua_tonumber(L, 2);
    int y0 = (int)lua_tonumber(L, 3);
	int x1 = (int)lua_tonumber(L, 4);
    int y1 = (int)lua_tonumber(L, 5);
	int c = (int)lua_tonumber(L, 6);

	boxfill8(sdlsw->sheet->buf,sdlsw->surface->w,c,x0,y0,x1,y1,sdlsw->surface->w,sdlsw->surface->h);
	//boxfill8((uint32_t *)sdlsw->surface->pixels,sdlsw->surface->w,c,x0,y0,x1,y1);

	lua_pop(L, lua_gettop(L));

	return 0;
}

int setwindowtitle(lua_State *L) {
	size_t len;
	struct SDL_SW *sdlsw = (struct SDL_SW *)lua_touserdata(L, 1);
    char *title = (char *)lua_tolstring(L, 2, &len);

	SDL_SetWindowTitle(sdlsw->window,title);

	lua_pop(L, lua_gettop(L));

	return 0;
}

int getticks(lua_State *L) {
	lua_pop(L, lua_gettop(L));
	lua_pushnumber(L,clock());

	return 1;
}

int createbutton(lua_State *L) {
	size_t len;
	struct W_OBJECT *obj = (struct W_OBJECT *)malloc(sizeof(struct W_OBJECT));
	struct SDL_SW *sdlsw = (struct SDL_SW *)lua_touserdata(L, 1);
	struct SHEET *sht = sdlsw->sheet;
	int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
	int w = (int)lua_tonumber(L, 4);
    int h = (int)lua_tonumber(L, 5);
	char *label = (char *)lua_tolstring(L, 6, &len);
	int tx, ty;
	
	obj->x = x;
	obj->y = y;
	obj->w = w;
	obj->h = h;
	obj->sht = sht;
	obj->data = label;
	obj->flag = 0;
	
	tx = (w / 2) - ((ulib_strlen(label) * 8) / 2);
	ty = (h / 2) - 8;
	
	/*boxfill8(sht->buf,sht->bxsize,0x000000,x,y,x+w,y+h, sht->bxsize, sht->bysize);
	boxfill8(sht->buf,sht->bxsize,0xc6c6c6,x+2,y+2,x+w-2,y+h-2, sht->bxsize, sht->bysize);*/
	boxfill8(sht->buf, sht->bxsize, 0x000000, x       , y       , x + w    , y + h    , sht->bxsize, sht->bysize);
	boxfill8(sht->buf, sht->bxsize, 0xFFFFFF, x       , y       , x + w - 1, y + h - 1, sht->bxsize, sht->bysize);
	boxfill8(sht->buf, sht->bxsize, 0x848484, x    + 1, y    + 1, x + w - 1, y + h - 1, sht->bxsize, sht->bysize);
	boxfill8(sht->buf, sht->bxsize, 0xC6C6C6, x    + 1, y    + 1, x + w - 2, y + h - 2, sht->bxsize, sht->bysize);
	putfonts_ttf(sht->buf,sht->bxsize, tx+x, ty+y, 0, label, sht->bxsize, sht->bysize);
	
	//sheet_refresh(sht,x,y,x+w,y+h);
	
	lua_pop(L, lua_gettop(L));
	
	lua_pushlightuserdata(L,obj);
	
	return 1;
}

int checkbutton(lua_State *L) {
	int bb = 0;
	struct W_OBJECT *obj = (struct W_OBJECT *)lua_touserdata(L, 1);
	struct SHEET *sht = obj->sht;
	int vx, vy;
	int x,y,w,h;
	int tx, ty;
	int mx,my;

	int clb = SDL_GetMouseState(&mx,&my);
	int cl = clb & SDL_BUTTON_LEFT;

	
	vx = mx - obj->sht->vx0;
	vy = my - obj->sht->vy0;
	
	x = obj->x;
	y = obj->y;
	w = obj->w;
	h = obj->h;
	
	tx = (w / 2) - ((ulib_strlen((char*)obj->data) * 8) / 2);
	ty = (h / 2) - 8;
	
	/*boxfill8(sht->buf,sht->bxsize,0x000000,x,y,x+w,y+h, sht->bxsize, sht->bysize);
	boxfill8(sht->buf,sht->bxsize,0xc6c6c6,x+2,y+2,x+w-2,y+h-2, sht->bxsize, sht->bysize);*/
	boxfill8(sht->buf, sht->bxsize, 0x000000, x       , y       , x + w    , y + h    , sht->bxsize, sht->bysize);
	boxfill8(sht->buf, sht->bxsize, 0xFFFFFF, x       , y       , x + w - 1, y + h - 1, sht->bxsize, sht->bysize);
	boxfill8(sht->buf, sht->bxsize, 0x848484, x    + 1, y    + 1, x + w - 1, y + h - 1, sht->bxsize, sht->bysize);
	boxfill8(sht->buf, sht->bxsize, 0xC6C6C6, x    + 1, y    + 1, x + w - 2, y + h - 2, sht->bxsize, sht->bysize);
	putfonts_ttf(sht->buf,sht->bxsize, tx+x, ty+y, 0x000000, (char *)obj->data, sht->bxsize, sht->bysize);
	bb = 0;

	if(vx >= x && vy >= y && vx < x + w && vy < y + h && cl != 0) {
		/*boxfill8(sht->buf,sht->bxsize,0xffffff,x,y,x+w,y+h, sht->bxsize, sht->bysize);
		boxfill8(sht->buf,sht->bxsize,0x000000,x+2,y+2,x+w-2,y+h-2, sht->bxsize, sht->bysize);*/
		boxfill8(sht->buf, sht->bxsize, 0x000000, x      , y      , x + w    , y + h    , sht->bxsize, sht->bysize);
		boxfill8(sht->buf, sht->bxsize, 0xFFFFFF, x   + 1, y   + 1, x + w    , y + h    , sht->bxsize, sht->bysize);
		boxfill8(sht->buf, sht->bxsize, 0x848484, x   + 1, y   + 1, x + w - 1, y + h - 1, sht->bxsize, sht->bysize);
		boxfill8(sht->buf, sht->bxsize, 0xC6C6C6, x   + 2, y   + 2, x + w - 1, y + h - 1, sht->bxsize, sht->bysize);
		putfonts_ttf(sht->buf,sht->bxsize, tx+x+2, ty+y+2, 0x000000, (char *)obj->data, sht->bxsize, sht->bysize);
		if(obj->flag == 0) bb = 1;
		obj->flag = 1;
	} else {
		obj->flag = 0;
	}
	
	lua_pop(L, lua_gettop(L));
	
	lua_pushnumber(L,bb);
	
	return 1;
}

int gdi_bitblt(lua_State *L)
{
	size_t len;
	struct SDL_SW *sdlsw = (struct SDL_SW *)lua_touserdata(L, 1);
	struct SHEET *sht = sdlsw->sheet;
	int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
	char *file = (char *)lua_tolstring(L, 4, &len);
	uint32_t *graph;
	int w, h;
	
	graph = stbi_createimage2(file,&w,&h);

	bitblt8(sht,graph,w,h,x,y,sdlsw->surface->w,sdlsw->surface->h);

	lua_pop(L, lua_gettop(L));
	
	lua_pushnumber(L,w);
	lua_pushnumber(L,h);

	return 2;
}

int gdi_clear(lua_State *L)
{
	struct SDL_SW *sdlsw = (struct SDL_SW *)lua_touserdata(L, 1);
	boxfill8(sdlsw->sheet->buf,sdlsw->surface->w,0xc6c6c6,0,0,sdlsw->surface->w,sdlsw->surface->h,sdlsw->surface->w,sdlsw->surface->h);
	return 0;
}

void lua_setupapi(lua_State *L)
{
	lua_register(L,"wuil_createwindow",createwin);
	lua_register(L,"wuil_destroywindow",destroywin);
	lua_register(L,"wuil_setwindowtitile",setwindowtitle);
	lua_register(L,"wuil_createbutton",createbutton);
	lua_register(L,"wuil_checkbutton",checkbutton);
	lua_register(L,"wuil_getticks",getticks);
	lua_register(L,"wuil_pollevent",pollevent);
	lua_register(L,"wuil_reflesh",reflesh);
	lua_register(L,"wuilgdi_drawstring",gdi_drawstring);
	lua_register(L,"wuilgdi_drawline",gdi_drawline);
	lua_register(L,"wuilgdi_drawbox",gdi_drawbox);
	lua_register(L,"wuilgdi_bitblt",gdi_bitblt);
	lua_register(L,"wuilgdi_clear",gdi_clear);
}
