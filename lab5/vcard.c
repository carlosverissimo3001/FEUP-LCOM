#include <lcom/lcf.h>
#include "vcard.h"

#include <machine/int86.h> 
#include <math.h>

#include <stdint.h>
#include <stdio.h>

struct minix_mem_range mr; /*physical memory range*/
unsigned int vram_base;  /*VRAM’s physical addresss*/
unsigned int vram_size;  /*VRAM’s size */
static uint8_t *video_mem;		/* Process (virtual) address to which VRAM is mapped */

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



void* (vg_init) (uint16_t mode)
{
  /* Returns: Virtual address VRAM was mapped to. NULL, upon failure. 
  The process needs to know the VRAM´s physical adress before mapping the graphics VRAM in its adress space*/
  vbe_mode_info_t vmi_p;
  
  if (vbe_get_mode_info(mode, &vmi_p) != 0){
    printf("Error retrieving VBE mode information\n");
    return NULL;  
  }
  
  /* obtain relevant info in order to Map the graphics mode VRAM into the process' address space and initialize the video graphics mode */
  fill_info(&vmi_p);
  
  /* Map the graphics mode VRAM */
  if (map_VRAM() != 0){
    printf("Error mapping VRAM to the process' address space\n");
    return NULL;
  }
  
  /* set the mode */
  if(set_vbe_mode(mode) != 0){
    printf("Error setting the interface to the desired graphics mode\n");
    return NULL;
  }
  
  /* Return virtual address VRAM was mapped to */
  return video_mem; 
}	
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color){

    unsigned i;
    uint8_t* ptr = video_mem;
    ptr+= bytes_per_pixel *(y*h_res + x);
    
    for (i = 0; i < bytes_per_pixel ; i++)
    {
      *ptr = (color >> 8*i);
      ptr++;
    }

    return 0;
}

int (vg_draw_hline)( uint16_t x,uint16_t y, 
                      uint16_t len, uint32_t color ){
  for (unsigned i = x; i <(x+len); i++)
  {
    if (i >= h_res)
      break;
    vg_draw_pixel(i, y, color);
  }
  
  return 0;
}


int (vg_draw_rectangle) (uint16_t x, uint16_t y,
                            uint16_t width, uint16_t height, uint32_t color ){
    for (unsigned i = y; i < (height+y) ; i++ )
    {
      if (i >= v_res)
        break;
      vg_draw_hline(x, i , width, color);
    }
  return 0;        
}

int (map_VRAM)(){
  
  int r;
  
  /* Allow memory mapping */
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  
  /*Map memory*/
  video_mem = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);
  if(video_mem == MAP_FAILED)
    panic("couldn’t map video memory");
  return 0;
}

int set_vbe_mode(uint16_t mode){

  reg86_t r86;
  memset(&r86, 0, sizeof(r86));	/* zero the structure */
 
  /* r86.ax = VBE_CALL; // VBE call, function 02 -- set VBE mode */
  r86.ah = VBE_CALL; 
  r86.al = SET_VBE_MODE; 
  
  /*  The mode must be passed in the BX register. Bit 14 of the BX register should be set to ease VRAM acess*/
  r86.bx = (LIN_FRAMEBUFF | mode); 
  
  /* BIOS SERVICES */
  r86.intno = STD_BIOS_SERV; 
  
  /* kernel call */
  if(sys_int86(&r86) != OK ){
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }
  
  /*  If the VBE function completed successfully, value 0x00 is returned in the AH, otherwise it is set to indicate the nature of the failure, as shown in the table */
  if (r86.ah != 0x00)
  {
    printf("VBE function was NOT completed successfully\n");
    return 1;
  }
  return 0;
}

void fill_info(vbe_mode_info_t *vmi_p){
  h_res = vmi_p->XResolution;
  v_res = vmi_p->YResolution;
  
  bits_per_pixel = vmi_p->BitsPerPixel;
  /* In order to get the no of bytes per pixel, we need to divide the bits per pixel by 8 and then use the funcion ceil(x) which returns the smallest integral value not less than x 
  Eg: if we have 15 bits per pixel, we will need 2 bytes per pixel */
  
  bytes_per_pixel = ceil(bits_per_pixel/8.0);
  
  vram_base =  vmi_p->PhysBasePtr;
  vram_size = h_res * v_res * bytes_per_pixel;

  R_Mask_Sz = vmi_p->RedMaskSize;
  R_Field_Pos = vmi_p->RedFieldPosition;
  G_Mask_Sz = vmi_p->GreenMaskSize;
  G_Field_Pos = vmi_p->GreenFieldPosition;
  B_Mask_Sz = vmi_p->BlueMaskSize;
  B_Field_Pos = vmi_p->BlueFieldPosition;
}

int vg_draw_rectangle_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step)
{
  /*  If color mode is indexed
  color = (first + (row * no_rectangles + col) * step) % (1 << bitsPerPixel) 
  */
  
  /* If the color mode is direct
  R(row, col) = (R(first) + col * step) % (1 << RedMaskSize)
	G(row, col) = (G(first) + row * step) % (1 << GreenMaskSize)
	B(row, col) = (B(first) + (col + row) * step) % (1 << BlueMaskSize)
  
  -> HINT: Replace (1 << RedMaskSize with BIT(RedMaskSize))
  */
  
  /* Since we have a pattern (matrix) of no_rectangles per no_rectangles, we should use nested for loops, 1 cycle for width(rows) and 1 cycle for height(colunms) */
  uint16_t width = v_res/no_rectangles;
  uint16_t height = h_res/no_rectangles;
  uint32_t color;
  
  uint8_t col, row;
  uint8_t red, green, blue; 
  uint8_t redFirst, greenFirst, blueFirst;
  
  redFirst = (first >> R_Field_Pos) & R_Mask_Sz; 
  greenFirst = (first >> G_Field_Pos) & G_Mask_Sz; 
  blueFirst = (first >> B_Field_Pos) & B_Mask_Sz;
  
  
  
  for (row = 0; row < no_rectangles; row++){
    for (col = 0; col < no_rectangles; col++){
      if(mode == COLOR_MODE_INDEXED) /* color mode is 0x105 (Indexed) */{
        color = (first + (row * no_rectangles + col) * step) % BIT(bits_per_pixel);
      }
      else{ /* color mode is NOT 0x105 (Indexed) */
        red = (redFirst + col * step) % BIT(R_Mask_Sz);
        green = (greenFirst + row * step) % BIT(G_Mask_Sz);
        blue = (blueFirst + (col+row) * step) % BIT(B_Mask_Sz);
      }
      color = (red << R_Field_Pos  | green << G_Field_Pos | blue << B_Field_Pos);
      vg_draw_rectangle(col*width, row*height, width, height, color);
    }
  }
  
  return 0;
}

int pixmap_draw(xpm_image_t img, uint8_t *map, uint16_t x, uint16_t y)
{
  /* use img.width and img.height to get the conditions for the nested loop */

  for (unsigned i = 0; i < img.height ; i++){ // vertical
    if ((i+y) >= v_res)
      break;
    for (unsigned j = 0; j < img.width; j++) // horziontal
    {
      if ((j+x) >= h_res)
        break;
      
      /* argument color of vg_draw_pixel has to be accessed*/
      vg_draw_pixel((j+x),(i+y), *(map + i*img.width + j)); 
    }
  }

  return 0;
}

