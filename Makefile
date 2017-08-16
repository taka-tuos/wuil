TARGET		= wuil
OBJS_TARGET	= wuil.o sheet.o sdl.o misc.o graphic.o glue.o

CFLAGS += -O2 -g `sdl2-config --cflags` `pkg-config lua5.1-c++ --cflags` -fpermissive
LIBS += `pkg-config lua5.1-c++ --libs` `sdl2-config --libs` -lm -lc -lstdc++ 

include Makefile.in
