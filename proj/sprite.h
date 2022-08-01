/**
 *  @author Joao Cardoso (jcard@fe.up.pt) ????
 *  Added by pfs@fe.up.pt
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

/** @defgroup sprite Sprite
 * @{
 *
 * Sprite related functions
 */

/** A Sprite is an "object" that contains all needed information to
 * create, animate, and destroy a pixmap.  The functions assume that
 * the background is BLACK and they take into account collision with
 * other graphical objects or the screen limits. 
 */
typedef struct {
  int x,y;             /**< current sprite position */
  int xspeed, yspeed;  /**< current speeds in the x and y direction */
  uint8_t *map;           /**< the sprite pixmap (use read_xpm()) */
  xpm_image_t img;
} Sprite;

/** Creates with random speeds (not zero) and position
 * (within the screen limits), a new sprite with pixmap "pic", in
 * memory whose address is "base";
 * Returns NULL on invalid pixmap.
 */
Sprite * create_sprite(xpm_map_t xpm , int x, int y,int xspeed, int yspeed);

void destroy_sprite(Sprite*sp);

void move_sprite(Sprite *sp, int xmov, int ymov);
int draw_sprite(Sprite *sp);
void erase_sprite(Sprite *sp);
void erase_hospital(Sprite *sp);


/** @} end of sprite */

#endif
