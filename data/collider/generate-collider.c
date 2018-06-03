/*
 * beans.c
 * This file is part of Bean Counters Classic
 *
 * Copyright (C) 2018 - Félix Arreola Rodríguez
 *
 * Bean Counters Classic is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Bean Counters Classic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bean Counters Classic; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <SDL.h>
#include <SDL_image.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

typedef struct _Collider {
	Uint32 offset_x, offset_y;
	Uint32 size_w, size_h;
	
	Uint32 pitch;
	
	Uint32 *pixels;
} Collider;

Uint32 collider_extract_block (Collider *c, int y, int x, int size);
static Uint32 collider_htonl (Uint32 byte);

Collider *generate_optimized (Collider *c, int min_x, int min_y, int max_x, int max_y) {
	Collider *c_o;
	Uint32 temp;
	int map_size;
	int bit_pos;
	int s, x, h;
	
	c_o = (Collider *) malloc (sizeof (Collider));

	c_o->size_w = max_x - min_x;
	c_o->size_h = max_y - min_y;
	c_o->offset_x = min_x;
	c_o->offset_y = min_y;
	
	if (c_o->size_w % 32 != 0) {
		c_o->pitch = (c_o->size_w / 32) + 2;
	} else {
		c_o->pitch = (c_o->size_w / 32) + 1;
	}
	map_size = c_o->pitch * c_o->size_h;

	/* Reservar los bytes necesarios */
	c_o->pixels = (Uint32 *) malloc (sizeof (Uint32) * map_size);

	memset (c_o->pixels, 0, sizeof (Uint32) * map_size);
	/* Ejecutar la optimización */
	for (h = min_y; h < max_y; h++) {
		s = max_x - min_x;
		x = min_x;
		
		while (s > 0) {
			temp = collider_extract_block (c, h, x, s);
			
			bit_pos = ((h - min_y) * c_o->pitch) + ((x - min_x) / 32);
			
			c_o->pixels[bit_pos] = temp;
			
			x = x + 32;
			s = s - 32;
		}
	}
	
	free (c->pixels);
	free (c);
	return c_o;
}

Collider *generate_collider (SDL_Surface *imagen) {
	Uint8 *p;
	int bpp, pos;
	int g, h, i;
	Uint32 pixel;
	Uint32 temp;
	Uint8 alpha;
	int min_x, min_y, max_x, max_y;
	int map_size;
	int bit_pos;
	Uint32 bit_a_prender;
	int s, x;
	
	Collider *c;
	
	c = (Collider *) malloc (sizeof (Collider));
	
	c->size_w = imagen->w;
	c->size_h = imagen->h;
	c->offset_x = c->offset_y = 0;
	
	max_x = max_y = 0;
	min_x = imagen->w;
	min_y = imagen->h;
	
	if (c->size_w % 32 != 0) {
		c->pitch = (c->size_w / 32) + 2;
	} else {
		c->pitch = (c->size_w / 32) + 1;
	}
	map_size = c->pitch * c->size_h;
	
	/* Reservar los bytes necesarios */
	c->pixels = (Uint32 *) malloc (sizeof (Uint32) * map_size);
	
	memset (c->pixels, 0, sizeof (Uint32) * map_size);
	
	bpp = imagen->format->BytesPerPixel;
	for (h = 0; h < imagen->h; h++) {
		for (g = 0; g < imagen->w; g++) {
			pos = (h * imagen->pitch) / bpp + g;
			
			pixel = ((Uint32 *) imagen->pixels)[pos];
			
			temp = pixel & imagen->format->Amask;
			temp = temp >> imagen->format->Ashift;
			temp = temp << imagen->format->Aloss;
			
			alpha = (Uint8) temp;
			
			bit_pos = (h * c->pitch) + (g / 32);
			
			if (alpha != 0) {
				bit_a_prender = 2147483648u >> (g % 32);
				
				c->pixels[bit_pos] = c->pixels[bit_pos] | bit_a_prender;
				
				if (h < min_y) {
					min_y = h;
				}
				
				if (h > max_y) {
					max_y = h;
				}
				
				if (g < min_x) {
					min_x = g;
				}
				
				if (g > max_x) {
					max_x = g;
				}
			}
		}
	}
	max_x++;
	max_y++;
	
	if (min_x != 0 || min_y != 0 || max_x != imagen->w || max_y != imagen->h) {
		return generate_optimized (c, min_x, min_y, max_x, max_y);
	}
	
	return c;
}

Uint32 collider_extract_block (Collider *c, int y, int x, int size) {
	int bit_pos;
	int align;
	Uint32 res;
	
	bit_pos = c->pitch * y + (x / 32);
	align = 32 - (x % 32);
	
	if (align == 32) {
		res = c->pixels[bit_pos];
	} else {
		res = c->pixels[bit_pos] << (32 - align);
		res = res | (c->pixels[bit_pos + 1] >> align);
	}
	
	if (size < 32) {
		/* Quitar los bits sobrantes */
		res = res ^ (res & ((1 << (32 - size)) - 1));
	}
	
	return res;
}

static Uint32 collider_htonl (Uint32 byte) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return byte;
#else
	unsigned char *data = (unsigned char *) &byte;
	
	return (data[3]<<0) | (data[2]<<8) | (data[1]<<16) | (data[0]<<24);
#endif
}

void save_collider (Collider *c, int fd) {
	Uint32 w, g, h;
	int res;
	w = 1; /* Número de versión */
	res = write (fd, &w, sizeof (w));
	
	w = 0; /* Sin uso, solo para alinear bytes */
	res = write (fd, &w, sizeof (w));
	
	w = c->offset_x;
	res = write (fd, &w, sizeof (w));
	
	w = c->offset_y;
	res = write (fd, &w, sizeof (w));
	
	w = c->size_w;
	write (fd, &w, sizeof (w));
	
	w = c->size_h;
	write (fd, &w, sizeof (w));
	
	for (h = 0; h < c->size_h; h++) {
		for (g = 0; g < c->pitch; g++) {
			w = c->pixels[(h * c->pitch) + g];
			write (fd, &w, sizeof (w));
		}
	}
}

int main (int argc, char *argv[]) {
	int g;
	SDL_Surface *image;
	Collider *c;
	
	/* Inicializar el Video SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf (stderr,
			"Error: Can't initialize the video subsystem\n"
			"The error returned by SDL is:\n"
			"%s\n", SDL_GetError());
		exit (1);
	}
	
	if (argc < 3) {
		fprintf (stderr, "Need two arguments, png-file output-file\n");
		
		exit (1);
	}
	
	image = IMG_Load (argv[1]);
	
	if (image == NULL) {
		fprintf (stderr,
			"Failed to load data file:\n"
			"%s\n"
			"The error returned by SDL is:\n"
			"%s\n", argv[1], SDL_GetError());
		SDL_Quit ();
		exit (1);
	}
	
	c = generate_collider (image);
	
	int fd;
	
	fd = open (argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0777);
	
	if (fd < 0) {
		fprintf (stderr, "Couldn't open %s for file writing\n", argv[2]);
		
		exit (1);
	}
	
	save_collider (c, fd);
	
	close (fd);
	
	return 0;
}

