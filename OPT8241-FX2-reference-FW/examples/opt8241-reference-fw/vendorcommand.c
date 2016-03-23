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

#define __VENDORCOMMAND_C__

#include <stdio.h>
#include <fx2regs.h>
#include <fx2macros.h>
#include <delay.h>
#include <eputils.h>
#include <i2c.h>
#include <common.h>
#include <pmic_reg.h>
#include <pmic.h>

extern BYTE hw_version;

/*
 * Layout of SERIAL number in EEPROM
 * 64-bytes
 */
// First byte will store length, last two bytes will store checksum
// Length and checksum will be automatically calculated by FX2 firmware,
// other data will be sent over the bus
// So maximum acceptable length will be 29 bytes and we will use only that many for storage
#define FX2_BOOTEEPROM_SLAVE			0x51
#define SERIAL_NUMBER_OFFSET			0x3FE0
#define SERIAL_NUMBER_LEN_OFFSET		0x3FD0
#define SERIAL_NUMBER_CHECKSUM_OFFSET	0x3FDE
#define SERIAL_NUMBER_LEN_DEFAULT		22
#define SERIAL_NUMBER_LEN_MAX			32

// For 16k eeprom boards, calib data can be 2048 bytes or less?
// So 16384-64-2048
#define CALIB_DATA_START_HW1			0x3700
#define CALIB_DATA_START_HW2			0x4100
#define CALIB_DATA_SIZE_HW1				0x0800
#define CALIB_DATA_SIZE_HW2				0xBF00	// 64k - 0x4100

WORD getCalibDataStart(void)
{
	if (hw_version == 2)
		return CALIB_DATA_START_HW2;
	else
		return CALIB_DATA_START_HW1;
}

static WORD getCalibDataSize(void)
{
	if (hw_version == 2)
		return CALIB_DATA_SIZE_HW2;
	else
		return CALIB_DATA_SIZE_HW1;
}

/* Vendor Request Codes */
/* This block of request codes is reserved for legacy code reasons */
#define ADDR_DATA_PAIR_WRITE	0x00
#define I2C_PAGE_WRITE			0x01
#define I2C_PAGE_READ			0x02
#define FW_DOWNLOAD_SIZE		0x05
#define TEST_MEMORY_ACCESS		0x27
#define TEST_EEPROM_READ		0x07
#define READ_BACK_TEMP			0x06

/*
 * This block is used for i2c transfers. We could have used a single
 * API for all I2C transfers, but split it up based on I2C data size
 * to allow for specific hooks while accessing certain slaves/registers.
 * the first two are there for legacy code reasons, as we may still have
 * some host code using them.
 *
 * For new code, please use PMIC_READ/WRITE instead of I2C_BYTE_READ/WRITE.
 */
#define I2C_BYTE_WRITE			0x03
#define I2C_BYTE_READ			0x04
#define TFC_READ				0x08
#define TFC_WRITE				0x09
#define INA226_READ				0x0A
#define INA226_WRITE			0x0B
#define PMIC_READ				0x0C
#define PMIC_WRITE				0x0D

#define EN_FSC					0x10
#define FX2_FIRMWARE_VERSION	0x1A

/* SPI and OPT9221 EEPROM related commands */
#define EEPROM_PROGRAMMING_INIT	0x17
#define SPI_WRITE_FILE			0x18
#define SPI_READ_FILE			0x19
#define SPI_STATUS_READ1		0x1B
#define SPI_STATUS_READ2		0x1C
#define SPI_JEDEC_ID			0x1D
#define SPI_WRITE_DATA			0x1E

/* FX2 EEPROM related commands */
#define GET_BOARD_REVISION		0x30
#define GET_EEPROM_SIZE			0x31
#define SET_EEPROM_SIZE			0x31
#define GET_SERIAL_NUMBER		0x32
#define SET_SERIAL_NUMBER		0x32
#define EEPROM_STORE			0x33
#define EEPROM_RETRIEVE			0x33

#define TEST_MULTIPLY			0x50
#define ILLUM_DIGIPOT_STORE		0x51
#define ILLUM_DIGIPOT_SLAVE		0x54

#define ILLUM_POWER_DIGIPOT_WRB	0x01
#define ILLUM_POWER_DIGIPOT_ACR	0x10
#define ILLUM_POWER_MAXVAL_REG	0x0C
#define ILLUM_POWER_SCALER_REG	0x0D


#define REG_OPT9221_PIX_CNT_MAX			0x5882
#define REG_OPT9221_ILLUM_DC_CORR		0x580E
#define REG_OPT9221_INTG_DUTY_CYCLE		0x5C4C

void handle_uvcrequest(void)
{

}

BYTE xdata illum_digipot_shadow = 0x80;
/*
 * Illum Power setting needs to be gated with a scaling factor for compliance with LASER safety
 */
static void maskIllumPowerRegisters(BYTE regVal)
{
	BYTE xdata tempReg, regAddr, scaler, maxVal;
	WORD xdata product;

	/*
	 * write ACR to allow nonvolatile write
	 * read scaler
	 * write ACR to disallow nonvolatile write
	 * multiply regVal with scaler, and take the MSB
	 * write MSB into WRB register
	 */
	tempReg = 0x40;	// Allow non-volatile access
	regAddr = ILLUM_POWER_DIGIPOT_ACR;
	i2c_write(ILLUM_DIGIPOT_SLAVE, 1, &regAddr, 1, &tempReg);

	regAddr = ILLUM_POWER_MAXVAL_REG;
	i2c_write(ILLUM_DIGIPOT_SLAVE, 1, &regAddr, 0, &tempReg);
	i2c_read(ILLUM_DIGIPOT_SLAVE, 1, &maxVal);

	regAddr = ILLUM_POWER_SCALER_REG;
	i2c_write(ILLUM_DIGIPOT_SLAVE, 1, &regAddr, 0, &tempReg);
	i2c_read(ILLUM_DIGIPOT_SLAVE, 1, &scaler);

	illum_digipot_shadow = regVal;

	tempReg = 0xC0; // Disallow non-volatile access
	regAddr = ILLUM_POWER_DIGIPOT_ACR;
	i2c_write(ILLUM_DIGIPOT_SLAVE, 1, &regAddr, 1, &tempReg);

	product = scaler * (255 - regVal);
	tempReg = (255 - MSB(product));

	regAddr = ILLUM_POWER_DIGIPOT_WRB;
	i2c_write(ILLUM_DIGIPOT_SLAVE, 1, &regAddr, 1, &tempReg);
}

/*
 * Some OPT9221 registers have to be masked for compliance with LASER safety
 */
static void maskTFCRegisters(BYTE slave_addr, BYTE reg_addr, xdata BYTE *dataBuf)
{
	WORD xdata matchRegister = MAKEWORD(slave_addr, reg_addr);

	if (matchRegister == REG_OPT9221_INTG_DUTY_CYCLE) {
		/* intg_duty_cycle (register 0x5C4C, bits 5:0) should not exceed 20/64 */
		if ((dataBuf[0] & 0x3F) > 0x14) {
			dataBuf[0] &= 0xC0;	// mask intg_duty_cycle
			dataBuf[0] |= 0x14; // and clamp to 20
		}
		/* lumped dead time (register 0x5C4C, bit 22) should be masked */
		dataBuf[2] &= 0xBF;
	} else if (matchRegister == REG_OPT9221_ILLUM_DC_CORR) {
		/* illum_dc_corr registers (register 0x580E, bits 17, 16:13) should be masked */
		dataBuf[2] &= 0xFC;
		dataBuf[1] &= 0x1F;
	} else if (matchRegister == REG_OPT9221_PIX_CNT_MAX) {
		/* pix_cnt_max (register 0x5882, bits 21:0) has to be restricted to 60000 or higher */
		if ((dataBuf[2] & 0x3F) == 0) {
			if (MAKEWORD(dataBuf[1], dataBuf[0]) < 60000) {
				dataBuf[0] = 0x60;
				dataBuf[1] = 0xEA;
			}
		}
	}
}

sbit at 0xF0+7 B7;
sbit at 0xF0+6 B6;
sbit at 0xF0+5 B5;
sbit at 0xF0+4 B4;
sbit at 0xF0+3 B3;
sbit at 0xF0+2 B2;
sbit at 0xF0+1 B1;
sbit at 0xF0+0 B0;

/*
 * OPT9221 boot EEPROM to FX2 connections
 *
 * PD0 CSOZ --> EEPROM CS
 * PD1 MOSI --> Write
 * PD2 MISO --> Read
 * PD3 CEZ  --> Tristate
 * PD6 CLK  --> CLK
 */
#define SCK		PD6
#define SCK_LO	SCK = 0; NOP;
#define SCK_HI	SCK = 1; NOP;
#define MISO	PD2
//#define MISO_LO	MISO = 0
//#define MISO_HI	MISO = 1
#define MOSI	PD1
#define MOSI_LO	MOSI = 0; NOP;
#define MOSI_HI	MOSI = 1; NOP;
#define nSS		PD0
#define nSS_LO	nSS = 0; NOP;
#define nSS_HI	nSS = 1; NOP;

void spi_write_byte(BYTE dataOut)
{
	B = dataOut;

	MOSI = B7; SCK_HI; SCK_LO;
	MOSI = B6; SCK_HI; SCK_LO;
	MOSI = B5; SCK_HI; SCK_LO;
	MOSI = B4; SCK_HI; SCK_LO;
	MOSI = B3; SCK_HI; SCK_LO;
	MOSI = B2; SCK_HI; SCK_LO;
	MOSI = B1; SCK_HI; SCK_LO;
	MOSI = B0; SCK_HI; SCK_LO;
}

BYTE spi_read_byte(void)
{
	B7 = MISO; SCK_HI; SCK_LO;
	B6 = MISO; SCK_HI; SCK_LO;
	B5 = MISO; SCK_HI; SCK_LO;
	B4 = MISO; SCK_HI; SCK_LO;
	B3 = MISO; SCK_HI; SCK_LO;
	B2 = MISO; SCK_HI; SCK_LO;
	B1 = MISO; SCK_HI; SCK_LO;
	B0 = MISO; SCK_HI; SCK_LO;

	return B;
}

/*!
 *=============================================================================
 *
 *  @fn		BOOL handle_vendorcommand(BYTE cmd)
 *
 *  @brief	Handles Vendor commands. Data to be moved here from setupdat.c
 *
 *=============================================================================
 */

BOOL handle_vendorcommand(BYTE cmd) {
		//SETUPDATA[0] -
/*		D7: Data transfer direction
 		0 = Host-to-device
		1 = Device-to-host
		D6...5: Type
		0 = Standard
		1 = Class
		2 = Vendor
		3 = Reserved
		D4...0: Recipient
		0 = Device
		1 = Interface
		2 = Endpoint
		3 = Other
		4...31 = Reserved
 */
	BOOL handshake = TRUE;
	BOOL direction;
	WORD len, i;
	BYTE bRequest, status;
	BYTE xdata dat[64];
	BYTE reg_addr[4];
	BYTE slave_addr;
	BYTE spi_addr[3];

	bRequest =  SETUPDAT[1];
	direction = (SETUPDAT[0] & 0x80);
	// wValue = MAKEWORD(SETUPDAT[3], SETUPDAT[2]);
	
	slave_addr = SETUPDAT[3];
	// wIndex = MAKEWORD(SETUPDAT[5] + (SETUPDAT[4]);

	reg_addr[0] = SETUPDAT[5];
	reg_addr[1] = SETUPDAT[4];
	len = MAKEWORD(SETUPDAT[7], SETUPDAT[6]) ;

	switch (bRequest) {
	case I2C_BYTE_WRITE:

		EP0BCL = 0; // allow pc transfer in
		while (EP0CS & bmEPBUSY); // wait
		for (i = 0; i < len; i++)
			dat[i] = EP0BUF[i];

		if (slave_addr == ILLUM_DIGIPOT_SLAVE) {
			if ((reg_addr[0] == ILLUM_POWER_SCALER_REG) || (reg_addr[0] == ILLUM_POWER_MAXVAL_REG)) {
				// Don't allow this write
			} else if(reg_addr[0] == ILLUM_POWER_DIGIPOT_WRB) {
				// Protect writes to wiper B
				maskIllumPowerRegisters(dat[0]);
			} else {
				// Allow writes as usual
				i2c_write(slave_addr, 1, &reg_addr[0], 1, &dat[0]);
			}
		} else {
			i2c_write(slave_addr, 1, &reg_addr[0], len, &dat[0]);
		}

		break;

	case I2C_BYTE_READ:

		if ((slave_addr == ILLUM_DIGIPOT_SLAVE) && (reg_addr[0] == ILLUM_POWER_DIGIPOT_WRB)) {
			EP0BUF[0] = illum_digipot_shadow;
		} else {
			status = i2c_write(slave_addr, 1, &reg_addr[0], 0, &dat[0]);
			if (status)
				status = i2c_read(slave_addr, len, &dat[0]);

			if (status) {
				for (i = 0; i < len; i++)
					EP0BUF[i] = dat[i];
			} else {
				len = 0;
			}
		}

		EP0BCH = 0;
		EP0BCL = len;

		break;


	// This works for now: Need to move this to A2 and implement the same
	// protocol that was used earlier
	// Add parameters to the transfer and packetize and send it out
	// Then implement similar code for EEPROM write
	case TEST_EEPROM_READ:

		status = eeprom_read(0x51, reg_addr[1] + (reg_addr[0] << 8), len > 64 ? 64 : len, &dat[0]);
		if (status)
			set_err_status(0x20);
		else
			set_err_status(0x40);

		if (status) {
			for (i = 0; i < len; i++)
				EP0BUF[i] = dat[i];
		} else {
			len = 0;
		}

		EP0BCH = 0;
		EP0BCL = len;

		break;

	case TFC_READ:
	case TFC_WRITE:
		slave_addr = SETUPDAT[2]; 	// wValue LSBYTE
		reg_addr[0] = SETUPDAT[4];	// wIndex LSBYTE
		if (direction) {
			// Read, Length is 3 always
			dat[0] = 0x0;
			dat[1] = 0x0;
			dat[2] = 0x0;

			i2c_write(slave_addr, 1, &reg_addr[0], 0, &dat[0]);
			status = i2c_read(slave_addr, 3, &EP0BUF[0]);
			EP0BCH = 0;
			EP0BCL = 3;
		} else {
			// Write, Length is 3 always
			EP0BCL = 0;
			while (EP0CS & bmEPBUSY);
			dat[0] = EP0BUF[0];
			dat[1] = EP0BUF[1];
			dat[2] = EP0BUF[2];
			maskTFCRegisters(slave_addr, reg_addr[0], &dat[0]);
			i2c_write(slave_addr, 1, &reg_addr[0], 3, &dat[0]);

			EP0BCH = 0;
			EP0BCL = 0;
		}
		break;

	case EEPROM_PROGRAMMING_INIT:
		// Disable 1V2
		// disable LDO7 and set to 1V2
		// Major FIXME: need to use board revision check and use the appropriate rail
		pmic_writeReg(LDO7, 0x10);

		// Make MOSI, MISO, CSOZ, CLK, CEz as outputs
		// Keep DC_JACK_UNPLUG and INITDONE as inputs
		// Keep resetz and CEz high all the time, others low for now
		IOD = 0x80;
		OED = 0xCF;
		PD3 = 1;

		EP0BCH = 0;
		EP0BCL = len;

		break;

	case SPI_WRITE_FILE:
		// addr 23..16 == wIndexL
		// addr 15..08 == wValueH
		// addr 07..00 == wValueL
		// wValue = SETUPDAT[2] + (SETUPDAT[3]<<8) ;
		// wIndex = SETUPDAT[4] + (SETUPDAT[5]<<8) ;
		spi_addr[2] = SETUPDAT[4];
		spi_addr[1] = SETUPDAT[3];
		spi_addr[0] = SETUPDAT[2];

		EP0BCL = 0; // allow pc transfer in
		while (EP0CS & bmEPBUSY); // wait
		
		nSS_HI;
		nSS_LO;
		SCK_LO;
		MISO = 0;

		// Page program instruction (0000 0010)
		spi_write_byte(0x02);

		// address 23..16
		spi_write_byte(spi_addr[2]);

		// address 15..8
		spi_write_byte(spi_addr[1]);

		// address 7..0
		spi_write_byte(spi_addr[0]);

		for (i = 0; i < len; i++) {
			spi_write_byte(EP0BUF[i]);
		}

		nSS_HI;

		break;

	case SPI_WRITE_DATA:
		// Assert CSz and then write some bytes out to SPI
		// Will be used to implement arbit SPI commands like chip erase, and write enable
		EP0BCL = 0;	// allow pc transfer in
		while (EP0CS & bmEPBUSY);	// wait

		nSS_HI;
		nSS_LO;
		SCK_LO;
		MISO = 0;

		// Read instruction (0000 0011)
		for (i = 0; i < len; i++) {
			spi_write_byte(EP0BUF[i]);
		}

		nSS_HI;

		break;

	case SPI_READ_FILE:
		//for (i = 0; i < len; i++)
		//	EP0BUF[i] = i;
		//
		// addr 23..16 == wIndexL
		// addr 15..08 == wValueH
		// addr 07..00 == wValueL
		// wValue = SETUPDAT[2] + (SETUPDAT[3]<<8) ;
		// wIndex = SETUPDAT[4] + (SETUPDAT[5]<<8) ;
		spi_addr[2] = SETUPDAT[4];
		spi_addr[1] = SETUPDAT[3];
		spi_addr[0] = SETUPDAT[2];

		nSS_HI;
		nSS_LO;
		SCK_LO;
		MISO = 0;

		// Read instruction (0000 0011)
		spi_write_byte(0x03);

		// address 23..16
		spi_write_byte(spi_addr[2]);

		// address 15..8
		spi_write_byte(spi_addr[1]);

		// address 7..0
		spi_write_byte(spi_addr[0]);

		for (i = 0; i < len; i++) {
			EP0BUF[i] = spi_read_byte();
		}

		nSS_HI;

		EP0BCH = 0;
		EP0BCL = len;

		break;

	case SPI_JEDEC_ID:
		nSS_HI;
		nSS_LO;
		SCK_LO;
		MISO = 0;

		// Read JEDEC ID instruction 0x9F (1001 1111)
		spi_write_byte(0x9F);

		for (i = 0; i < 3; i++) {
			EP0BUF[i] = spi_read_byte();
		}

		nSS_HI;
		
		EP0BCH = 0;
		EP0BCL = len;	

		break;

	case SPI_STATUS_READ2:
		nSS_HI;
		nSS_LO;
		SCK_LO;
		MISO = 0;

		// Read status 1 instruction (0011 0101)
		spi_write_byte(0x35);
		EP0BUF[0] = spi_read_byte();

		nSS_HI;
		
		EP0BCH = 0;
		EP0BCL = len;	

		break;

	case SPI_STATUS_READ1:
		nSS_HI;
		nSS_LO;
		SCK_LO;
		MISO = 0;

		// Read status 1 instruction (0000 0101)
		spi_write_byte(0x05);
		EP0BUF[0] = spi_read_byte();

		nSS_HI;
		
		EP0BCH = 0;
		EP0BCL = len;	

		break;

	case INA226_READ:
	case INA226_WRITE:
		slave_addr = SETUPDAT[2]; 	// wValue LSBYTE
		reg_addr[0] = SETUPDAT[4];	// wIndex LSBYTE
		if (direction) {
			// Read, Length is 2 always
			dat[0] = 0x0;
			dat[1] = 0x0;
			dat[2] = 0x0;

			i2c_write(slave_addr, 1, &reg_addr[0], 0, &dat[0]);
			status = i2c_read(slave_addr, 2, &EP0BUF[0]);
			EP0BCH = 0;
			EP0BCL = 2;
		} else {
			// Write, Length is 2 always
			EP0BCL = 0;
			while (EP0CS & bmEPBUSY);

			dat[0] = EP0BUF[0];
			dat[1] = EP0BUF[1];
			dat[2] = EP0BUF[2];
			i2c_write(slave_addr, 1, &reg_addr[0], 2, &dat[0]);

			EP0BCH = 0;
			EP0BCL = 0;
		}
		break;

	case PMIC_READ:
	case PMIC_WRITE:
		slave_addr = SETUPDAT[2]; 	// wValue LSBYTE
		reg_addr[0] = SETUPDAT[4];	// wIndex LSBYTE
		if (direction) {
			// Read, Length is 1 always
			dat[0] = 0x0;

			if ((slave_addr == ILLUM_DIGIPOT_SLAVE) && (reg_addr[0] == ILLUM_POWER_DIGIPOT_WRB)) {
				EP0BUF[0] = illum_digipot_shadow;
			} else {
				i2c_write(slave_addr, 1, &reg_addr[0], 0, &dat[0]);
				status = i2c_read(slave_addr, 1, &EP0BUF[0]);
			}
			EP0BCH = 0;
			EP0BCL = 1;
		} else {
			// Write, Length is 1 always
			EP0BCL = 0;
			while (EP0CS & bmEPBUSY);
			dat[0] = EP0BUF[0];
			if (slave_addr == ILLUM_DIGIPOT_SLAVE) {
				if ((reg_addr[0] == ILLUM_POWER_SCALER_REG) || (reg_addr[0] == ILLUM_POWER_MAXVAL_REG)) {
					// Don't allow this write
				} else if(reg_addr[0] == ILLUM_POWER_DIGIPOT_WRB) {
					// Protect writes to wiper B
					maskIllumPowerRegisters(dat[0]);
				} else {
					// Allow writes as usual
					i2c_write(slave_addr, 1, &reg_addr[0], 1, &dat[0]);
				}
			} else {
				i2c_write(slave_addr, 1, &reg_addr[0], 1, &dat[0]);
			}

			EP0BCH = 0;
			EP0BCL = 0;
		}
		break;

	case EN_FSC:
		if (direction) {
			if (PINFLAGSCD == 0x0F)
				// return 1
				EP0BUF[0] = 0x01;
			else
				// return 0
				EP0BUF[0] = 0x00;
			EP0BCH = 0;
			EP0BCL = 1;
		} else {
			// Write, EP0BUF[0] = 1 -> PINFLAGSCD = 0x0F; : PINFLAGSCD = 0x0B
			if (SETUPDAT[2] == 1)
				//PD3 = 1;
				PINFLAGSCD = 0x0F;	// EMPTY
			else
				//PD3 = 0;
				PINFLAGSCD = 0x0B;	// FULL
			EP0BCH = 0;
			EP0BCL = 0;
		}
		break;

	case FX2_FIRMWARE_VERSION:
		if (direction) {
			EP0BUF[0] = 0x00;
			EP0BUF[1] = 0x11;
			EP0BCH = 0;
			EP0BCL = 2;
		}
		break;

	case GET_BOARD_REVISION:
		if (direction) {
			EP0BUF[0] = hw_version;
			EP0BUF[1] = 0x00;
			EP0BCH = 0;
			EP0BCL = 2;
		}
		break;

	case GET_EEPROM_SIZE:
		if (direction) {
			// Read, return EEPROM size
			// Hardcoded 2048 bytes for now
			if (hw_version == 1) {
				if (SETUPDAT[4] == 1) {
					EP0BUF[0] = 0x00;
					EP0BUF[1] = 0x40;
					EP0BUF[2] = 0x00;
					EP0BUF[3] = 0x00;
				} else {
					EP0BUF[0] = 0x00;
					EP0BUF[1] = 0x08;
					EP0BUF[2] = 0x00;
					EP0BUF[3] = 0x00;
				}
			} else if (hw_version == 2) {
				// New rev, 64kB total, 48896 bytes usable, keeping a hole of 256 bytes
				if (SETUPDAT[4] == 1) {
					EP0BUF[0] = 0x00;
					EP0BUF[1] = 0x00;
					EP0BUF[2] = 0x01;
					EP0BUF[3] = 0x00;
				} else {
					EP0BUF[0] = 0x00;
					EP0BUF[1] = 0xBF;
					EP0BUF[2] = 0x00;
					EP0BUF[3] = 0x00;
				}
			}
			EP0BCH = 0;
			EP0BCL = 4;
		}
		break;

	case SET_SERIAL_NUMBER:
	// case GET_SERIAL_NUMBER as well
		if (direction) {
			// Read
			BYTE serialLength;

			eeprom_read(FX2_BOOTEEPROM_SLAVE, SERIAL_NUMBER_LEN_OFFSET, 1, &serialLength);
			if ((serialLength == 0x00) || (serialLength == 0xFF))
				serialLength = SERIAL_NUMBER_LEN_DEFAULT;

			status = eeprom_read(FX2_BOOTEEPROM_SLAVE, SERIAL_NUMBER_OFFSET, serialLength, &dat[0]);

			if (status) {
				for (i = 0; i < serialLength; i++)
					EP0BUF[i] = dat[i];
			} else {
				serialLength = 0;
			}

			EP0BCH = 0;
			EP0BCL = serialLength;
		} else {
			// Write
			WORD checksum;

			EP0BCL = 0;
			while (EP0CS & bmEPBUSY);
			if (len > 32)
				len = 32;
			dat[0] = (len & 0xFF);
			for (i = 0; i < len; i++)
				dat[i+1] = EP0BUF[i];
			for (i = len; i < 32; i++)
				dat[i+1] = 0x00;
			//eeprom_write(0x51, reg_addr[1] + (reg_addr[0] << 8), 16, &dat[0]);
			reg_addr[0] = MSB(SERIAL_NUMBER_LEN_OFFSET);
			reg_addr[1] = LSB(SERIAL_NUMBER_LEN_OFFSET);
			i2c_write(FX2_BOOTEEPROM_SLAVE, 2, &reg_addr[0], 1, &dat[0]);
			delay(10);
			reg_addr[0] = MSB(SERIAL_NUMBER_OFFSET);
			reg_addr[1] = LSB(SERIAL_NUMBER_OFFSET);
			i2c_write(FX2_BOOTEEPROM_SLAVE, 2, &reg_addr[0], 32, &dat[1]);
			delay(10);
			checksum = computeChecksum(&dat[0], len+1);
			dat[33] = (255 - (((checksum & 0xFF) + ((checksum >> 8) & 0xFF)) % 255));
			dat[34] = (255 - (((checksum & 0xFF) + dat[33]) % 255));
			reg_addr[0] = MSB(SERIAL_NUMBER_CHECKSUM_OFFSET);
			reg_addr[1] = LSB(SERIAL_NUMBER_CHECKSUM_OFFSET);
			i2c_write(FX2_BOOTEEPROM_SLAVE, 2, &reg_addr[0], 2, &dat[33]);

			EP0BCH = 0;
			EP0BCL = 0;
		}
		break;

	case ILLUM_DIGIPOT_STORE:
		//slave_addr = ILLUM_DIGIPOT_SLAVE;
		reg_addr[0] = SETUPDAT[4];	// wIndex LSBYTE
		if (direction) {
			// Read
			dat[0] = 0x0;

			i2c_write(ILLUM_DIGIPOT_SLAVE, 1, &reg_addr[0], 0, &dat[0]);
			status = i2c_read(ILLUM_DIGIPOT_SLAVE, 1, &EP0BUF[0]);
			EP0BCH = 0;
			EP0BCL = 1;
		} else {
			// Write
			EP0BCL = 0;
			while (EP0CS & bmEPBUSY);
			dat[0] = EP0BUF[0];
			i2c_write(ILLUM_DIGIPOT_SLAVE, 1, &reg_addr[0], 1, &dat[0]);
			EP0BCH = 0;
			EP0BCL = 0;
		}
		break;
	case TEST_MULTIPLY:
		if (direction) {
			WORD retVal = SETUPDAT[3] * SETUPDAT[2];
			// wValue = MAKEWORD(SETUPDAT[3], SETUPDAT[2]);
			EP0BUF[0] = LSB(retVal);
			EP0BUF[1] = MSB(retVal);

			EP0BCH = 0;
			EP0BCL = 2;
		}
		break;
	case EEPROM_STORE:
	// case EEPROM_RETRIEVE as well
		if (direction) {
			// Read -> Retrieve
			WORD readAddress = getCalibDataStart();
			BYTE transferSize;
			readAddress += reg_addr[1];
			readAddress += (reg_addr[0] << 8);
			i = len;	// reusing i as bytesPending

			while (i) {
				if (i < 64)
					transferSize = i;
				else
					transferSize = 64;

				while (EP0CS & bmEPBUSY);
				status = eeprom_read(FX2_BOOTEEPROM_SLAVE, readAddress, transferSize, &EP0BUF[0]);
				if (status) {
					//
				} else {
					len = 0;
				}
				EP0BCH = 0;
				EP0BCL = transferSize;

				readAddress += transferSize;
				i -= transferSize;
			}
		} else {
			// Write -> Store
			EP0BCL = 0;
			while (EP0CS & bmEPBUSY);

			// Page buffer size is 64 bytes for the 24AA128 and 128 bytes for the 24AA512
			// We restrict to 64 bytes at a time for both
			if (len > 64)
				len = 64;
			for (i = 0; i < len; i++)
				dat[i] = EP0BUF[i];
	
			reg_addr[0] = SETUPDAT[5];	// wIndexH
			reg_addr[1] = SETUPDAT[4];	// wIndexL
			// CalibData always starts on a 256 byte boundary
			//reg_addr[1] += ((CALIB_DATA_START) & 0xFF);

			// FIXME: add a check for exceeding a page boundary
			// offset = MAKEWORD(SETUPDAT[5], SETUPDAT[4]);
			// if (offset + len) >  max size, then ignore the write and return
			// else accept the write
			if ((MAKEWORD(reg_addr[0], reg_addr[1]) + len) <= getCalibDataSize()) {
				reg_addr[0] += MSB(getCalibDataStart());
				i2c_write(FX2_BOOTEEPROM_SLAVE, 2, &reg_addr[0], len, &dat[0]);
				// The EEPROM can take up to 5ms for a page write
				delay(5);
			}
			EP0BCH = 0;
			EP0BCL = 0;
		}
		break;

	case FW_DOWNLOAD_SIZE:
		EP0BCL = 0; // allow pc transfer in
		while (EP0CS & bmEPBUSY); // wait
		GPIFTCB3 = EP0BUF[3];
		GPIFTCB2 = EP0BUF[2];
		GPIFTCB1 = EP0BUF[1];
		GPIFTCB0 = EP0BUF[0];

		break;

	case READ_BACK_TEMP:

		EP0BCH = 0;
		EP0BCL = 4;
		break;

	default:
		handshake = FALSE;
		STALLEP0();
		EP0BCH = 0;
		EP0BCL = 0;
		break;
	}

	if (handshake)
		EP0CS |= bmHSNAK;
	return handshake;
}

#undef __VENDORCOMMAND_C__
/*! @} */