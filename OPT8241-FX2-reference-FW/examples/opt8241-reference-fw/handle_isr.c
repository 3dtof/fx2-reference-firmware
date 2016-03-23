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

#define __HANDLER_ISR_C__

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
#include <common.h>

extern volatile bit got_sud;
extern BYTE fw_download_done ;
extern volatile BYTE timer0_running;
xdata WORD sofct=0;
bit EP2PF ;
bit on5=0;


/*!
 *=============================================================================
 *
 *  @fn		void sudav_isr() interrupt SUDAV_ISR
 *
 *  @brief	Set up data arrived interrupt handler
 *
 *=============================================================================
 */
void sudav_isr() interrupt SUDAV_ISR
{
	got_sud = TRUE;
	CLEAR_SUDAV();
}

/*!
 *=============================================================================
 *
 *  @fn		void sof_isr () interrupt SOF_ISR
 *
 *  @brief
 *
 *=============================================================================
 */

void sof_isr() interrupt SOF_ISR using 1 {
    ++sofct;
    if (sofct == 8000) { // about 8000 sof interrupts per second at high speed
        on5=!on5;
        if (on5) {d5on();} else {d5off();}
        sofct=0;
    }
    CLEAR_SOF();
}

/*!
 *=============================================================================
 *
 *  @fn		void usbreset_isr() interrupt USBRESET_ISR
 *
 *  @brief
 *
 *=============================================================================
 */


void usbreset_isr() interrupt USBRESET_ISR {
    handle_hispeed(FALSE);
    CLEAR_USBRESET();
}

/*!
 *=============================================================================
 *
 *  @fn		void hispeed_isr() interrupt HISPEED_ISR
 *
 *  @brief
 *
 *=============================================================================
 */

void hispeed_isr() interrupt HISPEED_ISR {
    handle_hispeed(TRUE);
    CLEAR_HISPEED();
}


/*!
 *=============================================================================
 *
 *  @fn		void ep2pf_isr() interrupt EP2PF_ISR
 *
 *  @brief
 *
 *=============================================================================
 */
 #if 0
void ep2pf_isr() interrupt EP2PF_ISR {
		if (EP2PF) {
			EP2FIFOPFH = 0x40|lLimit_MSB; // decision =0 (equal than Or less than , pktstat = 1 - works on number of bytes in fifo
			EP2FIFOPFL = lLimit_LSB;
		} else {
			EP2FIFOPFH = 0xC0|hLimit_MSB; // decision =1 (equal to Or greater than , pktstat = 1 - works on number of bytes in fifo
			EP2FIFOPFL = hLimit_LSB;
		}
		EP2PF = 1-EP2PF;
		CLEAR_EP2PF();
		DISABLE_EP2PF();
		ENABLE_EP2PF();
		// toggle PD0
		PA7 = 1-PA7;
}
#endif


/*!
 *=============================================================================
 *
 *  @fn		void gpifdone_isr() interrupt GPIFDONE_ISR
 *
 *  @brief
 *
 *=============================================================================
 */

void gpifdone_isr() interrupt GPIFDONE_ISR
{

	CLEAR_GPIFDONE();
	PA3 = 1;
	SYNCDELAY();
	SYNCDELAY();
	delay(1);

	if (!PA3) {
		set_err_status(0x60); // blue
	} else {
		fw_download_done = 1;
		set_err_status(0x30); //green
			// done so lets re-enumerate as UVC
		RENUMERATE_UNCOND();
	}

}

/*!
 *=============================================================================
 *
 *  @fn		void timer0_start(WORD us)
 *
 *  @brief
 *
 *=============================================================================
 */

void timer0_start(WORD us)
{
	timer0_running= 1;
	fx2_setup_timer0(us);
}

/*!
 *=============================================================================
 *
 *  @fn		void timer0_callback(void)
 *
 *  @brief
 *
 *=============================================================================
 */

void timer0_callback(void)
{
	timer0_running= 0;
}

#undef __HANDLER_ISR_C__
/*! @} */
