#ifndef __MDNIE_TABLE_H__
#define __MDNIE_TABLE_H__

#include "mdnie.h"

/* 2013.10.18 */
static unsigned short tune_dynamic_gallery[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x008c,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0090, 0x0080,	/*DE egth*/
	0x0092, 0x0030,	/*DE pe*/
	0x0093, 0x0080,	/*DE pf*/
	0x0094, 0x0080,	/*DE pb*/
	0x0095, 0x0030,	/*DE ne*/
	0x0096, 0x0080,	/*DE nf*/
	0x0097, 0x0080,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1404,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x0d93,	/*CC lut r  16 144*/
	0x0022, 0x1aa5,	/*CC lut r  32 160*/
	0x0023, 0x29b7,	/*CC lut r  48 176*/
	0x0024, 0x39c8,	/*CC lut r  64 192*/
	0x0025, 0x4bd8,	/*CC lut r  80 208*/
	0x0026, 0x5de6,	/*CC lut r  96 224*/
	0x0027, 0x6ff4,	/*CC lut r 112 240*/
	0x0028, 0x81ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_dynamic_ui[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x0088,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1804,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x0d93,	/*CC lut r  16 144*/
	0x0022, 0x1aa5,	/*CC lut r  32 160*/
	0x0023, 0x29b7,	/*CC lut r  48 176*/
	0x0024, 0x39c8,	/*CC lut r  64 192*/
	0x0025, 0x4bd8,	/*CC lut r  80 208*/
	0x0026, 0x5de6,	/*CC lut r  96 224*/
	0x0027, 0x6ff4,	/*CC lut r 112 240*/
	0x0028, 0x81ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_dynamic_video[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x008c,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0092, 0x0080,	/*DE pe*/
	0x0093, 0x0080,	/*DE pf*/
	0x0094, 0x0080,	/*DE pb*/
	0x0095, 0x0080,	/*DE ne*/
	0x0096, 0x0080,	/*DE nf*/
	0x0097, 0x0080,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1404,	/*CS weight grayTH*/
	0x00e1, 0xff00,	/*SCR RrCr*/
	0x00e2, 0x00ff,	/*SCR RgCg*/
	0x00e3, 0x00ff,	/*SCR RbCb*/
	0x00e4, 0x00ff,	/*SCR GrMr*/
	0x00e5, 0xff00,	/*SCR GgMg*/
	0x00e6, 0x00ff,	/*SCR GbMb*/
	0x00e7, 0x00ff,	/*SCR BrYr*/
	0x00e8, 0x00ff,	/*SCR BgYg*/
	0x00e9, 0xff00,	/*SCR BbYb*/
	0x00ea, 0x00ff,	/*SCR KrWr*/
	0x00eb, 0x00ff,	/*SCR KgWg*/
	0x00ec, 0x00ff,	/*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x0d93,	/*CC lut r  16 144*/
	0x0022, 0x1aa5,	/*CC lut r  32 160*/
	0x0023, 0x29b7,	/*CC lut r  48 176*/
	0x0024, 0x39c8,	/*CC lut r  64 192*/
	0x0025, 0x4bd8,	/*CC lut r  80 208*/
	0x0026, 0x5de6,	/*CC lut r  96 224*/
	0x0027, 0x6ff4,	/*CC lut r 112 240*/
	0x0028, 0x81ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_dynamic_vt[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x008e,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0005,	/*FA cs1 | de8 dnr4 hdr2 fa1*/
	0x0039, 0x0080,	/*FA dnrWeight*/
	0x0080, 0x0fff,	/*DNR dirTh*/
	0x0081, 0x19ff,	/*DNR dirnumTh decon7Th*/
	0x0082, 0xff16,	/*DNR decon5Th maskTh*/
	0x0083, 0x0000,	/*DNR blTh*/
	0x0092, 0x00e0,	/*DE pe*/
	0x0093, 0x00e0,	/*DE pf*/
	0x0094, 0x00e0,	/*DE pb*/
	0x0095, 0x00e0,	/*DE ne*/
	0x0096, 0x00e0,	/*DE nf*/
	0x0097, 0x00e0,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0010,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1a04,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r  0*/
	0x0021, 0x0d93,	/*CC lut r  16 144*/
	0x0022, 0x1aa5,	/*CC lut r  32 160*/
	0x0023, 0x29b7,	/*CC lut r  48 176*/
	0x0024, 0x39c8,	/*CC lut r  64 192*/
	0x0025, 0x4bd8,	/*CC lut r  80 208*/
	0x0026, 0x5de6,	/*CC lut r  96 224*/
	0x0027, 0x6ff4,	/*CC lut r 112 240*/
	0x0028, 0x81ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_movie_gallery[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a0,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00e1, 0xd297, /*SCR RrCr*/
	0x00e2, 0x19f4, /*SCR RgCg*/
	0x00e3, 0x16e7, /*SCR RbCb*/
	0x00e4, 0x70db, /*SCR GrMr*/
	0x00e5, 0xf024, /*SCR GgMg*/
	0x00e6, 0x22e5, /*SCR GbMb*/
	0x00e7, 0x22f0, /*SCR BrYr*/
	0x00e8, 0x11f2, /*SCR BgYg*/
	0x00e9, 0xe140, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_movie_ui[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a0,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00e1, 0xd297, /*SCR RrCr*/
	0x00e2, 0x19f4, /*SCR RgCg*/
	0x00e3, 0x16e7, /*SCR RbCb*/
	0x00e4, 0x70db, /*SCR GrMr*/
	0x00e5, 0xf024, /*SCR GgMg*/
	0x00e6, 0x22e5, /*SCR GbMb*/
	0x00e7, 0x22f0, /*SCR BrYr*/
	0x00e8, 0x11f2, /*SCR BgYg*/
	0x00e9, 0xe140, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_movie_video[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a0,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0092, 0x0000,	/*DE pe*/
	0x0093, 0x0000,	/*DE pf*/
	0x0094, 0x0000,	/*DE pb*/
	0x0095, 0x0000,	/*DE ne*/
	0x0096, 0x0000,	/*DE nf*/
	0x0097, 0x0000,	/*DE nb*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1004,	/*CS weight grayTH*/
	0x00e1, 0xd297, /*SCR RrCr*/
	0x00e2, 0x19f4, /*SCR RgCg*/
	0x00e3, 0x16e7, /*SCR RbCb*/
	0x00e4, 0x70db, /*SCR GrMr*/
	0x00e5, 0xf024, /*SCR GgMg*/
	0x00e6, 0x22e5, /*SCR GbMb*/
	0x00e7, 0x22f0, /*SCR BrYr*/
	0x00e8, 0x11f2, /*SCR BgYg*/
	0x00e9, 0xe140, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_movie_vt[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a6,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0005,	/*FA cs1 | de8 dnr4 hdr2 fa1*/
	0x0039, 0x0080,	/*FA dnrWeight*/
	0x0080, 0x0fff,	/*DNR dirTh*/
	0x0081, 0x19ff,	/*DNR dirnumTh decon7Th*/
	0x0082, 0xff16,	/*DNR decon5Th maskTh*/
	0x0083, 0x0000,	/*DNR blTh*/
	0x0092, 0x0042,	/*DE pe*/
	0x0093, 0x0042,	/*DE pf*/
	0x0094, 0x0042,	/*DE pb*/
	0x0095, 0x0042,	/*DE ne*/
	0x0096, 0x0042,	/*DE nf*/
	0x0097, 0x0042,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0010,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1004,	/*CS weight grayTH*/
	0x00e1, 0xd297, /*SCR RrCr*/
	0x00e2, 0x19f4, /*SCR RgCg*/
	0x00e3, 0x16e7, /*SCR RbCb*/
	0x00e4, 0x70db, /*SCR GrMr*/
	0x00e5, 0xf024, /*SCR GgMg*/
	0x00e6, 0x22e5, /*SCR GbMb*/
	0x00e7, 0x22f0, /*SCR BrYr*/
	0x00e8, 0x11f2, /*SCR BgYg*/
	0x00e9, 0xe140, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_standard_gallery[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x008c,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0090, 0x0080,	/*DE egth*/
	0x0092, 0x0030,	/*DE pe*/
	0x0093, 0x0060,	/*DE pf*/
	0x0094, 0x0060,	/*DE pb*/
	0x0095, 0x0030,	/*DE ne*/
	0x0096, 0x0060,	/*DE nf*/
	0x0097, 0x0060,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_standard_ui[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x0088,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1604,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_standard_video[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x008c,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0092, 0x0060,	/*DE pe*/
	0x0093, 0x0060,	/*DE pf*/
	0x0094, 0x0060,	/*DE pb*/
	0x0095, 0x0060,	/*DE ne*/
	0x0096, 0x0060,	/*DE nf*/
	0x0097, 0x0060,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x00e1, 0xff00,	/*SCR RrCr*/
	0x00e2, 0x00ff,	/*SCR RgCg*/
	0x00e3, 0x00ff,	/*SCR RbCb*/
	0x00e4, 0x00ff,	/*SCR GrMr*/
	0x00e5, 0xff00,	/*SCR GgMg*/
	0x00e6, 0x00ff,	/*SCR GbMb*/
	0x00e7, 0x00ff,	/*SCR BrYr*/
	0x00e8, 0x00ff,	/*SCR BgYg*/
	0x00e9, 0xff00,	/*SCR BbYb*/
	0x00ea, 0x00ff,	/*SCR KrWr*/
	0x00eb, 0x00ff,	/*SCR KgWg*/
	0x00ec, 0x00ff,	/*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_standard_vt[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x008e,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0005,	/*FA cs1 | de8 dnr4 hdr2 fa1*/
	0x0039, 0x0080,	/*FA dnrWeight*/
	0x0080, 0x0fff,	/*DNR dirTh*/
	0x0081, 0x19ff,	/*DNR dirnumTh decon7Th*/
	0x0082, 0xff16,	/*DNR decon5Th maskTh*/
	0x0083, 0x0000,	/*DNR blTh*/
	0x0092, 0x00c0,	/*DE pe*/
	0x0093, 0x00c0,	/*DE pf*/
	0x0094, 0x00c0,	/*DE pb*/
	0x0095, 0x00c0,	/*DE ne*/
	0x0096, 0x00c0,	/*DE nf*/
	0x0097, 0x00c0,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0010,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1804,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_natural_gallery[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a4,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0090, 0x0080,	/*DE egth*/
	0x0092, 0x0030,	/*DE pe*/
	0x0093, 0x0060,	/*DE pf*/
	0x0094, 0x0060,	/*DE pb*/
	0x0095, 0x0030,	/*DE ne*/
	0x0096, 0x0060,	/*DE nf*/
	0x0097, 0x0060,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00e1, 0xf01d, /*SCR RrCr*/
	0x00e2, 0x13f1, /*SCR RgCg*/
	0x00e3, 0x0fde, /*SCR RbCb*/
	0x00e4, 0x14fb, /*SCR GrMr*/
	0x00e5, 0xe717, /*SCR GgMg*/
	0x00e6, 0x00e7, /*SCR GbMb*/
	0x00e7, 0x1cf0, /*SCR BrYr*/
	0x00e8, 0x0cf2, /*SCR BgYg*/
	0x00e9, 0xe827, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_natural_ui[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a0,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00e1, 0xf01d, /*SCR RrCr*/
	0x00e2, 0x13f1, /*SCR RgCg*/
	0x00e3, 0x0fde, /*SCR RbCb*/
	0x00e4, 0x14fb, /*SCR GrMr*/
	0x00e5, 0xe717, /*SCR GgMg*/
	0x00e6, 0x00e7, /*SCR GbMb*/
	0x00e7, 0x1cf0, /*SCR BrYr*/
	0x00e8, 0x0cf2, /*SCR BgYg*/
	0x00e9, 0xe827, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_natural_video[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a4,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0090, 0x0080,	/*DE egth*/
	0x0092, 0x0030,	/*DE pe*/
	0x0093, 0x0060,	/*DE pf*/
	0x0094, 0x0060,	/*DE pb*/
	0x0095, 0x0030,	/*DE ne*/
	0x0096, 0x0060,	/*DE nf*/
	0x0097, 0x0060,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1004,	/*CS weight grayTH*/
	0x00e1, 0xf01d, /*SCR RrCr*/
	0x00e2, 0x13f1, /*SCR RgCg*/
	0x00e3, 0x0fde, /*SCR RbCb*/
	0x00e4, 0x14fb, /*SCR GrMr*/
	0x00e5, 0xe717, /*SCR GgMg*/
	0x00e6, 0x00e7, /*SCR GbMb*/
	0x00e7, 0x1cf0, /*SCR BrYr*/
	0x00e8, 0x0cf2, /*SCR BgYg*/
	0x00e9, 0xe827, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_natural_vt[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a6,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0005,	/*FA cs1 | de8 dnr4 hdr2 fa1*/
	0x0039, 0x0080,	/*FA dnrWeight*/
	0x0080, 0x0fff,	/*DNR dirTh*/
	0x0081, 0x19ff,	/*DNR dirnumTh decon7Th*/
	0x0082, 0xff16,	/*DNR decon5Th maskTh*/
	0x0083, 0x0000,	/*DNR blTh*/
	0x0092, 0x00c0,	/*DE pe*/
	0x0093, 0x00c0,	/*DE pf*/
	0x0094, 0x00c0,	/*DE pb*/
	0x0095, 0x00c0,	/*DE ne*/
	0x0096, 0x00c0,	/*DE nf*/
	0x0097, 0x00c0,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0010,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1004,	/*CS weight grayTH*/
	0x00e1, 0xf01d, /*SCR RrCr*/
	0x00e2, 0x13f1, /*SCR RgCg*/
	0x00e3, 0x0fde, /*SCR RbCb*/
	0x00e4, 0x14fb, /*SCR GrMr*/
	0x00e5, 0xe717, /*SCR GgMg*/
	0x00e6, 0x00e7, /*SCR GbMb*/
	0x00e7, 0x1cf0, /*SCR BrYr*/
	0x00e8, 0x0cf2, /*SCR BgYg*/
	0x00e9, 0xe827, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_camera[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x000c,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0090, 0x0080,	/*DE egth*/
	0x0092, 0x0030,	/*DE pe*/
	0x0093, 0x0060,	/*DE pf*/
	0x0094, 0x0060,	/*DE pb*/
	0x0095, 0x0030,	/*DE ne*/
	0x0096, 0x0060,	/*DE nf*/
	0x0097, 0x0060,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_dynamic_browser[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x0088,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1404,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x0d93,	/*CC lut r  16 144*/
	0x0022, 0x1aa5,	/*CC lut r  32 160*/
	0x0023, 0x29b7,	/*CC lut r  48 176*/
	0x0024, 0x39c8,	/*CC lut r  64 192*/
	0x0025, 0x4bd8,	/*CC lut r  80 208*/
	0x0026, 0x5de6,	/*CC lut r  96 224*/
	0x0027, 0x6ff4,	/*CC lut r 112 240*/
	0x0028, 0x81ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_dynamic_ebook[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x0088,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1404,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x0d93,	/*CC lut r  16 144*/
	0x0022, 0x1aa5,	/*CC lut r  32 160*/
	0x0023, 0x29b7,	/*CC lut r  48 176*/
	0x0024, 0x39c8,	/*CC lut r  64 192*/
	0x0025, 0x4bd8,	/*CC lut r  80 208*/
	0x0026, 0x5de6,	/*CC lut r  96 224*/
	0x0027, 0x6ff4,	/*CC lut r 112 240*/
	0x0028, 0x81ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_standard_browser[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x0088,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_standard_ebook[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x0088,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_natural_browser[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a0,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00e1, 0xf01d, /*SCR RrCr*/
	0x00e2, 0x13f1, /*SCR RgCg*/
	0x00e3, 0x0fde, /*SCR RbCb*/
	0x00e4, 0x14fb, /*SCR GrMr*/
	0x00e5, 0xe717, /*SCR GgMg*/
	0x00e6, 0x00e7, /*SCR GbMb*/
	0x00e7, 0x1cf0, /*SCR BrYr*/
	0x00e8, 0x0cf2, /*SCR BgYg*/
	0x00e9, 0xe827, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_natural_ebook[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a0,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00e1, 0xf01d, /*SCR RrCr*/
	0x00e2, 0x13f1, /*SCR RgCg*/
	0x00e3, 0x0fde, /*SCR RbCb*/
	0x00e4, 0x14fb, /*SCR GrMr*/
	0x00e5, 0xe717, /*SCR GgMg*/
	0x00e6, 0x00e7, /*SCR GbMb*/
	0x00e7, 0x1cf0, /*SCR BrYr*/
	0x00e8, 0x0cf2, /*SCR BgYg*/
	0x00e9, 0xe827, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_movie_browser[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a0,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00e1, 0xd297, /*SCR RrCr*/
	0x00e2, 0x19f4, /*SCR RgCg*/
	0x00e3, 0x16e7, /*SCR RbCb*/
	0x00e4, 0x70db, /*SCR GrMr*/
	0x00e5, 0xf024, /*SCR GgMg*/
	0x00e6, 0x22e5, /*SCR GbMb*/
	0x00e7, 0x22f0, /*SCR BrYr*/
	0x00e8, 0x11f2, /*SCR BgYg*/
	0x00e9, 0xe140, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_movie_ebook[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a0,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00e1, 0xd297, /*SCR RrCr*/
	0x00e2, 0x19f4, /*SCR RgCg*/
	0x00e3, 0x16e7, /*SCR RbCb*/
	0x00e4, 0x70db, /*SCR GrMr*/
	0x00e5, 0xf024, /*SCR GgMg*/
	0x00e6, 0x22e5, /*SCR GbMb*/
	0x00e7, 0x22f0, /*SCR BrYr*/
	0x00e8, 0x11f2, /*SCR BgYg*/
	0x00e9, 0xe140, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f8, /*SCR KgWg*/
	0x00ec, 0x00f1, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_auto_ui[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x0088,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1604,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_auto_video[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00ac,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0092, 0x0060,	/*DE pe*/
	0x0093, 0x0060,	/*DE pf*/
	0x0094, 0x0060,	/*DE pb*/
	0x0095, 0x0060,	/*DE ne*/
	0x0096, 0x0060,	/*DE nf*/
	0x0097, 0x0060,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x00e1, 0xff00,	/*SCR RrCr*/
	0x00e2, 0x1cff,	/*SCR RgCg*/
	0x00e3, 0x1cff,	/*SCR RbCb*/
	0x00e4, 0x00ff,	/*SCR GrMr*/
	0x00e5, 0xff00,	/*SCR GgMg*/
	0x00e6, 0x00ff,	/*SCR GbMb*/
	0x00e7, 0x00ff,	/*SCR BrYr*/
	0x00e8, 0x00ff,	/*SCR BgYg*/
	0x00e9, 0xff00,	/*SCR BbYb*/
	0x00ea, 0x00ff,	/*SCR KrWr*/
	0x00eb, 0x00ff,	/*SCR KgWg*/
	0x00ec, 0x00ff,	/*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_auto_gallery[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00ac,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0090, 0x0080,	/*DE egth*/
	0x0092, 0x0030,	/*DE pe*/
	0x0093, 0x0060,	/*DE pf*/
	0x0094, 0x0060,	/*DE pb*/
	0x0095, 0x0030,	/*DE ne*/
	0x0096, 0x0060,	/*DE nf*/
	0x0097, 0x0060,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x00e1, 0xff00,	/*SCR RrCr*/
	0x00e2, 0x1cff,	/*SCR RgCg*/
	0x00e3, 0x1cff,	/*SCR RbCb*/
	0x00e4, 0x00ff,	/*SCR GrMr*/
	0x00e5, 0xff00,	/*SCR GgMg*/
	0x00e6, 0x00ff,	/*SCR GbMb*/
	0x00e7, 0x00ff,	/*SCR BrYr*/
	0x00e8, 0x00ff,	/*SCR BgYg*/
	0x00e9, 0xff00,	/*SCR BbYb*/
	0x00ea, 0x00ff,	/*SCR KrWr*/
	0x00eb, 0x00ff,	/*SCR KgWg*/
	0x00ec, 0x00ff,	/*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_auto_vt[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x008e,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0005,	/*FA cs1 | de8 dnr4 hdr2 fa1*/
	0x0039, 0x0080,	/*FA dnrWeight*/
	0x0080, 0x0fff,	/*DNR dirTh*/
	0x0081, 0x19ff,	/*DNR dirnumTh decon7Th*/
	0x0082, 0xff16,	/*DNR decon5Th maskTh*/
	0x0083, 0x0000,	/*DNR blTh*/
	0x0092, 0x00c0,	/*DE pe*/
	0x0093, 0x00c0,	/*DE pf*/
	0x0094, 0x00c0,	/*DE pb*/
	0x0095, 0x00c0,	/*DE ne*/
	0x0096, 0x00c0,	/*DE nf*/
	0x0097, 0x00c0,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0010,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1804,	/*CS weight grayTH*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_auto_browser[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a8,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x00e1, 0xff00,	/*SCR RrCr*/
	0x00e2, 0x1cff,	/*SCR RgCg*/
	0x00e3, 0x1cff,	/*SCR RbCb*/
	0x00e4, 0x00ff,	/*SCR GrMr*/
	0x00e5, 0xff00,	/*SCR GgMg*/
	0x00e6, 0x00ff,	/*SCR GbMb*/
	0x00e7, 0x00ff,	/*SCR BrYr*/
	0x00e8, 0x00ff,	/*SCR BgYg*/
	0x00e9, 0xff00,	/*SCR BbYb*/
	0x00ea, 0x00ff,	/*SCR KrWr*/
	0x00eb, 0x00ff,	/*SCR KgWg*/
	0x00ec, 0x00ff,	/*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_auto_ebook[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x00a8,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x00e1, 0xff00, /*SCR RrCr*/
	0x00e2, 0x00ff, /*SCR RgCg*/
	0x00e3, 0x00ff, /*SCR RbCb*/
	0x00e4, 0x00ff, /*SCR GrMr*/
	0x00e5, 0xff00, /*SCR GgMg*/
	0x00e6, 0x00ff, /*SCR GbMb*/
	0x00e7, 0x00ff, /*SCR BrYr*/
	0x00e8, 0x00ff, /*SCR BgYg*/
	0x00e9, 0xff00, /*SCR BbYb*/
	0x00ea, 0x00ff, /*SCR KrWr*/
	0x00eb, 0x00f7, /*SCR KgWg*/
	0x00ec, 0x00e6, /*SCR KbWb*/
	0x0000, 0x0001,	/*BANK 1*/
	0x001f, 0x0080,	/*CC chsel strength*/
	0x0020, 0x0000,	/*CC lut r   0*/
	0x0021, 0x1090,	/*CC lut r  16 144*/
	0x0022, 0x20a0,	/*CC lut r  32 160*/
	0x0023, 0x30b0,	/*CC lut r  48 176*/
	0x0024, 0x40c0,	/*CC lut r  64 192*/
	0x0025, 0x50d0,	/*CC lut r  80 208*/
	0x0026, 0x60e0,	/*CC lut r  96 224*/
	0x0027, 0x70f0,	/*CC lut r 112 240*/
	0x0028, 0x80ff,	/*CC lut r 128 255*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

static unsigned short tune_auto_camera[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0008, 0x002c,	/*Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000,	/*FA cs1 de8 hdr2 fa1*/
	0x0090, 0x0080,	/*DE egth*/
	0x0092, 0x0030,	/*DE pe*/
	0x0093, 0x0060,	/*DE pf*/
	0x0094, 0x0060,	/*DE pb*/
	0x0095, 0x0030,	/*DE ne*/
	0x0096, 0x0060,	/*DE nf*/
	0x0097, 0x0060,	/*DE nb*/
	0x0098, 0x1000,	/*DE max ratio*/
	0x0099, 0x0100,	/*DE min ratio*/
	0x00b0, 0x1010,	/*CS hg ry*/
	0x00b1, 0x1010,	/*CS hg gc*/
	0x00b2, 0x1010,	/*CS hg bm*/
	0x00b3, 0x1204,	/*CS weight grayTH*/
	0x00e1, 0xff00,	/*SCR RrCr*/
	0x00e2, 0x1cff,	/*SCR RgCg*/
	0x00e3, 0x1cff,	/*SCR RbCb*/
	0x00e4, 0x00ff,	/*SCR GrMr*/
	0x00e5, 0xff00,	/*SCR GgMg*/
	0x00e6, 0x00ff,	/*SCR GbMb*/
	0x00e7, 0x00ff,	/*SCR BrYr*/
	0x00e8, 0x00ff,	/*SCR BgYg*/
	0x00e9, 0xff00,	/*SCR BbYb*/
	0x00ea, 0x00ff,	/*SCR KrWr*/
	0x00eb, 0x00ff,	/*SCR KgWg*/
	0x00ec, 0x00ff,	/*SCR KbWb*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000
};

#ifdef CONFIG_FB_S5P_MDNIE_HIJACK
// Yank555.lu : hijack profile (use natural as base)
static unsigned short tune_hijack[] = {
	0x0000, 0x0000, /*( 0, 1) BANK 0*/
	0x0008, 0x00ac, /*( 2, 3) Dither8 UC4 ABC2 CP1 | CC8 MCM4 SCR2 SCC1 | CS8 DE4 DNR2 HDR1*/
	0x0030, 0x0000, /*( 4, 5) FA cs1 de8 hdr2 fa1*/
	0x0092, 0x0020, /*( 6, 7) DE pe*/
	0x0093, 0x0020, /*( 8, 9) DE pf*/
	0x0094, 0x0020, /*(10,11) DE pb*/
	0x0095, 0x0020, /*(12,13) DE ne*/
	0x0096, 0x0020, /*(14,15) DE nf*/
	0x0097, 0x0020, /*(16,17) DE nb*/
	0x0098, 0x1000, /*(18,19) DE max ratio*/
	0x0099, 0x0100, /*(20,21) DE min ratio*/
	0x00b0, 0x1010, /*(22,23) CS hg ry*/
	0x00b1, 0x1010, /*(24,25) CS hg gc*/
	0x00b2, 0x1010, /*(26,27) CS hg bm*/
	0x00b3, 0x1804, /*(28,29) CS weight grayTH*/
	0x00e1, 0xd6ac, /*(30,31) SCR RrCr*/
	0x00e2, 0x32ff, /*(32,33) SCR RgCg*/
	0x00e3, 0x2ef0, /*(34,35) SCR RbCb*/
	0x00e4, 0xa5fa, /*(36,37) SCR GrMr*/
	0x00e5, 0xff4d, /*(38,39) SCR GgMg*/
	0x00e6, 0x59ff, /*(40,41) SCR GbMb*/
	0x00e7, 0x00ff, /*(42,43) SCR BrYr*/
	0x00e8, 0x00fb, /*(44,45) SCR BgYg*/
	0x00e9, 0xff61, /*(46,47) SCR BbYb*/
	0x00ea, 0x00ff, /*(48,49) SCR KrWr*/
	0x00eb, 0x00fa, /*(50,51) SCR KgWg*/
	0x00ec, 0x00f8, /*(52,53) SCR KbWb*/
	0x0000, 0x0001, /*(54,55) BANK 1*/
	0x001f, 0x0080, /*(56,57) CC chsel strength*/
	0x0020, 0x0000, /*(58,59) CC lut r	 0*/
	0x0021, 0x1090, /*(60,61) CC lut r	16 144*/
	0x0022, 0x20a0, /*(62,63) CC lut r	32 160*/
	0x0023, 0x30b0, /*(64,65) CC lut r	48 176*/
	0x0024, 0x40c0, /*(66,67) CC lut r	64 192*/
	0x0025, 0x50d0, /*(68,69) CC lut r	80 208*/
	0x0026, 0x60e0, /*(70,71) CC lut r	96 224*/
	0x0027, 0x70f0, /*(72,73) CC lut r 112 240*/
	0x0028, 0x80ff, /*(74,75) CC lut r 128 255*/
	0x00ff, 0x0000, /*(76,77) Mask Release*/
	END_SEQ, 0x0000 /*(78,79)*/
};
#endif

struct mdnie_tuning_info tuning_table[CABC_MAX][MODE_MAX][SCENARIO_MAX] = {
	{
		{
			{"dynamic_ui",		(unsigned short *)&tune_dynamic_ui},
			{"dynamic_video",	(unsigned short *)&tune_dynamic_video},
			{"dynamic_video",	(unsigned short *)&tune_dynamic_video},
			{"dynamic_video",	(unsigned short *)&tune_dynamic_video},
			{"camera",		tune_camera},
			{"dynamic_ui",		(unsigned short *)&tune_dynamic_ui},
			{"dynamic_gallery",	(unsigned short *)&tune_dynamic_gallery},
			{"dynamic_vt",		(unsigned short *)&tune_dynamic_vt},
			{"dynamic_browser",	(unsigned short *)&tune_dynamic_browser},
			{"dynamic_ebook",	(unsigned short *)&tune_dynamic_ebook},
			{"email",		(unsigned short *)&tune_dynamic_ui},
		}, {
			{"standard_ui",		(unsigned short *)&tune_standard_ui},
			{"standard_video",	(unsigned short *)&tune_standard_video},
			{"standard_video",	(unsigned short *)&tune_standard_video},
			{"standard_video",	(unsigned short *)&tune_standard_video},
			{"camera",		tune_camera},
			{"standard_ui",		(unsigned short *)&tune_standard_ui},
			{"standard_gallery",	(unsigned short *)&tune_standard_gallery},
			{"standard_vt",		(unsigned short *)&tune_standard_vt},
			{"standard_browser",	(unsigned short *)&tune_standard_browser},
			{"standard_ebook",	(unsigned short *)&tune_standard_ebook},
			{"email",		(unsigned short *)&tune_standard_ui},
		}, {
			{"natural_ui",		(unsigned short *)&tune_natural_ui},
			{"natural_video",	(unsigned short *)&tune_natural_video},
			{"natural_video",	(unsigned short *)&tune_natural_video},
			{"natural_video",	(unsigned short *)&tune_natural_video},
			{"camera",		tune_camera},
			{"natural_ui",		(unsigned short *)&tune_natural_ui},
			{"natural_gallery",	(unsigned short *)&tune_natural_gallery},
			{"natural_vt",		(unsigned short *)&tune_natural_vt},
			{"natural_browser",	(unsigned short *)&tune_natural_browser},
			{"natural_ebook",	(unsigned short *)&tune_natural_ebook},
			{"email",		(unsigned short *)&tune_natural_ui},
		}, {
			{"movie_ui",		(unsigned short *)&tune_movie_ui},
			{"movie_video",		(unsigned short *)&tune_movie_video},
			{"movie_video",		(unsigned short *)&tune_movie_video},
			{"movie_video",		(unsigned short *)&tune_movie_video},
			{"camera",		tune_camera},
			{"movie_ui",		(unsigned short *)&tune_movie_ui},
			{"movie_gallery",	(unsigned short *)&tune_movie_gallery},
			{"movie_vt",		(unsigned short *)&tune_movie_vt},
			{"movie_browser",	(unsigned short *)&tune_movie_browser},
			{"movie_ebook",		(unsigned short *)&tune_movie_ebook},
			{"email",		(unsigned short *)&tune_movie_ui},
		}, {
			{"auto_ui",		(unsigned short *)&tune_auto_ui},
			{"auto_video",		(unsigned short *)&tune_auto_video},
			{"auto_video",		(unsigned short *)&tune_auto_video},
			{"auto_video",		(unsigned short *)&tune_auto_video},
			{"auto_camera",		(unsigned short *)&tune_auto_camera},
			{"auto_ui",		(unsigned short *)&tune_auto_ui},
			{"auto_gallery",	(unsigned short *)&tune_auto_gallery},
			{"auto_vt",		(unsigned short *)&tune_auto_vt},
			{"auto_browser",	(unsigned short *)&tune_auto_browser},
			{"auto_ebook",		(unsigned short *)&tune_auto_ebook},
			{"email",		(unsigned short *)&tune_auto_ui},
		}
	}
};

#endif/* __MDNIE_TABLE_H__ */
