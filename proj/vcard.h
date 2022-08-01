#include <lcom/lcf.h>

/* Sets Bit 14 : set the linear frame buffer model, which facilitates access to VRAM */
#define LIN_FRAMEBUFF BIT(14) 

#define STD_BIOS_SERV  0x10    /* Access to BIOS services is via the SW interrupt instruction */

#define VBE_CALL 0x4F /* When invoking a VBE function, the AH register must be set to 0x4F, thus distinguishing it from the standard VGA BIOS functions */

#define VBE_CONTROL_INFO 0x00 /* function 0x00 - Return VBE Controller Information */
#define VBE_MODE_INFO 0x01 /* function 0x01 - Return VBE Mode Information */
#define SET_VBE_MODE 0x02 /* function 0x02 - Set VBE Mode -> initializes the controller and sets a video mode (passed on the bx register) */


#define COLOR_MODE_INDEXED 0x105
#define COLOR_MODE_DIR_640_480 0X110
#define COLOR_MODE_DIR_800_600 0x115
#define COLOR_MODE_DIR_1280_1024 0x11A
#define COLOR_MODE_DIR_1152_864 0x14C

int map_VRAM();
/*


*/
int set_vbe_mode(uint16_t mode);
/*


*/
void fill_info(vbe_mode_info_t *vmi_p);
/*


*/
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
/*


*/
/* vg_draw_rectangle calls vg_draw_hline which also calls vg_draw_piwel */

int vg_draw_rectangle_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
/*

*/
int pixmap_draw(xpm_image_t img, uint8_t *map, uint16_t x, uint16_t y);
/*

*/
struct minix_mem_range mr; /*physical memory range*/
unsigned int vram_base;  /*VRAM’s physical addresss*/
unsigned int vram_size;  /*VRAM’s size */
static void *video_mem;		/* Process (virtual) address to which VRAM is mapped */
static void *buffer;

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */

static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */

/*Aditional info */
static unsigned R_Mask_Sz;		/* size of direct color red mask in bits */
static unsigned R_Field_Pos;	/* bit position of lsb of red mask */
static unsigned G_Mask_Sz;	/* size of direct color green mask in bits */
static unsigned G_Field_Pos;	/* bit position of lsb of green mask */
static unsigned B_Mask_Sz; 	/* size of direct color blue mask in bits */
static unsigned B_Field_Pos;	/* bit position of lsb of blue mask */



void double_buffering();


