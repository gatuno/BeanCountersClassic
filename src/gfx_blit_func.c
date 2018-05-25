/* 

SDL_gfxBlitFunc.c: custom blitters

Copyright (C) 2001-2012  Andreas Schiffler

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.

Andreas Schiffler -- aschiffler at ferzkopp dot net

*/

#include <SDL.h>
#include <SDL_video.h>

/*!
\brief Unwrap RGBA values from a pixel using mask, shift and loss for surface.
*/
#define GFX_RGBA_FROM_PIXEL(pixel, fmt, r, g, b, a)				\
{									\
r = ((pixel&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss; 		\
g = ((pixel&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss; 		\
b = ((pixel&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss; 		\
a = ((pixel&fmt->Amask)>>fmt->Ashift)<<fmt->Aloss;	 	\
}

/*!
\brief Disassemble buffer pointer into a pixel and separate RGBA values.
*/
#define GFX_DISASSEMBLE_RGBA(buf, bpp, fmt, pixel, r, g, b, a)			   \
do {									   \
pixel = *((Uint32 *)(buf));			   		   \
GFX_RGBA_FROM_PIXEL(pixel, fmt, r, g, b, a);			   \
pixel &= ~fmt->Amask;						   \
} while(0)

/*!
\brief Wrap a pixel from RGBA values using mask, shift and loss for surface.
*/
#define GFX_PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a)				\
{									\
pixel = ((r>>fmt->Rloss)<<fmt->Rshift)|				\
((g>>fmt->Gloss)<<fmt->Gshift)|				\
((b>>fmt->Bloss)<<fmt->Bshift)|				\
((a<<fmt->Aloss)<<fmt->Ashift);				\
}

/*!
\brief Assemble pixel into buffer pointer from separate RGBA values.
*/
#define GFX_ASSEMBLE_RGBA(buf, bpp, fmt, r, g, b, a)			\
{									\
Uint32 pixel;					\
\
GFX_PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a);	\
*((Uint32 *)(buf)) = pixel;			\
}

/*!
\brief Blend the RGB values of two pixels based on a source alpha value.
*/
#define GFX_ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB)	\
do {						\
dR = (((sR-dR)*(A))/255)+dR;		\
dG = (((sG-dG)*(A))/255)+dG;		\
dB = (((sB-dB)*(A))/255)+dB;		\
} while(0)

#define PERFECT_ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB, dA) \
do { \
	dR = ((sR * sA) + (dR * dA * (255 - sA))); \
	dG = ((sG * sA) + (dG * dA * (255 - sA))); \
	dB = ((sB * sA) + (dB * dA * (255 - sA))); \
	dA = (sA + (dA * (255 - sA))); \
	dR = dR / dA; \
	dG = dG / dA; \
	dB = dB / dA; \
	dA = (dA / 255); \
} while (0)

/*!
\brief 4-times unrolled DUFFs loop.

This is a very useful loop for optimizing blitters.
*/
#define GFX_DUFFS_LOOP4(pixel_copy_increment, width)			\
{ int n = (width+3)/4;							\
switch (width & 3) {						\
case 0: do {	pixel_copy_increment;				\
case 3:		pixel_copy_increment;				\
case 2:		pixel_copy_increment;				\
case 1:		pixel_copy_increment;				\
} while ( --n > 0 );					\
}								\
}

typedef struct {
	Uint8    *s_pixels;
	int       s_width;
	int       s_height;
	int       s_skip;
	Uint8    *d_pixels;
	int       d_width;
	int       d_height;
	int       d_skip;
	void     *aux_data;
	SDL_PixelFormat *src;
	Uint8    *table;
	SDL_PixelFormat *dst;
} SDL_gfxBlitInfo;

/*!  
\brief Alpha adjustment table for custom blitter.

The table provides values for a modified, non-linear 
transfer function which maintain brightness.

*/
const unsigned int GFX_ALPHA_ADJUST_ARRAY[256] = {
	0,  /* 0 */
	15,  /* 1 */
	22,  /* 2 */
	27,  /* 3 */
	31,  /* 4 */
	35,  /* 5 */
	39,  /* 6 */
	42,  /* 7 */
	45,  /* 8 */
	47,  /* 9 */
	50,  /* 10 */
	52,  /* 11 */
	55,  /* 12 */
	57,  /* 13 */
	59,  /* 14 */
	61,  /* 15 */
	63,  /* 16 */
	65,  /* 17 */
	67,  /* 18 */
	69,  /* 19 */
	71,  /* 20 */
	73,  /* 21 */
	74,  /* 22 */
	76,  /* 23 */
	78,  /* 24 */
	79,  /* 25 */
	81,  /* 26 */
	82,  /* 27 */
	84,  /* 28 */
	85,  /* 29 */
	87,  /* 30 */
	88,  /* 31 */
	90,  /* 32 */
	91,  /* 33 */
	93,  /* 34 */
	94,  /* 35 */
	95,  /* 36 */
	97,  /* 37 */
	98,  /* 38 */
	99,  /* 39 */
	100,  /* 40 */
	102,  /* 41 */
	103,  /* 42 */
	104,  /* 43 */
	105,  /* 44 */
	107,  /* 45 */
	108,  /* 46 */
	109,  /* 47 */
	110,  /* 48 */
	111,  /* 49 */
	112,  /* 50 */
	114,  /* 51 */
	115,  /* 52 */
	116,  /* 53 */
	117,  /* 54 */
	118,  /* 55 */
	119,  /* 56 */
	120,  /* 57 */
	121,  /* 58 */
	122,  /* 59 */
	123,  /* 60 */
	124,  /* 61 */
	125,  /* 62 */
	126,  /* 63 */
	127,  /* 64 */
	128,  /* 65 */
	129,  /* 66 */
	130,  /* 67 */
	131,  /* 68 */
	132,  /* 69 */
	133,  /* 70 */
	134,  /* 71 */
	135,  /* 72 */
	136,  /* 73 */
	137,  /* 74 */
	138,  /* 75 */
	139,  /* 76 */
	140,  /* 77 */
	141,  /* 78 */
	141,  /* 79 */
	142,  /* 80 */
	143,  /* 81 */
	144,  /* 82 */
	145,  /* 83 */
	146,  /* 84 */
	147,  /* 85 */
	148,  /* 86 */
	148,  /* 87 */
	149,  /* 88 */
	150,  /* 89 */
	151,  /* 90 */
	152,  /* 91 */
	153,  /* 92 */
	153,  /* 93 */
	154,  /* 94 */
	155,  /* 95 */
	156,  /* 96 */
	157,  /* 97 */
	158,  /* 98 */
	158,  /* 99 */
	159,  /* 100 */
	160,  /* 101 */
	161,  /* 102 */
	162,  /* 103 */
	162,  /* 104 */
	163,  /* 105 */
	164,  /* 106 */
	165,  /* 107 */
	165,  /* 108 */
	166,  /* 109 */
	167,  /* 110 */
	168,  /* 111 */
	168,  /* 112 */
	169,  /* 113 */
	170,  /* 114 */
	171,  /* 115 */
	171,  /* 116 */
	172,  /* 117 */
	173,  /* 118 */
	174,  /* 119 */
	174,  /* 120 */
	175,  /* 121 */
	176,  /* 122 */
	177,  /* 123 */
	177,  /* 124 */
	178,  /* 125 */
	179,  /* 126 */
	179,  /* 127 */
	180,  /* 128 */
	181,  /* 129 */
	182,  /* 130 */
	182,  /* 131 */
	183,  /* 132 */
	184,  /* 133 */
	184,  /* 134 */
	185,  /* 135 */
	186,  /* 136 */
	186,  /* 137 */
	187,  /* 138 */
	188,  /* 139 */
	188,  /* 140 */
	189,  /* 141 */
	190,  /* 142 */
	190,  /* 143 */
	191,  /* 144 */
	192,  /* 145 */
	192,  /* 146 */
	193,  /* 147 */
	194,  /* 148 */
	194,  /* 149 */
	195,  /* 150 */
	196,  /* 151 */
	196,  /* 152 */
	197,  /* 153 */
	198,  /* 154 */
	198,  /* 155 */
	199,  /* 156 */
	200,  /* 157 */
	200,  /* 158 */
	201,  /* 159 */
	201,  /* 160 */
	202,  /* 161 */
	203,  /* 162 */
	203,  /* 163 */
	204,  /* 164 */
	205,  /* 165 */
	205,  /* 166 */
	206,  /* 167 */
	206,  /* 168 */
	207,  /* 169 */
	208,  /* 170 */
	208,  /* 171 */
	209,  /* 172 */
	210,  /* 173 */
	210,  /* 174 */
	211,  /* 175 */
	211,  /* 176 */
	212,  /* 177 */
	213,  /* 178 */
	213,  /* 179 */
	214,  /* 180 */
	214,  /* 181 */
	215,  /* 182 */
	216,  /* 183 */
	216,  /* 184 */
	217,  /* 185 */
	217,  /* 186 */
	218,  /* 187 */
	218,  /* 188 */
	219,  /* 189 */
	220,  /* 190 */
	220,  /* 191 */
	221,  /* 192 */
	221,  /* 193 */
	222,  /* 194 */
	222,  /* 195 */
	223,  /* 196 */
	224,  /* 197 */
	224,  /* 198 */
	225,  /* 199 */
	225,  /* 200 */
	226,  /* 201 */
	226,  /* 202 */
	227,  /* 203 */
	228,  /* 204 */
	228,  /* 205 */
	229,  /* 206 */
	229,  /* 207 */
	230,  /* 208 */
	230,  /* 209 */
	231,  /* 210 */
	231,  /* 211 */
	232,  /* 212 */
	233,  /* 213 */
	233,  /* 214 */
	234,  /* 215 */
	234,  /* 216 */
	235,  /* 217 */
	235,  /* 218 */
	236,  /* 219 */
	236,  /* 220 */
	237,  /* 221 */
	237,  /* 222 */
	238,  /* 223 */
	238,  /* 224 */
	239,  /* 225 */
	240,  /* 226 */
	240,  /* 227 */
	241,  /* 228 */
	241,  /* 229 */
	242,  /* 230 */
	242,  /* 231 */
	243,  /* 232 */
	243,  /* 233 */
	244,  /* 234 */
	244,  /* 235 */
	245,  /* 236 */
	245,  /* 237 */
	246,  /* 238 */
	246,  /* 239 */
	247,  /* 240 */
	247,  /* 241 */
	248,  /* 242 */
	248,  /* 243 */
	249,  /* 244 */
	249,  /* 245 */
	250,  /* 246 */
	250,  /* 247 */
	251,  /* 248 */
	251,  /* 249 */
	252,  /* 250 */
	252,  /* 251 */
	253,  /* 252 */
	253,  /* 253 */
	254,  /* 254 */
	255   /* 255 */
};

/*!
\brief Internal blitter using adjusted destination alpha during RGBA->RGBA blits.

Performs the blit based on the 'info' structure and applies the transfer function
to the destination 'a' values.

\param info The blit info to use.
*/
void _SDL_gfxBlitBlitterRGBA(SDL_gfxBlitInfo * info)
{
	int       width = info->d_width;
	int       height = info->d_height;
	Uint8    *src = info->s_pixels;
	int       srcskip = info->s_skip;
	Uint8    *dst = info->d_pixels;
	int       dstskip = info->d_skip;
	SDL_PixelFormat *srcfmt = info->src;
	SDL_PixelFormat *dstfmt = info->dst;
	Uint8       srcbpp = srcfmt->BytesPerPixel;
	Uint8       dstbpp = dstfmt->BytesPerPixel;

	while (height--) {
		GFX_DUFFS_LOOP4( {
			Uint32 pixel;
			unsigned sR;
			unsigned sG;
			unsigned sB;
			unsigned sA;
			unsigned dR;
			unsigned dG;
			unsigned dB;
			unsigned dA;
			unsigned sAA;
			GFX_DISASSEMBLE_RGBA(src, srcbpp, srcfmt, pixel, sR, sG, sB, sA);
			GFX_DISASSEMBLE_RGBA(dst, dstbpp, dstfmt, pixel, dR, dG, dB, dA);
			//sAA=GFX_ALPHA_ADJUST_ARRAY[sA & 255];
			sAA = sA & 255;
			GFX_ALPHA_BLEND(sR, sG, sB, sAA, dR, dG, dB);
			dA |= sAA;
			GFX_ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
			src += srcbpp; dst += dstbpp;
		}, width);
		src += srcskip;
		dst += dstskip;
	}
}

/*!
\brief Internal blitter setup wrapper for RGBA->RGBA blits.

Sets up the blitter info based on the 'src' and 'dst' surfaces and rectangles.

\param src The source surface.
\param srcrect The source rectangle.
\param dst The destination surface.
\param dstrect The destination rectangle.

\returns Returns 1 if blit was performed, 0 otherwise.
*/
int _SDL_gfxBlitRGBACall(SDL_Surface * src, SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect)
{
	/*
	* Set up source and destination buffer pointers, then blit 
	*/
	if (srcrect->w && srcrect->h) {
		SDL_gfxBlitInfo info;

		/*
		* Set up the blit information 
		*/
#if (SDL_MINOR_VERSION == 3)
		info.s_pixels = (Uint8 *) src->pixels               + (Uint16) srcrect->y * src->pitch + (Uint16) srcrect->x * src->format->BytesPerPixel;
#else
		info.s_pixels = (Uint8 *) src->pixels + src->offset + (Uint16) srcrect->y * src->pitch + (Uint16) srcrect->x * src->format->BytesPerPixel;
#endif
		info.s_width = srcrect->w;
		info.s_height = srcrect->h;
		info.s_skip = (int)(src->pitch - info.s_width * src->format->BytesPerPixel);
#if (SDL_MINOR_VERSION == 3)
		info.d_pixels = (Uint8 *) dst->pixels               + (Uint16) dstrect->y * dst->pitch + (Uint16) dstrect->x * dst->format->BytesPerPixel;
#else
		info.d_pixels = (Uint8 *) dst->pixels + dst->offset + (Uint16) dstrect->y * dst->pitch + (Uint16) dstrect->x * dst->format->BytesPerPixel;
#endif
		info.d_width = dstrect->w;
		info.d_height = dstrect->h;
		info.d_skip = (int)(dst->pitch - info.d_width * dst->format->BytesPerPixel);
		info.aux_data = NULL;
		info.src = src->format;
		info.table = NULL;
		info.dst = dst->format;

		/*
		* Run the actual software blitter 
		*/
		_SDL_gfxBlitBlitterRGBA(&info);
		return 1;
	}

	return (0);
}

/*!
\brief Blitter for RGBA->RGBA blits with alpha adjustment.

Verifies the input 'src' and 'dst' surfaces and rectangles and performs blit.
The destination clip rectangle is honored.

\param src The source surface.
\param srcrect The source rectangle.
\param dst The destination surface.
\param dstrect The destination rectangle.

\returns Returns 1 if blit was performed, 0 otherwise, or -1 if an error occured.
*/
int SDL_gfxBlitRGBA(SDL_Surface * src, SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect)
{
	SDL_Rect  sr, dr;
	int       srcx, srcy, w, h;

	/*
	* Make sure the surfaces aren't locked 
	*/
	if (!src || !dst) {
		SDL_SetError("SDL_UpperBlit: passed a NULL surface");
		return (-1);
	}
	if ((src->locked) || (dst->locked)) {
		SDL_SetError("Surfaces must not be locked during blit");
		return (-1);
	}

	/*
	* If the destination rectangle is NULL, use the entire dest surface 
	*/
	if (dstrect == NULL) {
		dr.x = dr.y = 0;
		dr.w = dst->w;
		dr.h = dst->h;
	} else {
		dr = *dstrect;
	}

	/*
	* Clip the source rectangle to the source surface 
	*/
	if (srcrect) {
		int       maxw, maxh;

		srcx = srcrect->x;
		w = srcrect->w;
		if (srcx < 0) {
			w += srcx;
			dr.x -= srcx;
			srcx = 0;
		}
		maxw = src->w - srcx;
		if (maxw < w)
			w = maxw;

		srcy = srcrect->y;
		h = srcrect->h;
		if (srcy < 0) {
			h += srcy;
			dr.y -= srcy;
			srcy = 0;
		}
		maxh = src->h - srcy;
		if (maxh < h)
			h = maxh;

	} else {
		srcx = srcy = 0;
		w = src->w;
		h = src->h;
	}

	/*
	* Clip the destination rectangle against the clip rectangle 
	*/
	{
		SDL_Rect *clip = &dst->clip_rect;
		int       dx, dy;

		dx = clip->x - dr.x;
		if (dx > 0) {
			w -= dx;
			dr.x += dx;
			srcx += dx;
		}
		dx = dr.x + w - clip->x - clip->w;
		if (dx > 0)
			w -= dx;

		dy = clip->y - dr.y;
		if (dy > 0) {
			h -= dy;
			dr.y += dy;
			srcy += dy;
		}
		dy = dr.y + h - clip->y - clip->h;
		if (dy > 0)
			h -= dy;
	}

	if (w > 0 && h > 0) {
		sr.x = srcx;
		sr.y = srcy;
		sr.w = dr.w = w;
		sr.h = dr.h = h;
		return (_SDL_gfxBlitRGBACall(src, &sr, dst, &dr));
	}

	return 0;
}

