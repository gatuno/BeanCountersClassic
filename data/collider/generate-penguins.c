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

#include <SDL.h>
#include <SDL_image.h>

#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gfx_blit_func.h"
#include "savepng.h"

#define RANDOM(x) ((int) (x ## .0 * rand () / (RAND_MAX + 1.0)))

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif

/* Imágenes de los pingüinos */
enum {
	IMG_PENGUIN_1_BACK,
	IMG_PENGUIN_1_COLOR,
	IMG_PENGUIN_1_FRONT,
	
	IMG_PENGUIN_2_BACK,
	IMG_PENGUIN_2_COLOR,
	IMG_PENGUIN_2_FRONT,
	
	IMG_PENGUIN_3_BACK,
	IMG_PENGUIN_3_COLOR,
	IMG_PENGUIN_3_FRONT,
	
	IMG_PENGUIN_4_BACK,
	IMG_PENGUIN_4_COLOR,
	IMG_PENGUIN_4_FRONT,
	
	IMG_PENGUIN_5_BACK,
	IMG_PENGUIN_5_COLOR,
	IMG_PENGUIN_5_1_FRONT,
	IMG_PENGUIN_5_2_FRONT,
	IMG_PENGUIN_5_3_FRONT,
	
	IMG_PENGUIN_6_1_BACK,
	IMG_PENGUIN_6_2_BACK,
	IMG_PENGUIN_6_1_COLOR,
	IMG_PENGUIN_6_2_COLOR,
	
	IMG_PENGUIN_6_1_FRONT,
	IMG_PENGUIN_6_2_FRONT,
	IMG_PENGUIN_6_3_FRONT,
	IMG_PENGUIN_6_4_FRONT,
	IMG_PENGUIN_6_5_FRONT,
	IMG_PENGUIN_6_6_FRONT,
	
	IMG_PENGUIN_7_BACK,
	IMG_PENGUIN_7_COLOR,
	IMG_PENGUIN_7_FRONT,
	
	IMG_PENGUIN_8_BACK,
	IMG_PENGUIN_8_COLOR,
	IMG_PENGUIN_8_1_FRONT,
	IMG_PENGUIN_8_2_FRONT,
	IMG_PENGUIN_8_3_FRONT,
	
	NUM_PENGUIN_IMGS
};

const char *penguin_images_names[NUM_PENGUIN_IMGS] = {
	"images/penguin_1_back.png",
	"images/penguin_1_color.png",
	"images/penguin_1_front.png",
	
	"images/penguin_2_back.png",
	"images/penguin_2_color.png",
	"images/penguin_2_front.png",
	
	"images/penguin_3_back.png",
	"images/penguin_3_color.png",
	"images/penguin_3_front.png",
	
	"images/penguin_4_back.png",
	"images/penguin_4_color.png",
	"images/penguin_4_front.png",
	
	"images/penguin_5_back.png",
	"images/penguin_5_color.png",
	"images/penguin_5_1_front.png",
	"images/penguin_5_2_front.png",
	"images/penguin_5_3_front.png",
	
	"images/penguin_6_1_back.png",
	"images/penguin_6_2_back.png",
	"images/penguin_6_1_color.png",
	"images/penguin_6_2_color.png",
	
	"images/penguin_6_1_front.png",
	"images/penguin_6_2_front.png",
	"images/penguin_6_3_front.png",
	"images/penguin_6_4_front.png",
	"images/penguin_6_5_front.png",
	"images/penguin_6_6_front.png",
	
	"images/penguin_7_back.png",
	"images/penguin_7_color.png",
	"images/penguin_7_front.png",
	
	"images/penguin_8_back.png",
	"images/penguin_8_color.png",
	"images/penguin_8_1_front.png",
	"images/penguin_8_2_front.png",
	"images/penguin_8_3_front.png"
};

enum {
	PENGUIN_FRAME_1,
	PENGUIN_FRAME_2,
	PENGUIN_FRAME_3,
	PENGUIN_FRAME_4,
	PENGUIN_FRAME_5_1,
	PENGUIN_FRAME_5_2,
	PENGUIN_FRAME_5_3,
	PENGUIN_FRAME_6_1,
	PENGUIN_FRAME_6_2,
	PENGUIN_FRAME_6_3,
	PENGUIN_FRAME_6_4,
	PENGUIN_FRAME_6_5,
	PENGUIN_FRAME_6_6,
	PENGUIN_FRAME_7,
	PENGUIN_FRAME_8,
	PENGUIN_FRAME_9,
	PENGUIN_FRAME_10,
	
	NUM_PENGUIN_FRAMES
};

const SDL_Color penguin_colors[18] = {
	{0, 51, 102},
	{51, 51, 51},
	{206, 0, 0},
	{255, 204, 0},
	{0, 153, 0},
	{153, 102, 0},
	{255, 49, 156},
	{99, 0, 156},
	{0, 156, 204},
	{255, 102, 0},
	{0, 102, 0},
	{255, 99, 99},
	{139, 227, 3},
	{28, 150, 163},
	{240, 240, 216},
	{174, 159, 200},
	{128, 33, 75},
	{46, 71, 170}
};

void setup_and_color_penguin (void);

SDL_Surface * penguin_images[NUM_PENGUIN_FRAMES];
int color_penguin = 0;

int main (int argc, char *argv[]) {
	int g;
	/* Inicializar el Video SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf (stderr,
			"Error: Can't initialize the video subsystem\n"
			"The error returned by SDL is:\n"
			"%s\n", SDL_GetError());
		exit (1);
	}
	
	/* Generador de números aleatorios */
	srand ((unsigned int) getpid ());
	
	/* Colorear y organizar las imágenes de pingüinos */
	color_penguin = RANDOM (18);
	
	setup_and_color_penguin ();
	
	char buffer_file [8192];
	for (g = 0; g <= PENGUIN_FRAME_4; g++) {
		sprintf (buffer_file, "%s/penguin_%i.png", BUILD_DIR, g + 1);
		SDL_SavePNG (penguin_images[g], buffer_file);
	}
	
	/* El pingüino 5 se arma de 3 frames */
	SDL_gfxBlitRGBA (penguin_images[PENGUIN_FRAME_5_2], NULL, penguin_images[PENGUIN_FRAME_5_1], NULL);
	SDL_gfxBlitRGBA (penguin_images[PENGUIN_FRAME_5_3], NULL, penguin_images[PENGUIN_FRAME_5_1], NULL);
	
	sprintf (buffer_file, "%s/penguin_5.png", BUILD_DIR);
	SDL_SavePNG (penguin_images[PENGUIN_FRAME_5_1], buffer_file);
	
	/* El pingüino 6 se arma de 6 frames */
	SDL_gfxBlitRGBA (penguin_images[PENGUIN_FRAME_6_2], NULL, penguin_images[PENGUIN_FRAME_6_1], NULL);
	SDL_gfxBlitRGBA (penguin_images[PENGUIN_FRAME_6_3], NULL, penguin_images[PENGUIN_FRAME_6_1], NULL);
	SDL_gfxBlitRGBA (penguin_images[PENGUIN_FRAME_6_4], NULL, penguin_images[PENGUIN_FRAME_6_1], NULL);
	SDL_gfxBlitRGBA (penguin_images[PENGUIN_FRAME_6_5], NULL, penguin_images[PENGUIN_FRAME_6_1], NULL);
	SDL_gfxBlitRGBA (penguin_images[PENGUIN_FRAME_6_6], NULL, penguin_images[PENGUIN_FRAME_6_1], NULL);
	
	sprintf (buffer_file, "%s/penguin_6.png", BUILD_DIR);
	SDL_SavePNG (penguin_images[PENGUIN_FRAME_6_1], buffer_file);
	
	sprintf (buffer_file, "%s/penguin_7.png", BUILD_DIR);
	SDL_SavePNG (penguin_images[PENGUIN_FRAME_7], buffer_file);
	
	sprintf (buffer_file, "%s/penguin_8.png", BUILD_DIR);
	SDL_SavePNG (penguin_images[PENGUIN_FRAME_8], buffer_file);
	
	sprintf (buffer_file, "%s/penguin_9.png", BUILD_DIR);
	SDL_SavePNG (penguin_images[PENGUIN_FRAME_9], buffer_file);
	
	sprintf (buffer_file, "%s/penguin_10.png", BUILD_DIR);
	SDL_SavePNG (penguin_images[PENGUIN_FRAME_10], buffer_file);
	
	return 0;
}

void setup_and_color_penguin (void) {
	int g;
	SDL_Surface * image, *color_surface;
	SDL_Surface *temp_penguins[NUM_PENGUIN_IMGS];
	
	char buffer_file[8192];
	char *systemdata_path = DATA_DIR;
	
	for (g = 0; g < NUM_PENGUIN_IMGS; g++) {
		sprintf (buffer_file, "%s/%s", systemdata_path, penguin_images_names[g]);
		image = IMG_Load (buffer_file);
		
		if (image == NULL) {
			fprintf (stderr,
				"Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n", buffer_file, SDL_GetError());
			SDL_Quit ();
			exit (1);
		}
		
		temp_penguins[g] = image;
		/* TODO: Mostrar la carga de porcentaje */
	}
	
	color_surface = SDL_CreateRGBSurface (SDL_SWSURFACE, 196, 199, 32, RMASK, GMASK, BMASK, AMASK);
	SDL_FillRect (color_surface, NULL, SDL_MapRGB (color_surface->format, penguin_colors[color_penguin].r, penguin_colors[color_penguin].g, penguin_colors[color_penguin].b));
	
	for (g = 0; g < 4; g++) {
		penguin_images[PENGUIN_FRAME_1 + g] = temp_penguins[IMG_PENGUIN_1_BACK + (g * 3)];
		
		/* Colorear el pingüino */
		SDL_BlitSurface (color_surface, NULL, temp_penguins[IMG_PENGUIN_1_COLOR + (g * 3)], NULL);
		
		/* Copiar el color sobre el fondo */
		SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_1_COLOR + (g * 3)], NULL, penguin_images[PENGUIN_FRAME_1 + g], NULL);
		SDL_FreeSurface (temp_penguins[IMG_PENGUIN_1_COLOR + (g * 3)]);
		temp_penguins[IMG_PENGUIN_1_COLOR + (g * 3)] = NULL;
		
		/* Copiar el frente */
		SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_1_FRONT + (g * 3)], NULL, penguin_images[PENGUIN_FRAME_1 + g], NULL);
		SDL_FreeSurface (temp_penguins[IMG_PENGUIN_1_FRONT + (g * 3)]);
		temp_penguins[IMG_PENGUIN_1_FRONT + (g * 3)] = NULL;
	}
	
	/* Duplicar el fondo del frame 5 */
	penguin_images[PENGUIN_FRAME_5_1] = temp_penguins[IMG_PENGUIN_5_BACK];
	penguin_images[PENGUIN_FRAME_5_2] = SDL_CreateRGBSurface (SDL_SWSURFACE, 196, 199, 32, RMASK, GMASK, BMASK, AMASK);
	penguin_images[PENGUIN_FRAME_5_3] = SDL_CreateRGBSurface (SDL_SWSURFACE, 196, 199, 32, RMASK, GMASK, BMASK, AMASK);
	
	SDL_SetAlpha (penguin_images[PENGUIN_FRAME_5_1], 0, 0);
	SDL_BlitSurface (penguin_images[PENGUIN_FRAME_5_1], NULL, penguin_images[PENGUIN_FRAME_5_2], NULL);
	SDL_BlitSurface (penguin_images[PENGUIN_FRAME_5_1], NULL, penguin_images[PENGUIN_FRAME_5_3], NULL);
	SDL_SetAlpha (penguin_images[PENGUIN_FRAME_5_1], SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
	
	/* Colorear el pingüino */
	SDL_BlitSurface (color_surface, NULL, temp_penguins[IMG_PENGUIN_5_COLOR], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_5_COLOR], NULL, penguin_images[PENGUIN_FRAME_5_1], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_5_COLOR], NULL, penguin_images[PENGUIN_FRAME_5_2], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_5_COLOR], NULL, penguin_images[PENGUIN_FRAME_5_3], NULL);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_5_COLOR]);
	temp_penguins[IMG_PENGUIN_5_COLOR] = NULL;
	
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_5_1_FRONT], NULL, penguin_images[PENGUIN_FRAME_5_1], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_5_2_FRONT], NULL, penguin_images[PENGUIN_FRAME_5_2], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_5_3_FRONT], NULL, penguin_images[PENGUIN_FRAME_5_3], NULL);
	
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_5_1_FRONT]);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_5_2_FRONT]);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_5_3_FRONT]);
	
	temp_penguins[IMG_PENGUIN_5_1_FRONT] = temp_penguins[IMG_PENGUIN_5_2_FRONT] = temp_penguins[IMG_PENGUIN_5_3_FRONT] = NULL;
	
	/* Vamos por el frame 6 */
	SDL_SetAlpha (temp_penguins[IMG_PENGUIN_6_1_BACK], 0, 0);
	SDL_SetAlpha (temp_penguins[IMG_PENGUIN_6_2_BACK], 0, 0);
	
	SDL_BlitSurface (color_surface, NULL, temp_penguins[IMG_PENGUIN_6_1_COLOR], NULL);
	SDL_BlitSurface (color_surface, NULL, temp_penguins[IMG_PENGUIN_6_2_COLOR], NULL);
	
	/* 6 frames de animación del frame 6 */
	for (g = 0; g < 6; g++) {
		penguin_images[PENGUIN_FRAME_6_1 + g] = SDL_CreateRGBSurface (SDL_SWSURFACE, 196, 199, 32, RMASK, GMASK, BMASK, AMASK);
		
		/* Clonar el fondo */
		SDL_BlitSurface (temp_penguins[IMG_PENGUIN_6_1_BACK + (g % 2)], NULL, penguin_images[PENGUIN_FRAME_6_1 + g], NULL);
		
		SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_6_1_COLOR + (g % 2)], NULL, penguin_images[PENGUIN_FRAME_6_1 + g], NULL);
		
		SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_6_1_FRONT + g], NULL, penguin_images[PENGUIN_FRAME_6_1 + g], NULL);
		
		SDL_FreeSurface (temp_penguins[IMG_PENGUIN_6_1_FRONT + g]);
		temp_penguins[IMG_PENGUIN_6_1_FRONT + g] = NULL;
	}
	
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_6_1_BACK]);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_6_2_BACK]);
	temp_penguins[IMG_PENGUIN_6_1_BACK] = temp_penguins[IMG_PENGUIN_6_2_BACK] = NULL;
	
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_6_1_COLOR]);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_6_2_COLOR]);
	temp_penguins[IMG_PENGUIN_6_1_COLOR] = temp_penguins[IMG_PENGUIN_6_2_COLOR] = NULL;
	
	/* Armar el frame 7 */
	penguin_images[PENGUIN_FRAME_7] = temp_penguins[IMG_PENGUIN_7_BACK];
	
	/* Colorear el pingüino */
	SDL_BlitSurface (color_surface, NULL, temp_penguins[IMG_PENGUIN_7_COLOR], NULL);
	
	/* Copiar el color sobre el fondo */
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_7_COLOR], NULL, penguin_images[PENGUIN_FRAME_7], NULL);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_7_COLOR]);
	temp_penguins[IMG_PENGUIN_7_COLOR] = NULL;
	
	/* Copiar el frente */
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_7_FRONT], NULL, penguin_images[PENGUIN_FRAME_7], NULL);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_7_FRONT]);
	temp_penguins[IMG_PENGUIN_7_FRONT] = NULL;
	
	/* Generar los otros 3 estados */
	penguin_images[PENGUIN_FRAME_8] = temp_penguins[IMG_PENGUIN_8_BACK];
	penguin_images[PENGUIN_FRAME_9] = SDL_CreateRGBSurface (SDL_SWSURFACE, 196, 199, 32, RMASK, GMASK, BMASK, AMASK);
	penguin_images[PENGUIN_FRAME_10] = SDL_CreateRGBSurface (SDL_SWSURFACE, 196, 199, 32, RMASK, GMASK, BMASK, AMASK);
	
	SDL_SetAlpha (penguin_images[PENGUIN_FRAME_8], 0, 0);
	SDL_BlitSurface (penguin_images[PENGUIN_FRAME_8], NULL, penguin_images[PENGUIN_FRAME_9], NULL);
	SDL_BlitSurface (penguin_images[PENGUIN_FRAME_8], NULL, penguin_images[PENGUIN_FRAME_10], NULL);
	SDL_SetAlpha (penguin_images[PENGUIN_FRAME_8], SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
	
	/* Colorear el pingüino */
	SDL_BlitSurface (color_surface, NULL, temp_penguins[IMG_PENGUIN_8_COLOR], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_8_COLOR], NULL, penguin_images[PENGUIN_FRAME_8], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_8_COLOR], NULL, penguin_images[PENGUIN_FRAME_9], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_8_COLOR], NULL, penguin_images[PENGUIN_FRAME_10], NULL);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_8_COLOR]);
	temp_penguins[IMG_PENGUIN_8_COLOR] = NULL;
	
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_8_1_FRONT], NULL, penguin_images[PENGUIN_FRAME_8], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_8_2_FRONT], NULL, penguin_images[PENGUIN_FRAME_9], NULL);
	SDL_gfxBlitRGBA (temp_penguins[IMG_PENGUIN_8_3_FRONT], NULL, penguin_images[PENGUIN_FRAME_10], NULL);
	
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_8_1_FRONT]);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_8_2_FRONT]);
	SDL_FreeSurface (temp_penguins[IMG_PENGUIN_8_3_FRONT]);
	
	temp_penguins[IMG_PENGUIN_8_1_FRONT] = temp_penguins[IMG_PENGUIN_8_2_FRONT] = temp_penguins[IMG_PENGUIN_8_3_FRONT] = NULL;
}
