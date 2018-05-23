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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "path.h"

#define FPS (1000/24)

/* Enumerar las imágenes */
enum {
	IMG_NONE,
	
	NUM_IMAGES
};

/* Los nombres de archivos */
const char *images_names[NUM_IMAGES] = {
	"images/none.png",
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

/* Prototipos de función */
int game_intro (void);
int game_loop (void);
int game_finish (void);
void setup (void);
SDL_Surface * set_video_mode(unsigned flags);

/* Variables globales */
SDL_Surface * screen;
SDL_Surface * images[NUM_IMAGES];
int use_sound;

Mix_Chunk * sounds[NUM_SOUNDS];
Mix_Music * mus_carnie;

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
		if (game_intro () == GAME_QUIT) break;
		if (game_loop () == GAME_QUIT) break;
		if (game_finish () == GAME_QUIT) break;
	} while (1 == 0);
	
	SDL_Quit ();
	return EXIT_SUCCESS;
}

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

int game_loop (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	Uint32 last_time, now_time;
	SDL_Rect rect;
	
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
					break;
			}
		}
		
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
	TTF_Font *ttf10, *ttf14, *ttf16, *ttf26, *temp_font;
	SDL_Color color;
	SDL_Rect rect, rect2;
	int g;
	char buffer_file[8192];
	char *systemdata_path = get_systemdata_path ();
	
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
	
	if (use_sound) {
		for (g = 0; g < NUM_SOUNDS; g++) {
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
		}
		
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
	
	/* Generador de números aleatorios */
	srand (SDL_GetTicks ());
}

