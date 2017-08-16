#include "wuil.h"
#include <stdio.h>
#include <stdlib.h>
#include "sheet.h"
#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_truetype.h"

char *ttf_dat;
stbtt_fontinfo ttf_ctl;
float ttf_scale;

void init_hwfont(void)
{
	FILE *ttffile = fopen(lib_iniread_s("TTF_FONT"),"rb");

	ttf_dat = (char *)malloc(16*1024*1024);

	fread(ttf_dat, 1, 16*1024*1024, ttffile);

	stbtt_InitFont(&ttf_ctl, (unsigned char *)ttf_dat, stbtt_GetFontOffsetForIndex((unsigned char *)ttf_dat,0));
	ttf_scale = stbtt_ScaleForPixelHeight(&ttf_ctl, 16);

	fclose(ttffile);
}

void putfont8_stb(uint32_t *vram, int xsize, int x, int y, unsigned int c, int ch, int ww, int wh)
{
	int w, h, ox, oy;
	int fx, fy;
	int b;
	uint32_t vc;
	int r1, g1, b1;
	int r2, g2, b2;
	int r3, b3, g3;
	unsigned char *bit = stbtt_GetCodepointBitmap(&ttf_ctl, 0, ttf_scale, ch, &w, &h, &ox, &oy);

	r1 = c >> 16;
	g1 = (c >> 8) & 0xff;
	b1 = c & 0xff;

	for(fy = 0; fy < h; fy++) {
		for(fx = 0; fx < w; fx++) {
			b = bit[fy * w + fx];
			int vy = (y + oy + fy + 16);
			int vx = (x + ox + fx);
			if(vy >= 0 && vy < wh && vx >= 0 && vx < ww) {
				vc = vram[vy * xsize + vx];

				r2 = vc >> 16;
				g2 = (vc >> 8) & 0xff;
				b2 = vc & 0xff;

				r3 = (r1 * b + r2 * (255 - b)) / 255;
				g3 = (g1 * b + g2 * (255 - b)) / 255; 
				b3 = (b1 * b + b2 * (255 - b)) / 255;

				if(r3 > 255) r3 = 255;
				if(g3 > 255) g3 = 255;
				if(b3 > 255) b3 = 255;

				if(b != 0) vram[vy * xsize + vx] = (r3 << 16) | (g3 << 8) | b3;
			}
		}
	}

	stbtt_FreeBitmap(bit,0);
}

void putfonts_ttf(uint32_t *vram, int xsize, int x, int y, int c, char *u, int w, int h)
{
	char *font;
	int l = 1;
	int k,t;
	int langbyte1 = 0;

	uint16_t ucs4[512];
	ulib_utf8_to_ucs4(u,ucs4);

	uint16_t *s = ucs4;
	
	for (; *s != 0x00; s++) {
		if (*s < 0x100 || *s > 0xff20) {
			putfont8_stb(vram, xsize, x, y, c, *s, w, h);
		} else {
			putfont8_stb(vram, xsize, x, y, c, *s, w, h);
			x += 8;
		}
		x += 8;
	}
	return;
}

void boxfill8(uint32_t *vram, int xsize, int c, int x0, int y0, int x1, int y1, int w, int h)
{
	int x, y;
	int tmp;
	if(y0 > y1) {tmp=y0;y0=y1;y1=tmp;}
	if(x0 > x1) {tmp=x0;x0=x1;x1=tmp;}

	for (y = y0; y < y1; y++) {
		for (x = x0; x < x1; x++) {
			if(x >= 0 && x < w && y >= 0 && y < h) vram[y * xsize + x] = c;
		}
	}
	return;
}

void drawline(uint32_t *vram, int xsize, int x0, int y0, int x1, int y1, int col, int w, int h)
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2, e2;
	int a = (dy<dx)? (dx):(dy);
 
	for(;;){
		if(x0 >= 0 && x0 < w && y0 >= 0 && y0 < h) vram[y0 * xsize + x0] = col;
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

SDL_Surface *stbi_createimage(const char *file)
{
	int w,h,bpp;
	uint8_t *pixels = stbi_load(file,&w,&h,&bpp,4);
	return SDL_CreateRGBSurfaceFrom(pixels,w,h,32,w*4,0x000000ff,0x0000ff00,0x00ff0000,0xff000000);
}

uint32_t *stbi_createimage2(char *fname, int *ix, int *iy)
{
	stbi_uc *pixels;
	int bpp, sx, sy;
	int x, y;
	uint32_t *pix;
	pixels = stbi_load(fname, &sx, &sy, &bpp, 4);
	pix = (uint32_t *)malloc(sx*sy*4);

	*ix = sx;
	*iy = sy;
	
	for(y = 0; y < sy; y++) {
		for(x = 0; x < sx; x++) {
			int r, g, b, a;
			r = pixels[(y * sx + x) * 4 + 0];
			g = pixels[(y * sx + x) * 4 + 1];
			b = pixels[(y * sx + x) * 4 + 2];
			a = pixels[(y * sx + x) * 4 + 3];
			pix[y * sx + x]  = (a << 24) | (r << 16) | (g << 8) | b;
		}
	}

	stbi_image_free(pixels);
	return pix;
}

void bitblt8(struct SHEET *sht, uint32_t *graph, int bx, int by, int x, int y, int w, int h)
{
	int vx, vy;
	
	for(vy = 0; vy < by; vy++) {
		for(vx = 0; vx < bx; vx++) {
			int ey = (vy + y);
			int ex = (vx + x);
			if(ex >= 0 && ex < sht->bxsize && ey >= 0 && ey < sht->bysize) sht->buf[ey * sht->bxsize + ex] = alphamix(sht->buf[ey * sht->bxsize + ex],graph[vy * bx + vx] & 0xffffff,graph[vy * bx + vx] >> 24);
		}
	}
}

int alphamix(int c1, int c2, int alpha)
{
	int r, g, b;
	int r1, g1, b1;
	int r2, g2, b2;
	
	r1 = c1 >> 16;
	g1 = (c1 >> 8) & 0xff;
	b1 = c1 & 0xff;
	
	r2 = c2 >> 16;
	g2 = (c2 >> 8) & 0xff;
	b2 = c2 & 0xff;
	
	r = (r2 * alpha + r1 * (255 - alpha)) / 255;   // Bチャンネル.
	g = (g2 * alpha + g1 * (255 - alpha)) / 255;   // Gチャンネル.
	b = (b2 * alpha + b1 * (255 - alpha)) / 255;   // Rチャンネル.
	
	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;
	
	
	return (r << 16) | (g << 8) | b;
}