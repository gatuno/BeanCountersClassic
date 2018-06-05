/*
 * collider.c
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

#include <SDL.h>

#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "collider.h"
#include "sdl2_rect.h"

struct _Collider {
	Uint32 offset_x, offset_y;
	Uint32 size_w, size_h;
	
	Uint32 pitch;
	
	Uint32 *pixels;
};

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

Collider * collider_new_from_file (const char *filename) {
	int fd;
	Collider *new;
	Uint32 temp;
	Uint32 four_int[4];
	int res, g, h;
	int map_size;
	
	fd = open (filename, O_RDONLY);
	
	if (fd < 0) {
		return NULL;
	}
	
	new = (Collider *) malloc (sizeof (Collider));
	
	if (new == NULL) goto bad_load;
	
	/* Empezar a leer los bytes iniciales para saber cuánto reservar en pixeles */
	res = read (fd, &temp, sizeof (temp));
	if (res < 0) goto bad_load_and_free;
	
	/* Versión incorrecta */
	if (temp != 1) goto bad_load_and_free;
	
	/* Consumir el byte extra de alineación de bytes */
	res = read (fd, &temp, sizeof (temp));
	if (res < 0) goto bad_load_and_free;
	
	res = read (fd, four_int, 4 * sizeof (Uint32));
	if (res < 4 * sizeof (Uint32)) goto bad_load_and_free;
	
	new->offset_x = four_int[0];
	new->offset_y = four_int[1];
	new->size_w = four_int[2];
	new->size_h = four_int[3];
	
	if (new->size_w % 32 != 0) {
		new->pitch = (new->size_w / 32) + 2;
	} else {
		new->pitch = (new->size_w / 32) + 1;
	}
	map_size = new->pitch * new->size_h;
	
	new->pixels = (Uint32 *) malloc (sizeof (Uint32) * map_size);
	if (new->pixels == NULL) goto bad_load_and_free;
	
	memset (new->pixels, 0, sizeof (Uint32) * map_size);
	
	for (h = 0; h < new->size_h; h++) {
		for (g = 0; g < new->pitch; g++) {
			res = read (fd, &temp, sizeof (Uint32));
			if (res < 0) goto bad_load_and_free_pixels;
			
			new->pixels[(h * new->pitch) + g] = temp;
		}
	}
	
	close (fd);
	
	return new;
	
bad_load_and_free_pixels:
	free (new->pixels);
bad_load_and_free:
	free (new);
bad_load:
	close (fd);
	
	return NULL;
}

int collider_hittest (Collider *a, int x1, int y1, Collider *b, int x2, int y2) {
	SDL_Rect rect_left, rect_right, result;
	int first = SDL_FALSE;
	
	int g, h;
	int s, x;
	int offset_a_x, offset_a_y;
	int offset_b_x, offset_b_y;
	
	rect_left.x = x1 + a->offset_x; // Sumar los offsets del collider
	rect_left.y = y1 + a->offset_y;
	rect_left.w = a->size_w;
	rect_left.h = a->size_h;
	
	rect_right.x = x2 + b->offset_x;
	rect_right.y = y2 + b->offset_y;
	rect_right.w = b->size_w;
	rect_right.h = b->size_h;
	
	first = SDL_IntersectRect (&rect_left, &rect_right, &result);
	
	if (first == SDL_FALSE) {
		/* Ni siquiera cercas */
		//printf ("Ni siquiera cercas de colision\n");
		return 0;
	}
	
	offset_a_y = result.y - rect_left.y;
	offset_a_x = result.x - rect_left.x;
	offset_b_y = result.y - rect_right.y;
	offset_b_x = result.x - rect_right.x;
	
	Uint32 block_a, block_b;
	
	for (h = result.h; h >= 0; h++) {
		s = result.w;
		x = 0;
		while (s > 0) {
			block_a = collider_extract_block (a, h + offset_a_y, x + offset_a_x, s);
			block_b = collider_extract_block (b, h + offset_b_y, x + offset_b_x, s);
			
			block_a = block_a & block_b;
			
			if (block_a != 0) {
				return 1;
			}
			
			x = x + 32;
			s = s - 32;
		}
	}
	
	return 0;
}

