#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBC_IRQ 1
#define MOUSE_IRQ	12

#define DELAY_US 		    20000

#define KBC_IN_BUF 0x60
#define KBC_OUT_BUF 0x60 

#define KBC_CMD_REG 0x64
#define KBC_STATUS_REG 0x64

#define KBC_ENABLE_INTERRUPT BIT(0)
#define KBC_STATUS_AUX BIT(5)

#define EXTEND_SCODE 0xE0
#define ESC_MAKE 0x01
#define ESC_BREAK 0x81
#define BREAK_CODE_MASK BIT(7)

#define KBC_STATUS_IBFULL BIT(1)
#define KBC_STATUS_OBFULL BIT(0)
#define KBC_PAR_ERR BIT(7)
#define KBC_TO_ERR BIT(6)

#define KBC_READ_CMD 0x20
#define KBC_WRITE_CMD 0x60

#define KBC_MAX_DELAY 20000
#define KBC_MAX_TRIES 20

#define SPACEBAR_MAKE 0x39
#define SPACEBAR_BREAK 0xB9

#define NUMB1_MAKE 0x02
#define NUMB1_BREAK 0x82

#define NUMB2_MAKE 0x03
#define NUMB2_BREAK 0x83

#define NUMB3_MAKE 0x04
#define NUMB3_BREAK 0x84

#define A_MAKE 0x1E
#define A_BREAK 0x9E

#define D_MAKE 0x20
#define D_BREAK 0xA0

#define W_MAKE 0x11
#define W_BREAK 0x91

/*
#define LEFT_ARROW_MAKE 0xE0 0x4B
#define LEFT_ARROW_BREAK 0xE0 0xCB

#define RIGHT_ARROW_MAKE 0xE0 0x4D
#define RIGHT_ARROW_BREAK 0xE0 0xCD

#define UP_ARROW_MAKE 0xE0 0x48
#define UP_ARROW_BREAK 0xE0 0xC8 */


#define BIT1_0          0XFD
#define BIT1_1          0X02

#define STAT_REG        0x64
#define STATUS_PORT 	  0x64
#define COM_PORT        0x64
#define OUT_BUF	 	      0x60
#define IN_BUF	 	      0x60
#define CMD_DATA 		    0X20
#define CMD_BUF 	      0x64

#define PARITY			    BIT(7)
#define TIMEOUT 		    BIT(6)
#define IBF 			      BIT(1)
#define OBF				      BIT(0)

//Mouse Packets Bits

#define Y_OVFL			   BIT(7)
#define X_OVFL			   BIT(6)
#define	Y_SIGN			   BIT(5)
#define X_SIGN 			   BIT(4)
#define MOUSE_MB		   BIT(2)
#define MOUSE_RB		   BIT(1)
#define MOUSE_LB		   BIT(0)

//KBC commands

#define READ_CMD_BYTE	 0X20
#define	WRITE_CMD_BYTE   0X60
#define DIS_MOUSE	     0XA7
#define	ENB_MOUSE	     0XA8
#define CHK_MOUSE_INT	 0XA9
#define	WRT_BYTE_MOUSE   0XD4

//Mouse commands

#define MOUSE_RESET				           0XFF //Mouse reset
#define MOUSE_RESEND 			           0xFE //For serial communications errors
#define MOUSE_SET_DEF			           0xF6 //Set default values
#define MOUSE_DISABLE_DATA_REPORTING       0xF5 //In stream mode, should be sent before any other command
#define MOUSE_ENABLE_DATA_REPORTING	       0xF4 //In stream mode only
#define MOUSE_SET_SAMPLERATE 	           0xF3 //Sets state sampling rate
#define MOUSE_SET_REMOTEMODE 	           0xF0 //Send data on request only
#define MOUSE_READ_DATA 		           0xEB //Send data packet request
#define MOUSE_SET_STREAM		           0xEA //Send data on events
#define MOUSE_STAT_REQ			           0xE9 //Get mouse configuration (3 bytes)
#define MOUSE_SET_RESOL			           0xE8
#define MOUSE_SCALLING21		           0xE7 //Acceleration mode
#define MOUSE_SCALLING11 		           0xE6 //Linear mode
#define MOUSE_ACK				           0xFA //if everything OK
#define MOUSE_NACK				           0xFE //if invalid byte
#define MOUSE_ERROR				           0xFC //second consecutive invalid byte


#endif /* _LCOM_I8042_H */
