// Copyright (C) 2010 Trygve Laugstøl <trygvis@inamo.no>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef FX2_BITS_H
#define FX2_BITS_H

__xdata __at 0x0088 union {
    struct {
        unsigned char IT0:1;
        unsigned char IE0:1;
        unsigned char IT1:1;
        unsigned char IE1:1;
        unsigned char TR0:1;
        unsigned char TF0:1;
        unsigned char TR1:1;
        unsigned char TF1:1;
    };
} bTCON;

__xdata __at 0x0089 union {
    struct {
        unsigned char M_0:2;
        unsigned char C_T0:1;
        unsigned char GATE0:1;
        unsigned char M_1:2;
        unsigned char C_T1:1;
        unsigned char GATE1:1;
    };
} bTMOD;

__xdata __at 0x008E union {
    struct {
        unsigned char MD:3;
        unsigned char T0M:3;
        unsigned char T1M:3;
        unsigned char T2M:3;
    };
} bCKCON;

__xdata __at 0xE600 union {
    struct {
        unsigned char :1;
        unsigned char CLKOE:1;
        unsigned char CLKINV:1;
        unsigned char CLKSPD:2;
        unsigned char PORTCSTB:1;
        unsigned char :1;
        unsigned char :1;
    };
} bCPUCS;

#endif
