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
#include "draw-text.h"
#include "zoom.h"
#include "cp-button.h"

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

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

enum {
	NO_NEXT_LEVEL = 0,
	NEXT_LEVEL,
	GAME_WIN
};

typedef struct _BeanBag {
	int bag;
	int throw_length;
	int frame;
	
	union {
		const int (*bag_points)[3];
		const int (*object_points)[2];
	};
	
	struct _BeanBag *prev;
	struct _BeanBag *next;
} BeanBag;

/* Enumerar las imágenes */
enum {
	IMG_GAMEINTRO,
	
	IMG_PENGUIN_INTRO_BACK,
	IMG_PENGUIN_INTRO_COLOR,
	IMG_PENGUIN_INTRO_FRONT,
	
	IMG_INTRO_PLATAFORM,
	
	IMG_BACKGROUND,
	IMG_PLATAFORM,
	
	IMG_BAG_1,
	IMG_BAG_2,
	IMG_BAG_3,
	IMG_BAG_4,
	
	IMG_BAG_STACK_1,
	IMG_BAG_STACK_2,
	IMG_BAG_STACK_3,
	IMG_BAG_STACK_4,
	IMG_BAG_STACK_5,
	IMG_BAG_STACK_6,
	IMG_BAG_STACK_7,
	IMG_BAG_STACK_8,
	IMG_BAG_STACK_9,
	IMG_BAG_STACK_10,
	
	IMG_BAG_STACK_11,
	IMG_BAG_STACK_12,
	IMG_BAG_STACK_13,
	IMG_BAG_STACK_14,
	IMG_BAG_STACK_15,
	IMG_BAG_STACK_16,
	IMG_BAG_STACK_17,
	IMG_BAG_STACK_18,
	IMG_BAG_STACK_19,
	IMG_BAG_STACK_20,
	
	IMG_BAG_STACK_21,
	IMG_BAG_STACK_22,
	IMG_BAG_STACK_23,
	IMG_BAG_STACK_24,
	IMG_BAG_STACK_25,
	IMG_BAG_STACK_26,
	IMG_BAG_STACK_27,
	IMG_BAG_STACK_28,
	IMG_BAG_STACK_29,
	IMG_BAG_STACK_30,
	
	IMG_BAG_STACK_31,
	IMG_BAG_STACK_32,
	IMG_BAG_STACK_33,
	IMG_BAG_STACK_34,
	IMG_BAG_STACK_35,
	IMG_BAG_STACK_36,
	IMG_BAG_STACK_37,
	IMG_BAG_STACK_38,
	IMG_BAG_STACK_39,
	IMG_BAG_STACK_40,
	
	IMG_BAG_STACK_41,
	IMG_BAG_STACK_42,
	IMG_BAG_STACK_43,
	IMG_BAG_STACK_44,
	IMG_BAG_STACK_45,
	IMG_BAG_STACK_46,
	IMG_BAG_STACK_47,
	IMG_BAG_STACK_48,
	IMG_BAG_STACK_49,
	IMG_BAG_STACK_50,
	
	IMG_BAG_STACK_51,
	IMG_BAG_STACK_52,
	IMG_BAG_STACK_53,
	IMG_BAG_STACK_54,
	IMG_BAG_STACK_55,
	IMG_BAG_STACK_56,
	IMG_BAG_STACK_57,
	IMG_BAG_STACK_58,
	IMG_BAG_STACK_59,
	IMG_BAG_STACK_60,
	
	IMG_TRUCK,
	
	IMG_ANVIL_0,
	IMG_ANVIL_1,
	IMG_ANVIL_2,
	IMG_ANVIL_3,
	IMG_ANVIL_4,
	IMG_ANVIL_5,
	IMG_ANVIL_6,
	IMG_ANVIL_7,
	IMG_ANVIL_8,
	IMG_ANVIL_9,
	IMG_ANVIL_10,
	IMG_ANVIL_11,
	IMG_ANVIL_12,
	IMG_ANVIL_13,
	IMG_ANVIL_14,
	IMG_ANVIL_15,
	IMG_ANVIL_16,
	IMG_ANVIL_17,
	IMG_ANVIL_18,
	IMG_ANVIL_19,
	IMG_ANVIL_20,
	IMG_ANVIL_21,
	IMG_ANVIL_22,
	IMG_ANVIL_23,
	
	IMG_ONEUP,
	
	IMG_FISH,
	IMG_FISH_DROPPED,
	
	IMG_FLOWER,
	IMG_FLOWER_DROPPED,
	
	IMG_CRASH_1,
	IMG_CRASH_2,
	IMG_CRASH_3,
	IMG_CRASH_4,
	
	IMG_LEFT,
	IMG_RIGHT,
	
	NUM_IMAGES
};

/* Los nombres de archivos */
const char *images_names[NUM_IMAGES] = {
	"images/gameintro.png",
	
	"images/penguin_intro_back.png",
	"images/penguin_intro_color.png",
	"images/penguin_intro_front.png",
	
	"images/intro_plataform.png",
	
	"images/background.png",
	"images/plataform.png",
	
	"images/bag_1.png",
	"images/bag_2.png",
	"images/bag_3.png",
	"images/bag_4.png",
	
	"images/bag_stack_01.png",
	"images/bag_stack_02.png",
	"images/bag_stack_03.png",
	"images/bag_stack_04.png",
	"images/bag_stack_05.png",
	"images/bag_stack_06.png",
	"images/bag_stack_07.png",
	"images/bag_stack_08.png",
	"images/bag_stack_09.png",
	"images/bag_stack_10.png",
	"images/bag_stack_11.png",
	"images/bag_stack_12.png",
	"images/bag_stack_13.png",
	"images/bag_stack_14.png",
	"images/bag_stack_15.png",
	"images/bag_stack_16.png",
	"images/bag_stack_17.png",
	"images/bag_stack_18.png",
	"images/bag_stack_19.png",
	"images/bag_stack_20.png",
	"images/bag_stack_21.png",
	"images/bag_stack_22.png",
	"images/bag_stack_23.png",
	"images/bag_stack_24.png",
	"images/bag_stack_25.png",
	"images/bag_stack_26.png",
	"images/bag_stack_27.png",
	"images/bag_stack_28.png",
	"images/bag_stack_29.png",
	"images/bag_stack_30.png",
	"images/bag_stack_31.png",
	"images/bag_stack_32.png",
	"images/bag_stack_33.png",
	"images/bag_stack_34.png",
	"images/bag_stack_35.png",
	"images/bag_stack_36.png",
	"images/bag_stack_37.png",
	"images/bag_stack_38.png",
	"images/bag_stack_39.png",
	"images/bag_stack_40.png",
	"images/bag_stack_41.png",
	"images/bag_stack_42.png",
	"images/bag_stack_43.png",
	"images/bag_stack_44.png",
	"images/bag_stack_45.png",
	"images/bag_stack_46.png",
	"images/bag_stack_47.png",
	"images/bag_stack_48.png",
	"images/bag_stack_49.png",
	"images/bag_stack_50.png",
	"images/bag_stack_51.png",
	"images/bag_stack_52.png",
	"images/bag_stack_53.png",
	"images/bag_stack_54.png",
	"images/bag_stack_55.png",
	"images/bag_stack_56.png",
	"images/bag_stack_57.png",
	"images/bag_stack_58.png",
	"images/bag_stack_59.png",
	"images/bag_stack_60.png",
	
	"images/truck.png",
	
	"images/anvil_00.png",
	"images/anvil_01.png",
	"images/anvil_02.png",
	"images/anvil_03.png",
	"images/anvil_04.png",
	"images/anvil_05.png",
	"images/anvil_06.png",
	"images/anvil_07.png",
	"images/anvil_08.png",
	"images/anvil_09.png",
	"images/anvil_10.png",
	"images/anvil_11.png",
	"images/anvil_12.png",
	"images/anvil_13.png",
	"images/anvil_14.png",
	"images/anvil_15.png",
	"images/anvil_16.png",
	"images/anvil_17.png",
	"images/anvil_18.png",
	"images/anvil_19.png",
	"images/anvil_20.png",
	"images/anvil_21.png",
	"images/anvil_22.png",
	"images/anvil_23.png",
	
	"images/oneup.png",
	
	"images/fish.png",
	"images/fish_dropped.png",
	
	"images/flower.png",
	"images/flower_dropped.png",
	
	"images/crash_1.png",
	"images/crash_2.png",
	"images/crash_3.png",
	"images/crash_4.png",
	
	"images/left.png",
	"images/right.png",
};

enum {
	SND_NONE,
	
	NUM_SOUNDS
};

const char *sound_names[NUM_SOUNDS] = {
	"sounds/none.wav",
};

/* Para el motor de botones */
enum {
	BUTTON_NONE,
	
	BUTTON_UI_INSTRUCTIONS,
	BUTTON_UI_PLAY_GAME,
	BUTTON_CLOSE,
	
	BUTTON_NEXT_PAGE,
	BUTTON_EXPLAIN_PLAY_GAME,
	
	NUM_BUTTONS
};

enum {
	BLANK_UP,
	BLANK_OVER,
	BLANK_DOWN
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
	
	COLLIDER_ONEUP,
	
	NUM_COLLIDERS
};

enum {
	TEXT_LIVES,
	TEXT_TRUCKS,
	TEXT_SCORE,
	
	TEXT_TRY_AGAIN,
	
	TEXT_UNLOADED,
	TEXT_NEXT_TRUCK,
	
	TEXT_GAME_OVER,
	
	TEXT_TITLE_BEAN_COUNTERS,
	TEXT_INSTRUCTIONS,
	TEXT_PLAY_GAME,
	TEXT_NEXT_PAGE,
	
	TEXT_CONTROLS,
	
	TEXT_EXPLAIN_1,
	TEXT_EXPLAIN_2,
	TEXT_EXPLAIN_3,
	TEXT_EXPLAIN_4,
	
	NUM_TEXTS
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
	"collider/penguin_10.col",
	
	"collider/oneup.col"
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

const int bag_stack_offsets[30][2] = {
	{15, 251},
	{14, 236},
	{13, 227},
	{13, 220},
	{13, 211},
	{13, 203},
	{12, 192},
	{12, 182},
	{13, 173},
	{9, 165}, // 10
	{10, 158},
	{10, 149},
	{10, 141},
	{8, 136},
	{8, 125},
	{8, 120},
	{0, 114},
	{2, 106},
	{2, 94},
	{2, 85}, // 20
	{2, 79},
	{2, 70},
	{2, 69},
	{2, 60},
	{2, 52},
	{2, 42},
	{2, 35},
	{2, 25},
	{2, 20}
	// {2, 11} del 30 al 60
};

const int anvil_offsets [24][2] = {
	{631, 276},
	{609, 226},
	{589, 185},
	{570, 151},
	{551, 126},
	{534, 109},
	{519, 101},
	{509, 100},
	{503, 100},
	{495, 103},
	{486, 111},
	{478, 124},
	{470, 137},
	{464, 154},
	{458, 171},
	{454, 192},
	{450, 214},
	{446, 238},
	{443, 263},
	{441, 291},
	{437, 320},
	{435, 352},
	{432, 384},
	{422, 406}
};

const int anvil_collider_offsets [23][2] = {
	{692, 298},
	{669, 245},
	{647, 201},
	{627, 165},
	{608, 138},
	{590, 120},
	{574, 111},
	{564, 109},
	{559, 109},
	{550, 112},
	{541, 119},
	{533, 132},
	{525, 145},
	{519, 162},
	{512, 180},
	{508, 202},
	{503, 223},
	{499, 249},
	{496, 275},
	{492, 304},
	{488, 333},
	{485, 366},
	{482, 399}
};

const int fish_collider_offsets [10][2] = {
	{337, 157},
	{326, 173},
	{315, 191},
	{303, 207},
	{292, 229},
	{282, 251},
	{271, 272},
	{260, 299},
	{250, 325},
	{240, 351},
};

const int flower_collider_offsets [11][2] = {
	{465, 163},
	{455, 174},
	{446, 184},
	{436, 194},
	{427, 210},
	{419, 226},
	{410, 242},
	{403, 264},
	{396, 286},
	{388, 308},
	{381, 330}
};

const int oneup_offsets[35][2] = {
	{636, 142},
	{624, 130},
	{611, 119},
	{598, 107},
	{586, 96},
	{573, 84},
	{560, 72},
	{547, 70},
	{534, 68},
	{521, 66},
	{507, 64},
	{494, 62},
	{481, 60},
	{468, 58},
	{454, 56},
	{442, 67},
	{431, 80},
	{420, 93},
	{409, 105},
	{398, 118},
	{392, 139},
	{386, 159},
	{380, 180},
	{373, 201},
	{367, 221},
	{361, 242},
	{355, 263},
	{349, 284},
	{343, 304},
	{337, 325},
	{334, 351},
	{330, 378},
	{327, 404},
	{324, 431},
	{321, 458}
};

const int fish_offsets[35][2] = {
	{634, 247},
	{603, 213},
	{574, 184},
	{546, 158},
	{518, 136},
	{492, 119},
	{468, 105},
	{445, 96},
	{425, 90},
	{407, 87},
	{393, 86},
	{383, 86},
	{375, 87},
	{368, 88},
	{361, 90},
	{354, 91},
	{347, 93},
	{336, 98},
	{325, 104},
	{314, 110},
	{302, 121},
	{289, 132},
	{277, 143},
	{265, 160},
	{254, 177},
	{242, 194},
	{231, 215},
	{220, 237},
	{210, 259},
	{200, 285},
	{189, 311},
	{179, 337},
	{170, 367},
	{160, 397},
	{122, 433}
};

const int flower_offsets[32][2] = {
	{646, 217},
	{626, 192},
	{606, 167},
	{587, 147},
	{569, 128},
	{551, 115},
	{534, 101},
	{519, 94},
	{503, 87},
	{492, 85},
	{481, 82},
	{475, 82},
	{467, 82},
	{463, 83},
	{457, 85},
	{452, 86},
	{443, 91},
	{435, 95},
	{426, 100},
	{417, 110},
	{408, 120},
	{399, 131},
	{390, 146},
	{382, 162},
	{374, 178},
	{366, 198},
	{359, 218},
	{352, 239},
	{345, 264},
	{339, 288},
	{332, 313},
	{334, 382}
};

const char *text_strings[NUM_TEXTS] = {
	gettext_noop ("LIVES:"),
	gettext_noop ("TRUCK:"),
	gettext_noop ("SCORE:"),
	
	gettext_noop ("TRY AGAIN..."),
	
	gettext_noop ("TRUCK\nUNLOADED!!"),
	gettext_noop ("NEXT  TRUCK!!"),
	
	gettext_noop ("Game Over!"),
	
	gettext_noop ("BEAN\nCOUNTERS!"),
	gettext_noop ("INSTRUCTIONS"),
	gettext_noop ("PLAY GAME!"),
	
	gettext_noop ("NEXT PAGE"),
	
	gettext_noop ("CONTROLS:"),
	
	gettext_noop ("The Coffee Shop is out of coffee! They need more coffee\nbeans, and you can help! Bags of coffee beans will be\ntossed out of the back of the van. You must catch them\nand stack them in the red plataform."),
	gettext_noop ("You can hold more than one bag at a time, but watch out!\nIf you hold too many bags at once, they'll be too heavy for\nyou to carry! Every bag you deposit adds points. The more\npoints you have in the end, the more coins you'll get."),
	gettext_noop ("Move mouse left and right to move penguin."),
	gettext_noop ("Click left mouse button when at the red platform to\ndrop off bags (if you have any)."),
};

/* Prototipos de función */
int game_intro (void);
int game_loop (void);
int game_explain (void);
int game_finish (void);
void setup (void);
SDL_Surface * set_video_mode (unsigned flags);
void setup_and_color_penguin (void);
void add_bag (int tipo);
void delete_bag (BeanBag *p);
int map_button_in_intro (int x, int y);
int map_button_in_explain (int x, int y, int escena);

/* Variables globales */
SDL_Surface * screen;
SDL_Surface * images[NUM_IMAGES];
SDL_Surface * texts[NUM_TEXTS];
SDL_Surface * penguin_images[NUM_PENGUIN_FRAMES];
int use_sound;
Collider *colliders[NUM_COLLIDERS];
Collider *colliders_hazard_block;
Collider *colliders_hazard_fish[10];
int color_penguin = 0;

Mix_Chunk * sounds[NUM_SOUNDS];
Mix_Music * mus_carnie;

BeanBag *first_bag = NULL;
BeanBag *last_bag = NULL;

TTF_Font *ttf24_klickclack;
TTF_Font *ttf196_klickclack;

int main (int argc, char *argv[]) {
	/* Recuperar las rutas del sistema */
	initSystemPaths (argv[0]);
	
	/* Inicializar l18n */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, get_l10n_path ());
	
	textdomain (PACKAGE);
	
	setup ();
	bind_textdomain_codeset (PACKAGE, "UTF-8");
	
	/* Registrar botones */
	cp_registrar_botones (NUM_BUTTONS);
	cp_registrar_boton (BUTTON_UI_INSTRUCTIONS, BLANK_UP);
	cp_registrar_boton (BUTTON_UI_PLAY_GAME, BLANK_UP);
	cp_registrar_boton (BUTTON_NEXT_PAGE, BLANK_UP);
	cp_registrar_boton (BUTTON_EXPLAIN_PLAY_GAME, BLANK_UP);
	
	cp_button_start ();
	
	do {
		if (game_intro () == GAME_QUIT) break;
		if (game_loop () == GAME_QUIT) break;
		//if (game_finish () == GAME_QUIT) break;
	} while (1 == 0);
	
	SDL_Quit ();
	return EXIT_SUCCESS;
}

int game_intro (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	SDL_Rect rect;
	int map;
	Uint32 last_time, now_time;
	SDL_Surface *color_surface;
	SDL_Rect update_rects[6];
	int num_rects;
	
	Uint32 color, blanco2;
	SDL_Surface *trans;
	int explain = 0;
	
	color = SDL_MapRGB (screen->format, 255, 255, 255);
	trans = SDL_CreateRGBSurface (SDL_SWSURFACE | SDL_SRCALPHA, 246, 24, 32, RMASK, GMASK, BMASK, AMASK);
	blanco2 = SDL_MapRGBA (trans->format, 255, 255, 255, 128);
	SDL_FillRect (trans, NULL, blanco2); /* Blanco */
	
	/* Precolorear el cuerpo del pingüino */
	color_surface = SDL_CreateRGBSurface (SDL_SWSURFACE, images[IMG_PENGUIN_INTRO_COLOR]->w, images[IMG_PENGUIN_INTRO_COLOR]->h, 32, RMASK, GMASK, BMASK, AMASK);
	SDL_FillRect (color_surface, NULL, SDL_MapRGB (color_surface->format, penguin_colors[color_penguin].r, penguin_colors[color_penguin].g, penguin_colors[color_penguin].b));
	
	SDL_BlitSurface (color_surface, NULL, images[IMG_PENGUIN_INTRO_COLOR], NULL);
	
	SDL_FreeSurface (color_surface);
	
	/* Predibujar todo */
	SDL_BlitSurface (images[IMG_BACKGROUND], NULL, screen, NULL);

	/* Dibujar la plataforma */
	rect.x = 0;
	rect.y = 355;
	rect.w = images[IMG_PLATAFORM]->w;
	rect.h = images[IMG_PLATAFORM]->h;
	
	SDL_BlitSurface (images[IMG_PLATAFORM], NULL, screen, &rect);
	
	/* Dibujar el game intro */
	rect.x = 113;
	rect.y = 35;
	rect.w = images[IMG_GAMEINTRO]->w;
	rect.h = images[IMG_GAMEINTRO]->h;
	
	SDL_BlitSurface (images[IMG_GAMEINTRO], NULL, screen, &rect);
	
	/* Dibujar el pingüino */
	rect.x = 127;
	rect.y = 68;
	rect.w = images[IMG_PENGUIN_INTRO_BACK]->w;
	rect.h = images[IMG_PENGUIN_INTRO_BACK]->h;
	
	SDL_BlitSurface (images[IMG_PENGUIN_INTRO_BACK], NULL, screen, &rect);
	
	rect.x = 134;
	rect.y = 162;
	rect.w = images[IMG_PENGUIN_INTRO_COLOR]->w;
	rect.h = images[IMG_PENGUIN_INTRO_COLOR]->h;
	
	SDL_BlitSurface (images[IMG_PENGUIN_INTRO_COLOR], NULL, screen, &rect);
	
	rect.x = 157;
	rect.y = 239;
	rect.w = images[IMG_PENGUIN_INTRO_FRONT]->w;
	rect.h = images[IMG_PENGUIN_INTRO_FRONT]->h;
	
	SDL_BlitSurface (images[IMG_PENGUIN_INTRO_FRONT], NULL, screen, &rect);
	
	/* Dibujar el título */
	rect.w = texts[TEXT_TITLE_BEAN_COUNTERS]->w;
	rect.h = texts[TEXT_TITLE_BEAN_COUNTERS]->h;
	rect.x = 504 - (rect.w / 2);
	rect.y = 92;
	
	SDL_BlitSurface (texts[TEXT_TITLE_BEAN_COUNTERS], NULL, screen, &rect);
	
	rect.w = texts[TEXT_INSTRUCTIONS]->w;
	rect.h = texts[TEXT_INSTRUCTIONS]->h;
	rect.x = 504 - (rect.w / 2);
	rect.y = 327;
	
	SDL_BlitSurface (texts[TEXT_INSTRUCTIONS], NULL, screen, &rect);
	
	rect.w = texts[TEXT_PLAY_GAME]->w;
	rect.h = texts[TEXT_PLAY_GAME]->h;
	rect.x = 504 - (rect.w / 2);
	rect.y = 383;
	
	SDL_BlitSurface (texts[TEXT_PLAY_GAME], NULL, screen, &rect);
	
	SDL_Flip (screen);
	
	do {
		last_time = SDL_GetTicks ();
		num_rects = 0;
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					map = map_button_in_intro (event.motion.x, event.motion.y);
					cp_button_motion (map);
					break;
				case SDL_MOUSEBUTTONDOWN:
					map = map_button_in_intro (event.button.x, event.button.y);
					cp_button_down (map);
					break;
				case SDL_MOUSEBUTTONUP:
					map = map_button_in_intro (event.button.x, event.button.y);
					map = cp_button_up (map);
					
					switch (map) {
						case BUTTON_UI_INSTRUCTIONS:
							done = GAME_CONTINUE;
							explain = 1;
							break;
						case BUTTON_UI_PLAY_GAME:
							done = GAME_CONTINUE;
							break;
					}
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
		
		if (cp_button_refresh[BUTTON_UI_INSTRUCTIONS]) {
			rect.x = 381; rect.y = 327;
			rect.w = 246; rect.h = 24;
			SDL_FillRect (screen, &rect, color);
			update_rects[num_rects++] = rect;
			
			rect.w = texts[TEXT_INSTRUCTIONS]->w;
			rect.h = texts[TEXT_INSTRUCTIONS]->h;
			rect.x = 504 - (rect.w / 2);
			rect.y = 327;
	
			SDL_BlitSurface (texts[TEXT_INSTRUCTIONS], NULL, screen, &rect);
			
			if (cp_button_frames[BUTTON_UI_INSTRUCTIONS] != BLANK_UP) {
				rect.x = 381;
				rect.y = 327;
				rect.w = 246;
				rect.h = 24;
				
				SDL_BlitSurface (trans, NULL, screen, &rect);
			}
			
			cp_button_refresh[BUTTON_UI_INSTRUCTIONS] = 0;
		}
		
		if (cp_button_refresh[BUTTON_UI_PLAY_GAME]) {
			rect.x = 381; rect.y = 383;
			rect.w = 246; rect.h = 24;
			SDL_FillRect (screen, &rect, color);
			update_rects[num_rects++] = rect;
			
			rect.w = texts[TEXT_PLAY_GAME]->w;
			rect.h = texts[TEXT_PLAY_GAME]->h;
			rect.x = 504 - (rect.w / 2);
			rect.y = 383;
	
			SDL_BlitSurface (texts[TEXT_PLAY_GAME], NULL, screen, &rect);
			
			if (cp_button_frames[BUTTON_UI_PLAY_GAME] != BLANK_UP) {
				rect.x = 381;
				rect.y = 383;
				rect.w = 246;
				rect.h = 24;
				
				SDL_BlitSurface (trans, NULL, screen, &rect);
			}
			
			cp_button_refresh[BUTTON_UI_PLAY_GAME] = 0;
		}
		
		SDL_UpdateRects (screen, num_rects, update_rects);
		
		//SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	SDL_FreeSurface (trans);
	
	if (explain) {
		done = game_explain ();
	}
		
	return done;
}

int game_explain (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	SDL_Rect rect;
	Uint32 last_time, now_time;
	
	SDL_Rect update_rects[6];
	int num_rects;
	int escena = 0;
	int cambiar_escena = 0;
	int map;
	int g;
	int shake_frame;
	
	Uint32 color, blanco2;
	SDL_Surface *trans1, *trans2, *mini_p, *mini_bag, *mini_shake[6];
	
	color = SDL_MapRGB (screen->format, 255, 255, 255);
	trans1 = SDL_CreateRGBSurface (SDL_SWSURFACE | SDL_SRCALPHA, texts[TEXT_NEXT_PAGE]->w, texts[TEXT_NEXT_PAGE]->h, 32, RMASK, GMASK, BMASK, AMASK);
	trans2 = SDL_CreateRGBSurface (SDL_SWSURFACE | SDL_SRCALPHA, texts[TEXT_PLAY_GAME]->w, texts[TEXT_PLAY_GAME]->h, 32, RMASK, GMASK, BMASK, AMASK);
	blanco2 = SDL_MapRGBA (trans1->format, 255, 255, 255, 128);
	SDL_FillRect (trans1, NULL, blanco2); /* Blanco */
	SDL_FillRect (trans2, NULL, blanco2); /* Blanco */
	
	mini_p = zoomSurface (penguin_images [PENGUIN_FRAME_1], 0.7654, 0.7654, 1);
	mini_bag = zoomSurface (images[IMG_BAG_3], 0.7596, 0.7596, 1);
	for (g = PENGUIN_FRAME_6_1; g <= PENGUIN_FRAME_6_6; g++) {
		mini_shake[g - PENGUIN_FRAME_6_1] = zoomSurface (penguin_images[g], 0.7654, 0.7654, 1);
	}
	
	/* Predibujar todo */
	SDL_BlitSurface (images[IMG_BACKGROUND], NULL, screen, NULL);

	/* Dibujar la plataforma */
	rect.x = 0;
	rect.y = 355;
	rect.w = images[IMG_PLATAFORM]->w;
	rect.h = images[IMG_PLATAFORM]->h;
	
	SDL_BlitSurface (images[IMG_PLATAFORM], NULL, screen, &rect);
	
	/* Dibujar el game intro */
	rect.x = 113;
	rect.y = 35;
	rect.w = images[IMG_GAMEINTRO]->w;
	rect.h = images[IMG_GAMEINTRO]->h;
	
	SDL_BlitSurface (images[IMG_GAMEINTRO], NULL, screen, &rect);
	
	rect.x = 139;
	rect.y = 319;
	rect.w = images[IMG_INTRO_PLATAFORM]->w;
	rect.h = images[IMG_INTRO_PLATAFORM]->h;
	
	SDL_BlitSurface (images[IMG_INTRO_PLATAFORM], NULL, screen, &rect);
	
	rect.x = 282;
	rect.y = 274;
	rect.w = mini_p->w;
	rect.h = mini_p->h;
	
	SDL_BlitSurface (mini_p, NULL, screen, &rect);
	
	rect.x = 396;
	rect.y = 262;
	rect.w = mini_bag->w;
	rect.h = mini_bag->h;
	
	SDL_BlitSurface (mini_bag, NULL, screen, &rect);
	
	rect.x = 470;
	rect.y = 342;
	rect.w = texts[TEXT_NEXT_PAGE]->w;
	rect.h = texts[TEXT_NEXT_PAGE]->h;
	
	SDL_BlitSurface (texts[TEXT_NEXT_PAGE], NULL, screen, &rect);
	
	rect.w = texts[TEXT_EXPLAIN_1]->w;
	rect.h = texts[TEXT_EXPLAIN_1]->h;
	rect.x = 113 + (images[IMG_GAMEINTRO]->w - rect.w) / 2;
	rect.y = 85;
	
	SDL_BlitSurface (texts[TEXT_EXPLAIN_1], NULL, screen, &rect);
	
	SDL_Flip (screen);
	
	do {
		last_time = SDL_GetTicks ();
		num_rects = 0;
		
		while (SDL_PollEvent(&event) > 0) {
			/* fprintf (stdout, "Evento: %i\n", event.type);*/
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					map = map_button_in_explain (event.motion.x, event.motion.y, escena);
					cp_button_motion (map);
					break;
				case SDL_MOUSEBUTTONDOWN:
					map = map_button_in_explain (event.button.x, event.button.y, escena);
					cp_button_down (map);
					break;
				case SDL_MOUSEBUTTONUP:
					map = map_button_in_explain (event.button.x, event.button.y, escena);
					map = cp_button_up (map);
					
					switch (map) {
						case BUTTON_NEXT_PAGE:
							escena++;
							cambiar_escena = 1;
							break;
						case BUTTON_EXPLAIN_PLAY_GAME:
							done = 1;
							break;
					}
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
		
		if (cambiar_escena) {
			rect.x = 123;
			rect.y = 45;
			rect.w = images[IMG_GAMEINTRO]->w - 20;
			rect.h = images[IMG_GAMEINTRO]->h - 20;
			
			SDL_FillRect (screen, &rect, color);
			update_rects[num_rects++] = rect;
			
			/* Redibujar la escena */
			if (escena == 1) {
				/* TODO: Poner el texto nuevo */
				rect.w = texts[TEXT_EXPLAIN_2]->w;
				rect.h = texts[TEXT_EXPLAIN_2]->h;
				rect.x = 113 + (images[IMG_GAMEINTRO]->w - rect.w) / 2;
				rect.y = 85;
	
				SDL_BlitSurface (texts[TEXT_EXPLAIN_2], NULL, screen, &rect);
				
				shake_frame = 0;
			} else if (escena == 2) {
				rect.w = texts[TEXT_CONTROLS]->w;
				rect.h = texts[TEXT_CONTROLS]->h;
				rect.x = 113 + (images[IMG_GAMEINTRO]->w - rect.w) / 2;
				rect.y = 65;
	
				SDL_BlitSurface (texts[TEXT_CONTROLS], NULL, screen, &rect);
				
				/* El texto de "mover" */
				rect.w = texts[TEXT_EXPLAIN_3]->w;
				rect.h = texts[TEXT_EXPLAIN_3]->h;
				rect.x = 113 + (images[IMG_GAMEINTRO]->w - rect.w) / 2;
				rect.y = 115;
	
				SDL_BlitSurface (texts[TEXT_EXPLAIN_3], NULL, screen, &rect);
				
				/* Dibujar el pinguino y las flechas */
				rect.x = 295;
				rect.y = 134;
				rect.w = mini_p->w;
				rect.h = mini_p->h;
	
				SDL_BlitSurface (mini_p, NULL, screen, &rect);
				
				rect.x = 283;
				rect.y = 195;
				rect.w = images[IMG_LEFT]->w;
				rect.h = images[IMG_LEFT]->h;
				
				SDL_BlitSurface (images[IMG_LEFT], NULL, screen, &rect);
				
				rect.x = 434;
				rect.y = 195;
				rect.w = images[IMG_RIGHT]->w;
				rect.h = images[IMG_RIGHT]->h;
				
				SDL_BlitSurface (images[IMG_RIGHT], NULL, screen, &rect);
				
				/* El texto de "mover" */
				rect.w = texts[TEXT_EXPLAIN_4]->w;
				rect.h = texts[TEXT_EXPLAIN_4]->h;
				rect.x = 113 + (images[IMG_GAMEINTRO]->w - rect.w) / 2;
				rect.y = 286;
	
				SDL_BlitSurface (texts[TEXT_EXPLAIN_4], NULL, screen, &rect);
				
				/* Forzar el dibujado del botón de jugar */
				cp_button_refresh[BUTTON_EXPLAIN_PLAY_GAME] = 1;
			}
			
			cambiar_escena = 0;
		}
		
		if (escena == 1) {
			/* Borrar */
			rect.x = 139;
			rect.y = 270;
			rect.w = 498;
			rect.h = 122 + 50;
			
			SDL_FillRect (screen, &rect, color);
			update_rects[num_rects++] = rect;
			
			/* Redibujar la plataforma y con el pinguino */
			rect.x = 139;
			rect.y = 319;
			rect.w = images[IMG_INTRO_PLATAFORM]->w;
			rect.h = images[IMG_INTRO_PLATAFORM]->h;
			
			SDL_BlitSurface (images[IMG_INTRO_PLATAFORM], NULL, screen, &rect);
			
			rect.x = 282;
			rect.y = 274;
			rect.w = mini_shake[shake_frame / 2]->w;
			rect.h = mini_shake[shake_frame / 2]->h;
			
			SDL_BlitSurface (mini_shake[shake_frame / 2], NULL, screen, &rect);
			
			shake_frame++;
			if (shake_frame >= 12) shake_frame = 0;
			
			/* Forzar a que se redibuje el boton de página siguiente */
			cp_button_refresh[BUTTON_NEXT_PAGE] = 1;
		}
		
		if (escena < 2 && cp_button_refresh[BUTTON_NEXT_PAGE]) {
			rect.x = 470;
			rect.y = 342;
			rect.w = texts[TEXT_NEXT_PAGE]->w;
			rect.h = texts[TEXT_NEXT_PAGE]->h;
			SDL_FillRect (screen, &rect, color);
			update_rects[num_rects++] = rect;
			
			rect.x = 470;
			rect.y = 342;
			rect.w = texts[TEXT_NEXT_PAGE]->w;
			rect.h = texts[TEXT_NEXT_PAGE]->h;
	
			SDL_BlitSurface (texts[TEXT_NEXT_PAGE], NULL, screen, &rect);
			
			if (cp_button_frames[BUTTON_NEXT_PAGE] != BLANK_UP) {
				rect.x = 470;
				rect.y = 342;
				rect.w = texts[TEXT_NEXT_PAGE]->w;
				rect.h = texts[TEXT_NEXT_PAGE]->h;
				
				SDL_BlitSurface (trans1, NULL, screen, &rect);
			}
			
			cp_button_refresh[BUTTON_NEXT_PAGE] = 0;
		}
		
		if (escena == 2 && cp_button_refresh[BUTTON_EXPLAIN_PLAY_GAME]) {
			rect.w = texts[TEXT_PLAY_GAME]->w;
			rect.h = texts[TEXT_PLAY_GAME]->h;
			rect.x = 386 - (rect.w / 2);
			rect.y = 383;
			SDL_FillRect (screen, &rect, color);
			update_rects[num_rects++] = rect;
			
			rect.w = texts[TEXT_PLAY_GAME]->w;
			rect.h = texts[TEXT_PLAY_GAME]->h;
			rect.x = 386 - (rect.w / 2);
			rect.y = 383;
	
			SDL_BlitSurface (texts[TEXT_PLAY_GAME], NULL, screen, &rect);
			
			if (cp_button_frames[BUTTON_EXPLAIN_PLAY_GAME] != BLANK_UP) {
				rect.w = texts[TEXT_PLAY_GAME]->w;
				rect.h = texts[TEXT_PLAY_GAME]->h;
				rect.x = 386 - (rect.w / 2);
				rect.y = 383;
				
				SDL_BlitSurface (trans2, NULL, screen, &rect);
			}
			
			cp_button_refresh[BUTTON_EXPLAIN_PLAY_GAME] = 0;
		}
		
		SDL_UpdateRects (screen, num_rects, update_rects);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	SDL_FreeSurface (trans1);
	SDL_FreeSurface (trans2);
	SDL_FreeSurface (mini_p);
	SDL_FreeSurface (mini_bag);
	
	for (g = 0; g < 6; g++) {
		SDL_FreeSurface (mini_shake [g]);
	}
	
	return done;
}

#if 0
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
	int i, j, k, l;
	int vidas = 3;
	int animacion;
	int try_visible = FALSE, gameover_visible = FALSE;
	int next_level_visible = NO_NEXT_LEVEL;
	int level, activator;
	int bag_activity = 15;
	int airbone = 0, max_airbone = 1;
	int nivel = 1;
	int anvil_out = FALSE, fish_out = FALSE, flower_out = FALSE;
	int oneup_toggle = TRUE;
	int fish_max = 4;
	int fish_counter = 0;
	int crash_anim = -1;
	int score = 0;
	int bag_stack = 0;
	char buffer[20];
	SDL_Surface *numbers[3][20];
	double z;
	
	SDL_Color negro, blanco, amarillo;
	blanco.r = blanco.g = blanco.b = 255;
	blanco.unused = 255;
	negro.r = negro.g = negro.b = 0;
	negro.unused = 255;
	amarillo.r = amarillo.g = 255;
	amarillo.b = 0;
	amarillo.unused = 255;
	
	SDL_Surface *vidas_p, *nivel_p, *score_p;
	
	vidas_p = draw_text_with_shadow (ttf24_klickclack, 2, "3", blanco, negro);
	nivel_p = draw_text_with_shadow (ttf24_klickclack, 2, "1", blanco, negro);
	score_p = draw_text_with_shadow (ttf24_klickclack, 2, "0", blanco, negro);
	
	/* Preparar los números en tamaño grande */
	numbers[0][19] = draw_text_with_shadow (ttf196_klickclack, 3, "1", amarillo, negro);
	numbers[1][19] = draw_text_with_shadow (ttf196_klickclack, 3, "2", amarillo, negro);
	numbers[2][19] = draw_text_with_shadow (ttf196_klickclack, 3, "3", amarillo, negro);
	
	z = 1.0;
	for (i = 18; i >= 0; i--) {
		z = z - 0.016118421;
		numbers[0][i] = zoomSurface (numbers[0][19], z, z, 1);
		numbers[1][i] = zoomSurface (numbers[1][19], z, z, 1);
		numbers[2][i] = zoomSurface (numbers[2][19], z, z, 1);
	}
	
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
					if (event.button.button != SDL_BUTTON_LEFT) break;
					if (penguinx <= 230 && bags > 0 && bags < 6 && next_level_visible != GAME_WIN) {
						bag_stack++;
						bags--;
						
						if (next_level_visible == NO_NEXT_LEVEL) {
							score = score + (nivel * 3);
							SDL_FreeSurface (score_p);
							snprintf (buffer, sizeof (buffer), "%d", score);
							score_p = draw_text_with_shadow (ttf24_klickclack, 2, buffer, blanco, negro);
						} else if (next_level_visible == NEXT_LEVEL) {
							score = score + (nivel * 25);
							SDL_FreeSurface (score_p);
							snprintf (buffer, sizeof (buffer), "%d", score);
							score_p = draw_text_with_shadow (ttf24_klickclack, 2, buffer, blanco, negro);
						}
						/* TODO: Sonido de poner bolsa */
						
						if (bag_stack == (nivel + 1) * 10) {
							/* Activar la pantalla de next_level */
							if (nivel != 5) {
								next_level_visible = NEXT_LEVEL;
								animacion = 0;
								
								airbone = 1000;
								printf ("Next level visible\n");
								oneup_toggle = TRUE;
								fish_counter = 0;
								fish_max = fish_max + 4;
							} else {
								/* TODO: Fin del juego */
								next_level_visible = GAME_WIN;
								airbone = 1000;
							}
						}
					}
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
		
		if (bags < 6 && next_level_visible == NO_NEXT_LEVEL) {
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
				} else if (i == 5 && nivel >= 2 && anvil_out == FALSE) {
					add_bag (5);
					airbone++;
					anvil_out = TRUE;
				} else if (i == 4 && oneup_toggle == TRUE && nivel >= 3 && nivel % 2 == 1) {
					add_bag (4);
					oneup_toggle = FALSE;
					airbone++;
				} else if (i == 6 && nivel >= 3 && fish_out == FALSE && fish_counter <= fish_max) {
					add_bag (6);
					fish_counter++;
					fish_out = TRUE;
					airbone++;
				} else if (i == 7 && nivel >= 4 && flower_out == FALSE) {
					add_bag (7);
					flower_out = TRUE;
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
			
			/* Nota, no entra a este if si el pinguino está crasheado */
			if (j < 0 && next_level_visible == NO_NEXT_LEVEL && bags < 6 && thisbag->bag <= 3 && thisbag->frame > 6) {
				/* Calcular aquí la colisión contra el pingüino */
				i = collider_hittest (colliders[COLLIDER_BAG_3], thisbag->bag_points[thisbag->frame][1], thisbag->bag_points[thisbag->frame][2], colliders[k], penguinx - 120, 251);
			
				if (i == SDL_TRUE) {
					if (bags < 6) bags++;
					
					k = COLLIDER_PENGUIN_1 + bags;
				
					/* Reproducir el sonido de "Agarrar bolsa" */
					
					if (bags >= 6) {
						/* Esta bolsa crasheó al pingüino */
						printf ("Penguin Crash\n");
						if (vidas > 0) {
							try_visible = TRUE;
							printf ("Try again visible\n");
							animacion = 0;
							airbone = 1000; /* El airbone bloquea que salgan más objetos */
							vidas--;
							SDL_FreeSurface (vidas_p);
							snprintf (buffer, sizeof (buffer), "%d", vidas);
							vidas_p = draw_text_with_shadow (ttf24_klickclack, 2, buffer, blanco, negro);
							/* TODO: Reproducir aquí el sonido de golpe */
						} else {
							gameover_visible = TRUE;
							printf ("Game Over visible\n");
						}
					} else {
						/* Sumar solo si no crasheó al pinguino */
						score = score + (nivel * 2);
						SDL_FreeSurface (score_p);
						snprintf (buffer, sizeof (buffer), "%d", score);
						score_p = draw_text_with_shadow (ttf24_klickclack, 2, buffer, blanco, negro);
					}
					airbone--;
					printf ("Airbone: %i\n", airbone);
					delete_bag (thisbag);
					thisbag = nextbag;
					continue;
				}
			} else if (j < 0 && thisbag->bag == 5 && next_level_visible == NO_NEXT_LEVEL && thisbag->frame > 6) {
				i = collider_hittest (colliders_hazard_block, anvil_collider_offsets[thisbag->frame][0], anvil_collider_offsets[thisbag->frame][1], colliders[k], penguinx - 120, 251);
				
				if (i == SDL_TRUE) {
					bags = 7;
					
					/* TODO: Reproducir el sonido de golpe de yunque */
					crash_anim = 0;
					
					printf ("Penguin Crash by anvil\n");
					if (vidas > 0 && try_visible == FALSE) {
						try_visible = TRUE;
						printf ("Try again visible\n");
						animacion = 0;
						airbone = 1000; /* El airbone bloquea que salgan más objetos */
						printf ("Airbone: %i\n", airbone);
						vidas--;
						SDL_FreeSurface (vidas_p);
						snprintf (buffer, sizeof (buffer), "%d", vidas);
						vidas_p = draw_text_with_shadow (ttf24_klickclack, 2, buffer, blanco, negro);
					} else if (try_visible == FALSE) {
						gameover_visible = TRUE;
						printf ("Game Over visible\n");
					}
					
					anvil_out = FALSE;
					airbone--;
					printf ("Airbone: %i\n", airbone);
					delete_bag (thisbag);
					thisbag = nextbag;
					continue;
				}
			} else if (j < 0 && thisbag->bag == 4 && next_level_visible == NO_NEXT_LEVEL && thisbag->frame > 6 && bags < 6) {
				i = collider_hittest (colliders[COLLIDER_ONEUP], thisbag->object_points[thisbag->frame][0], thisbag->object_points[thisbag->frame][1], colliders[k], penguinx - 120, 251);
				
				if (i == SDL_TRUE) {
					vidas++;
					
					SDL_FreeSurface (vidas_p);
					snprintf (buffer, sizeof (buffer), "%d", vidas);
					vidas_p = draw_text_with_shadow (ttf24_klickclack, 2, buffer, blanco, negro);
					/* TODO: Reproducir sonido boing */
					
					/* TODO: Mostrar la notificación de 1 vida */
					airbone--;
					delete_bag (thisbag);
					thisbag = nextbag;
					continue;
				}
			} else if (thisbag->bag == 6 && thisbag->frame >= 22 && thisbag->frame <= 31 && next_level_visible == NO_NEXT_LEVEL) {
				l = thisbag->frame - 22;
				
				i = collider_hittest (colliders_hazard_fish[l], fish_collider_offsets[l][0], fish_collider_offsets[l][1], colliders[k], penguinx - 120, 251);
				
				if (i == SDL_TRUE) {
					bags = 8;
					
					/* TODO: Reproducir el sonido de golpe de pescado */
					crash_anim = 0;
					if (vidas > 0 && try_visible == FALSE) {
						try_visible = TRUE;
						animacion = 0;
						airbone = 1000; /* El airbone bloquea que salgan más objetos */
						vidas--;
						SDL_FreeSurface (vidas_p);
						snprintf (buffer, sizeof (buffer), "%d", vidas);
						vidas_p = draw_text_with_shadow (ttf24_klickclack, 2, buffer, blanco, negro);
					} else if (try_visible == FALSE) {
						gameover_visible = TRUE;
					}
					
					fish_out = FALSE;
					airbone--;
					delete_bag (thisbag);
					thisbag = nextbag;
					continue;
				}
			} else if (thisbag->bag == 7 && thisbag->frame >= 18 && thisbag->frame <= 28 && next_level_visible == NO_NEXT_LEVEL) {
				l = thisbag->frame - 18;
				i = collider_hittest (colliders_hazard_block, flower_collider_offsets[l][0], flower_collider_offsets[l][1], colliders[k], penguinx - 120, 251);
				
				if (i == SDL_TRUE) {
					bags = 9;
					
					/* TODO: Reproducir el sonido de golpe de florero */
					crash_anim = 0;
					
					if (vidas > 0 && try_visible == FALSE) {
						try_visible = TRUE;
						animacion = 0;
						airbone = 1000; /* El airbone bloquea que salgan más objetos */
						vidas--;
						SDL_FreeSurface (vidas_p);
						snprintf (buffer, sizeof (buffer), "%d", vidas);
						vidas_p = draw_text_with_shadow (ttf24_klickclack, 2, buffer, blanco, negro);
					} else if (try_visible == FALSE) {
						gameover_visible = TRUE;
					}
					
					flower_out = FALSE;
					airbone--;
					delete_bag (thisbag);
					thisbag = nextbag;
					continue;
				}
			}
			
			if (thisbag->bag <= 3 && j == 0) {
				/* Eliminar del airbone */
				airbone--;
			} else if (thisbag->bag == 5 && j == 0) {
				/* Eliminar el yunque del airbone */
				airbone--;
				anvil_out = FALSE;
			} else if (thisbag->bag == 6 && j == 0) {
				/* Eliminar el pescado del airbone */
				airbone--;
				fish_out = FALSE;
			} else if (thisbag->bag == 7 && j == 0) {
				airbone--;
				flower_out = FALSE;
			}
			
			if (thisbag->bag == 4 && j >= 0) {
				/* Eliminar la vida */
				airbone--;
				delete_bag (thisbag);
			} else if (j >= 35) {
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
		
		/* Dibujar la pila de bolsas de café, arriba de la plataforma, por detrás del camión */
		if (bag_stack > 0) {
			// 64.95, 376.95
			if (bag_stack < 30) {
				rect.x = -6 + bag_stack_offsets[bag_stack - 1][0];
				rect.y = 100 + bag_stack_offsets[bag_stack - 1][1];
			} else {
				rect.x = -4;
				rect.y = 111;
			}
			
			i = IMG_BAG_STACK_1 + (bag_stack - 1);
			rect.w = images[i]->w;
			rect.h = images[i]->h;
			
			SDL_BlitSurface (images[i], NULL, screen, &rect);
		}
		
		if (gameover_visible == TRUE) {
			rect.w = texts[TEXT_GAME_OVER]->w;
			rect.h = texts[TEXT_GAME_OVER]->h;
			
			rect.x = 365 - (rect.w / 2);
			rect.y = 145;
			
			SDL_BlitSurface (texts[TEXT_GAME_OVER], NULL, screen, &rect);
		}
		
		/* Los mensajes de texto van antes de las bolsas */
		rect.x = 30;
		rect.y = 8;
		rect.w = texts[TEXT_LIVES]->w;
		rect.h = texts[TEXT_LIVES]->h;
		
		SDL_BlitSurface (texts[TEXT_LIVES], NULL, screen, &rect);
		
		rect.x = 30 + texts[TEXT_LIVES]->w + 2;
		rect.y = 8;
		rect.w = vidas_p->w;
		rect.h = vidas_p->h;
		
		SDL_BlitSurface (vidas_p, NULL, screen, &rect);
		
		rect.x = 216;
		rect.y = 8;
		rect.w = texts[TEXT_TRUCKS]->w;
		rect.h = texts[TEXT_TRUCKS]->h;
		
		SDL_BlitSurface (texts[TEXT_TRUCKS], NULL, screen, &rect);
		
		rect.x = 216 + texts[TEXT_TRUCKS]->w + 5;
		rect.y = 8;
		rect.w = nivel_p->w;
		rect.h = nivel_p->h;
		
		SDL_BlitSurface (nivel_p, NULL, screen, &rect);
		
		rect.x = 390;
		rect.y = 8;
		rect.w = texts[TEXT_SCORE]->w;
		rect.h = texts[TEXT_SCORE]->h;
		
		SDL_BlitSurface (texts[TEXT_SCORE], NULL, screen, &rect);
		
		rect.x = 390 + texts[TEXT_SCORE]->w + 5;
		rect.h = 8;
		rect.w = score_p->w;
		rect.h = score_p->h;
		
		SDL_BlitSurface (score_p, NULL, screen, &rect);
		
		/* Dibujar los objetos en pantalla */
		thisbag = first_bag;
		while (thisbag != NULL) {
			if (thisbag->bag <= 3) {
				/* Dibujar las bolsas de café estándar */
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
			} else if (thisbag->bag == 5) {
				/* Dibujar un yunque */
				if (thisbag->frame < thisbag->throw_length) {
					i = IMG_ANVIL_0 + thisbag->frame;
					rect.x = thisbag->object_points[thisbag->frame][0];
					rect.y = thisbag->object_points[thisbag->frame][1];
					j = 0;
				} else {
					i = IMG_ANVIL_23;
					rect.x = thisbag->object_points[23][0];
					rect.y = thisbag->object_points[23][1];
					j = thisbag->frame - thisbag->throw_length;
				}
				
				rect.w = images[i]->w;
				rect.h = images[i]->h;
				
				if (i == IMG_ANVIL_23 && j > 25) {
					SDL_gfxBlitRGBAWithAlpha (images[i], NULL, screen, &rect, 255 - SDL_ALPHA_OPAQUE * (j - 25) / 10);
				} else {
					SDL_BlitSurface (images[i], NULL, screen, &rect);
				}
			} else if (thisbag->bag == 4) {
				/* Dibujar la vida */
				i = IMG_ONEUP;
				rect.x = thisbag->object_points[thisbag->frame][0];
				rect.y = thisbag->object_points[thisbag->frame][1];
				rect.w = images[i]->w;
				rect.h = images[i]->h;
				
				SDL_BlitSurface (images[i], NULL, screen, &rect);
			} else if (thisbag->bag == 6) {
				if (thisbag->frame < thisbag->throw_length) {
					i = IMG_FISH;
					rect.x = thisbag->object_points[thisbag->frame][0];
					rect.y = thisbag->object_points[thisbag->frame][1];
				} else {
					i = IMG_FISH_DROPPED;
					rect.x = thisbag->object_points[34][0];
					rect.y = thisbag->object_points[34][1];
					j = thisbag->frame - thisbag->throw_length;
				}
				rect.w = images[i]->w;
				rect.h = images[i]->h;
				
				if (i == IMG_FISH_DROPPED && j > 25) {
					SDL_gfxBlitRGBAWithAlpha (images[i], NULL, screen, &rect, 255 - SDL_ALPHA_OPAQUE * (j - 25) / 10);
				} else {
					SDL_BlitSurface (images[i], NULL, screen, &rect);
				}
			} else if (thisbag->bag == 7) {
				if (thisbag->frame < thisbag->throw_length) {
					i = IMG_FLOWER;
					rect.x = thisbag->object_points[thisbag->frame][0];
					rect.y = thisbag->object_points[thisbag->frame][1];
				} else {
					i = IMG_FLOWER_DROPPED;
					rect.x = thisbag->object_points[31][0];
					rect.y = thisbag->object_points[31][1];
					j = thisbag->frame - thisbag->throw_length;
				}
				rect.w = images[i]->w;
				rect.h = images[i]->h;
				
				if (i == IMG_FLOWER_DROPPED && j > 25) {
					SDL_gfxBlitRGBAWithAlpha (images[i], NULL, screen, &rect, 255 - SDL_ALPHA_OPAQUE * (j - 25) / 10);
				} else {
					SDL_BlitSurface (images[i], NULL, screen, &rect);
				}
			}
			
			thisbag = thisbag->next;
		}
		
		if (crash_anim >= 0) {
			if (crash_anim == 4) {
				i = IMG_CRASH_4;
			} else {
				i = IMG_CRASH_1 + crash_anim;
			}
			rect.x = penguinx - 90;
			rect.y = 282;
			rect.w = images[i]->w;
			rect.h = images[i]->h;
			
			SDL_BlitSurface (images[i], NULL, screen, &rect);
			
			crash_anim++;
			
			if (crash_anim == 5) crash_anim = -1;
		}
		
		if (try_visible == TRUE) {
			rect.w = texts[TEXT_TRY_AGAIN]->w;
			rect.h = texts[TEXT_TRY_AGAIN]->h;
			rect.x = 388 - (rect.w / 2);
			rect.y = 126;
			
			SDL_BlitSurface (texts[TEXT_TRY_AGAIN], NULL, screen, &rect);
			
			/* Poner el número 3, 2, 1 */
			i = -1;
			if (animacion >= 20 && animacion < 40) {
				j = animacion - 20;
				i = 2;
			} else if (animacion >= 44 && animacion < 64) {
				j = animacion - 44;
				i = 1;
			} else if (animacion >= 68 && animacion < 88) {
				j = animacion - 68;
				i = 0;
			}
			
			if (i >= 0) {
				rect.w = numbers[i][j]->w;
				rect.h = numbers[i][j]->h;
				rect.x = 371 - (rect.w / 2);
				rect.y = 122 - j;
				SDL_gfxBlitRGBAWithAlpha (numbers[i][j], NULL, screen, &rect, (255 - (12.75 * ((float) j))));
			}
			
			animacion++;
		}
		
		if (next_level_visible == NEXT_LEVEL) {
			if (animacion < 52) {
				/* Presentar el texto del camión descargado */
				rect.w = texts[TEXT_UNLOADED]->w;
				rect.h = texts[TEXT_UNLOADED]->h;
				
				rect.x = 388 - (rect.w / 2);
				rect.y = 115;
				
				SDL_BlitSurface (texts[TEXT_UNLOADED], NULL, screen, &rect);
			} else if (animacion > 62) {
				rect.w = texts[TEXT_NEXT_TRUCK]->w;
				rect.h = texts[TEXT_NEXT_TRUCK]->h;
				
				rect.x = 378 - (rect.w / 2);
				rect.y = 121;
				
				SDL_BlitSurface (texts[TEXT_NEXT_TRUCK], NULL, screen, &rect);
			}
			
			if (animacion < 36) {
				rect.x = 568 + (198 * animacion) / 36;
			} else if (animacion >= 36 && animacion < 60) {
				rect.x = 766; /* Fuera, no dibuja */
			} else if (animacion >= 60 && animacion < 77) {
				rect.x = 646 + (120 * (77 - animacion)) / 16;
			} else if (animacion >= 77) {
				rect.x = 568 + (78 * (97 - animacion)) / 20;
			}
			rect.y = 72;
			rect.w = images[IMG_TRUCK]->w;
			rect.h = images[IMG_TRUCK]->h;
			
			SDL_BlitSurface (images[IMG_TRUCK], NULL, screen, &rect);
			animacion++;
		} else {
			/* Dibujar el camión normal */
			rect.x = 568;
			rect.y = 72;
			rect.w = images[IMG_TRUCK]->w;
			rect.h = images[IMG_TRUCK]->h;
	
			SDL_BlitSurface (images[IMG_TRUCK], NULL, screen, &rect);
		}
		
		SDL_Flip (screen);
		
		if (try_visible == TRUE && animacion >= 92) {
			/* Continuar nivel */
			airbone = 0;
			bags = 0;
			try_visible = FALSE;
		}
		
		if (next_level_visible == NEXT_LEVEL && animacion >= 97) {
			/* Pasar de nivel */
			if (bag_activity > 1) {
				bag_activity = bag_activity - 3;
			}
			
			if (nivel != 2) {
				max_airbone++;
			}
			
			nivel++;
			
			SDL_FreeSurface (nivel_p);
			snprintf (buffer, sizeof (buffer), "%d", nivel);
			nivel_p = draw_text_with_shadow (ttf24_klickclack, 2, buffer, blanco, negro);
			airbone = 0;
			
			bag_stack = 0;
			bags = 0;
			penguin_frame = 0;
			
			next_level_visible = NO_NEXT_LEVEL;
		}
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	/* Liberar los números usados */
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 20; j++) {
			SDL_FreeSurface (numbers[i][j]);
		}
	}
	
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
	TTF_Font *ttf48_klickclack, *ttf52_klickclack, *ttf40_klickclack, *ttf18_burbank;
	
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
	
	/* Generar los colliders de bloque */
	colliders_hazard_block = collider_new_block (9, 45);
	
	colliders_hazard_fish[0] = collider_new_block (22, 18);
	colliders_hazard_fish[1] = collider_new_block (21, 18);
	colliders_hazard_fish[2] = collider_new_block (20, 18);
	colliders_hazard_fish[3] = collider_new_block (19, 18);
	colliders_hazard_fish[4] = collider_new_block (18, 18);
	colliders_hazard_fish[5] = collider_new_block (17, 18);
	colliders_hazard_fish[6] = collider_new_block (15, 18);
	colliders_hazard_fish[7] = collider_new_block (14, 18);
	colliders_hazard_fish[8] = collider_new_block (13, 18);
	colliders_hazard_fish[9] = collider_new_block (11, 18);
	
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
	
	sprintf (buffer_file, "%s%s", systemdata_path, "klickclack.ttf");
	ttf24_klickclack = TTF_OpenFont (buffer_file, 24);
	ttf196_klickclack = TTF_OpenFont (buffer_file, 196);
	ttf48_klickclack = TTF_OpenFont (buffer_file, 48);
	ttf40_klickclack = TTF_OpenFont (buffer_file, 40);
	ttf52_klickclack = TTF_OpenFont (buffer_file, 52);
	
	if (!ttf24_klickclack || !ttf196_klickclack || !ttf48_klickclack || !ttf52_klickclack || !ttf40_klickclack) {
		fprintf (stderr,
			_("Failed to load font file 'Klick Clack\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	sprintf (buffer_file, "%s%s", systemdata_path, "burbanks.ttf");
	ttf18_burbank = TTF_OpenFont (buffer_file, 18);
	
	if (!ttf18_burbank) {
		fprintf (stderr,
			_("Failed to load font file 'Burbank Small\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	bind_textdomain_codeset (PACKAGE, "UTF-8");
	
	SDL_Color negro, blanco, azul1;
	blanco.r = blanco.g = blanco.b = 255;
	negro.r = negro.g = negro.b = 0;
	azul1.r = 0x01;
	azul1.g = 0x34;
	azul1.b = 0x9a;
	
	for (g = TEXT_LIVES; g <= TEXT_SCORE; g++) {
		texts[g] = draw_text_with_shadow (ttf24_klickclack, 2, _(text_strings[g]), blanco, negro);
	}
	
	texts[TEXT_TRY_AGAIN] = draw_text_with_shadow (ttf48_klickclack, 2, _(text_strings[TEXT_TRY_AGAIN]), blanco, negro);
	texts[TEXT_UNLOADED] = draw_text_with_shadow (ttf48_klickclack, 2, _(text_strings[TEXT_UNLOADED]), blanco, negro);
	texts[TEXT_NEXT_TRUCK] = draw_text_with_shadow (ttf48_klickclack, 2, _(text_strings[TEXT_NEXT_TRUCK]), blanco, negro);
	texts[TEXT_GAME_OVER] = draw_text_with_shadow (ttf52_klickclack, 3, _(text_strings[TEXT_GAME_OVER]), blanco, negro);
	
	texts[TEXT_TITLE_BEAN_COUNTERS] = draw_text (ttf40_klickclack, _(text_strings[TEXT_TITLE_BEAN_COUNTERS]), &azul1);
	texts[TEXT_INSTRUCTIONS] = draw_text (ttf24_klickclack, _(text_strings[TEXT_INSTRUCTIONS]), &azul1);
	texts[TEXT_PLAY_GAME] = draw_text (ttf24_klickclack, _(text_strings[TEXT_PLAY_GAME]), &azul1);
	texts[TEXT_NEXT_PAGE] = draw_text (ttf24_klickclack, _(text_strings[TEXT_NEXT_PAGE]), &azul1);
	texts[TEXT_CONTROLS] = draw_text (ttf40_klickclack, _(text_strings[TEXT_CONTROLS]), &azul1);
	texts[TEXT_EXPLAIN_1] = draw_text (ttf18_burbank, _(text_strings[TEXT_EXPLAIN_1]), &azul1);
	texts[TEXT_EXPLAIN_2] = draw_text (ttf18_burbank, _(text_strings[TEXT_EXPLAIN_2]), &azul1);
	texts[TEXT_EXPLAIN_3] = draw_text (ttf18_burbank, _(text_strings[TEXT_EXPLAIN_3]), &azul1);
	texts[TEXT_EXPLAIN_4] = draw_text (ttf18_burbank, _(text_strings[TEXT_EXPLAIN_4]), &azul1);
	
	TTF_CloseFont (ttf18_burbank);
	TTF_CloseFont (ttf48_klickclack);
	TTF_CloseFont (ttf52_klickclack);
	TTF_CloseFont (ttf40_klickclack);
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
	} else if (tipo == 5) {
		new->throw_length = 23;
		new->object_points = anvil_offsets;
	} else if (tipo == 4) {
		new->throw_length = 35;
		new->object_points = oneup_offsets;
	} else if (tipo == 6) {
		new->throw_length = 34;
		new->object_points = fish_offsets;
	} else if (tipo == 7) {
		new->throw_length = 31;
		new->object_points = flower_offsets;
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

int map_button_in_intro (int x, int y) {
	if (x >= 381 && x < 627 && y >= 324 && y < 348) return BUTTON_UI_INSTRUCTIONS;
	if (x >= 381 && x < 627 && y >= 380 && y < 404) return BUTTON_UI_PLAY_GAME;
	return BUTTON_NONE;
}

int map_button_in_explain (int x, int y, int escena) {
	if (escena < 2 && x >= 470 && x < 470 + texts[TEXT_NEXT_PAGE]->w && y >= 339 && y < 339 + texts[TEXT_NEXT_PAGE]->h) return BUTTON_NEXT_PAGE;
	if (escena == 2 && x >= 386 - (texts[TEXT_PLAY_GAME]->w / 2) && x < 386 + (texts[TEXT_PLAY_GAME]->w / 2) && y >= 378 && y < 383 + texts[TEXT_PLAY_GAME]->h) return BUTTON_EXPLAIN_PLAY_GAME;
	return BUTTON_NONE;
}

