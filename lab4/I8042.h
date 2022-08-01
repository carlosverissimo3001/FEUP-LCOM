#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define ESC_KEY         0x81
#define SPC_KEY			    0XE0
#define BIT1_0          0XFD
#define BIT1_1          0X02

#define KBD_IRQ	        1
#define MOUSE_IRQ		    12

#define DELAY_US 		    20000

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
