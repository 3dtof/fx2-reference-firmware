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

#ifndef __PMIC_REG_H__
#define __PMIC_REG_H__


#define PMIC_STD_I2C_ADDR 0x2D
#define PMIC_AVS_I2C_ADDR 0x13


#define DCDC1_CTRL 0x00
#define DCDC2_CTRL 0x01
#define DCDC3_CTRL 0x02
#define DCDC4_CTRL 0x03

#define DCDC1_OP 	0x04
#define DCDC1_AVS 	0x05
#define DCDC1_LIMIT 0x06

#define DCDC2_OP 	0x07
#define DCDC2_AVS 	0x08
#define DCDC2_LIMIT 0x09
#define DCDC3_OP 	0x0A
#define DCDC3_AVS 	0x0B
#define DCDC3_LIMIT 0x0C
#define DCDC4_OP 	0x0D
#define DCDC4_AVS 	0x0E
#define DCDC4_LIMIT 0x0F
#define LDO1_OP		0x10
#define LDO1_AVS	0x11
#define LDO1_LIMIT	0x12
#define LDO2_OP		0x13
#define LDO2_AVS	0x14
#define LDO2_LIMIT	0x15
#define LDO3_OP		0x16
#define LDO3_AVS	0x17
#define LDO3_LIMIT	0x18
#define LDO4_OP		0x19
#define LDO4_AVS	0x1A
#define LDO4_LIMIT	0x1B
#define LDO5		0x1C
#define LDO6		0x1D
#define LDO7		0x1E
#define LDO8		0x1F
#define LDO9		0x20
#define LDO10		0x21
#define I2C_SPI_CFG	0x26
#define LOADSWITCH	0x62

#define GPIO3		0x43
#define GPIO4		0x44
#define GPIO5		0x45

#define LEDA_CTRL1	0x47
#define LEDA_CTRL2	0x48
#define LEDA_CTRL3	0x49
#define LEDA_CTRL4	0x4A
#define LEDA_CTRL5	0x4B
#define LEDA_CTRL6	0x4C
#define LEDA_CTRL7	0x4D
#define LEDA_CTRL8	0x4E

#define LEDB_CTRL1	0x4F
#define LEDB_CTRL2	0x50
#define LEDB_CTRL3	0x51
#define LEDB_CTRL4	0x52
#define LEDB_CTRL5	0x53
#define LEDB_CTRL6	0x54
#define LEDB_CTRL7	0x55
#define LEDB_CTRL8	0x56

#define LEDC_CTRL1	0x57
#define LEDC_CTRL2	0x58
#define LEDC_CTRL3	0x59
#define LEDC_CTRL4	0x5A
#define LEDC_CTRL5	0x5B
#define LEDC_CTRL6	0x5C
#define LEDC_CTRL7	0x5D
#define LEDC_CTRL8	0x5E

#define LED_RAMP_UP_TIME	0x5F
#define LED_RAMP_DOWN_TIME	0x60
#define LED_SEQ_EN			0x61


#endif /* __PMIC_REG_H__ */
/*! @} */
