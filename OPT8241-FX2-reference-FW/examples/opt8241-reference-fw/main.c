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

#define __MAIN_C__

#include <stdio.h>
#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>
#include <autovector.h>
#include <lights.h>
#include <setupdat.h>
#include <eputils.h>
#include <i2c.h>
#include <fx2timer.h>
#include <gpif_fifowrite_ctl2_data.h>
#include <pmic_reg.h>
#include <main.h>
#include <pmic.h>
#include <fifo_gpif_config.h>
#include <common.h>


extern volatile BYTE timer0_running;
extern volatile bit got_sud;
extern BYTE fw_download_done;
extern BYTE config_done;
extern BYTE gpif_mode_on ;
extern BYTE slave_mode_on;
extern BYTE hw_version;


extern code WORD serialnumber_0;

#define FX2_BOOTEEPROM_SLAVE			0x51
#define SERIAL_NUMBER_OFFSET			0x3FE0
#define SERIAL_NUMBER_LEN_OFFSET		0x3FD0
#define SERIAL_NUMBER_CHECKSUM_OFFSET	0x3FDE
#define SERIAL_NUMBER_LEN_DEFAULT		22
#define SERIAL_NUMBER_LEN_MAX			32

static void checkHWVersion(void)
{
	BYTE xdata c2;

	eeprom_read(FX2_BOOTEEPROM_SLAVE, 0x4000, 1, &c2);

	if (c2 == 0xFF) {
		// 0xFF implies a larger EEPROM; ideally we should try at the next jump
		// for now, we will hardcode the version
		hw_version = 2;
	} else if (c2 == 0xC2) {
		// 0xC2 implies we wrapped around, so we're a 16kB EEPROM
		// and therefore version 1
		hw_version = 1;
	}
}

static void patch_descriptors(void)
{
	BYTE xdata dat[SERIAL_NUMBER_LEN_MAX+3];
	xdata BYTE *pSerial = (xdata BYTE *) &serialnumber_0;
	BYTE status;
	WORD checksum;
	BYTE xdata eepromChecksum[2];
	BYTE xdata serialLength;
	int i = 0;

	// Try to read the length of the serial number and the checksum
	eeprom_read(FX2_BOOTEEPROM_SLAVE, SERIAL_NUMBER_LEN_OFFSET, 1, &serialLength);
	if ((serialLength == 0x00) || (serialLength == 0xFF))
		serialLength = SERIAL_NUMBER_LEN_DEFAULT;

	eeprom_read(FX2_BOOTEEPROM_SLAVE, SERIAL_NUMBER_CHECKSUM_OFFSET, 2, &eepromChecksum[0]);

	dat[0] = serialLength;
	status = eeprom_read(FX2_BOOTEEPROM_SLAVE, SERIAL_NUMBER_OFFSET, serialLength, &dat[1]);
	dat[serialLength+1] = eepromChecksum[0];
	dat[serialLength+2] = eepromChecksum[1];
	// FIXME: Checksum and read from backup locations
	// The way this should behave is: read from serialnumber_0 and get the checksum
	// Check if this checksum matches the one for the default serialnumber case
	// If it does, then attempt to read from SERIAL_NUMBER_OFFSET, and check the checksum
	// If this checksum matches, then update the serial number in place at once,
	// so that we don't have to make too many i2c transfers at startup
	// If this checksum does not match, then attempt to get the serial number from a backup location
	// and construct the serial number or live with a placeholder?

	checksum = computeChecksum(&dat[0], serialLength+3);
	// checksum should be 0
	if (!checksum) {
		//pmic_writeReg(LED_SEQ_EN, 0x10);
		pSerial[0] = 2*(serialLength+1);
		for (i = 0; i < serialLength; i++) {
			pSerial[2*(i+1)] = dat[i+1];
		}
	} else {
		//pmic_writeReg(LED_SEQ_EN, 0x20);
	}
}

/*!
 *=============================================================================
 *
 *  @fn		void main()
 *
 *  @brief	Main FX2 Voxel function
 *
 *=============================================================================
 */

void main()
{
	int n = 0;
	BOOL dc_jack_unplugged;
	fw_download_done = 0;

	// All PortA and PortD signals are inputs
	// except PD7, which is OPT9221 resetZ
	OEA = 0x00;
	OED = 0x88;
	PD7 = 1;
	PD3 = 0;

	/* First stage init to turn off all rails except FX2 */
	PMIC_init();

	/* Check hardware revision based on EEPROM size */
	checkHWVersion();

	/* Call the second stage init to turn the rails on in sequence */
	PMIC_init2();

	//using advanced endpoint controls
	REVCTL=0x3;
	I2CTL = 0x01;

	got_sud=FALSE;

	patch_descriptors();
	// renumerate
	RENUMERATE_UNCOND();

	// run on internal IFCLOCK
	SETINTCLOCK();

	// CPU runs at 48 MHz
	SETCPUFREQ(CLK_48M);
	SYNCDELAY();

	//internal clock of 48MHz
	SETIF48MHZ();

	//enable ifclock drive
	SETENIFCLOCKDRIVE();

	//enable interrupts , SOF required??
	USE_USB_INTS();
	USE_GPIF_INTS();
	ENABLE_SUDAV();
	ENABLE_SOF();
	ENABLE_HISPEED();
	ENABLE_USBRESET();
	// global interrupt enable
	EA=1;


	if (PD4 == 1) {
		dc_jack_unplugged = 1;
		setLEDBrightness(0x00);
		PINFLAGSCD = 0x0B ; // EP8-EF
	} else {
		dc_jack_unplugged = 0;
		setLEDBrightness(0x1F);
		PINFLAGSCD = 0x0F ; // EP8-FF
	}

	// Need to run this based on TFC boot state, not unconditionally
	{
		EP2FIFOCFG = 0x4C;
		SYNCDELAY();
		EP4FIFOCFG &= 0xFE;
		SYNCDELAY();
		EP6FIFOCFG &= 0xFE;
		SYNCDELAY();
		EP8FIFOCFG = 0x4C;
		SYNCDELAY();
		TD_Init(MODE_SLAVE_FIFO);
		PD7 = 0;
		delay(100);
		PD7 = 1;
		delay(100);
		slave_mode_on = 1;

		enableMix();
	}

	while (TRUE) {
		if (got_sud) {
		  handle_setupdata();
		  got_sud=FALSE;
		}
	}
}

#undef __MAIN_C__
/*! @} */
