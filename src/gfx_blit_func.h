/*
 * sdl_blit_func.h
 * This file is part of Pizzatron
 *
 * Copyright (C) 2017 - Félix Arreola Rodríguez
 *
 * Pizzatron is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Pizzatron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pizzatron; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __GFX_BLIT_FUNC_H__
#define __GFX_BLIT_FUNC_H__

int SDL_gfxBlitRGBA(SDL_Surface * src, SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect);
int SDL_gfxBlitRGBAWithAlpha(SDL_Surface * src, SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect, Uint8 alpha);

#endif /* __GFX_BLIT_FUNC_H__ */

