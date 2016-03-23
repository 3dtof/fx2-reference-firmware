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

#define __PMIC_C__

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
#include <pmic.h>
#include <main.h>

extern BYTE hw_version;

void pmic_writeReg(BYTE reg_addr, BYTE dat)
{
	i2c_write(PMIC_STD_I2C_ADDR, 1, &reg_addr, 1, &dat);
}

/*!
 *=============================================================================
 *
 *  @fn		void PMIC_init(void)
 *
 *  @brief	First stage init of the PMIC.
 *  Only 3.3V (LDO9) and 1.8V DCDC2 and DCDC3 are kept alive. All other supplies are disabled.
 *
 *=============================================================================
 */

void PMIC_init(void)
{
	pmic_writeReg(DCDC1_AVS, 0x00);
	pmic_writeReg(DCDC4_AVS, 0x00);

	pmic_writeReg(LDO1_AVS, 0x00);
	pmic_writeReg(LDO2_AVS, 0x00);
	pmic_writeReg(LDO3_AVS, 0x00);
	pmic_writeReg(LDO4_AVS, 0x00);
	pmic_writeReg(LDO5, 0x00);
	pmic_writeReg(LDO6, 0x00);
	pmic_writeReg(LDO7, 0x00);
	pmic_writeReg(LDO8, 0x00);
	pmic_writeReg(LDO10, 0x00);

	/* DCDCx_CTRL, set for slowest ramp */
	pmic_writeReg(DCDC1_CTRL, 0x1C);
	pmic_writeReg(DCDC2_CTRL, 0x1C);
	pmic_writeReg(DCDC3_CTRL, 0x1C);
	pmic_writeReg(DCDC4_CTRL, 0x1C);
}

static void enable1v2(void)
{
	if (hw_version == 2)
		pmic_writeReg(DCDC4_AVS, 0x8E); // 1.2V
	else if (hw_version == 1)
		pmic_writeReg(LDO7, 0x90);	// 1.2V
	return;
}

static void enablePVDD(void)
{
	if (hw_version == 2)
		pmic_writeReg(LDO7, 0xBF);	// 3.3V
	else
		pmic_writeReg(DCDC4_AVS, 0xB8);	// 3.3V
	return;
}

/*!
 *=============================================================================
 *
 *  @fn		void PMIC_init2 (void)
 *
 *  @brief	Initializes the PMIC. All the DCDC and LDOs are brought upto required voltages
 *
 *=============================================================================
 */

/*
 * DCDC1: VCC_OUT_1V5 for Mix
 * DCDC2: VCC_OUT_1V8 for primary 1V8 source for the board
 * 			- Do not disable this rail
 * DCDC3: VCC_OUT_3V3 for most 3V3 supplies
 *         except FX2 which has a dedicated LDO
 *         and Sensor AVDD 3V3 on hw2 which also uses a dedicated LDO
 * DCDC4: VCC_OUT_1V2 on hw2, VCC_OUT_3V6 for PVDD on hw1
 * LDO1: VCC_ILLUM_VREF - how much?
 * LDO2: VCC_FB - how much?
 * LDO3: VCC_OUT_2V5 for TFC
 * LDO6: VCC_OUT_0V9 for DDR
 * LDO7: VCC_OUT_PVDD on hw2, VCC_OUT_1V2 for TFC on hw1
 * LDO8: VCC_AVDD_OUT_3V3 for OPT8241
 * LDO9: VCC_LDO_OUT_3V3 for FX2 3V3
 *			- Do not disable this rail
 * LDO10: VCC_LDO_OUT_1V8 for PLL
 */

void PMIC_init2(void)
{
	BYTE reg_addr, dat;


	// DCDC1: Control set to AVS register, no value in output
	// DCDC1: Do not enable until TFC is up
	pmic_writeReg(DCDC1_LIMIT, 0xFF);
	pmic_writeReg(DCDC1_OP, 0x40);
	pmic_writeReg(DCDC1_AVS, 0x14);

	// DCDC3: 3.3 V
	pmic_writeReg(DCDC3_LIMIT, 0xFF);
	pmic_writeReg(DCDC3_OP, 0x78);
	pmic_writeReg(DCDC3_AVS, 0xB8);

	pmic_writeReg(DCDC4_LIMIT, 0xFF);
	pmic_writeReg(DCDC4_OP, 0x40);
	enable1v2();

	//set proper voltages for LDO1 and LDO2, but do not enable the LDOs yet
	pmic_writeReg(LDO1_OP, 0x46);
	pmic_writeReg(LDO1_LIMIT, 0x3F);

	pmic_writeReg(LDO2_OP, 0x44);
	pmic_writeReg(LDO2_LIMIT, 0x3F);

	//enable LDOs 3, 6, 7 and 10
	pmic_writeReg(LDO3_OP, 0x32);
	pmic_writeReg(LDO3_LIMIT, 0x3F);
	pmic_writeReg(LDO3_AVS, 0xB2);

	pmic_writeReg(LDO6, 0x84);

	enablePVDD();
	if (hw_version == 2)
		pmic_writeReg(LDO8, 0xBF);	// 3.3V
	
	pmic_writeReg(LDO10, 0xA4);

	//enable the load switch
	pmic_writeReg(LOADSWITCH, 0x0D);

	//enable the LEDs
	pmic_writeReg(GPIO3, 0x40);
	pmic_writeReg(GPIO4, 0x40);
	pmic_writeReg(GPIO5, 0x40);

	//dc current of 2ma, no ramp
	pmic_writeReg(LEDA_CTRL1, 0x00);

	//T1-- LED: seq time is zero. set nayone to some value
	pmic_writeReg(LEDA_CTRL2, 0x0F);
	pmic_writeReg(LEDB_CTRL2, 0x0F);
	pmic_writeReg(LEDC_CTRL2, 0x0F);

	//set the ON duty cycle 100 %
	pmic_writeReg(LEDA_CTRL7, 0x1F);
	pmic_writeReg(LEDB_CTRL7, 0x1F);
	pmic_writeReg(LEDC_CTRL7, 0x1F);

	//set the on time < seq time
	pmic_writeReg(LEDA_CTRL8, 0x0C);
	pmic_writeReg(LEDB_CTRL8, 0x0C);
	pmic_writeReg(LEDC_CTRL8, 0x0C);

	//ramp up ramp down times are zero
	pmic_writeReg(LED_RAMP_UP_TIME, 0x00);
	pmic_writeReg(LED_RAMP_DOWN_TIME, 0x00);

	//configure to enable LED
	pmic_writeReg(LED_SEQ_EN, 0x70);
}


/*!
 *=============================================================================
 *
 *  @fn		void enable_dcdc1(void)
 *
 *  @brief	Initializes the DCDC1. Mix voltages come up at 1V5
 *
 *=============================================================================
 */
void enableMix(void)
{
	pmic_writeReg(DCDC1_LIMIT, 0xFF);
	pmic_writeReg(DCDC1_OP, 0x40);
	pmic_writeReg(DCDC1_AVS, 0x94);
}

void setLEDBrightness(BYTE level)
{
	pmic_writeReg(LEDA_CTRL7, level);
	pmic_writeReg(LEDB_CTRL7, level);
	pmic_writeReg(LEDC_CTRL7, level);
}

#undef __PMIC_C__
/*! @} */
