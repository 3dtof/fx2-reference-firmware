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

#define __FIFO_GPIF_CFG_C__

#include <stdio.h>
#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>
#include <autovector.h>
#include <lights.h>
#include <eputils.h>
#include <i2c.h>
#include <fx2timer.h>
#include <gpif_fifowrite_ctl2_data.h>
#include <pmic_reg.h>
#include <main.h>
#include <pmic.h>
#include <common.h>

extern BYTE fw_tx_size[4];

void TD_Init(char mode)
{
	// Registers which require a synchronization delay, see section 15.14
	// FIFORESET		FIFOPINPOLAR
	// INPKTEND			OUTPKTEND
	// EPxBCH:L			REVCTL
	// GPIFTCB3			GPIFTCB2
	// GPIFTCB1			GPIFTCB0
	// EPxFIFOPFH:L		EPxAUTOINLENH:L
	// EPxFIFOCFG		EPxGPIFFLGSEL
	// PINFLAGSxx		EPxFIFOIRQ
	// EPxFIFOIE		GPIFIRQ
	// GPIFIE			GPIFADRH:L
	// UDMACRCH:L		EPxGPIFTRIG
	// GPIFTRIG

	// Note: The pre-REVE EPxGPIFTCH/L register are affected, as well...
	//      ...these have been replaced by GPIFTC[B3:B0] registers

	// default: all endpoints have their VALID bit set
	// default: TYPE1 = 1 and TYPE0 = 0 --> BULK
	// default: EP2 and EP4 DIR bits are 0 (OUT direction)
	// default: EP6 and EP8 DIR bits are 1 (IN direction)
	// default: EP2, EP4, EP6, and EP8 are double buffered

	if (mode == MODE_GPIF) {
		IFCONFIG = 0xF2 ; //GPIF mode, internal clock, 48MHz, clock out enabled
		//turn all the valid bits off
		SYNCDELAY();
		GpifInit( );
		//ENABLE_GPIFDONE ();
		//turn all the EP valid bits off
		EP2CFG &= 0x7F;
		SYNCDELAY();
		EP4CFG &=0x7F;
		SYNCDELAY();
		EP8CFG &=0x7F;
		SYNCDELAY();

		EP6CFG = 0xA2; // EP6 is DIR=OUT, TYPE=BULK, SIZE=512, BUF=2x
		SYNCDELAY();
		FIFORESET = 0x80; // Reset the FIFO
		SYNCDELAY();
		FIFORESET = 0x06;
		SYNCDELAY();
		FIFORESET = 0x00;
		SYNCDELAY();
		OUTPKTEND = 0x86; // Arm both EP2 buffers to “prime the pump”
		SYNCDELAY();
		OUTPKTEND = 0x86;
		SYNCDELAY();
		EP6FIFOCFG = 0x11; // EP2 is AUTOOUT=1, AUTOIN=0, ZEROLEN=0, WORDWIDE=1
		SYNCDELAY();
	} else if (mode == MODE_SLAVE_FIFO) {
		// set the slave FIFO interface to 48MHz
		//IFCONFIG = 0xE3; //-- slave fifo + internal clock synchronous
		// Invert the clock to be in line with the CDK
		IFCONFIG = 0x13; //-- slave fifo //C3 - internal clock
		SYNCDELAY();                    // see TRM section 15.14

		FIFOPINPOLAR = 0x2C;
		SYNCDELAY();

		//turn all the EP valid bits off
		EP6CFG &=0x7F;
		SYNCDELAY();
		EP4CFG &=0x7F;
		SYNCDELAY();
		EP8CFG = 0x92;
		SYNCDELAY();
		EP2CFG = 0xE0   ; //valid = 1, IN = 1, type = BULK, size = 512, buf = quad
		SYNCDELAY();

		FIFORESET = 0x80; // reset all FIFOs
		SYNCDELAY();
		FIFORESET = 0x02;
		SYNCDELAY();
		FIFORESET = 0x08;
		SYNCDELAY();
		FIFORESET = 0x00;
		SYNCDELAY();

		EP4FIFOCFG &= 0xFE;
		SYNCDELAY();
		EP6FIFOCFG &= 0xFE;
		SYNCDELAY();
		EP8FIFOCFG &= 0xFE;
		SYNCDELAY();
		EP2FIFOCFG = 0x4C; // Full-minus-one, auto IN, zero len packet enabled
		SYNCDELAY();
		EP2AUTOINLENH = 0x02; // MoBL-USB FX2LP18 automatically commits data in 1024-byte chunks
		SYNCDELAY();
		EP2AUTOINLENL = 0x00;
		SYNCDELAY();
		EP8AUTOINLENH = 0x02; // MoBL-USB FX2LP18 automatically commits data in 1024-byte chunks
		SYNCDELAY();
		EP8AUTOINLENL = 0x00;
		SYNCDELAY();
		EP2ISOINPKTS =0x01 ; //DTA1/DATA0 PID auto taken care of + 3 packets per uF

		EP2FIFOPFH = 0xC3; // level of 1000 bytes
		SYNCDELAY();
		EP2FIFOPFL = 0xe6;
		SYNCDELAY();
		PINFLAGSAB = 0xCC ; // A- FF B - PF
		SYNCDELAY();
		PINFLAGSCD = 0x0F ; // EP8-FF
		set_err_status(0x10); // Green
	}
}

#undef __FIFO_GPIF_CFG_C__
/*! @} */
