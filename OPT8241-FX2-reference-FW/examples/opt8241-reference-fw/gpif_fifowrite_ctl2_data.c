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

#include <fx2regs.h>
#include <fx2macros.h>
#include <delay.h>
#include <gpif_fifowrite_ctl2_data.h>

#define SYNCDELAY() SYNCDELAY4

// firmware download ctrl_2
// DO NOT EDIT ...
const char xdata WaveData[128] =
{
// Wave 0
/* LenBr */ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* Output*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
// Wave 1
/* LenBr */ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* Output*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
// Wave 2
/* LenBr */ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* Output*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
// Wave 3
/* LenBr */ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x02,     0x02,     0x06,     0x02,     0x02,     0x02,     0x02,     0x00,
/* Output*/ 0x00,     0x04,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
};
// END DO NOT EDIT

// DO NOT EDIT ...
const char xdata FlowStates[36] =
{
/* Wave 0 FlowStates */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* Wave 1 FlowStates */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* Wave 2 FlowStates */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* Wave 3 FlowStates */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
// END DO NOT EDIT

// DO NOT EDIT ...
const char xdata InitData[7] =
{
/* Regs  */ 0xA0,0x10,0x00,0x00,0xEE,0x4E,0x00
};
// END DO NOT EDIT

// TO DO: You may add additional code below.

void GpifInit( void )
{
	BYTE i;

	// Registers which require a synchronization delay, see section 15.14
	// FIFORESET        FIFOPINPOLAR
	// INPKTEND         OUTPKTEND
	// EPxBCH:L         REVCTL
	// GPIFTCB3         GPIFTCB2
	// GPIFTCB1         GPIFTCB0
	// EPxFIFOPFH:L     EPxAUTOINLENH:L
	// EPxFIFOCFG       EPxGPIFFLGSEL
	// PINFLAGSxx       EPxFIFOIRQ
	// EPxFIFOIE        GPIFIRQ
	// GPIFIE           GPIFADRH:L
	// UDMACRCH:L       EPxGPIFTRIG
	// GPIFTRIG

	// Note: The pre-REVE EPxGPIFTCH/L register are affected, as well...
	//      ...these have been replaced by GPIFTC[B3:B0] registers

	// 8051 doesn't have access to waveform memories 'til
	// the part is in GPIF mode.

	//IFCONFIG = 0xEE;
	// IFCLKSRC=1   , FIFOs executes on internal clk source
	// xMHz=1       , 48MHz internal clk rate
	// IFCLKOE=0    , Don't drive IFCLK pin signal at 48MHz
	// IFCLKPOL=0   , Don't invert IFCLK pin signal from internal clk
	// ASYNC=1      , master samples asynchronous
	// GSTATE=1     , Drive GPIF states out on PORTE[2:0], debug WF
	// IFCFG[1:0]=10, FX2 in GPIF master mode

	GPIFABORT = 0xFF;  // abort any waveforms pending

	GPIFREADYCFG = InitData[ 0 ];
	GPIFCTLCFG = InitData[ 1 ];
	GPIFIDLECS = InitData[ 2 ];
	GPIFIDLECTL = InitData[ 3 ];
	GPIFWFSELECT = InitData[ 5 ];
	GPIFREADYSTAT = InitData[ 6 ];

	// use dual autopointer feature...
	AUTOPTRSETUP = 0x07;          // inc both pointers,
                                // ...warning: this introduces pdata hole(s)
                                // ...at E67B (XAUTODAT1) and E67C (XAUTODAT2)

	// source
	AUTOPTRH1 = MSB( &WaveData );
	AUTOPTRL1 = LSB( &WaveData );

	// destination
	AUTOPTRH2 = 0xE4;
	AUTOPTRL2 = 0x00;

	// transfer
	for (i = 0x00; i < 128; i++)
	{
		EXTAUTODAT2 = EXTAUTODAT1;
	}

#if 0
	// Configure GPIF Address pins, output initial value,
	PORTCCFG = 0xFF;    // [7:0] as alt. func. GPIFADR[7:0]
	OEC = 0xFF;         // and as outputs
	PORTECFG |= 0x80;   // [8] as alt. func. GPIFADR[8]
	OEE |= 0x80;        // and as output

	// ...OR... tri-state GPIFADR[8:0] pins
	//  PORTCCFG = 0x00;  // [7:0] as port I/O
	//  OEC = 0x00;       // and as inputs
	//  PORTECFG &= 0x7F; // [8] as port I/O
	//  OEE &= 0x7F;      // and as input

	// GPIF address pins update when GPIFADRH/L written
	SYNCDELAY;                    //
	GPIFADRH = 0x00;    // bits[7:1] always 0
	SYNCDELAY;                    //
	GPIFADRL = 0x00;    // point to PERIPHERAL address 0x0000
#endif
	// Configure GPIF FlowStates registers for Wave 0 of WaveData
	FLOWSTATE = FlowStates[ 0 ];
	FLOWLOGIC = FlowStates[ 1 ];
	FLOWEQ0CTL = FlowStates[ 2 ];
	FLOWEQ1CTL = FlowStates[ 3 ];
	FLOWHOLDOFF = FlowStates[ 4 ];
	FLOWSTB = FlowStates[ 5 ];
	FLOWSTBEDGE = FlowStates[ 6 ];
	FLOWSTBHPERIOD = FlowStates[ 7 ];
}
