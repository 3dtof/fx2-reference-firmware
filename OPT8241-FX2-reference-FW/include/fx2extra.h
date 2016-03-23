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

#ifndef FX2_EXTRA_H
#define FX2_EXTRA_H

// TODO: These (or some defines like it) should go into fx2lib
#define EPCFG_DIRECTION_IN  bmBIT6
#define EPCFG_DIRECTION_OUT 0
#define EPCFG_TYPE_ISO      bmBIT4
#define EPCFG_TYPE_BULK     bmBIT5
#define EPCFG_TYPE_INT      bmBIT5 | bmBIT4
#define EPCFG_BUFFER_QUAD   0
#define EPCFG_BUFFER_DOUBLE bmBIT1
#define EPCFG_BUFFER_TRIPLE bmBIT3 | bmBIT2

#endif
