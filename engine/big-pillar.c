/*
  big-pillar.c -- big pillar module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kernel/video.h"
#include "floor.h"
#include "room.h"
#include "big-pillar.h"

/* dungeon vga */
ALLEGRO_BITMAP *dv_big_pillar_bottom_left, *dv_big_pillar_bottom_right,
  *dv_big_pillar_top_left, *dv_big_pillar_top_right, *dv_big_pillar_top_right_top,
  *dv_big_pillar_bottom_fg;

/* palace vga */
ALLEGRO_BITMAP *pv_big_pillar_bottom_left, *pv_big_pillar_bottom_right,
  *pv_big_pillar_top_left, *pv_big_pillar_top_right, *pv_big_pillar_top_right_top,
  *pv_big_pillar_bottom_fg;

void
load_big_pillar (void)
{
  /* dungeon vga */
  dv_big_pillar_bottom_left = load_bitmap (DV_BIG_PILLAR_BOTTOM_LEFT);
  dv_big_pillar_bottom_right = load_bitmap (DV_BIG_PILLAR_BOTTOM_RIGHT);
  dv_big_pillar_top_left = load_bitmap (DV_BIG_PILLAR_TOP_LEFT);
  dv_big_pillar_top_right = load_bitmap (DV_BIG_PILLAR_TOP_RIGHT);
  dv_big_pillar_top_right_top = load_bitmap (DV_BIG_PILLAR_TOP_RIGHT_TOP);
  dv_big_pillar_bottom_fg = load_bitmap (DV_BIG_PILLAR_BOTTOM_FG);

  /* palace vga */
  pv_big_pillar_bottom_left = load_bitmap (PV_BIG_PILLAR_BOTTOM_LEFT);
  pv_big_pillar_bottom_right = load_bitmap (PV_BIG_PILLAR_BOTTOM_RIGHT);
  pv_big_pillar_top_left = load_bitmap (PV_BIG_PILLAR_TOP_LEFT);
  pv_big_pillar_top_right = load_bitmap (PV_BIG_PILLAR_TOP_RIGHT);
  pv_big_pillar_top_right_top = load_bitmap (PV_BIG_PILLAR_TOP_RIGHT_TOP);
  pv_big_pillar_bottom_fg = load_bitmap (PV_BIG_PILLAR_BOTTOM_FG);
}

void
unload_big_pillar (void)
{
  /* dungeon vga */
  al_destroy_bitmap (dv_big_pillar_bottom_left);
  al_destroy_bitmap (dv_big_pillar_bottom_right);
  al_destroy_bitmap (dv_big_pillar_top_left);
  al_destroy_bitmap (dv_big_pillar_top_right);
  al_destroy_bitmap (dv_big_pillar_top_right_top);
  al_destroy_bitmap (dv_big_pillar_bottom_fg);

  /* palace vga */
  al_destroy_bitmap (pv_big_pillar_bottom_left);
  al_destroy_bitmap (pv_big_pillar_bottom_right);
  al_destroy_bitmap (pv_big_pillar_top_left);
  al_destroy_bitmap (pv_big_pillar_top_right);
  al_destroy_bitmap (pv_big_pillar_top_right_top);
  al_destroy_bitmap (pv_big_pillar_bottom_fg);
}

void
draw_big_pillar_bottom (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  draw_floor_base (bitmap, p, em, vm);
  draw_big_pillar_bottom_left (bitmap, p, em, vm);
  draw_big_pillar_bottom_right (bitmap, p, em, vm);
}

void
draw_big_pillar_bottom_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *big_pillar_bottom_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: big_pillar_bottom_left = dv_big_pillar_bottom_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: big_pillar_bottom_left = pv_big_pillar_bottom_left; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (big_pillar_bottom_left, bitmap,
                big_pillar_bottom_left_coord (p, &c), 0);
}

void
draw_big_pillar_bottom_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *big_pillar_bottom_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: big_pillar_bottom_right = dv_big_pillar_bottom_right; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: big_pillar_bottom_right = pv_big_pillar_bottom_right; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (big_pillar_bottom_right, bitmap,
                big_pillar_bottom_right_coord (p, &c), 0);
}

void
draw_big_pillar_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *big_pillar_bottom_fg = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: big_pillar_bottom_fg = dv_big_pillar_bottom_fg; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: big_pillar_bottom_fg = pv_big_pillar_bottom_fg; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (big_pillar_bottom_fg, bitmap, big_pillar_bottom_fg_coord (p, &c), 0);
}

void
draw_big_pillar_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  draw_floor_base (bitmap, p, em, vm);
  draw_big_pillar_top_left (bitmap, p, em, vm);
  draw_big_pillar_top_right (bitmap, p, em, vm);
}

void
draw_big_pillar_top_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *big_pillar_top_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: big_pillar_top_left = dv_big_pillar_top_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: big_pillar_top_left = pv_big_pillar_top_left; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (big_pillar_top_left, bitmap,
                big_pillar_top_left_coord (p, &c), 0);
}

void
draw_big_pillar_top_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *big_pillar_top_right = NULL,
    *big_pillar_top_right_top = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA:
      big_pillar_top_right = dv_big_pillar_top_right;
      big_pillar_top_right_top = dv_big_pillar_top_right_top;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA:
      big_pillar_top_right = pv_big_pillar_top_right;
      big_pillar_top_right_top = pv_big_pillar_top_right_top;
      break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (big_pillar_top_right, bitmap,
                big_pillar_top_right_coord (p, &c), 0);
  draw_bitmapc (big_pillar_top_right_top, bitmap,
                big_pillar_top_right_top_coord (p, &c), 0);
}

struct coord *
big_pillar_bottom_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_bottom_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_top_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_top_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_top_right_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 3;
  c->room = p->room;
  return c;
}

struct coord *
big_pillar_bottom_fg_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}