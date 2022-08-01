#ifndef _COVID_HANDLER_H_
#define _COVID_HANDLER_H_

#include "sprite.h"

typedef enum{
  IN_HOW_TO_PLAY,
  PLAY_GAME,
  IN_MENU,
  EXIT_GAME
} menu;

typedef enum{
  MENU,
  IN_GAME
} game;


struct {
  int timer;
  int kbc;
  //int mouse;
} actions;

typedef enum{
  A,
  A_RELEASE,
  D,
  D_RELEASE,
  SPACEBAR,
  SPACEBAR_RELEASE,
  NUMB1_RELEASE,
  NUMB2_RELEASE,
  NUMB3_RELEASE,
  ESC,
  W,
  W_RELEASE
} kbc_actions;

/*typedef enum{
  M1_RELEASE,
  M2_RELEASE
} mouse_actions;
*/

typedef enum{
  HOSPITAL_NEW,
  F5,
} timer_actions;

void create_xpms();

void play_game();

void timer_process_actions();

void kbc_process_actions();


void check_collision();

void game_end();

void hospital_on_left_border();

void draw_sprite_covid(Sprite *sp, Sprite *background, int xmov, int ymov);
void draw_sprite_hospital(Sprite *sp, Sprite *background);
void new_hospital(int sel);
void game_over();


#endif /* _COVID_HANDLER_H_ */
