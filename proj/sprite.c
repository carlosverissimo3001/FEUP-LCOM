#include <lcom/lcf.h>
#include "vcard.h"
#include "sprite.h"

Sprite*create_sprite(xpm_map_t xpm , int x, int y, int xspeed, int yspeed) {
  //allocate space for the "object"
  Sprite*sp = (Sprite*) malloc (sizeof(Sprite));
  
  if(sp == NULL)
    return NULL;
  
  // read the sprite pixmap
  sp->map = xpm_load(xpm, XPM_INDEXED, &(sp->img));
  
  if( sp->map == NULL){
    free(sp);
    return NULL;
  }
  
  sp->x = x;
  sp->y = y;
  sp->xspeed = xspeed;
  sp->yspeed = yspeed;

  return sp;
}



void destroy_sprite(Sprite*sp){
  if(sp == NULL)
    return;
  
  if(sp->map)
    free(sp->map);
  
  free(sp);
  
  sp = NULL;     // XXX: pointer is passed by value
  //                should do this @ the caller
}


int draw_sprite(Sprite *sp){
  /* size verifications */
  pixmap_draw(sp->img, sp->map, sp->x, sp->y);
  return 0;
}

void move_sprite(Sprite *sp, int xmov, int ymov)
{
  sp->x = sp->x + xmov;
  sp->y = sp->y + ymov;
}

void erase_sprite(Sprite *sp)
{
  /* black has color index 0 */
  /*only need to "blackout" the region of the screen which the xpm was drawn, so use sp->width instead of h_res, same for y*/
  //vg_draw_rectangle(sp->x, sp->y, sp->width, sp->height, 0);
}

