/*
 * Copyright (c) 2013-2015 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 * Limited License.  
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, 
 * non-exclusive license under copyrights and patents it now or hereafter 
 * owns or controls to make, have made, use, import, offer to sell and sell ("Utilize")
 * this software subject to the terms herein.  With respect to the foregoing patent 
 * license, such license is granted  solely to the extent that any such patent is necessary 
 * to Utilize the software alone.  The patent license shall not apply to any combinations which 
 * include this software, other than combinations with devices manufactured by or for TI (“TI Devices”).  
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license (including the 
 * above copyright notice and the disclaimer and (if applicable) source code license limitations below) 
 * in the documentation and/or other materials provided with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided that the following
 * conditions are met:
 *
 *             * No reverse engineering, decompilation, or disassembly of this software is permitted with respect to any 
 *     software provided in binary form.
 *             * any redistribution and use are licensed by TI for use only with TI Devices.
 *             * Nothing shall obligate TI to provide you with source code for the software licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the source code are permitted 
 * provided that the following conditions are met:
 *
 *   * any redistribution and use of the source code, including any resulting derivative works, are licensed by 
 *     TI for use only with TI Devices.
 *   * any redistribution and use of any object code compiled from the source code and any resulting derivative 
 *     works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers may be used to endorse or 
 * promote products derived from this software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI’S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL TI AND TI’S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define __UVC_C__

#include <stdio.h>
#include <fx2regs.h>
#include <fx2macros.h>
#include <delay.h>
#include <eputils.h>
#include <i2c.h>

#define SET_CUR 0x01
#define GET_CUR 0x81
#define GET_MIN 0x82
#define GET_MAX 0x83
#define GET_RES 0x84
#define GET_LEN 0x85
#define GET_INFO 0x86
#define GET_DEF 0x87

BOOL deliberate_reset = FALSE;
extern BYTE fw_download_done;
BYTE frameindex = 1;
BYTE framerate[4];


// We enter here when there is a class request for UVC
// index == SETUPDAT[3]
// interface == SETUPDAT[4]
// entity = SETUPDAT[5]

// wValue = (SETUPDAT[3] << 8) + SETUPDAT[2]
// We need to check the interface to which this is addressed as well
// wIndex: interace to which this request is addressed
// wValue: control number on that interface
// wIndex high has the entity ID
// wIndex low has the interface to which the request is addressed
// VS_PROBE_CONTROL is control number 1
// VS_COMMIT_CONTROL is control number 2
// EntityID is 0 for this
// For the XU, entityID is 3
// Interface number is 0 for the XU
// Interface number is 1 for the probe/commit
// because it's the streaming interface

// SETUPDAT4 --> Interface
// SETUPDAT5 --> Entity

void handle_uvcrequest(void) {
	BOOL handshake = TRUE;
	WORD len;
	BYTE bRequest;
	BYTE slave_addr_1;
	BYTE index, interface, entity;
	BYTE xdata dat[32];
	BYTE reg_addr[4];

	index = SETUPDAT[3];	// FIXME: This is actually the control number?
	interface = SETUPDAT[4];
	entity = SETUPDAT[5];
	len = SETUPDAT[6] + (SETUPDAT[7] << 8) ;
	bRequest = SETUPDAT[1];

	if ((interface == 1) && (entity == 0)) {
		// this is a request for probe or commit
		// index = 0x01: PROBE CONTROL
		// index = 0x02: COMMIT CONTROL
		// Length will be 0x1A for 26
		// Fields:

		switch (bRequest) {
		case SET_CUR:
			EP0BCL = 0;
			while(EP0CS & bmEPBUSY); // wait

			// if reading chunk of data from BUF, arm the EP0 again.
			// then wait until it becomes "un"- busy
			slave_addr_1 = EP0BUF[0];
			reg_addr[0] = EP0BUF[1] ;
			dat[0] = EP0BUF[2] ;
			dat[1] = EP0BUF[3] ;
			dat[2] = EP0BUF[4] ;

			switch (index) {
			case 1:
			case 2:
				frameindex = EP0BUF[3];
				framerate[0] = EP0BUF[4];
				framerate[1] = EP0BUF[5];
				framerate[2] = EP0BUF[6];
				framerate[3] = EP0BUF[7];
				break;
			case 4:
				break;
			case 6:
				//CPUCS = 0x0;
				deliberate_reset = TRUE;
				RENUMERATE_UNCOND();
				//delay(1);
				//CPUCS = 0x0;
				break;
			default:
				break;
			}
			EP0BCH = 0;
			EP0BCL = 0;
			break;

		case GET_MIN:
		case GET_MAX:
		case GET_RES:
		case GET_DEF:
		case GET_CUR:
			// index = 1: PROBE Control, index = 2: COMMIT Control
			// Need to fill this with 26 bytes
			// And the fields are well defined

			EP0BUF[0] = 0x01;	// bmHint
			EP0BUF[1] = 0x00;	// bmHint
			EP0BUF[2] = 0x01;	// bFormatIndex
			if (GET_MAX == bRequest)
				EP0BUF[3] = 0x0B;	// bFrameIndex
			else if (GET_CUR == bRequest)
				EP0BUF[3] = frameindex;
			else
				EP0BUF[3] = 0x01;	// bFrameIndex
			if (GET_MIN == bRequest) {
				EP0BUF[4] = 0xA0;	// dwFrameInterval
				EP0BUF[5] = 0x86;	// dwFrameInterval
				EP0BUF[6] = 0x01;	// dwFrameInterval
				EP0BUF[7] = 0x00;	// dwFrameInterval
			} else if (GET_MAX == bRequest) {
				EP0BUF[4] = 0xD0;	// dwFrameInterval
				EP0BUF[5] = 0x12;	// dwFrameInterval
				EP0BUF[6] = 0x13;	// dwFrameInterval
				EP0BUF[7] = 0x00;	// dwFrameInterval
			} else if (GET_CUR == bRequest) {
				EP0BUF[4] = framerate[0];	// dwFrameInterval
				EP0BUF[5] = framerate[1];	// dwFrameInterval
				EP0BUF[6] = framerate[2];	// dwFrameInterval
				EP0BUF[7] = framerate[3];	// dwFrameInterval
			} else {
				EP0BUF[4] = 0x15;	// dwFrameInterval
				EP0BUF[5] = 0x16;	// dwFrameInterval
				EP0BUF[6] = 0x05;	// dwFrameInterval
				EP0BUF[7] = 0x00;	// dwFrameInterval
			}
			EP0BUF[8] = 0x00;	// wKeyFrameRate
			EP0BUF[9] = 0x00;	// wKeyFrameRate
			EP0BUF[10] = 0x00;	// wPFrameRate
			EP0BUF[11] = 0x00;	// wPFrameRate
			EP0BUF[12] = 0x00;	// wCompQuality
			EP0BUF[13] = 0x00;	// wCompQuality
			EP0BUF[14] = 0x00;	// wCompWindowSize
			EP0BUF[15] = 0x00;	// wCompWindowSize
			EP0BUF[16] = 0x00;	// wDelay
			EP0BUF[17] = 0x00;	// wDelay
			switch (frameindex) {
			case 1:
				// 320x240 raw mode 6 quad (960*480)
				EP0BUF[18] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x10;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x0E;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 2:
			default:
				// 320x240 default
				EP0BUF[18] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0xB0;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x04;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 3:
				// 320x240 raw mode 4 quad (640*480)
				EP0BUF[18] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x60;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x09;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 4:
				// 320x120x4
				EP0BUF[18] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x58;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x02;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 5:
				// 160x120x4
				EP0BUF[18] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x2C;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x01;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 6:
				// 160x60x4
				EP0BUF[18] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x96;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 7:
				// 80x60x4
				EP0BUF[18] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x4B;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 8:
				// 80x30x4
				EP0BUF[18] = 0x80;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x25;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 9:
				// 40x30x4
				EP0BUF[18] = 0xC0;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x12;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 10:
				// 40x15x4
				EP0BUF[18] = 0x60;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x09;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			case 11:
				// 320x240x4 + 96
				EP0BUF[18] = 0x60;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0xB0;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x04;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
				break;
			}
			if (GET_MAX == bRequest) {
				EP0BUF[18] = 0x00;	// dwMaxVideoFrameSize
				EP0BUF[19] = 0x10;	// dwMaxVideoFrameSize
				EP0BUF[20] = 0x0E;	// dwMaxVideoFrameSize
				EP0BUF[21] = 0x00;	// dwMaxVideoFrameSize
			}
			EP0BUF[22] = 0x00;	// dwMaxPayloadTransferSize
			EP0BUF[23] = 0x04;	// dwMaxPayloadTransferSize
			EP0BUF[24] = 0x00;	// dwMaxPayloadTransferSize
			EP0BUF[25] = 0x00;	// dwMaxPayloadTransferSize

			EP0BCH = 0;
			EP0BCL = len;
		break;

		case GET_LEN:
			EP0BUF[0] = 0x1A ;
			EP0BUF[1] = 0x00;
			EP0BCH = 0;
			EP0BCL = len;
			break;

		case GET_INFO:
			switch (index) {
			// D0 : support GET_CUR
			// D1 : supports SET_CUR
			case 1: // only write suppoerts
			case 2:
			case 4:
			case 6:
				EP0BUF[0] = 0x02 ;	//write i2c reg + data (3)
				break;

			case 3: // only read supported
			case 5:
				EP0BUF[0] = 0x01 ; // read i2c
				break;

			default:
				EP0BUF[0] = 0x03 ; // supports both ( ?? )
				break;
			}
			EP0BCH = 0;
			EP0BCL = len;
			break;

		default :
			handshake = FALSE;
			STALLEP0();
		}
	} else {
		// this must be a request for the XU or camera
		// so handle it as usual

		// index = 0x01 , LEN = 5, SET_CUR -- I2C write reg addr + 3 bytes of data , GET_CUR -- returns some dont care data
		// following are parameters:
		// slave_addr_1 =  EP0BUF[0]
		// reg_addr[0] = EP0BUF[1]
		// data[0] =EP0BUF[2]
		// data[1] =EP0BUF[3]
		// data[2] =EP0BUF[4]

		// index = 0x02 , LEN = 2, SET_CUR -- I2C write reg address , GET_CUR -- returns some dont care data
		// this can be used to set address before reading I2C data
		// following are parameters:
		// slave_addr_1 =  EP0BUF[0]
		// reg_addr[0] = EP0BUF[1]

		// index = 0x03, LEN = 3, SET_CUR -- does nothing , GET_CUR -- returns 3 bytes of data from I2C read. Reg address/slave address to be set using index = 2
		// EP0BUF[0] = data[0]
		// EP0BUF[1] = data[1]
		// EP0BUF[2] = data[2]

		// index = 0x04 , LEN = 3, SET_CUR -- I2C write reg address + 1 bytes of data , GET_CUR -- returns some dont care data
		// this can be used to set address before reading I2C data
		// following are parameters:
		// slave_addr_1 =  EP0BUF[0]
		// reg_addr[0] = EP0BUF[1]
		// data = EP0BUF[2]

		// index = 0x05, LEN = 1, SET_CUR -- does nothing , GET_CUR -- returns 1 bytes of data from I2C read. Reg address/slave address to be set using index = 2
		// EP0BUF[0] = data[0]

		//index = 0x06, reset the 8051
		// LEN = 1, write only

		switch (bRequest) {
		case SET_CUR:

			EP0BCL = 0;
			while(EP0CS & bmEPBUSY); // wait

			// if reading chunk of data from BUF, arm the EP0 again.
			// then wait until it becomes "un"- busy
			slave_addr_1 = EP0BUF[0];
			reg_addr[0] = EP0BUF[1] ;
			dat[0] = EP0BUF[2] ;
			dat[1] = EP0BUF[3] ;
			dat[2] = EP0BUF[4] ;

			switch (index) {
			case 1:
				i2c_write(slave_addr_1, 1, &reg_addr[0], 3, &dat[0]);
				break;
			case 2:
				i2c_write(slave_addr_1, 1, &reg_addr[0], 0, &dat[0]);
				break;
			case 4:
				i2c_write(slave_addr_1, 1, &reg_addr[0], 1, &dat[0]);
				break;
			case 6:
				//CPUCS = 0x0;
				deliberate_reset = TRUE;
				fw_download_done = 0;
				RENUMERATE_UNCOND();
				//delay(1);
				//CPUCS = 0x0;
				break;
			case 8:
				i2c_write(slave_addr_1, 1, &reg_addr[0], 2, &dat[0]);
				break;
			case 7:
				break;
			case 10:
				if (EP0BUF[0] == 1)
					PINFLAGSCD = 0x0F;
				else
					PINFLAGSCD = 0x0B;
				break;
			default:
				break;
			}
			EP0BCH = 0;
			EP0BCL = 0;
			break;

		case GET_CUR:
			switch (index) {
			case 3:
				i2c_read(slave_addr_1, 3, &EP0BUF[0]);
				EP0BCH = 0;
				EP0BCL = len;
				break;

			case 5:
				i2c_read(slave_addr_1, 1, &EP0BUF[0]);
				EP0BCH = 0;
				EP0BCL = len;
				break;

			case 9:
				i2c_read(slave_addr_1, 2, &EP0BUF[0]);
				EP0BCH = 0;
				EP0BCL = len;
				break;

			case 7:
				// 4 bytes
				// 0 ==> FW minor
				// 1 ==> FW Major
				// 2 ==> Status
				// 3 ==> Status
				EP0BUF[0] = 0x0A;
				EP0BUF[1] = 0;

				//if (config_done_flag == 1)
				//	EP0BUF[2] = 1;
				//else
				//	EP0BUF[2] = 0;
				// EP0BUF[2] = 0; //AG FIX
				EP0BUF[3] = 0;
				EP0BCH = 0;
				EP0BCL = len;
				break;

			default :
				//if (config_done_flag == 1)
				//	EP0BUF[0] = 0xAA;
				//else
				//	EP0BUF[0] = 0xAB;
				EP0BUF[0] = 0xAB; //AG FIX
				EP0BUF[1] = 0xBB;
				EP0BUF[2] = 0xAA;
				EP0BUF[3] = 0xBB;
				EP0BUF[4] = 0xBB;
				EP0BCH = 0;
				EP0BCL = len;
				break;
			}
			break;

		case GET_MIN:
			EP0BUF[0] = 0x00 ;
			EP0BUF[1] = 0x00;
			EP0BUF[2] = 0x00 ;
			EP0BUF[3] = 0x00;
			EP0BUF[4] = 0x00;
			EP0BCH = 0;
			EP0BCL = len; //max len possible is 4
			break;

		case GET_MAX:
			EP0BUF[0] = 0xFF ;
			EP0BUF[1] = 0xFF;
			EP0BUF[2] = 0xFF ;
			EP0BUF[3] = 0xFF;
			EP0BUF[4] = 0xFF;
			EP0BCH = 0;
			EP0BCL = len;
			break;

		case GET_RES:
			EP0BUF[0] = 0x01 ;
			EP0BUF[1] = 0x00;
			EP0BCH = 0;
			EP0BCL = len;
			break;
		case GET_LEN:
			switch (index) {
			case 1 :  //i2c write --> slave addr(1) + reg address(1) + dat(0) + dat(1) + dat(2)
				EP0BUF[0] = 0x05 ;
				break;

			case 2 : //i2c reg address write --> slave addr(1) + reg address(1)
				EP0BUF[0] = 0x02 ;
				break;

			case 3 : //i2c reg address read --> dat(0) + dat(1) + dat(2)
			case 4:  // i2c write  slave addr(1) + reg address(1) + dat(1)
				EP0BUF[0] = 0x03 ;
				break;

			case 5 : //i2c reg address read --> dat(0)
			case 6: // 8051 reset
				EP0BUF[0] = 0x01 ;
				break;
			case 7:
				EP0BUF[0] = 0x04;
				break;
			case 8:
				EP0BUF[0] = 0x04;
				break;
			case 9:
				EP0BUF[0] = 0x02;
				break;
			default:
				EP0BUF[0] = 0x02 ;
				break;
			}
			EP0BUF[1] = 0x00;
			EP0BCH = 0;
			EP0BCL = len;
			break;

		case GET_INFO:
			switch (index) {
			// D0 : support GET_CUR
			// D1 : supports SET_CUR
			case 1: // only write supported
			case 2:
			case 4:
			case 6:
				EP0BUF[0] = 0x02 ;	//write i2c reg + data (3)
				break;

			case 3: // only read supported
			case 5:
			case 7:
				EP0BUF[0] = 0x01 ; // read i2c
				break;

			default:
				EP0BUF[0] = 0x03 ; // supports both ( ?? )
				break;
			}
			EP0BCH = 0;
			EP0BCL = len;
			break;

		case GET_DEF:
			EP0BUF[0] = 0x01 ;
			EP0BUF[1] = 0x00;
			EP0BCH = 0;
			EP0BCL = len;
			break;
		default :
			handshake = FALSE;
			STALLEP0();
		}
	}
}

#undef __UVC_C__
/*! @} */