/*
 *Yet another DTMF signals encoder/decoder.
 *Copyright (C) 2012  Ilya Arefiev <arefiev.id@gmail.com>
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

#ifndef ANI_H
#define ANI_H

#include    "common.h"

/*
 *       | 700 900 1100 1300
 *  -----+------------------
 *  900  |  1
 *  1100 |  2   3
 *  1300 |  4   5   6
 *  1500 |  7   8   9    0
 *  1700 |          S    R
 */

static const char* ANI_NUMBERS = "S1234567890R";

enum ANI_CONSTANTS {
      ANI_REQUEST_FREQ  = 500   /*Hz*/
    , ANI_REQUEST_DUR   = 100   /*ms*/

    , ANI_SIGNAL_DUR    = 40    /*ms*/

    , ANI_FREQ_1        = 700   /*Hz*/
    , ANI_FREQ_2        = 900
    , ANI_FREQ_3        = 1100
    , ANI_FREQ_4        = 1300
    , ANI_FREQ_5        = 1500
    , ANI_FREQ_6        = 1700
    , ANI_FREQ_COUNT    = 7     /* freq1-fre6 + request_freq */

    , ANI_READ_BLOCK_SIZE   = 256
};

typedef struct ANI_KEY_FREQ_STRUCT {
    short hi;
    short lo;
} ANI_KEY_FREQ;

typedef enum ANI_KEYPAD_ENUM {
      ANI_KP_S

    , ANI_KP_1
    , ANI_KP_2
    , ANI_KP_3
    , ANI_KP_4
    , ANI_KP_5
    , ANI_KP_6
    , ANI_KP_7
    , ANI_KP_8
    , ANI_KP_9
    , ANI_KP_0

    , ANI_KP_R

    , ANI_KP_COUNT
} ANI_KEYPAD;

static const ANI_KEY_FREQ ANI_KEYPAD_FREQ[ ANI_KP_COUNT ] = {
      { ANI_FREQ_6, ANI_FREQ_3 }    /* S */

    , { ANI_FREQ_2, ANI_FREQ_1 }    /* 1 */
    , { ANI_FREQ_3, ANI_FREQ_1 }
    , { ANI_FREQ_3, ANI_FREQ_2 }
    , { ANI_FREQ_4, ANI_FREQ_1 }
    , { ANI_FREQ_4, ANI_FREQ_2 }
    , { ANI_FREQ_4, ANI_FREQ_3 }
    , { ANI_FREQ_5, ANI_FREQ_1 }
    , { ANI_FREQ_5, ANI_FREQ_2 }
    , { ANI_FREQ_5, ANI_FREQ_3 }
    , { ANI_FREQ_5, ANI_FREQ_4 }    /* 0 */

    , { ANI_FREQ_6, ANI_FREQ_4 }    /* R */
};

ANI_KEYPAD ani_c2kp( const char );
EXIT_STATUS ani_kp2c( const ANI_KEYPAD, char* );
BOOL ani_is_keypad_value( const ANI_KEYPAD );
EXIT_STATUS ani_kf2kp( const ANI_KEY_FREQ*, ANI_KEYPAD* );

#endif
