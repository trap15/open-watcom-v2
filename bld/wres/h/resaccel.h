/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef RESACCEL_INCLUDED
#define RESACCEL_INCLUDED

typedef struct AccelTableEntry32 {
    uint_16     Flags;
    uint_16     Ascii;
    uint_16     Id;
    uint_16     Unknown;            /* I don't know what this field is for. */
} AccelTableEntry32;                /* MS makes it 0. (padding?) */

#define AccelTableEntry32_SIZE  (4 * sizeof( uint_16 ))

typedef struct AccelTableEntry {
    uint_8      Flags;
    uint_16     Ascii;
    uint_16     Id;
} AccelTableEntry;

#define AccelTableEntry_SIZE    (1 + 2 * sizeof( uint_16 ))

typedef uint_8  AccelFlags;
#define ACCEL_ASCII     0x00        /* last bit is 0 */
#define ACCEL_VIRTKEY   0x01
#define ACCEL_NOINVERT  0x02
#define ACCEL_SHIFT     0x04
#define ACCEL_CONTROL   0x08
#define ACCEL_ALT       0x10
#define ACCEL_LAST      0x80

/* reswaccl.c */

extern bool ResWriteAccelEntry( AccelTableEntry * currentry, FILE *fp );
extern bool ResWriteAccelEntry32( AccelTableEntry32 *, FILE *fp );

/* resraccl.c */

extern bool ResReadAccelEntry( AccelTableEntry * currentry, FILE *fp );
extern bool ResReadAccelEntry32( AccelTableEntry32 *, FILE *fp );

#endif
