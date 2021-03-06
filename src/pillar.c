/*
  pillar.c -- pillar module;

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

#include "mininim.h"

/* dungeon cga */
ALLEGRO_BITMAP *dc_pillar_left, *dc_pillar_right,
  *dc_pillar_top, *dc_pillar_fg;

/* palace cga */
ALLEGRO_BITMAP *pc_pillar_left, *pc_pillar_right,
  *pc_pillar_top, *pc_pillar_fg;

/* dungeon ega */
ALLEGRO_BITMAP *de_pillar_left, *de_pillar_right,
  *de_pillar_top, *de_pillar_fg;

/* palace ega */
ALLEGRO_BITMAP *pe_pillar_left, *pe_pillar_right,
  *pe_pillar_top, *pe_pillar_fg;

/* dungeon vga */
ALLEGRO_BITMAP *dv_pillar_left, *dv_pillar_right,
  *dv_pillar_top, *dv_pillar_fg;

/* palace vga */
ALLEGRO_BITMAP *pv_pillar_left, *pv_pillar_right,
  *pv_pillar_top, *pv_pillar_fg;

void
load_pillar (void)
{
  /* dungeon cga */
  dc_pillar_left = load_bitmap (DC_PILLAR_LEFT);
  dc_pillar_right = load_bitmap (DC_PILLAR_RIGHT);
  dc_pillar_top = load_bitmap (DC_PILLAR_TOP);
  dc_pillar_fg = load_bitmap (DC_PILLAR_FG);

  /* palace cga */
  pc_pillar_left = load_bitmap (PC_PILLAR_LEFT);
  pc_pillar_right = load_bitmap (PC_PILLAR_RIGHT);
  pc_pillar_top = load_bitmap (PC_PILLAR_TOP);
  pc_pillar_fg = load_bitmap (PC_PILLAR_FG);

  /* dungeon ega */
  de_pillar_left = load_bitmap (DE_PILLAR_LEFT);
  de_pillar_right = load_bitmap (DE_PILLAR_RIGHT);
  de_pillar_top = load_bitmap (DE_PILLAR_TOP);
  de_pillar_fg = load_bitmap (DE_PILLAR_FG);

  /* palace ega */
  pe_pillar_left = load_bitmap (PE_PILLAR_LEFT);
  pe_pillar_right = load_bitmap (PE_PILLAR_RIGHT);
  pe_pillar_top = load_bitmap (PE_PILLAR_TOP);
  pe_pillar_fg = load_bitmap (PE_PILLAR_FG);

  /* dungeon vga */
  dv_pillar_left = load_bitmap (DV_PILLAR_LEFT);
  dv_pillar_right = load_bitmap (DV_PILLAR_RIGHT);
  dv_pillar_top = load_bitmap (DV_PILLAR_TOP);
  dv_pillar_fg = load_bitmap (DV_PILLAR_FG);

  /* palace vga */
  pv_pillar_left = load_bitmap (PV_PILLAR_LEFT);
  pv_pillar_right = load_bitmap (PV_PILLAR_RIGHT);
  pv_pillar_top = load_bitmap (PV_PILLAR_TOP);
  pv_pillar_fg = load_bitmap (PV_PILLAR_FG);
}

void
unload_pillar (void)
{
  /* dungeon cga */
  al_destroy_bitmap (dc_pillar_left);
  al_destroy_bitmap (dc_pillar_right);
  al_destroy_bitmap (dc_pillar_top);
  al_destroy_bitmap (dc_pillar_fg);

  /* palace cga */
  al_destroy_bitmap (pc_pillar_left);
  al_destroy_bitmap (pc_pillar_right);
  al_destroy_bitmap (pc_pillar_top);
  al_destroy_bitmap (pc_pillar_fg);

  /* dungeon ega */
  al_destroy_bitmap (de_pillar_left);
  al_destroy_bitmap (de_pillar_right);
  al_destroy_bitmap (de_pillar_top);
  al_destroy_bitmap (de_pillar_fg);

  /* palace ega */
  al_destroy_bitmap (pe_pillar_left);
  al_destroy_bitmap (pe_pillar_right);
  al_destroy_bitmap (pe_pillar_top);
  al_destroy_bitmap (pe_pillar_fg);

  /* dungeon vga */
  al_destroy_bitmap (dv_pillar_left);
  al_destroy_bitmap (dv_pillar_right);
  al_destroy_bitmap (dv_pillar_top);
  al_destroy_bitmap (dv_pillar_fg);

  /* palace vga */
  al_destroy_bitmap (pv_pillar_left);
  al_destroy_bitmap (pv_pillar_right);
  al_destroy_bitmap (pv_pillar_top);
  al_destroy_bitmap (pv_pillar_fg);
}

void
draw_pillar (ALLEGRO_BITMAP *bitmap, struct pos *p,
             enum em em, enum vm vm)
{
  draw_floor_base (bitmap, p, em, vm);
  draw_pillar_left (bitmap, p, em, vm);
  draw_pillar_right (bitmap, p, em, vm);
}

void
draw_pillar_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *pillar_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: pillar_left = dc_pillar_left; break;
    case EGA: pillar_left = de_pillar_left; break;
    case VGA: pillar_left = dv_pillar_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: pillar_left = pc_pillar_left; break;
    case EGA: pillar_left = pe_pillar_left; break;
    case VGA: pillar_left = pv_pillar_left; break;
    }
    break;
  }

  if (hgc) pillar_left = apply_palette (pillar_left, hgc_palette);

  struct coord c;
  draw_bitmapc (pillar_left, bitmap, pillar_left_coord (p, &c), 0);
}

void
draw_pillar_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *pillar_right = NULL, *pillar_top = NULL;

  pos2coord_f pillar_top_coord = NULL;

  switch (em) {
  case DUNGEON:
    pillar_top_coord = d_pillar_top_coord;
    switch (vm) {
    case CGA:
      pillar_right = dc_pillar_right;
      pillar_top = dc_pillar_top;
      break;
    case EGA:
      pillar_right = de_pillar_right;
      pillar_top = de_pillar_top;
      break;
    case VGA:
      pillar_right = dv_pillar_right;
      pillar_top = dv_pillar_top;
      break;
    }
    break;
  case PALACE:
    pillar_top_coord = p_pillar_top_coord;
    switch (vm) {
    case CGA:
      pillar_right = pc_pillar_right;
      pillar_top = pc_pillar_top;
      break;
    case EGA:
      pillar_right = pe_pillar_right;
      pillar_top = pe_pillar_top;
      break;
    case VGA:
      pillar_right = pv_pillar_right;
      pillar_top = pv_pillar_top;
      break;
    }
    break;
  }

  if (hgc) {
    pillar_right = apply_palette (pillar_right, hgc_palette);
    pillar_top = apply_palette (pillar_top, hgc_palette);
  }

  struct coord c;
  draw_bitmapc (pillar_right, bitmap, pillar_right_coord (p, &c), 0);
  draw_bitmapc (pillar_top, bitmap, pillar_top_coord (p, &c), 0);
}

void
draw_pillar_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *pillar_fg = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: pillar_fg = dc_pillar_fg; break;
    case EGA: pillar_fg = de_pillar_fg; break;
    case VGA: pillar_fg = dv_pillar_fg; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: pillar_fg = pc_pillar_fg; break;
    case EGA: pillar_fg = pe_pillar_fg; break;
    case VGA: pillar_fg = pv_pillar_fg; break;
    }
    break;
  }

  if (hgc) pillar_fg = apply_palette (pillar_fg, hgc_palette);

  struct coord c;
  draw_bitmapc (pillar_fg, bitmap, pillar_coord (p, &c), 0);
}

struct coord *
pillar_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
pillar_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
pillar_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
d_pillar_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 4;
  c->room = p->room;
  return c;
}

struct coord *
p_pillar_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 7;
  c->room = p->room;
  return c;
}
