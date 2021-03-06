/*
 *Yet another DTMF signals encoder/decoder.
 *Copyright (C) 2013  Ilya Arefiev <arefiev.id@gmail.com>
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DTMF_H
#define DTMF_H

#include    "common.h"

// should be defines/enum/etc, not static const <type>
// http://stackoverflow.com/a/3025106
enum DTMF_CONSTANTS {
    //frequencies
      DTMF_X1_FREQ = 1209 /* Hz */
    , DTMF_X2_FREQ = 1336 /* Hz */
    , DTMF_X3_FREQ = 1477 /* Hz */
    , DTMF_X4_FREQ = 1633 /* Hz */

    , DTMF_Y1_FREQ = 697 /* Hz */
    , DTMF_Y2_FREQ = 770 /* Hz */
    , DTMF_Y3_FREQ = 852 /* Hz */
    , DTMF_Y4_FREQ = 941 /* Hz */
    , DTMF_FREQ_COUNT = 8

    //signal-related constants
    , DTMF_SIGNAL_LENGTH        = 70 /* ms */
    , DTMF_SIGNAL_LENGTH_MIN    = 40 /* ms */
    , DTMF_PAUSE_LENGTH         = 50 /* ms */
    , DTMF_PAUSE_LENGTH_MIN     = 40 /* ms */
    , DTMF_SAMPLE_RATE          = (8 * 1000) /* Hz */
    , DTMF_READ_BLOCK_SIZE      = 256
};

typedef struct DTMF_KEY_FREQ_STRUCT {
    short hi;
    short lo;
} DTMF_KEY_FREQ;

typedef enum DTMF_KEYPAD_ENUM {
      DTMF_KP_1
    , DTMF_KP_2
    , DTMF_KP_3
    , DTMF_KP_A

    , DTMF_KP_4
    , DTMF_KP_5
    , DTMF_KP_6
    , DTMF_KP_B

    , DTMF_KP_7
    , DTMF_KP_8
    , DTMF_KP_9
    , DTMF_KP_C

    , DTMF_KP_AST
    , DTMF_KP_0
    , DTMF_KP_POUND
    , DTMF_KP_D

    , DTMF_KP_PAUSE     /* not a real key - used for preallocated signal space */
    , DTMF_KP_COUNT
} DTMF_KEYPAD;

DTMF_KEYPAD dtmf_c2kp( const char );
EXIT_STATUS dtmf_kp2c( const DTMF_KEYPAD, char* );
BOOL dtmf_is_keypad_value( const DTMF_KEYPAD );
EXIT_STATUS dtmf_kf2kp( const DTMF_KEY_FREQ*, DTMF_KEYPAD* );

typedef struct DTMF_KEY_SIGNAL_STRUCT {
    BOOL    filled;
    void*   data;
    size_t  datasz;
} DTMF_KEY_SIGNAL;

extern const char* DTMF_NUMBERS;
extern const DTMF_KEY_FREQ DTMF_KEYPAD_FREQ[ DTMF_KP_COUNT ];
extern DTMF_KEY_SIGNAL DTMF_KEY_SIGNALS[ DTMF_KP_COUNT ];

#endif
