#include "wuil.h"
#include <string.h>

typedef struct {
    int mmask;
    int m1st;
} utf8table;

int ulib_utf8_to_ucs4(char *utf8, uint16_t *ucs4)
{
	static utf8table utf8tbl[] = {{ 0x7f, 0x000 },
								  { 0x1f, 0x0c0 },
								  { 0x0f, 0x0e0 },
								  { 0x07, 0x0f0 },
								  { 0x03, 0x0f8 },
								  { 0x01, 0x0fc }};
	int slen;
	int i;
	unsigned long ucs4_c, work;

	// utf8 -> ucs4‚É•ÏŠ·
	while(*utf8 != 0) {
		// •¶š”‚ğ”»’è‚·‚é
		work = (unsigned char)*utf8;
		for (slen = 1; slen < 6; slen++) {
			if (work < utf8tbl[slen].m1st) {
				break;
			}
		}

		ucs4_c = (unsigned char)*utf8++;
		ucs4_c &= utf8tbl[slen-1].mmask;
		for (i = 1; i < slen; i++) {
			work = (unsigned char)*utf8++;
			work &= 0x3f;
			ucs4_c <<= 6;
			ucs4_c |= work;
		}

		// ‚à‚µucs2ˆÈ~‚Ì•¶š‚È‚çA ‚É‘‚«Š·‚¦
		if (ucs4_c > 0xffff) {
			ucs4_c = 0x25a1;
		}
		*ucs4 = (uint16_t)ucs4_c;
		ucs4++;
	}
	*ucs4 = 0;
	return slen;
}

int ulib_strlen(char *s)
{
	uint16_t ucs4[512];
	int i;
	int k = 0;
	ulib_utf8_to_ucs4(s,ucs4);

	for(i = 0; ucs4[i] != 0; i++) {
		int tmp = k;
		if(ucs4[i] < 0x100 || ucs4[i] > 0xff20) k++;
		else k += 2;
	}

	return k;
}

#define is_hex(c) \
	(c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F')

#define hex2num(hex, num, min, max, up) \
	if ((hex>=min) && (hex<=max)) num = num * 16 + up + hex - min

int lib_atoi16(const char *str)
{
	int val = 0;
	int sign;
	if (str) {
		int i = (*str == '-' || *str == '+') ? 1 : 0;
		sign = (*str == '-') ? -1 : 1;
		while (is_hex(str[i]))
		{
			hex2num(str[i], val, '0', '9', 0);
			hex2num(str[i], val, 'a', 'f', 10);
			hex2num(str[i], val, 'A', 'F', 10);
			i++;
		}
	}
	return sign * val;
}

int lib_iniread_n(char *name)
{
	int n;
	char buf[256];
	FILE *fp;
	char *p;
	
	fp = fopen("config.ini","rt");
	buf[0] = 0;
	for(;strncmp(buf,name,strlen(name)) != 0;)fgets(buf,256,fp);
	for(p = buf;*p != '$'; p++);
	p++;
	n = lib_atoi16(p);
	
	fclose(fp);
	
	return n;
}

char *lib_iniread_s(char *name)
{
	char buf[256];
	char *s;
	FILE *fp;
	char *p;
	int i;
	
	s = (char *)malloc(256);
	
	fp = fopen("config.ini","rt");
	buf[0] = 0;
	for(;strncmp(buf,name,strlen(name)) != 0;)fgets(buf,256,fp);
	for(p = buf;*p != '\"'; p++);
	
	for(p++,i=0;*p != '\"';p++,i++) s[i] = *p;
	s[i] = 0;
	
	fclose(fp);
	
	return s;
}