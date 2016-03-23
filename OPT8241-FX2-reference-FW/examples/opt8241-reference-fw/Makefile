#==============================================================================
#
#  FX2 CDK-OPT8241 makefile
#
#  This makefile makes *.ihx, *.hex, *.iic from the FX2 source structure.  
#  The output are in either the 'debug' or 'release' directory.
#
#  To use this makefile, the following environment variables should be 
#  defined:
#
#  	FX2_DIR			FX2 home directory
#	PROJ_DIR 		Project source directory	
#	FX2_INC_DIR 		FX2 include directory	
#	OUTPUT			Output file name without file extension	
#	DOXY_DIR		Doxygen output directory	
#	DOXYFILE		Doxygen config file name (under DOXY_DIR) 	
#
#  Edit the following tags in the Doxyfile to reflect your specific paths:
#
#	OUTPUT_DIRECTORY
#	INPUT
#
#
#  The possible make targets:
#
#  	$ make			- Generic make
#  	$ make all		- Make everything (except dox)
#	$ make clean		- General file cleaning
#	$ make super_clean	- Exhaustive file cleaning
#  	$ make dox		- Make doxygen HTML
#  	$ make dox_clean	- Delete doxygen HTML
#
#  SDCC build tools can be installed from:
#
#	http://sourceforge.net/projects/sdcc/files/sdcc-win32/2.9.0/
#
#  FX2 tools can be downloaded from:
#
#  	http://www.cypress.com/go/CY3687
#
#  Doxygen can be downloaded from:
#
#  	http://www.doxygen.org
#
#  Doxygen needs GraphViz, which can be downloaded from:
#
#  	http://www.graphviz.org
#
#  GNU Make for Windows can be downloaded from:
#
# 	http://gnuwin32.sourceforge.net/packages/make.htm
#
#
#  Copyright(c) 2013-15 Texas Instruments Incorporated.  All Rights Reserved.
#  TI makes NO WARRANTY as to software products, which are supplied "AS-IS".
#
#==============================================================================

#---------------------------------------------------------
#  	Build Type - Select One 
#---------------------------------------------------------
#BUILD		= debug
BUILD		= release

#SHELL
SHELL=C:/Windows/System32/cmd.exe

#---------------------------------------------------------
#  	Project Directories - modify as required
#---------------------------------------------------------
FX2_DIR		:= D:\Code\OPT8241-FX2-reference-FW
PROJ_DIR 	:= $(FX2_DIR)/examples/opt8241-reference-fw
FX2_INC_DIR 	:= $(FX2_DIR)/include
INCLUDES 	:= -I $(FX2_INC_DIR) -I $(PROJ_DIR)
OUT_DIR 	:= $(PROJ_DIR)/$(BUILD)
OUTPUT		:= fx2_main
LIBS 		:= $(FX2_DIR)/lib/fx2.lib
DOXY_DIR	:= $(PROJ_DIR)/dox
DOXYFILE	:= $(DOXY_DIR)/Doxyfile

#---------------------------------------------------------
#  	C Modules - add as required
#---------------------------------------------------------
CC_MODULES	:=						\
		$(PROJ_DIR)/main.c 				\
		$(PROJ_DIR)/pmic.c		\
		$(PROJ_DIR)/handler.c		\
		$(PROJ_DIR)/handle_isr.c			\
		$(PROJ_DIR)/common.c				\
		$(PROJ_DIR)/gpif_fifowrite_ctl2_data.c	\
		$(PROJ_DIR)/globals.c	\
		$(PROJ_DIR)/vendorcommand.c \
		$(PROJ_DIR)/fifo_gpif_config.c	

#$(PROJ_DIR)/uvc.c	\
#---------------------------------------------------------
#  	Assembly Modules - add as required
#---------------------------------------------------------
AS_MODULES 	:= 						\
		$(PROJ_DIR)/dscr.a51

#---------------------------------------------------------
#  	FX2 Build Tools	
#---------------------------------------------------------
AS		:= asx8051
CC 		:= sdcc
LD		:= sdcc -l
AR		:= sdcclib
PACKIHX		:= packihx
HEX2BIX		:= hex2bix 
DOXYGEN		:= doxygen

#---------------------------------------------------------
#  	AS Flags	
#---------------------------------------------------------
ASFLAGS		:= -logs

#---------------------------------------------------------
#  	CC Flags	
#---------------------------------------------------------
CCFLAGS		:= -mmcs51 $(INCLUDES) -c

#---------------------------------------------------------
#  	LD Flags	
#---------------------------------------------------------
LDFLAGS		:= --code-size 0x3900 --xram-size 0x0200 --xram-loc 0xE000 
LDFLAGS		+= -Wl"-b DSCR_AREA=0x3900" -Wl"-b INT2JT=0x3f00" -Wl"-b INT4JT=0x3e00"

#---------------------------------------------------------
#  	Set the shell type	
#---------------------------------------------------------
SHELL		:= /bin/sh 
RM		:= rm
CP		:= cp
MKDIR		:= mkdir
MAKE		:= make

#---------------------------------------------------------
#  	Set the Suffixes	
#---------------------------------------------------------
.SUFFIX:	.a51 .c .rel .ihx .hex .iic .lib

#---------------------------------------------------------
#  	Build Targets 
#---------------------------------------------------------
all : $(OUT_DIR)/$(OUTPUT).hex $(OUT_DIR)	\
	$(OUT_DIR)/$(OUTPUT).iic $(OUT_DIR)

#$(OUT_DIR) :
#	$(MKDIR) $@

$(OUT_DIR)/$(OUTPUT).ihx : $(AS_MODULES:.a51=.rel) $(CC_MODULES:.c=.rel) $(OUT_DIR)
	$(LD) $(LIBS) $(LDFLAGS) $(CC_MODULES:.c=.rel) $(AS_MODULES:.a51=.rel) \
		-o $(OUT_DIR)/$(OUTPUT).ihx

$(OUT_DIR)/$(OUTPUT).hex : $(OUT_DIR)/$(OUTPUT).ihx
	$(PACKIHX) $(OUT_DIR)/$(OUTPUT).ihx > $(OUT_DIR)/$(OUTPUT).hex

$(OUT_DIR)/$(OUTPUT).iic : $(OUT_DIR)/$(OUTPUT).hex
	$(HEX2BIX) -m 0x8000 -i -c 0x00 -f 0xC2 -o $(OUT_DIR)\$(OUTPUT).iic $(OUT_DIR)\$(OUTPUT).hex

%.rel : %.c
	$(CC) $(CCFLAGS) -o $@ $<	

%.rel : %.a51
	$(AS) $(ASFLAGS) $<

#---------------------------------------------------------
#  	Clean Up 
#---------------------------------------------------------
clean : $(OUT_DIR)
	-$(RM) $(OUT_DIR)/*.hex
	-$(RM) $(OUT_DIR)/*.ihx
	-$(RM) $(OUT_DIR)/*.iic
	-$(RM) $(OUT_DIR)/*.map
	-$(RM) $(OUT_DIR)/*.mem
	-$(RM) $(OUT_DIR)/*.lnk

.PHONY: super_clean
super_clean :
	-$(MAKE) clean
	-$(RM) $(PROJ_DIR)/*.rel
	-$(RM) $(PROJ_DIR)/*.rst
	-$(RM) $(PROJ_DIR)/*.lst
	-$(RM) $(PROJ_DIR)/*.sym
	-$(RM) $(PROJ_DIR)/*.asm
	-$(RM) $(PROJ_DIR)/*.c~
	-$(RM) $(PROJ_DIR)/*.h~
	-$(RM) $(PROJ_DIR)/*.a51~
	-$(RM) $(PROJ_DIR)/Makefile~


#---------------------------------------------------------
#  	Doxygen 
#---------------------------------------------------------
.PHONY: dox
dox:
	$(DOXYGEN) $(DOXYFILE)


.PHONY:	dox_clean
dox_clean:
	-$(RM) $(DOXY_DIR)/html/*
	-$(RM) $(DOXY_DIR)/html/search/*
