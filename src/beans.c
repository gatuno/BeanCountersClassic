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
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "path.h"

#include "gfx_blit_func.h"
#include "collider.h"

#define FPS (1000/24)
#define RANDOM(x) ((int) (x ## .0 * rand () / (RAND_MAX + 1.0)))
#define RANDOM_VAR(x) ((int) (((float) x) * rand () / (RAND_MAX + 1.0)))

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

typedef struct _BeanBag {
	int bag;
	int throw_length;
	int frame;
	
	const int (*bag_points)[3];
	
	struct _BeanBag *prev;
	struct _BeanBag *next;
} BeanBag;

/* Enumerar las imágenes */
enum {
	IMG_BACKGROUND,
	IMG_PLATAFORM,
	
	IMG_BAG_1,
	IMG_BAG_2,
	IMG_BAG_3,
	IMG_BAG_4,
	
	NUM_IMAGES
};

/* Los nombres de archivos */
const char *images_names[NUM_IMAGES] = {
	"images/background.png",
	"images/plataform.png",
	
	"images/bag_1.png",
	"images/bag_2.png",
	"images/bag_3.png",
	"images/bag_4.png"
};

enum {
	SND_NONE,
	
	NUM_SOUNDS
};

const char *sound_names[NUM_SOUNDS] = {
	"sounds/none.wav",
};

/* Codigos de salida */
enum {
	GAME_NONE = 0, /* No usado */
	GAME_CONTINUE,
	GAME_QUIT
};

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

enum {
	COLLIDER_BAG_3,
	
	COLLIDER_PENGUIN_1,
	COLLIDER_PENGUIN_2,
	COLLIDER_PENGUIN_3,
	COLLIDER_PENGUIN_4,
	COLLIDER_PENGUIN_5,
	COLLIDER_PENGUIN_6,
	COLLIDER_PENGUIN_7,
	COLLIDER_PENGUIN_8,
	COLLIDER_PENGUIN_9,
	COLLIDER_PENGUIN_10,
	
	NUM_COLLIDERS
};

const char *collider_names[NUM_COLLIDERS] = {
	"collider/bag_3.col",
	
	"collider/penguin_1.col",
	"collider/penguin_2.col",
	"collider/penguin_3.col",
	"collider/penguin_4.col",
	"collider/penguin_5.col",
	"collider/penguin_6.col",
	"collider/penguin_7.col",
	"collider/penguin_8.col",
	"collider/penguin_9.col",
	"collider/penguin_10.col"
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

const int bag_0_points[31][3] = {
	{0, 638, 108},
	{0, 613, 98},
	{0, 588, 87},
	{0, 565, 80},
	{0, 543, 72},
	{0, 523, 68},
	{0, 504, 65},
	{0, 485, 61},
	{1, 493, 28},
	{1, 479, 27},
	{1, 465, 27},
	{1, 456, 28},
	{2, 427, 86},
	{2, 413, 89},
	{2, 401, 93},
	{2, 388, 97},
	{2, 371, 103},
	{2, 355, 110},
	{2, 340, 119},
	{2, 322, 131},
	{2, 305, 146},
	{2, 290, 161},
	{2, 273, 181},
	{2, 259, 203},
	{2, 246, 226},
	{2, 233, 254},
	{2, 223, 282},
	{2, 214, 311},
	{2, 205, 343},
	{2, 196, 375},
	{3, 165, 408}
};

const int bag_1_points[22][3] = {
	{0, 633, 209},
	{0, 622, 168},
	{0, 610, 134},
	{0, 598, 107},
	{0, 586, 87},
	{0, 574, 74},
	{0, 563, 69},
	{0, 558, 68},
	{1, 579, 38},
	{1, 571, 42},
	{1, 563, 51},
	{1, 554, 65},
	{2, 527, 141},
	{2, 520, 164},
	{2, 512, 187},
	{2, 505, 215},
	{2, 498, 246},
	{2, 491, 280},
	{2, 484, 317},
	{2, 478, 358},
	{2, 472, 402},
	{3, 441, 420}
};

const int bag_2_points[28][3] = {
	{0, 633, 229},
	{0, 620, 202},
	{0, 606, 175},
	{0, 593, 154},
	{0, 579, 132},
	{0, 565, 116},
	{0, 550, 100},
	{0, 536, 89},
	{1, 548, 49},
	{1, 535, 46},
	{1, 523, 43},
	{1, 510, 39},
	{2, 477, 99},
	{2, 461, 104},
	{2, 444, 114},
	{2, 427, 126},
	{2, 413, 140},
	{2, 399, 160},
	{2, 385, 179},
	{2, 370, 199},
	{2, 357, 224},
	{2, 344, 249},
	{2, 331, 273},
	{2, 319, 304},
	{2, 307, 335},
	{2, 294, 365},
	{2, 282, 396},
	{3, 251, 405}
};

const int bag_3_points[33][3] = {
	{0, 647, 305},
	{0, 620, 267},
	{0, 595, 236},
	{0, 570, 204},
	{0, 546, 179},
	{0, 522, 153},
	{0, 499, 135},
	{0, 477, 116},
	{1, 481, 71},
	{1, 463, 63},
	{1, 444, 54},
	{1, 429, 50},
	{2, 397, 106},
	{2, 389, 107},
	{2, 381, 108},
	{2, 373, 109},
	{2, 365, 110},
	{2, 354, 114},
	{2, 343, 117},
	{2, 330, 125},
	{2, 316, 132},
	{2, 302, 145},
	{2, 288, 158},
	{2, 275, 176},
	{2, 261, 194},
	{2, 249, 217},
	{2, 237, 240},
	{2, 227, 267},
	{2, 217, 294},
	{2, 208, 326},
	{2, 198, 358},
	{2, 189, 389},
	{3, 141, 422}
};

/* Prototipos de función */
int game_intro (void);
int game_loop (void);
int game_finish (void);
void setup (void);
SDL_Surface * set_video_mode(unsigned flags);
void setup_and_color_penguin (void);
void add_bag (int tipo);
void delete_bag (BeanBag *p);

/* Variables globales */
SDL_Surface * screen;
SDL_Surface * images[NUM_IMAGES];
SDL_Surface * penguin_images[NUM_PENGUIN_FRAMES];
int use_sound;
Collider *colliders[NUM_COLLIDERS];

int color_penguin = 0;

Mix_Chunk * sounds[NUM_SOUNDS];
Mix_Music * mus_carnie;

BeanBag *first_bag = NULL;
BeanBag *last_bag = NULL;

int main (int argc, char *argv[]) {
	/* Recuperar las rutas del sistema */
	initSystemPaths (argv[0]);
	
	/* Inicializar l18n */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, get_l10n_path ());
	
	textdomain (PACKAGE);
	
	setup ();
	bind_textdomain_codeset (PACKAGE, "UTF-8");
	do {
		//if (game_intro () == GAME_QUIT) break;
		if (game_loop () == GAME_QUIT) break;
		//if (game_finish () == GAME_QUIT) break;
	} while (1 == 0);
	
	SDL_Quit ();
	return EXIT_SUCCESS;
}

#if 0
int game_intro (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	SDL_Rect rect;
	Uint32 last_time, now_time;
	
	/* Predibujar todo */
	SDL_FillRect (screen, NULL, 0);
	SDL_Flip (screen);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					break;
				case SDL_KEYDOWN:
					/* Tengo una tecla presionada */
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						SDL_WM_ToggleFullScreen (screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					break;
			}
		}
		
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}

int game_finish (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	SDL_Rect rect;
	Uint32 last_time, now_time;
	
	/* Predibujar todo */
	SDL_FillRect (screen, NULL, 0);
	SDL_Flip (screen);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			/* fprintf (stdout, "Evento: %i\n", event.type);*/
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					break;
				case SDL_KEYDOWN:
					/* Tengo una tecla presionada */
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						SDL_WM_ToggleFullScreen (screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					break;
			}
		}
		
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}
#endif

int game_loop (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	Uint32 last_time, now_time;
	SDL_Rect rect;
	int penguinx, handposx;
	BeanBag *thisbag, *nextbag;
	
	int bags = 0;
	int penguin_frame = 0;
	int i, j, k;
	
	int level, activator;
	int bag_activity = 15;
	int airbone, max_airbone = 1;
	
	/* Predibujar todo */
	/*SDL_FillRect (screen, NULL, 0);
	SDL_Flip (screen);*/
	
	SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEBUTTONDOWN:
					/* Tengo un Mouse Down */
					break;
				case SDL_MOUSEBUTTONUP:
					/* Tengo un mouse Up */
					break;
				case SDL_KEYDOWN:
					/* Tengo una tecla presionada */
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						SDL_WM_ToggleFullScreen (screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					if (key == SDLK_a) {
						if (bags < 9) {
							bags++;
							penguin_frame = 0;
						}
					} else if (key == SDLK_s) {
						if (bags > 0) {
							bags--;
							penguin_frame = 0;
						}
					} else if (key == SDLK_q) {
						add_bag (0);
					} else if (key == SDLK_w) {
						add_bag (1);
					} else if (key == SDLK_e) {
						add_bag (2);
					} else if (key == SDLK_r) {
						add_bag (3);
					}
					break;
			}
		}
		
		if (bags < 6) {
			SDL_GetMouseState (&handposx, NULL);
		
			penguinx = handposx;
			if (penguinx < 190) {
				penguinx = 190;
			} else if (penguinx > 555) {
				penguinx = 555;
			}
		}
		
		activator = RANDOM_VAR (bag_activity);
		
		if (activator <= 2 && bags < 6 /* AND Game Over not visible */) {
			if (airbone < max_airbone) {
				i = RANDOM (8);
				
				if (i <= 3) {
					add_bag (i);
					airbone++;
				}
			}
		}
		
		if (bags >= 0 && bags <= 6) {
			k = COLLIDER_PENGUIN_1 + bags;
		} else {
			k = COLLIDER_PENGUIN_7;
		}
		
		/* Procesar las bolsas */
		thisbag = first_bag;
		while (thisbag != NULL) {
			nextbag = thisbag->next;
			
			thisbag->frame++;
			
			j = thisbag->frame - thisbag->throw_length;
			
			if (j < 0) {
				/* Calcular aquí la colisión contra el pingüino */
				i = collider_hittest (colliders[COLLIDER_BAG_3], thisbag->bag_points[thisbag->frame][1], thisbag->bag_points[thisbag->frame][2], colliders[k], penguinx - 120, 251);
			
				if (i == SDL_TRUE) {
					bags++;
					k = COLLIDER_PENGUIN_1 + bags;
				
					/* Reproducir el sonido de "Agarrar bolsa" */
				
					/* Sumar score = score + (nivel * 2); */
					airbone--;
					delete_bag (thisbag);
					thisbag = nextbag;
					continue;
				}
			}
			
			if (j == 0) {
				/* Eliminar del airbone */
				airbone--;
			}
			
			if (j >= 35) {
				/* Eliminar esta bolsa */
				delete_bag (thisbag);
			}
			
			thisbag = nextbag;
		}
		
		SDL_BlitSurface (images[IMG_BACKGROUND], NULL, screen, NULL);
		
		if (bags >= 0 && bags < 4) {
			i = PENGUIN_FRAME_1 + bags;
		} else if (bags == 4) {
			i = PENGUIN_FRAME_5_1 + (penguin_frame / 2);
			penguin_frame++;
			
			if (penguin_frame >= 6) {
				penguin_frame = 0;
			}
		} else if (bags == 5) {
			i = PENGUIN_FRAME_6_1 + (penguin_frame / 2);
			penguin_frame++;
			
			if (penguin_frame >= 12) {
				penguin_frame = 0;
			}
		} else if (bags == 6) {
			i = PENGUIN_FRAME_7;
		} else if (bags > 6) {
			i = PENGUIN_FRAME_8 + (bags - 7);
		}
		
		/* Dibujar al pinguino */
		rect.x = penguinx - 120;
		rect.y = 251;
		rect.w = penguin_images[i]->w;
		rect.h = penguin_images[i]->h;
		
		SDL_BlitSurface (penguin_images[i], NULL, screen, &rect);
		
		/* Dibujar la plataforma */
		rect.x = 0;
		rect.y = 355;
		rect.w = images[IMG_PLATAFORM]->w;
		rect.h = images[IMG_PLATAFORM]->h;
		
		SDL_BlitSurface (images[IMG_PLATAFORM], NULL, screen, &rect);
		
		/* Dibujar las bolsas de café, arriba de la plataforma, por detrás del camión */
		thisbag = first_bag;
		while (thisbag != NULL) {
			if (thisbag->frame < thisbag->throw_length) {
				/* Dibujar la bolsa */
				i = IMG_BAG_1 + thisbag->bag_points[thisbag->frame][0];
				rect.x = thisbag->bag_points[thisbag->frame][1];
				rect.y = thisbag->bag_points[thisbag->frame][2];
			} else {
				i = IMG_BAG_4;
				rect.x = thisbag->bag_points[thisbag->throw_length][1];
				rect.y = thisbag->bag_points[thisbag->throw_length][2];
				j = thisbag->frame - thisbag->throw_length;
			}
			
			rect.w = images[i]->w;
			rect.h = images[i]->h;
			
			if (i == IMG_BAG_4 && j > 25) {
				SDL_gfxBlitRGBAWithAlpha (images[i], NULL, screen, &rect, 255 - SDL_ALPHA_OPAQUE * (j - 25) / 10);
			} else {
				SDL_BlitSurface (images[i], NULL, screen, &rect);
			}
			
			thisbag = thisbag->next;
		}
		
		/* TODO: Dibujar aquí el camión */
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}
/* Set video mode: */
/* Mattias Engdegard <f91-men@nada.kth.se> */
SDL_Surface * set_video_mode (unsigned flags) {
	/* Prefer 16bpp, but also prefer native modes to emulated 16bpp. */

	int depth;

	depth = SDL_VideoModeOK (760, 480, 16, flags);
	return depth ? SDL_SetVideoMode (760, 480, depth, flags) : NULL;
}

void setup (void) {
	SDL_Surface * image;
	int g;
	char buffer_file[8192];
	char *systemdata_path = get_systemdata_path ();
	Collider *c;
	
	/* Inicializar el Video SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the video subsystem\n"
			"The error returned by SDL is:\n"
			"%s\n"), SDL_GetError());
		exit (1);
	}
	sprintf (buffer_file, "%simages/icon.png", systemdata_path);
	image = IMG_Load (buffer_file);
	if (image) {
		SDL_WM_SetIcon (image, NULL);
		SDL_FreeSurface (image);
	}
	SDL_WM_SetCaption (_("Bean Counters Classic"), _("Bean Counters Classic"));
	
	/* Crear la pantalla de dibujado */
	screen = set_video_mode (0);
	
	if (screen == NULL) {
		fprintf (stderr,
			_("Error: Can't setup 760x480 video mode.\n"
			"The error returned by SDL is:\n"
			"%s\n"), SDL_GetError());
		exit (1);
	}
	
	use_sound = 1;
	if (SDL_InitSubSystem (SDL_INIT_AUDIO) < 0) {
		fprintf (stdout,
			_("Warning: Can't initialize the audio subsystem\n"
			"Continuing...\n"));
		use_sound = 0;
	}
	
	if (use_sound) {
		/* Inicializar el sonido */
		if (Mix_OpenAudio (22050, AUDIO_S16, 2, 4096) < 0) {
			fprintf (stdout,
				_("Warning: Can't initialize the SDL Mixer library\n"));
			use_sound = 0;
		}
	}
	
	for (g = 0; g < NUM_IMAGES; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, images_names[g]);
		image = IMG_Load (buffer_file);
		
		if (image == NULL) {
			fprintf (stderr,
				_("Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n"), buffer_file, SDL_GetError());
			SDL_Quit ();
			exit (1);
		}
		
		images[g] = image;
		/* TODO: Mostrar la carga de porcentaje */
	}
	
	/* Generador de números aleatorios */
	srand ((unsigned int) getpid ());
	
	/* Colorear y organizar las imágenes de pingüinos */
	color_penguin = RANDOM (18);
	
	setup_and_color_penguin ();
	
	/* Cargar los colliders de los pingüinos */
	for (g = 0; g < NUM_COLLIDERS; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, collider_names[g]);
		c = collider_new_from_file (buffer_file);
		
		if (c == NULL) {
			fprintf (stderr,
				_("Failed to load data file:\n"
				"%s\n"), buffer_file);
			SDL_Quit ();
			exit (1);
		}
		colliders[g] = c;
	}
	
	if (use_sound) {
		/*for (g = 0; g < NUM_SOUNDS; g++) {
			sprintf (buffer_file, "%s%s", systemdata_path, sound_names[g]);
			sounds[g] = Mix_LoadWAV (buffer_file);
			
			if (sounds[g] == NULL) {
				fprintf (stderr,
					_("Failed to load data file:\n"
					"%s\n"
					"The error returned by SDL is:\n"
					"%s\n"), buffer_file, SDL_GetError ());
				SDL_Quit ();
				exit (1);
			}
			Mix_VolumeChunk (sounds[g], MIX_MAX_VOLUME / 2);
		}*/
		
		/* Cargar la música */
		//sprintf (buffer_file, "%s%s", systemdata_path, MUS_CARNIE);
		//mus_carnie = Mix_LoadMUS (buffer_file);
		
		/*if (mus_carnie == NULL) {
			fprintf (stderr,
				_("Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n"), buffer_file, SDL_GetError ());
			SDL_Quit ();
			exit (1);
		}*/
	}
	
	if (TTF_Init () < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the SDL TTF library\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	// TODO: Favor de manejar correctamente el bind_textdomain_codeset
	//bind_textdomain_codeset (PACKAGE, "UTF-8");
}

void setup_and_color_penguin (void) {
	int g;
	SDL_Surface * image, *color_surface;
	SDL_Surface *temp_penguins[NUM_PENGUIN_IMGS];
	
	char buffer_file[8192];
	char *systemdata_path = get_systemdata_path ();
	
	for (g = 0; g < NUM_PENGUIN_IMGS; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, penguin_images_names[g]);
		image = IMG_Load (buffer_file);
		
		if (image == NULL) {
			fprintf (stderr,
				_("Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n"), buffer_file, SDL_GetError());
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

void add_bag (int tipo) {
	BeanBag *new;
	
	new = (BeanBag *) malloc (sizeof (BeanBag));
	
	new->bag = tipo;
	new->frame = -1;
	
	if (tipo == 0) {
		new->throw_length = 30;
		new->bag_points = bag_0_points;
	} else if (tipo == 1) {
		new->throw_length = 21;
		new->bag_points = bag_1_points;
	} else if (tipo == 2) {
		new->throw_length = 27;
		new->bag_points = bag_2_points;
	} else if (tipo == 3) {
		new->throw_length = 32;
		new->bag_points = bag_3_points;
	}
	
	/* Ahora sus campos para lista doble ligada */
	new->next = NULL;
	new->prev = last_bag;
	
	if (last_bag == NULL) {
		first_bag = last_bag = new;
	} else {
		last_bag->next = new;
		last_bag = new;
	}
}

void delete_bag (BeanBag *p) {
	if (p == NULL) return;
	
	if (p->prev == NULL) { /* El primero de la lista */
		first_bag = p->next;
	} else {
		p->prev->next = p->next;
	}
	
	if (p->next == NULL) {
		last_bag = p->prev;
	} else {
		p->next->prev = p->prev;
	}
	
	free (p);
}
