/*
  level.c -- level module;

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

/* functions */
static void draw_level (void);
static void compute_level (void);
static void process_keys (void);
static void draw_lives (ALLEGRO_BITMAP *bitmap, struct anim *k, enum vm vm);

struct level *vanilla_level;
struct level level;
static int last_auto_show_time;
static ALLEGRO_TIMER *death_timer;

bool no_room_drawing, pause_game, step_one_cycle;
int room_view;
int retry_level = -1;
int anti_camera_room;
int camera_follow_kid;
int auto_rem_time_1st_cycle = 24;

void
play_level (struct level *lv)
{
  char *text;
  vanilla_level = lv;
  death_timer = create_timer (1.0 / 12);

 start:
  cutscene = false;
  level = *lv;
  register_cons ();

  stop_all_samples ();
  if (level.start) level.start ();

  anim_cycle = 0;
  last_auto_show_time = -1;
  current_kid_id = 0;

  if (level.nominal_number >= 0) {
    xasprintf (&text, "LEVEL %i", level.nominal_number);
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  al_stop_timer (death_timer);
  al_set_timer_count (death_timer, 0);

  play_anim (draw_level, compute_level, 12);

  switch (quit_anim) {
  case NO_QUIT: break;
  case RESTART_LEVEL:
    retry_level = level.number;
    destroy_anims ();
    destroy_cons ();
    draw_bottom_text (NULL, NULL);
   goto start;
  case NEXT_LEVEL:
    destroy_anims ();
    destroy_cons ();
    if (level.next_level) level.next_level (level.number + 1);
    draw_bottom_text (NULL, NULL);
    if (level.cutscene) {
      cutscene_started = false;
      cutscene = true;
      stop_video_effect ();
      stop_all_samples ();
      play_anim (level.cutscene, NULL, 10);
      stop_video_effect ();
      stop_all_samples ();

      if (quit_anim == RESTART_GAME) goto restart_game;
    }
    goto start;
  case RESTART_GAME:
  restart_game:
    retry_level = -1;
    destroy_anims ();
    destroy_cons ();
    draw_bottom_text (NULL, NULL);
    break;
  case QUIT_GAME: break;
  case OUT_OF_TIME:
    cutscene_started = false;
    cutscene = true;
    stop_video_effect ();
    stop_all_samples ();
    play_anim (cutscene_out_of_time_anim, NULL, 10);
    stop_video_effect ();
    stop_all_samples ();
    goto restart_game;
    break;
  }
}

void
register_cons (void)
{
  struct pos p;
  for (p.room = 0; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++)
        switch (con (&p)->fg) {
        case LOOSE_FLOOR: register_loose_floor (&p); break;
        case OPENER_FLOOR: register_opener_floor (&p); break;
        case CLOSER_FLOOR: register_closer_floor (&p); break;
        case SPIKES_FLOOR: register_spikes_floor (&p); break;
        case DOOR: register_door (&p); break;
        case LEVEL_DOOR: register_level_door (&p); break;
        case CHOPPER: register_chopper (&p); break;
        case MIRROR: register_mirror (&p); break;
        default: break;
        }
}

void
destroy_cons (void)
{
  destroy_array ((void **) &loose_floor, &loose_floor_nmemb);
  destroy_array ((void **) &opener_floor, &opener_floor_nmemb);
  destroy_array ((void **) &closer_floor, &closer_floor_nmemb);
  destroy_array ((void **) &spikes_floor, &spikes_floor_nmemb);
  destroy_array ((void **) &door, &door_nmemb);
  destroy_array ((void **) &level_door, &level_door_nmemb);
  destroy_array ((void **) &chopper, &chopper_nmemb);
}

void
load_level (void)
{
  load_room ();
  load_fire ();
  load_potion ();
  load_sword ();
  load_kid ();
  load_guard ();
  load_mouse ();
}

void
unload_level (void)
{
  unload_room ();
  unload_fire ();
  unload_potion ();
  unload_sword ();
  unload_kid ();
  unload_guard ();
  unload_mouse ();
}

static void
compute_level (void)
{
  size_t i;

  process_keys ();

  if (pause_game) return;

  /* this condition is necessary to honor any floor press the start
     level function might have */
  if (anim_cycle > 0) {
    unpress_opener_floors ();
    unpress_closer_floors ();
    uncross_mirrors ();
  }

  struct anim *current_kid = get_anim_by_id (current_kid_id);

  int prev_room = current_kid->f.c.room;

  for (i = 0; i < anima_nmemb; i++) {
    struct anim *a = &anima[i];
    a->splash = false;
    a->xf.b = NULL;
  }

  compute_loose_floors ();

  get_gamepad_state (&current_kid->key);

  for (i = 0; i < anima_nmemb; i++) enter_fight_logic (&anima[i]);
  for (i = 0; i < anima_nmemb; i++) leave_fight_logic (&anima[i]);
  for (i = 0; i < anima_nmemb; i++) fight_ai (&anima[i]);
  for (i = 0; i < anima_nmemb; i++) anima[i].action (&anima[i]);
  for (i = 0; i < anima_nmemb; i++) fight_mechanics (&anima[i]);

  fight_turn_controllable (current_kid);

  clear_anims_keyboard_state ();

  if (current_kid->f.c.room != prev_room
      && current_kid->f.c.room != 0
      && current_kid->f.c.room != anti_camera_room)  {
    if (camera_follow_kid == current_kid->id)
      room_view = current_kid->f.c.room;
    make_links_locally_consistent (prev_room, room_view);
  }

  if (level.special_events) level.special_events ();

  compute_closer_floors ();
  compute_opener_floors ();
  compute_spikes_floors ();
  compute_doors ();
  compute_level_doors ();
  compute_choppers ();
}

static void
process_keys (void)
{
  char *text = NULL;

  struct anim *current_kid = get_anim_by_id (current_kid_id);

  int prev_room = room_view;

  /* clear the keyboard buffer at the first cycle, so any key pressed
     on the title don't trigger any action */
  if (anim_cycle == 0) {
    memset (&key, 0, sizeof (key));
    button = -1;
  }

  /* ESC: pause game */
  if (step_one_cycle) {
    step_one_cycle = false;
    pause_game = true;
    al_stop_timer (play_time);
  }

  if (was_key_pressed (ALLEGRO_KEY_ESCAPE, 0, 0, true)
      || was_button_pressed (joystick_pause_button, true)) {
    if (pause_game) {
      step_one_cycle = true;
      pause_game = false;
      al_start_timer (play_time);
    } else {
      pause_game = true;
      al_stop_timer (play_time);
    }
  } else if (pause_game && (key.keyboard.keycode || button != -1)) {
    pause_game = false;
    draw_bottom_text (NULL, NULL);
    memset (&key, 0, sizeof (key));
    button = -1;
    al_start_timer (play_time);
  }

  if (pause_game) anim_cycle--;

  /* R: resurrect kid */
  if (was_key_pressed (ALLEGRO_KEY_R, 0, 0, true)
      && is_kid_dead (&current_kid->f))
    kid_resurrect (current_kid);

  /* HOME: camera on kid */
  if (was_key_pressed (ALLEGRO_KEY_HOME, 0, 0, true))
    room_view = current_kid->f.c.room;

  /* A: alternate between kid and its shadows */
  if (was_key_pressed (ALLEGRO_KEY_A, 0, 0, true)) {
    do {
      current_kid = &anima[(current_kid - anima + 1) % anima_nmemb];
    } while (current_kid->type != KID || ! current_kid->controllable);
    current_kid_id = current_kid->id;
    room_view = current_kid->f.c.room;
  }

  /* K: kill enemy */
  if (was_key_pressed (ALLEGRO_KEY_K, 0, 0, true)) {
    struct anim *ke = get_anim_by_id (current_kid->enemy_id);
    if (ke) {
      ke->current_lives = 0;
      play_sample (guard_hit_sample, ke->f.c.room);
    }
  }

  /* I: enable/disable immortal mode */
  if (was_key_pressed (ALLEGRO_KEY_I, 0, 0, true)) {
    immortal_mode = ! immortal_mode;
    current_kid->immortal = immortal_mode;
    xasprintf (&text, "%s MODE", immortal_mode
               ? "IMMORTAL" : "MORTAL");
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  /* SHIFT+S: incremet kid current lives */
  if (was_key_pressed (ALLEGRO_KEY_S, 0, ALLEGRO_KEYMOD_SHIFT, true))
    increase_kid_current_lives (current_kid);

  /* SHIFT+T: incremet kid total lives */
  if (was_key_pressed (ALLEGRO_KEY_T, 0, ALLEGRO_KEYMOD_SHIFT, true))
    increase_kid_total_lives (current_kid);

  /* SHIFT+W: float kid */
  if (was_key_pressed (ALLEGRO_KEY_W, 0, ALLEGRO_KEYMOD_SHIFT, true))
    float_kid (current_kid);

  /* CTRL+A: restart level */
  if (was_key_pressed (ALLEGRO_KEY_A, 0, ALLEGRO_KEYMOD_CTRL, true))
    quit_anim = RESTART_LEVEL;

  /* SHIFT+L: warp to next level */
  if (was_key_pressed (ALLEGRO_KEY_L, 0, ALLEGRO_KEYMOD_SHIFT, true))
    quit_anim = NEXT_LEVEL;

  /* C: show direct coordinates */
  if (was_key_pressed (ALLEGRO_KEY_C, 0, 0, true)) {
    int s = room_view;
    int l = roomd (room_view, LEFT);
    int r = roomd (room_view, RIGHT);
    int a = roomd (room_view, ABOVE);
    int b = roomd (room_view, BELOW);

    xasprintf (&text, "S%i L%i R%i A%i B%i", s, l, r, a, b);
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  /* SHIFT+C: show indirect coordinates */
  if (was_key_pressed (ALLEGRO_KEY_C, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
    int a = roomd (room_view, ABOVE);
    int b = roomd (room_view, BELOW);
    int al = roomd (a, LEFT);
    int ar = roomd (a, RIGHT);
    int bl = roomd (b, LEFT);
    int br = roomd (b, RIGHT);

    xasprintf (&text, "LV%i AL%i AR%i BL%i BR%i",
               level.number, al, ar, bl, br);
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  /* SPACE: display remaining time */
  if (was_key_pressed (ALLEGRO_KEY_SPACE, 0, 0, true)
      || was_button_pressed (joystick_time_button, true))
    display_remaining_time ();

  /* +: increment and display remaining time */
  if (was_key_pressed (ALLEGRO_KEY_EQUALS, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
    int t = time_limit - al_get_timer_count (play_time);
    int d = t > 60 ? -60 : -1;
    al_add_timer_count (play_time, d);
    display_remaining_time ();
  }

  /* -: decrement and display remaining time */
  if (was_key_pressed (ALLEGRO_KEY_MINUS, 0, 0, true)) {
    int t = time_limit - al_get_timer_count (play_time);
    int d = t > 60 ? +60 : +1;
    al_add_timer_count (play_time, d);
    display_remaining_time ();
  }

  /* TAB: display skill */
  if (was_key_pressed (ALLEGRO_KEY_TAB, 0, 0, true))
    display_skill (current_kid);

  /* CTRL+=: increment counter attack skill */
  if (was_key_pressed (ALLEGRO_KEY_EQUALS, 0, ALLEGRO_KEYMOD_CTRL, true)) {
    if (current_kid->skill.counter_attack_prob < 99)
      current_kid->skill.counter_attack_prob++;
    display_skill (current_kid);
  }

  /* CTRL+-: decrement counter attack skill */
  if (was_key_pressed (ALLEGRO_KEY_MINUS, 0, ALLEGRO_KEYMOD_CTRL, true)) {
    if (current_kid->skill.counter_attack_prob > -1)
      current_kid->skill.counter_attack_prob--;
    display_skill (current_kid);
  }

  /* ALT+=: increment counter defense skill */
  if (was_key_pressed (ALLEGRO_KEY_EQUALS, 0, ALLEGRO_KEYMOD_ALT, true)) {
    if (current_kid->skill.counter_defense_prob < 99)
      current_kid->skill.counter_defense_prob++;
    display_skill (current_kid);
  }

  /* ALT+-: decrement counter defense skill */
  if (was_key_pressed (ALLEGRO_KEY_MINUS, 0, ALLEGRO_KEYMOD_ALT, true)) {
    if (current_kid->skill.counter_defense_prob > -1)
      current_kid->skill.counter_defense_prob--;
    display_skill (current_kid);
  }

  /* F10: change guard mode */
  if (was_key_pressed (ALLEGRO_KEY_F10, 0, 0, true)) {
    char *gm_str = NULL;

    /* get next guard mode */
    switch (gm) {
    case ORIGINAL_GM: gm = GUARD_GM, gm_str = "GUARD"; break;
    case GUARD_GM: gm = FAT_GUARD_GM, gm_str = "FAT GUARD"; break;
    case FAT_GUARD_GM: gm = VIZIER_GM, gm_str = "VIZIER"; break;
    case VIZIER_GM: gm = SKELETON_GM, gm_str = "SKELETON"; break;
    case SKELETON_GM: gm = SHADOW_GM, gm_str = "SHADOW"; break;
    case SHADOW_GM: gm = ORIGINAL_GM, gm_str = "ORIGINAL"; break;
    }

    /* apply next guard mode */
    int i;
    for (i = 0; i < anima_nmemb; i++) apply_guard_mode (&anima[i], gm);

    xasprintf (&text, "GUARD MODE: %s", gm_str);
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  /* CTRL+G: save game */
  if (was_key_pressed (ALLEGRO_KEY_G, 0, ALLEGRO_KEYMOD_CTRL, true)
      && ! save_game_dialog_thread) {
    save_game_dialog_thread =
      create_thread (save_game_dialog, NULL);
    al_start_thread (save_game_dialog_thread);
  }

  /* Restart level after death */
  struct anim *k = get_anim_by_id (0);
  if (is_kid_dead (&k->f)
      && ! pause_game) {
    al_start_timer (death_timer);
    int64_t t = al_get_timer_count (death_timer);

    if (t < 12) k->sample = NULL;

    if (t >= 12 && ! k->sample) {
      ALLEGRO_SAMPLE *sample;
      switch (k->death_reason) {
      case SHADOW_FIGHT_DEATH: sample = success_suspense_sample; break;
      case FIGHT_DEATH: sample = fight_death_sample; break;
      default: sample = death_sample; break;
      }
      k->sample = play_sample (sample, -1);
    }

    if (t < 60) {
      key.keyboard.keycode = 0;
      button = -1;
    }

    if (t >= 60) {
      if (t < 240 || t % 12 < 8) {
        if (t >= 252 && t % 12 == 0)
          play_sample (press_key_sample, -1);
        xasprintf (&text, "Press Button to Continue");
        draw_bottom_text (NULL, text);
        al_free (text);
      } else draw_bottom_text (NULL, "");

      if (key.keyboard.keycode || button != -1)
        quit_anim = RESTART_LEVEL;
    }
  } else if (al_get_timer_started (death_timer)
             && ! pause_game) {
    al_stop_timer (death_timer);
    al_set_timer_count (death_timer, 0);
    draw_bottom_text (NULL, NULL);
  }

  if (room_view == 0) room_view = prev_room;
  camera_follow_kid = (current_kid->f.c.room == room_view)
    ? current_kid->id : -1;
}

static void
draw_level (void)
{
  /* drawing */
  clear_bitmap (screen, BLACK);

  struct pos p;
  p.room = room_view;

  run_room_callbacks (p.room);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_fire (screen, &p, vm);
      draw_balcony_stars (screen, &p, vm);
    }

  if (! no_room_drawing) draw_room (screen, room_view, em, vm);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      update_mirror_bitmap (screen, &p);
      if (! no_room_drawing) draw_mirror (screen, &p, em, vm);
    }

  /* loose_floor_fall_debug (); */

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_falling_loose_floor (screen, &p, em, vm);
    }

  draw_anims (screen, em, vm);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_potion (screen, &p, em, vm);
      draw_sword (screen, &p, vm);
    }

  draw_lives (uscreen, get_anim_by_id (current_kid_id), vm);

  /* automatic remaining time display */
  int rem_time = time_limit - al_get_timer_count (play_time);
  if ((rem_time % (5 * 60) == 0
       && last_auto_show_time != rem_time
       && anim_cycle > 720)
      || (auto_rem_time_1st_cycle >= 0
          && last_auto_show_time != rem_time
          && anim_cycle >= auto_rem_time_1st_cycle
          && anim_cycle <= auto_rem_time_1st_cycle + 6)
      || rem_time <= 60) {
    display_remaining_time ();
    if (rem_time <= 60 && rem_time != last_auto_show_time)
      play_sample (press_key_sample, -1);
    last_auto_show_time = rem_time;
  }
  if (rem_time <= 0) quit_anim = OUT_OF_TIME;

  if (pause_game) draw_bottom_text (NULL, "GAME PAUSED");
}

void
display_remaining_time (void)
{
  char *text;
  int t = time_limit - al_get_timer_count (play_time);
  if (t < 0) t = 0;
  int m = t / 60 + ((t % 60) ? 1 : 0);
  if (t > 60) xasprintf (&text, "%i MINUTES LEFT", m);
  else xasprintf (&text, "%i SECONDS LEFT", t);
  draw_bottom_text (NULL, text);
  al_free (text);
}

void
display_skill (struct anim *k)
{
  char *text;
  struct anim *ke = get_anim_by_id (k->enemy_id);
  if (ke) xasprintf (&text, "KCA%i KCD%i A%i CA%i D%i CD%i",
                      k->skill.counter_attack_prob + 1,
                      k->skill.counter_defense_prob + 1,
                      ke->skill.attack_prob + 1,
                      ke->skill.counter_attack_prob + 1,
                      ke->skill.defense_prob + 1,
                      ke->skill.counter_defense_prob + 1);
  else xasprintf (&text, "KCA%i KCD%i",
                  k->skill.counter_attack_prob + 1,
                  k->skill.counter_defense_prob + 1);
  draw_bottom_text (NULL, text);
  al_free (text);
}

static void
draw_lives (ALLEGRO_BITMAP *bitmap, struct anim *k, enum vm vm)
{
  if (k->f.c.room == room_view) {
    draw_kid_lives (bitmap, k, vm);
    if (k->enemy_id != -1) {
      struct anim *ke = get_anim_by_id (k->enemy_id);
      if (ke) draw_guard_lives (bitmap, ke, vm);
    }
  }
}
