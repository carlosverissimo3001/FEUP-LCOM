#include <lcom/lcf.h>

#include "i8042.h"
#include "covid_escape.h"
#include "i8254.h"
#include "kbc_int.h"
#include "kbc_io.h"
#include "kbc_sc.h"
#include "kbc_verification.h"
#include "sprite.h"
#include "timer_int.h"
#include "vcard.h"
#include "xpms.h"

/* external variables */
extern int n_interrupts; /* declared @ timer_int_handler() (timer_int.c) */
extern uint32_t irq_timer_set, irq_kbc_set, irq_mouse_set; /*declared @ main()(proj.c)*/

/* sprites containing mostly everything that´s gonna be used in the game */

/* particle which is gonna be manipulated using the keys, by the user */
/* has 70 witdh per 70 height */
static Sprite *covidSP;

/* all of the above have the screen´s resolution, 1024:768 */
static Sprite *how_to_playSP;
static Sprite *backgroundSP;
static Sprite *background_leftSP;
static Sprite *main_menuSP;

static Sprite *hospitals[6];

static Sprite *game_overSP;
static Sprite *your_scoreSP;
static Sprite *best_scoreSP;
static Sprite *wait5SP;

static Sprite *num_0;
static Sprite *num_1;
static Sprite *num_2;
static Sprite *num_3;
static Sprite *num_4;
static Sprite *num_5;
static Sprite *num_6;
static Sprite *num_7;
static Sprite *num_8;
static Sprite *num_9; 


/* at the start of everything, the state of the menu is: IN_MENU and the state of the game is also MENU */
static game curr_case = MENU;
static menu menu1 = IN_MENU;

static timer_actions timer_act;
static kbc_actions kbc_act;
//static mouse_actions mouse_act;
int score[2] = {0};
int hActive[6] = {0};
int bestScore[2] = {0};

int menuu=0, backg = 0, rules=0;

#define GRAVITY 80 /* yspeed of the particle is 80 */


void create_xpms(){
  main_menuSP = create_sprite(main_menu_xpm, 0, 0, 0, 0);
  covidSP = create_sprite(covid_xpm, 200, 768/2 - 35 , 0, GRAVITY);
  how_to_playSP = create_sprite(how_to_play_xpm, 0, 0, 0, 0);
  backgroundSP = create_sprite(background_xpm, 0, 0, 0, 0);
  background_leftSP = create_sprite(background_left_xpm, 0, 0, 0, 0);
  game_overSP = create_sprite(game_over_xpm, 0, 0, 0, 0);
  your_scoreSP = create_sprite(your_score_xpm, 0, 175, 0, 0);
  best_scoreSP = create_sprite(best_score_xpm, 0, 310, 0, 0);
  wait5SP = create_sprite(wait5_xpm, 0, 650, 0, 0);
  hospitals[0] = create_sprite(h0_xpm, 840, 0, -150, 0);
  hospitals[1] = create_sprite(h1_xpm, 840, 0, -150, 0);
  hospitals[2] = create_sprite(h2_xpm, 840, 0, -150, 0);
  hospitals[3] = create_sprite(h3_xpm, 840, 0, -150, 0);
  hospitals[4] = create_sprite(h4_xpm, 840, 0, -150, 0);
  hospitals[5] = create_sprite(h5_xpm, 840, 0, -150, 0);
}


void play_game(){
  int r, ipc_status;
  message msg;
  actions.timer = 0, actions.kbc = 0;
  
  /* main loop which will contain interruptions from the devices */
  uint8_t fps = 60;
  
  while (menu1 != EXIT_GAME) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          /* TIMER INTERRUPTIONS */
          if (msg.m_notify.interrupts & irq_timer_set) { 
            timer_int_handler(); /* Handles it */
            if ((n_interrupts % fps) == 0 ){
              /* process timer*/
              timer_act = F5;
              actions.timer = 1;
            }
            if ((n_interrupts % 133  == 0) && (curr_case == IN_GAME)){ /* Draw hospital */
              timer_act = HOSPITAL_NEW;
              actions.timer = 1;
            }
            if ((n_interrupts % 10  == 0) && (curr_case == IN_GAME)){ 
              check_collision();
              //game_over();
            }
          }
          /* KEYBOARD INTERRUPTIONS */
          if (msg.m_notify.interrupts & irq_kbc_set) { 
            kbc_ih(); /* Handles it */
            if (kbc_done) {
              //kbd_print_scancode(is_break_code(), sc_size, sc_code);
              sc_size = 0;
            }
            switch (sc_code[0])
            {
            /* GAME RELATED KEYS */
            case A_MAKE:  /* Small left adjustment */
              actions.kbc = 1;
              kbc_act = A;
              break;
            case A_BREAK:
              actions.kbc = 1;
              kbc_act = A_RELEASE;
              break;
            
            case D_MAKE: /* Small right adjustment */
              actions.kbc = 1;
              kbc_act = D;
              break;
            case D_BREAK:
              kbc_act = D_RELEASE;
              actions.kbc = 1;
              break;
            
            case SPACEBAR_MAKE: /* jump */
              kbc_act = SPACEBAR;
              actions.kbc = 1;
              break;
            case SPACEBAR_BREAK:
              kbc_act = SPACEBAR_RELEASE;
              actions.kbc = 1;
              break;
            
            case W_MAKE: /*exit game  */
              kbc_act = W;
              actions.kbc = 1;
              break;
            case W_BREAK: /*exit game  */
              kbc_act = W_RELEASE;
              actions.kbc = 1;
              break;
            
            /* MENU RELATED KEYS */
            case NUMB1_BREAK: /* Selects option play game */
              kbc_act = NUMB1_RELEASE;
              actions.kbc = 1;
              break;
            
            case NUMB2_BREAK: /* Selects option rules  */
              kbc_act = NUMB2_RELEASE;
              actions.kbc = 1;
              break;
            
            case NUMB3_BREAK: /* Selects option exit game  */
              kbc_act = NUMB3_RELEASE;
              actions.kbc = 1;
              break;
           
            case ESC_BREAK: /*exit game  */
              kbc_act = ESC;
              actions.kbc = 1;
              break;
            default:
              break;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
      /* if any of these are true, i.e, if there has been any interruptions made by either the kbc, mouse or timer, we will handle the actions below, appropriately */
      if (actions.timer)  timer_process_actions();  
      if (actions.kbc)  kbc_process_actions();
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }
  game_end();
} /* end of main loop */


void timer_process_actions(){
  switch (curr_case)
  {
    case MENU:
      if (menu1 == IN_HOW_TO_PLAY){
        if (backg == 0){
          backg = 1;
          draw_sprite(backgroundSP);
        }
        if (rules == 0 && menu1 == IN_HOW_TO_PLAY){
          draw_sprite(how_to_playSP);
          rules = 1;
        }
      }
      else if (menu1 == IN_MENU){
        if (backg == 0){
          draw_sprite(backgroundSP);
          backg = 1;
        }
        if (menuu == 0){
          draw_sprite(main_menuSP);
          menuu = 1;
        } 
      }
      else if (menu1 == PLAY_GAME){
        draw_sprite(backgroundSP);
        draw_sprite(covidSP);
        curr_case = IN_GAME; 
      }
      break;
    case IN_GAME:
      if (timer_act == F5){
        draw_sprite(backgroundSP);
        draw_sprite_covid(covidSP, backgroundSP, 0, GRAVITY);
        hospital_on_left_border();
        if (hActive[0]) draw_sprite_hospital(hospitals[0], backgroundSP);
        if (hActive[1]) draw_sprite_hospital(hospitals[1], backgroundSP);
        if (hActive[2]) draw_sprite_hospital(hospitals[2], backgroundSP);
        if (hActive[3]) draw_sprite_hospital(hospitals[3], backgroundSP);
        if (hActive[4]) draw_sprite_hospital(hospitals[4], backgroundSP);
        if (hActive[5]) draw_sprite_hospital(hospitals[5], backgroundSP);
        
        if (covidSP->y >= (768 - 70) || covidSP->y < 10){ /* particle has touched the floor */
          game_over();     
          break;
        }
      }
      
      if (timer_act == HOSPITAL_NEW){ /* RESPONSIBLE FOR DRAWING HOSTPITALS ON THE SCREEN */
        int sel;
        if (score[1] % 2 == 0)
          sel = 0;
        else
          sel = 3;
        new_hospital(sel);
        if (score[1] != 9)
          score[1]++;
        else
        {
          score[0]++;
          score[1] = 0;
        }
      }
      break;
    default:
      break;
  }
  actions.timer = 0;
}
void kbc_process_actions(){
  if (curr_case == IN_GAME)
  {
    switch (kbc_act)
    {
      case A:
        draw_sprite_covid(covidSP, backgroundSP, -5, 0);
        break;

      case A_RELEASE:
        draw_sprite_covid(covidSP, backgroundSP, 0, 0);
        break;

      case D:
        draw_sprite_covid(covidSP, backgroundSP, 5, 0);
        break;

      case D_RELEASE:
        draw_sprite_covid(covidSP, backgroundSP, 0, 0);
        break;

      case SPACEBAR:
        draw_sprite_covid(covidSP, backgroundSP, 0, -50);
        break;

      case SPACEBAR_RELEASE:
        draw_sprite_covid(covidSP, backgroundSP, 0, 0);
        break;
      case ESC:
        game_over();
        break;
      case W:
        draw_sprite_covid(covidSP, backgroundSP, 0, -50);
        break;
      case W_RELEASE:
        draw_sprite_covid(covidSP, backgroundSP, 0, 0);
        break;
      default:
        break;
    }
  }
  else if (curr_case == MENU){
    if (menu1 == IN_MENU){
      if (kbc_act == NUMB1_RELEASE){
        menu1 = PLAY_GAME;
        backg = 0;
        menuu = 0;
      }
      if (kbc_act == NUMB2_RELEASE){
        backg = 0;
        menuu = 0;
        menu1 = IN_HOW_TO_PLAY;
      }
      if (kbc_act == NUMB3_RELEASE){
        menu1 = EXIT_GAME;
      }
      if (kbc_act == ESC){
        menu1 = EXIT_GAME;
      }
    }
    
    else if (menu1 == IN_HOW_TO_PLAY){
      if (kbc_act == ESC){
        menu1 = IN_MENU;
        rules = 0;
        draw_sprite(backgroundSP);
        backg = 1;
      }
    }    
  }
  
  actions.kbc = 0;
}

void check_collision(){
  /* we have to check collision with every single hospital -> keep in mind that we need to check colision with the top one and the bottom one aswell*/
  if ((covidSP->x + 65 > hospitals[0]->x && covidSP->x + 65 < hospitals[0]->x + 165) && hActive[0]){
    if ((covidSP->y) < 309  || (covidSP->y+62) > 531){ /* CHECKS IF THE MIDDLE OF THE PARTICLE IS ABOVE THE TOP HOSPITAL OR BELOW THE BOTTOM ONE */
      game_over();
    }
  }
  
  if ((covidSP->x + 65 > hospitals[1]->x && covidSP->x + 65 < hospitals[1]->x + 165) && hActive[1]){
    if ((covidSP->y) < 296 || (covidSP->y+62) > 515)
      game_over();
  }
  
  if ((covidSP->x + 65 > hospitals[2]->x && covidSP->x + 65 < hospitals[2]->x + 165) && hActive[2]){
    if ((covidSP->y) < 281 || (covidSP->y+62) > 502)
      game_over();
  }

  if ((covidSP->x + 65 > hospitals[3]->x && covidSP->x + 65 < hospitals[3]->x + 165) && hActive[3]){
    if ((covidSP->y) < 238 || (covidSP->y+62) > 459)
      game_over();
  }

  if ((covidSP->x + 65 > hospitals[4]->x && covidSP->x + 65 < hospitals[4]->x + 165) && hActive[4]){
    if ((covidSP->y) < 252 || (covidSP->y+62) > 473)
      game_over();
  }

  if ((covidSP->x + 65 > hospitals[5]->x && covidSP->x + 65 < hospitals[5]->x + 165) && hActive[5]){
    if ((covidSP->y) < 267 || (covidSP->y+62) > 487 )
      game_over();
  }
}

void hospital_on_left_border(){
  for (int u  = 0; u < 6; u++){
    if (hActive[u]){
      if (hospitals[u]->x < 20){
        hospitals[u]->x = 840;
        hospitals[u]->y = 0;
        hActive[u] = 0;
      }
    }
  }
}


void game_end(){
  for (int u  = 0; u < 6; u++)
    destroy_sprite(hospitals[u]);
  destroy_sprite(covidSP);
  destroy_sprite(how_to_playSP);
  destroy_sprite(main_menuSP);
  destroy_sprite(backgroundSP);
  destroy_sprite(background_leftSP);
  destroy_sprite(game_overSP);
  destroy_sprite(your_scoreSP);
  destroy_sprite(best_scoreSP);
  destroy_sprite(wait5SP);
  destroy_sprite(num_0);
  destroy_sprite(num_1);
  destroy_sprite(num_2);
  destroy_sprite(num_3);
  destroy_sprite(num_4);
  destroy_sprite(num_5);
  destroy_sprite(num_6);
  destroy_sprite(num_7);
  destroy_sprite(num_8);
  destroy_sprite(num_9);
}

void draw_sprite_covid(Sprite *sp, Sprite *background, int xmov, int ymov){
   for (int i = sp->y ; i <= sp->y + 70; i++){
    for (int j = sp->x ; j <= sp->x + 70; j++){
      vg_draw_pixel(j, i, *(background->map + i*background->img.width + j));
    }
  } 
  move_sprite(sp, xmov, ymov);
  draw_sprite(sp);
}

void draw_sprite_hospital(Sprite *sp, Sprite *background){
  move_sprite(sp, sp->xspeed, sp->yspeed);
  draw_sprite(sp);
}

void new_hospital(int sel){
  // sel can be 0 or 3
  for (int i = sel; i < sel+3; i++){
      if (hActive[i] == 0){ /* SEEKS THE FIRST NON-DRAWN HOSPITAL */
        hActive[i] = 1;
        break;
    } 
  }
}

void game_over(){
  for (int p = 0; p < 6; p++){
    hospitals[p]->x = 840;
    hospitals[p]->y = 0;
    hActive[p] = 0;
  }
  covidSP->x = 200;
  covidSP->y = 768/2;
  covidSP->xspeed = 0;
  covidSP->yspeed = GRAVITY;
  draw_sprite(backgroundSP);
  draw_sprite(game_overSP);
  draw_sprite(your_scoreSP);
  draw_sprite(best_scoreSP);
  draw_sprite(wait5SP);
  
  num_0 = create_sprite(n0_xpm, 0, 0, 0, 0);
  num_1 = create_sprite(n1_xpm, 0, 0, 0, 0);
  num_2 = create_sprite(n2_xpm, 0, 0, 0, 0);
  num_3 = create_sprite(n3_xpm, 0, 0, 0, 0);
  num_4 = create_sprite(n4_xpm, 0, 0, 0, 0);
  num_5 = create_sprite(n5_xpm, 0, 0, 0, 0);
  num_6 = create_sprite(n6_xpm, 0, 0, 0, 0);
  num_7 = create_sprite(n7_xpm, 0, 0, 0, 0);
  num_8 = create_sprite(n8_xpm, 0, 0, 0, 0);
  num_9 = create_sprite(n9_xpm, 0, 0, 0, 0);
  
  if (score[0] > bestScore[0]){
    bestScore[0] = score[0];
    bestScore[1] = score[1];
  }
  else if (score[0] == bestScore[0]){
    if (score[1] > bestScore[1]){
      //bestScore[0] = score[0];
      bestScore[1] = score[1];
    }
  }
  
  if (score[0] == 0){
    num_0->x = 475;
    num_0->y = 250;
    draw_sprite(num_0); 
  }
  else if (score[0] == 1){
    num_1->x = 475;
    num_1->y = 250; 
    draw_sprite(num_1);   
  }
  else if (score[0] == 2){
    num_2->x = 475;
    num_2->y = 250; 
    draw_sprite(num_2);  
  }
  else if (score[0] == 3){
    num_3->x = 475;
    num_3->y = 250;
    draw_sprite(num_3);   
  }
  else if (score[0] == 4){
    num_4->x = 475;
    num_4->y = 250;
    draw_sprite(num_4);   
  }
  else if (score[0] == 5){
    num_5->x = 475;
    num_5->y = 250;
    draw_sprite(num_5);   
  }
  else if (score[0] == 6){
    num_6->x =  475;
    num_6->y =  250;
    draw_sprite(num_6);  
  }
  else if (score[0] == 7){
    num_7->x =  475;
    num_7->y =  250;
    draw_sprite(num_7);  
  }
  else if (score[0] == 8){
    num_8->x =  475;
    num_8->y =  250;
    draw_sprite(num_8);  
  }
  else if (score[0] == 9){
    num_9->x = 475;
    num_9->y = 250;
    draw_sprite(num_9);   
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if (score[1] == 0){
    num_0->x = 515;
    num_0->y = 250;
    draw_sprite(num_0); 
  }
  else if (score[1] == 1){
    num_1->x = 515;
    num_1->y = 250;
    draw_sprite(num_1);   
  }
  else if (score[1] == 2){
    num_2->x = 515;
    num_2->y = 250;
    draw_sprite(num_2);  
  }
  else if (score[1] == 3){
    num_3->x = 515;
    num_3->y = 250;
    draw_sprite(num_3);   
  }
  else if (score[1] == 4){
    num_4->x = 515;
    num_4->y = 250;
    draw_sprite(num_4);   
  }
  else if (score[1] == 5){
    num_5->x = 515;
    num_5->y = 250;
    draw_sprite(num_5);   
  }
  else if (score[1] == 6){
    num_6->x = 515;
    num_6->y = 250;
    draw_sprite(num_6);  
  }
  else if (score[1] == 7){
    num_7->x = 515;
    num_7->y = 250;
    draw_sprite(num_7);  
  }
  else if (score[1] == 8){
    num_8->x = 515;
    num_8->y = 250;
    draw_sprite(num_8);  
  }
  else if (score[1] == 9){
    num_9->x = 515;
    num_9->y = 250;
    draw_sprite(num_9);   
  }

  /* - - - - Best Score- - - - - - - - - - - - - */
  
  if (bestScore[0] == 0){
    num_0->x = 475;
    num_0->y = 385;
    draw_sprite(num_0); 
  }
  else if (bestScore[0] == 1){
    num_1->x = 475;
    num_1->y = 385;
    draw_sprite(num_1);   
  }
  else if (bestScore[0] == 2){
    num_2->x = 475;
    num_2->y = 385;
    draw_sprite(num_2);  
  }
  else if (bestScore[0] == 3){
    num_3->x = 475;
    num_3->y = 385;
    draw_sprite(num_3);   
  }
  else if (bestScore[0] == 4){
    num_4->x = 475;
    num_4->y = 385;
    draw_sprite(num_4);   
  }
  else if (bestScore[0] == 5){
    num_5->x = 475;
    num_5->y = 385;
    draw_sprite(num_5);   
  }
  else if (bestScore[0] == 6){
    num_6->x = 475;
    num_6->y = 385;
    draw_sprite(num_6);  
  }
  else if (bestScore[0] == 7){
    num_7->x = 475;
    num_7->y = 385; 
    draw_sprite(num_7);  
  }
  else if (bestScore[0] == 8){
    num_8->x = 475;
    num_8->y = 385;
    draw_sprite(num_8);  
  }
  else if (bestScore[0] == 9){
    num_9->x = 475;
    num_9->y = 385;
    draw_sprite(num_9);   
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if (bestScore[1] == 0){
    num_0->x = 515;
    num_0->y = 385;
    draw_sprite(num_0); 
  }
  else if (bestScore[1] == 1){
    num_1->x = 515;
    num_1->y = 385;
    draw_sprite(num_1);   
  }
  else if (bestScore[1] == 2){
    num_2->x = 515;
    num_2->y = 385;
    draw_sprite(num_2);  
  }
  else if (bestScore[1] == 3){
    num_3->x = 515;
    num_3->y = 385;
    draw_sprite(num_3);   
  }
  else if (bestScore[1] == 4){
    num_4->x = 515;
    num_4->y = 385;
    draw_sprite(num_4);   
  }
  else if (bestScore[1] == 5){
    num_5->x = 515;
    num_5->y = 385;
    draw_sprite(num_5);   
  }
  else if (bestScore[1] == 6){
    num_6->x = 515;
    num_6->y = 385;
    draw_sprite(num_6);  
  }
  else if (bestScore[1] == 7){
    num_7->x = 515;
    num_7->y = 385;
    draw_sprite(num_7);  
  }
  else if (bestScore[1] == 8){
    num_8->x = 515;
    num_8->y = 385;
    draw_sprite(num_8);  
  }
  else if (bestScore[1] == 9){
    num_9->x = 515;
    num_9->y = 385; 
    draw_sprite(num_9);   
  }
  score[0] = 0;
  score[1] = 0;
  sleep(5);
  curr_case = MENU;      
  menu1 = IN_MENU;
}
