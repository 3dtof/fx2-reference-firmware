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

#define __HANDLER_C__

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

// value (low byte) = ep
#define VC_EPSTAT 0xB1
BYTE alt_ifc_if1 = 0;

const char xdata LUT_AUTOINLENL[4] = {0xE0, 0xD0, 0x2C,0x00};
const char xdata LUT_AUTOINLENH[4] = {0x01, 0x02, 0x03,0x04};
BYTE bConfigurationValue;

/*!
 *=============================================================================
 *
 *  @fn		BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc)
 *
 *  @brief	handles get interface commands
 *
 *=============================================================================
 */
// this firmware only supports 0,0
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc)
{
	if (ifc == 0)
		*alt_ifc = 0;
	if (ifc == 1)
		*alt_ifc = alt_ifc_if1;
	return TRUE;
}

/*!
 *=============================================================================
 *
 *  @fn		BOOL handle_set_interface(BYTE ifc, BYTE* alt_ifc)
 *
 *  @brief	handles set interface commands
 *
 *=============================================================================
 */
BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc)
{

	EP2AUTOINLENL = LUT_AUTOINLENL[alt_ifc] ;
	EP2AUTOINLENH = LUT_AUTOINLENH [alt_ifc];
	alt_ifc_if1 = alt_ifc;
	// SEE TRM 2.3.7
	// reset toggles
	RESETTOGGLE(0x82);
	RESETTOGGLE(0x06);
	RESETFIFOS();
	EP2BCL=0x80;
	SYNCDELAY();
	EP2BCL=0X80;
	SYNCDELAY();
	EP2BCL=0x80;
	SYNCDELAY();
	EP2BCL=0X80;
	SYNCDELAY();
	EP6BCL=0x80;
	SYNCDELAY();
	EP6BCL=0X80;
	SYNCDELAY();
	EP6BCL=0x80;
	SYNCDELAY();
	EP6BCL=0X80;
	SYNCDELAY();
	return TRUE;
}

/*!
 *=============================================================================
 *
 *  @fn		BYTE handle_get_configuration()
 *
 *  @brief	handles get configuration commands
 *
 *=============================================================================
 */
// get/set configuration
BYTE handle_get_configuration()
{
	return bConfigurationValue;
}

/*!
 *=============================================================================
 *
 *  @fn		BOOL handle_set_configuration(BYTE cfg)
 *
 *  @brief	handles set configuration commands
 *
 *=============================================================================
 */
 BOOL handle_set_configuration(BYTE cfg)
{
	bConfigurationValue = cfg; // TBD : check if we plan to switch b/w configs
	switch (cfg) {
	case 0 :
		return TRUE;
		break;
	case 1:
		return TRUE;
		break;
	default:
		return TRUE;
		break;
	}
}

#undef __HANDLER_C__
/*! @} */
