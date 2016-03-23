
	;---------------------------------------------------
	; constants-----------------------------------------
	;---------------------------------------------------
	CY_FX_BOS_DSCR_TYPE				=	15
	CY_FX_DEVICE_CAPB_DSCR_TYPE		=	16
	CY_FX_SS_EP_COMPN_DSCR_TYPE		=	48

	;---------------------------------------------------
	;  Device Capability Type Codes
	;---------------------------------------------------
	CY_FX_WIRELESS_USB_CAPB_TYPE	=	1
	CY_FX_USB2_EXTN_CAPB_TYPE		=	2
	CY_FX_SS_USB_CAPB_TYPE			=	3
	CY_FX_CONTAINER_ID_CAPBD_TYPE	=	4

	;---------------------------------------------------
	;  descriptor types
	;---------------------------------------------------
	CY_FX_USB_DEVICE_DESCR = 0x01				;  Device descriptor
	CY_FX_USB_CONFIG_DESCR = 0x02				;  Configuration descriptor
	CY_FX_USB_STRING_DESCR = 0x03				;  String descriptor
	CY_FX_USB_INTRFC_DESCR = 0x04				;  Interface descriptor
	CY_FX_USB_ENDPNT_DESCR = 0x05				;  Endpoint descriptor
	CY_FX_USB_DEVQUAL_DESCR = 0x06				;  Device Qualifier descriptor
	CY_FX_USB_OTHERSPEED_DESCR = 0x07			;  Other Speed Configuration descriptor
	CY_FX_USB_INTRFC_POWER_DESCR = 0x08			;  Interface power descriptor descriptor
	CY_FX_USB_OTG_DESCR = 0x09					;  OTG descriptor
	CY_FX_BOS_DESCR = 0x0F						;  BOS descriptor
	CY_FX_DEVICE_CAPB_DESCR = 0x10				;  Device Capability descriptor
	CY_FX_USB_HID_DESCR = 0x21					;  HID descriptor
	CY_FX_USB_REPORT_DESCR = 0x22				;  Report descriptor
	CY_FX_SS_EP_COMPN_DESCR = 0x30				;  Endpoint companion descriptor

	;---------------------------------------------------
	;  EP types
	;---------------------------------------------------
	CY_FX_USB_EP_CONTROL = 0					;  Control Endpoint Type
	CY_FX_USB_EP_ISO = 1						;  Isochronous Endpoint Type
	CY_FX_USB_EP_BULK = 2						;  Bulk Endpoint Type
	CY_FX_USB_EP_INTR = 3						;  Interrupt Endpoint Type
	CY_FX_EP_BULK_VIDEO = 0x82					;  ISOC endpoint for data streaming
	CY_FX_INTF_ASSN_DSCR_TYPE = 0x0B			;  IAD type
	CY_FX_VS_COLORFORMAT = 0x0D					;  color format
	CY_FX_EP_CONTROL_STATUS = 0x81				;  need to setup EP1 as interrupt ?? TBD

	;---------------------------------------------------
	.module DEV_DSCR
	;---------------------------------------------------
	; These need to be in code memory.  If
	; they aren't you'll have to manually copy them somewhere
	; in code memory otherwise SUDPTRH:L don't work right
	;---------------------------------------------------

	.globl	_dev_dscr_uvc ,_dev_qual_dscr_uvc, _fullspd_dscr_uvc, _highspd_dscr_uvc, _dev_strings, _string0, _string1,_string2
	.globl	_dev_dscr_fwld ,_dev_qual_dscr_fwld, _fullspd_dscr_fwld, _highspd_dscr_fwld

	.area	DSCR_AREA	(CODE)
	.even


;---------------------------------------------------
;  Standard Device Descriptor
;---------------------------------------------------
_dev_dscr_uvc:
		.db 0x12							;  Descriptor Size
		.db CY_FX_USB_DEVICE_DESCR			;  Device Descriptor Type
		.dw 0x0002							;  USB 2.0
		.db 0xEF							;  Device Class
		.db 0x02							;  Device Sub-class
		.db 0x01							;  Device protocol
		.db 0x40							;  Maxpacket size for EP0 : 64 bytes
		.dw 0xB404							;  Vendor ID  0x04B4
		.dw 0x1386							;  Product ID  0x8613
		.dw 0x0000							;  Device release number
		.db 0x01							;  Manufacture string index
		.db 0x02							;  Product string index
		.db 0x03							;  Serial number string index
		.db 0x01							;  Number of configurations
_dev_dscr_uvc_end:


;---------------------------------------------------
;  Standard Device Qualifier Descriptor
;---------------------------------------------------
_dev_qual_dscr_uvc:
		.db 0x0A							;  Descriptor Size
		.db CY_FX_USB_DEVQUAL_DESCR			;  Device Qualifier Descriptor Type
		.dw 0x0002							;  USB 2.0
		.db 0xEF							;  Device Class
		.db 0x02							;  Device Sub-class
		.db 0x01							;  Device protocol
		.db 0x40							;  Maxpacket size for EP0 : 64 bytes
		.db 0x01							;  Number of configurations
		.db 0x00							;  Reserved
_dev_qual_dscr_uvc_end:


;---------------------------------------------------
;  Standard Full Speed Configuration Descriptor
;---------------------------------------------------
_fullspd_dscr_uvc :
		;  Configuration Descriptor Type
		.db 0x09							;  Descriptor Size
		.db CY_FX_USB_CONFIG_DESCR			;  Configuration Descriptor Type
		.dw 0x0900							;  Length of this descriptor and all sub descriptors
		.db 0x00							;  Number of interfaces
		.db 0x01							;  Configuration number
		.db 0x00							;  Configuration string index
		.db 0x80							;  Config characteristics - Bus powered
		.db 0x32							;  Max power consumption of device (in 2mA unit) : 100mA
_fullspd_dscr_uvc_end:

		.db 0x00							;  word align high speed

;---------------------------------------------------
_highspd_dscr_uvc:
;---------------------------------------------------

		;---------------------------------------------------
		;  Configuration Descriptor Type
		;---------------------------------------------------
		.db 0x09											;  Descriptor Size
		.db CY_FX_USB_CONFIG_DESCR							;  Configuration Descriptor Type
		.db	(_highspd_dscr_uvc_end-_highspd_dscr_uvc) % 256 ;  total length of config lsb
		.db	(_highspd_dscr_uvc_end-_highspd_dscr_uvc) / 256 ;  total length of config msb
		.db 0x02											;  Number of interfaces
		.db 0x01											;  Configuration number
		.db 0x00											;  Configuration string index
		.db 0x80											;  Config characteristics - Bus powered
		.db 0xFA											;  Max power consumption of device (in 2mA units) : 500 mA

		;---------------------------------------------------
		;  Interface Association Descriptor
		;---------------------------------------------------
		.db 0x08							;  Descriptor Size
		.db CY_FX_INTF_ASSN_DSCR_TYPE		;  Interface Association Descr Type: 11
		.db 0x00							;  I/f number of first VideoControl i/f
		.db 0x02							;  Number of Video i/f
		.db 0x0E							;  CC_VIDEO : Video i/f class code
		.db 0x03							;  SC_VIDEO_INTERFACE_COLLECTION : Subclass code
		.db 0x00							;  Protocol : Not used
		.db 0x00							;  String desc index for interface

		;---------------------------------------------------
		;  Standard Video Control Interface Descriptor
		;---------------------------------------------------
		.db 0x09							;  Descriptor size
		.db CY_FX_USB_INTRFC_DESCR			;  Interface Descriptor type
		.db 0x00							;  Interface number
		.db 0x00							;  Alternate setting number
		.db 0x00							;  Number of end points
		.db 0x0E							;  CC_VIDEO : Interface class
		.db 0x01							;  CC_VIDEOCONTROL : Interface sub class
		.db 0x00							;  Interface protocol code
		.db 0x02							;  Interface descriptor string index

		;---------------------------------------------------
		;  Class specific VC Interface Header Descriptor
		;---------------------------------------------------
		.db 0x0D							;  Descriptor size
		.db 0x24							;  Class Specific I/f Header Descriptor type
		.db 0x01							;  Descriptor Sub type : VC_HEADER
		.dw 0x0001							;  Revision of class spec : 1.0
		.dw 0x5000							;  Total Size of class specific descriptors (till Output terminal)
		.dw 0x006C							;  Clock frequency : 48MHz(Deprecated)
		.dw 0xDC02
		.db 0x01							;  Number of streaming interfaces
		.db 0x01							;  Video streaming I/f 1 belongs to VC i/f

		;---------------------------------------------------
		;  Input (Camera) Terminal Descriptor
		;---------------------------------------------------
		.db 0x12							;  Descriptor size
		.db 0x24							;  Class specific interface desc type
		.db 0x02							;  Input Terminal Descriptor type
		.db 0x01							;  ID of this terminal
		.dw 0x0102							;  Camera terminal type
		.db 0x00							;  No association terminal
		.db 0x00							;  String desc index : Not used
		.dw 0x0000							;  No optical zoom supported
		.dw 0x0000							;  No optical zoom supported
		.dw 0x0000							;  No optical zoom supported
		.db 0x03							;  Size of controls field for this terminal : 3 bytes
		.dw 0x0000							;  No controls supported
		.db 0x00

		;---------------------------------------------------
		;  Processing Unit Descriptor
		;---------------------------------------------------
		.db 0x0C							;  Descriptor size
		.db 0x24							;  Class specific interface desc type
		.db 0x05							;  Processing Unit Descriptor type
		.db 0x02							;  ID of this terminal
		.db 0x01							;  Source ID : 1 : Conencted to input terminal
		.dw 0x0040							;  Digital multiplier
		.db 0x03							;  Size of controls field for this terminal : 3 bytes
		.dw 0x0000							;  No controls supported
		.db 0x00
		.db 0x00							;  String desc index : Not used

		;---------------------------------------------------
		;  Extension Unit Descriptor
		;---------------------------------------------------
		.db 0x1C							;  Descriptor size
		.db 0x24							;  Class specific interface desc type
		.db 0x06							;  Extension Unit Descriptor type
		.db 0x03							;  ID of this terminal
		.dw 0xDD88							;  16 byte GUID
		.dw 0x0F8A
		.dw 0x1CBA
		.dw 0x4954
		.dw 0x8A25
		.dw 0xF787
		.dw 0x5967
		.dw 0xF0F7
		.db 0x0A							;  Number of controls in this terminal
		.db 0x01							;  Number of input pins in this terminal
		.db 0x02							;  Source ID : 2 : Connected to Proc Unit
		.db 0x03							;  Size of controls field for this terminal : 3 bytes
		.dw 0xFF03							;  No controls supported
		.db 0x00
		.db 0x01							;  String desc index : Not used

		;---------------------------------------------------
		;  Output Terminal Descriptor
		;---------------------------------------------------
		.db 0x09							;  Descriptor size
		.db 0x24							;  Class specific interface desc type
		.db 0x03							;  Output Terminal Descriptor type
		.db 0x04							;  ID of this terminal
		.dw 0x0101							;  USB Streaming terminal type
		.db 0x00							;  No association terminal
		.db 0x03							;  Source ID : 3 : Connected to Extn Unit
		.db 0x00							;  String desc index : Not used

		;---------------------------------------------------
		;  Standard Video Streaming Interface Descriptor (Alternate Setting 0)
		;---------------------------------------------------
		.db 0x09							;  Descriptor size
		.db CY_FX_USB_INTRFC_DESCR			;  Interface Descriptor type
		.db 0x01							;  Interface number
		.db 0x00							;  Alternate setting number
		.db 0x00							;  Number of end points
		.db 0x0E							;  Interface class : CC_VIDEO
		.db 0x02							;  Interface sub class : CC_VIDEOSTREAMING
		.db 0x00							;  Interface protocol code : Undefined
		.db 0x00							;  Interface descriptor string index

		;---------------------------------------------------
		;  Class-specific Video Streaming Input Header Descriptor
		;---------------------------------------------------
		.db 0x0E							;  Descriptor size
		.db 0x24							;  Class-specific VS I/f Type
		.db 0x01							;  Descriptotor Subtype : Input Header
		.db 0x01							;  1 format desciptor follows
		.dw 0x4700							;  Total size of Class specific VS descr
		.db CY_FX_EP_BULK_VIDEO				;  EP address for BULK video data
		.db 0x00							;  No dynamic format change supported
		.db 0x04							;  Output terminal ID : 4
		.db 0x00							;  Still image capture method 1 supported
		.db 0x00							;  Hardware trigger NOT supported
		.db 0x00							;  Hardware to initiate still image capture NOT supported
		.db 0x01							;  Size of controls field : 1 byte
		.db 0x00							;  D2 : Compression quality supported

		;---------------------------------------------------
		;  Class specific Uncompressed VS format descriptor
		;---------------------------------------------------
		.db 0x1B							;  Descriptor size
		.db 0x24							;  Class-specific VS I/f Type
		.db 0x04							;  Subtype : uncompressed format I/F
		.db 0x01							;  Format desciptor index
		.db 0x0B							;  Number of frame descriptor followed
		.dw 0x5955							;  GUID used to identify streaming-encoding format: YUY2
		.dw 0x5932
		.dw 0x0000
		.dw 0x1000
		.dw 0x8000
		.dw 0x00AA
		.dw 0x0038
		.dw 0x9B71
		.db 0x10							;  Number of bits per pixel
		.db 0x01							;  Optimum Frame Index for this stream: 1
		.db 0x00							;  X dimension of the picture aspect ratio; Non-interlaced
		.db 0x00							;  Y dimension of the pictuer aspect ratio: Non-interlaced
		.db 0x00							;  Interlace Flags: Progressive scanning  no interlace
		.db 0x00							;  duplication of the video stream restriction: 0 - no restriction

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 1: 320x240 raw mode 6 quad (960*480)
		;---------------------------------------------------
		.db 0x1E							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x01							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0xC003							;  Width in pixel
		.dw 0xe001							;  Height in pixel
		.dw 0x0000							;  Min bit rate bits/s.
		.dw 0x6504
		.dw 0x0000							;  Max bit rate bits/s.
		.dw 0x6504
		.dw 0x0010							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0E00
		.dw 0xD012
		.dw 0x1300
		.db 0x01
		.dw 0xD012							;  125.00 ms == 8 fps
		.dw 0x1300

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 2: 320x240 x4 Bytes-per-pixel (Default)
		;---------------------------------------------------
		.db 0x1E							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x02							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0x8002							;  Width in pixel
		.dw 0xf000							;  Height in pixel
		.dw 0x0080							;  Min bit rate bits/s.
		.dw 0xA903
		.dw 0x0080							;  Max bit rate bits/s.
		.dw 0xA903
		.dw 0x00B0							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0400
		.dw 0x801A
		.dw 0x0600
		.db 0x01
		.dw 0x801A							;  40.000 ms == 25 fps
		.dw 0x0600

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 3: 320x240 raw mode 4 quad (640x480)
		;---------------------------------------------------
		.db 0x1E							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x03							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0x8002							;  Width in pixel
		.dw 0xE001							;  Height in pixel
		.dw 0x0080							;  Min bit rate bits/s.
		.dw 0xA903
		.dw 0x0080							;  Max bit rate bits/s.
		.dw 0xA903
		.dw 0x0060							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0900
		.dw 0x0035
		.dw 0x0C00
		.db 0x01
		.dw 0x0035							;  80.000 ms == 12.5 fps
		.dw 0x0C00

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 4: 320x120 normal, or 320x240 2 bytes mode
		;---------------------------------------------------
		.db 0x22							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x04							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0x4001							;  Width in pixel
		.dw 0xF000							;  Height in pixel
		.dw 0x0080							;  Min bit rate bits/s.
		.dw 0x3202
		.dw 0x0080							;  Max bit rate bits/s.
		.dw 0xA903
		.dw 0x0058							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0200
		.dw 0x1516
		.dw 0x0500
		.db 0x02
		.dw 0x400D							;  20.000 ms == 50 fps
		.dw 0x0300
		.dw 0x1516							;  33.333 ms == 30 fps
		.dw 0x0500

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 5: 160x120 QQVGA mode (320x120)
		;---------------------------------------------------
		.db 0x22							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x05							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0x4001							;  Width in pixel
		.dw 0x7800							;  Height in pixel
		.dw 0x0040							;  Min bit rate bits/s.
		.dw 0x1901
		.dw 0x0080							;  Max bit rate bits/s.
		.dw 0xA903
		.dw 0x002C							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0100
		.dw 0x1516
		.dw 0x0500
		.db 0x02
		.dw 0xA086							;  10.000 ms == 100 fps
		.dw 0x0100
		.dw 0x1516							;  33.333 ms == 30 fps
		.dw 0x0500

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 6: 160x60 4 Bytes mode(160x120)
		;---------------------------------------------------
		.db 0x1E							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x06							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0xA000							;  Width in pixel
		.dw 0x7800							;  Height in pixel
		.dw 0x00A0							;  Min bit rate bits/s.
		.dw 0x8C00
		.dw 0x00A0							;  Max bit rate bits/s.
		.dw 0x8C00
		.dw 0x0096							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0000
		.dw 0x1516
		.dw 0x0500
		.db 0x01
		.dw 0x1516							;  33.333 ms == 30 fps
		.dw 0x0500

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 7: 80x60 (QQQVGA 4Bytes mode) (160x60)
		;---------------------------------------------------
		.db 0x1E							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x07							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0xA000							;  Width in pixel
		.dw 0x3C00							;  Height in pixel
		.dw 0x0050							;  Min bit rate bits/s.
		.dw 0x4600
		.dw 0x0050							;  Max bit rate bits/s.
		.dw 0x4600
		.dw 0x004B							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0000
		.dw 0x1516
		.dw 0x0500
		.db 0x01
		.dw 0x204E							;  02.0000 == 500 fps; 33.333 ms == 30 fps
		.dw 0x0000

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 8: 80x30 (80x60)
		;---------------------------------------------------
		.db 0x1E							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x08							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0x5000							;  Width in pixel
		.dw 0x3C00							;  Height in pixel
		.dw 0x0028							;  Min bit rate bits/s.
		.dw 0x2300
		.dw 0x0028							;  Max bit rate bits/s.
		.dw 0x2300
		.dw 0x8025							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0000
		.dw 0x1516
		.dw 0x0500
		.db 0x01
		.dw 0x1516							;  33.333 ms == 30 fps
		.dw 0x0500

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 9: 40x30 (80x30)
		;---------------------------------------------------
		.db 0x1E							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x09							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0x5000							;  Width in pixel
		.dw 0x1e00							;  Height in pixel
		.dw 0x0094							;  Min bit rate bits/s.
		.dw 0x1100
		.dw 0x0094							;  Max bit rate bits/s.
		.dw 0x1100
		.dw 0xC012							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0000
		.dw 0x1516
		.dw 0x0500
		.db 0x01
		.dw 0x1516							;  33.333 ms == 30 fps
		.dw 0x0500

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 10: 40x15 (40x30)
		;---------------------------------------------------
		.db 0x1E							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x0A							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0x2800							;  Width in pixel
		.dw 0x1E00							;  Height in pixel
		.dw 0x00CA							;  Min bit rate bits/s.
		.dw 0x0800
		.dw 0x00CA							;  Max bit rate bits/s.
		.dw 0x0800
		.dw 0x6009							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0000
		.dw 0x1516
		.dw 0x0500
		.db 0x01
		.dw 0x1516							;  33.333 ms == 30 fps
		.dw 0x0500

		;---------------------------------------------------
		;  Class specific Uncompressed VS frame descriptor
		;  Index 11: 320x240x4 + 96 (396*388)
		;---------------------------------------------------
		.db 0x1E							;  Descriptor size
		.db 0x24							;  Descriptor type
		.db 0x05							;  Subtype: uncompressed frame I/F
		.db 0x0B							;  Frame Descriptor Index
		.db 0x00							;  Still image capture method 1 supported  fixed frame rate
		.dw 0x8C01							;  Width in pixel
		.dw 0x8401							;  Height in pixel
		.dw 0x00CA							;  Min bit rate bits/s.
		.dw 0x0800
		.dw 0x00CA							;  Max bit rate bits/s.
		.dw 0x0800
		.dw 0x60B0							;  Maximum video or still frame size in bytes(Deprecated)
		.dw 0x0400
		.dw 0x801A
		.dw 0x0600
		.db 0x01
		.dw 0x801A							;  40.000 ms == 25 fps
		.dw 0x0600

		;---------------------------------------------------
		;  Class specific Uncompressed VS color descriptor
		;---------------------------------------------------
		.db 0x06							;  size
		.db 0x24							;  Class-specific VS I/f Type
		.db CY_FX_VS_COLORFORMAT
		.db 0x01							;  RGB format
		.db 0x01
		.db 0x04

		;---------------------------------------------------
		;  Standard Video Streaming Interface Descriptor (Alternate Setting 1)
		;---------------------------------------------------
		.db 0x09							;  Descriptor size
		.db CY_FX_USB_INTRFC_DESCR			;  Interface Descriptor type
		.db 0x01							;  Interface number
		.db 0x01							;  Alternate setting number
		.db 0x01							;  Number of end points
		.db 0x0E							;  Interface class : CC_VIDEO
		.db 0x02							;  Interface sub class : CC_VIDEOSTREAMING
		.db 0x00							;  Interface protocol code : Undefined
		.db 0x00

		;---------------------------------------------------
		;  Endpoint Descriptor for BULK Streaming Video Data
		;---------------------------------------------------
		.db 0x07							;  Descriptor size
		.db CY_FX_USB_ENDPNT_DESCR			;  Endpoint Descriptor Type
		.db CY_FX_EP_BULK_VIDEO				;  Endpoint address and description
		.db 0x05							;  BULK End point
		.dw 0x0004							;  EP MaxPcktSize: 2*1024B
		.db 0x01							;  Servicing interval for data transfers


;---------------------------------------------------
;  Interface descriptor string index
;---------------------------------------------------

_highspd_dscr_uvc_end:


_dev_strings:
_string0:
		.db 0x04							;  Descriptor Size
		.db CY_FX_USB_STRING_DESCR			;  Device Descriptor Type
		.dw 0x0904							;  Language ID supported
_string0_end:

_string1:
		.db 0x06							;  Descriptor Size
		.db CY_FX_USB_STRING_DESCR			;  Device Descriptor Type
		.ascii 'T'
		.db 0x00
		.ascii 'I'
		.db 0x00
_string1_end:

_string2:
		;.db 0x10							;  Descriptor Size
		.db _string2_end - _string2
		.db CY_FX_USB_STRING_DESCR			;  Device Descriptor Type
		.ascii 'C'
		.db 0x00
		.ascii 'D'
		.db 0x00
		.ascii 'K'
		.db 0x00
		.ascii '-'
		.db 0x00
		.ascii 'O'
		.db 0x00
		.ascii 'P'
		.db 0x00
		.ascii 'T'
		.db 0x00
		.ascii '8'
		.db 0x00
		.ascii '2'
		.db 0x00
		.ascii '4'
		.db 0x00
		.ascii '1'
		.db 0x00

_string2_end:

_serialnumber_0::
_string3:
		.db _string3_end - _string3
		.db	CY_FX_USB_STRING_DESCR
		.ascii 'S'
		.db 0x00
		.ascii 'e'
		.db 0x00
		.ascii 'r'
		.db 0x00
		.ascii 'i'
		.db 0x00
		.ascii 'a'
		.db 0x00
		.ascii 'l'
		.db 0x00
		.ascii '_'
		.db 0x00
		.ascii 'N'
		.db 0x00
		.ascii 'o'
		.db 0x00
		.ascii '.'
		.db 0x00
		.ascii '_'
		.db 0x00
		.ascii 'P'
		.db 0x00
		.ascii 'l'
		.db 0x00
		.ascii 'a'
		.db 0x00
		.ascii 'c'
		.db 0x00
		.ascii 'e'
		.db 0x00
		.ascii 'h'
		.db 0x00
		.ascii 'o'
		.db 0x00
		.ascii 'l'
		.db 0x00
		.ascii 'd'
		.db 0x00
		.ascii 'e'
		.db 0x00
		.ascii 'r'
		.db 0x00
		
_string3_end:
		; Placeholders for a longer serial number
		.dw 0x0000
		.dw 0x0000
		.dw 0x0000
		.dw 0x0000
		.dw 0x0000
		.dw 0x0000
		.dw 0x0000
		.dw 0x0000
		.dw 0x0000
		.dw 0x0000

_dev_strings_end:

.dw 0x0000				;  just in case someone passes an index higher than the end to the firmware

;---------------------------------------------------
; ------------- second set of descriptors , Firmware load
;---------------------------------------------------





;---------------------------------------------------
;  Standard Device Descriptor
;---------------------------------------------------
_dev_dscr_fwld:
	.db	dev_dscr_fwld_end-_dev_dscr_fwld	;  len
	.db	CY_FX_USB_DEVICE_DESCR				;  type
	.dw	0x0002								;  usb 2.0
	.db	0x00  								;  class (vendor specific)
	.db	0x00								;  subclass (vendor specific)
	.db	0x00								;  protocol (vendor specific)
	.db	64									;  packet size (ep0)
	.dw	0xB404								;  vendor id 0x04B4
	.dw	0x1386								;  product id 0x8613
	.dw	0x0100								;  version id
	.db	1									;  manufacturure str idx
	.db	2									;  product str idx
	.db	3									;  serial str idx
	.db	1									;  n configurations
dev_dscr_fwld_end:


;---------------------------------------------------
;  Standard Device Qualifier Descriptor
;---------------------------------------------------
_dev_qual_dscr_fwld:
	.db	dev_qual_dscr_fwld_end-_dev_qual_dscr_fwld
	.db	CY_FX_USB_DEVQUAL_DESCR
	.dw	0x0002								;  usb 2.0
	.db	0x00
	.db	0x00
	.db	0x00
	.db	64									;  max packet
	.db	1									;  n configs
	.db	0									;  extra reserved byte
dev_qual_dscr_fwld_end:


;---------------------------------------------------
;  High Speed Descriptor
;---------------------------------------------------
_highspd_dscr_fwld:
	.db	highspd_dscr_fwld_end-_highspd_dscr_fwld				;  dscr len
	.db	CY_FX_USB_CONFIG_DESCR
	.db	(highspd_dscr_fwld_real_end-_highspd_dscr_fwld) % 256 	;  total length of config lsb
	.db	(highspd_dscr_fwld_real_end-_highspd_dscr_fwld) / 256 	;  total length of config msb
	.db	1														;  n interfaces
	.db	1														;  config number
	.db	0														;  config string
	.db	0x80													;  attrs = bus powered, no wakeup
	.db	0x32													;  max power = 100ma
highspd_dscr_fwld_end:

;---------------------------------------------------
;  Interface Descriptor
;---------------------------------------------------
	.db	0x09
	.db	CY_FX_USB_INTRFC_DESCR
	.db	0					;  index
	.db	0					;  alt setting idx
	.db	1					;  n endpoints
	.db	0x00				;  class
	.db	0x00
	.db	0x00
	.db	2					;  string index

; endpoint 6 out
	.db	0x07
	.db	CY_FX_USB_ENDPNT_DESCR
	.db	0x82				;  ep6 dir=in and address
	.db	CY_FX_USB_EP_BULK	;  type
	.db	0x00				;  max packet LSB
	.db	0x02				;  max packet size=512 bytes
	.db	0x00				;  polling interval

highspd_dscr_fwld_real_end:

	.even

;---------------------------------------------------
;  Full Speed Descriptor
;---------------------------------------------------
_fullspd_dscr_fwld:
	.db	fullspd_dscr_fwld_end-_fullspd_dscr_fwld				;  dscr len
	.db	CY_FX_USB_CONFIG_DESCR
	.db	(fullspd_dscr_fwld_real_end-_fullspd_dscr_fwld) % 256	;  total length of config lsb
	.db	(fullspd_dscr_fwld_real_end-_fullspd_dscr_fwld) / 256	;  total length of config msb
	.db	1								 						;  n interfaces
	.db	1								 						;  config number
	.db	0								 						;  config string
	.db	0x80													;  attrs = bus powered, no wakeup
	.db	0x32													;  max power = 100ma
fullspd_dscr_fwld_end:


;---------------------------------------------------
;  Interface Descriptor
;---------------------------------------------------
	.db	0x09
	.db	CY_FX_USB_INTRFC_DESCR
	.db	0					;  index
	.db	0					;  alt setting idx
	.db	2					;  n endpoints
	.db	0x00				;  class
	.db	0x00
	.db	0x00
	.db	2					;  string index

; endpoint 6 out
	.db	0x07
	.db	CY_FX_USB_ENDPNT_DESCR
	.db	0x82				;  ep6 dir=in and address
	.db	CY_FX_USB_EP_BULK	;  type
	.db	0x00				;  max packet LSB
	.db	0x02				;  max packet size=512 bytes
	.db	0x00				;  polling interval

fullspd_dscr_fwld_real_end:

.even

	.dw 0x0000				;  just in case someone passes an index higher than the end to the firmware

