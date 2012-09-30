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

#include    "dtmf.h"

DTMF_KEYPAD dtmf_c2kp( const char _c )
{
    DTMF_KEYPAD ret = DTMF_KP_COUNT;

    switch ( _c )
    {
    case '0':
        ret = DTMF_KP_0;
        break;
    case '1':
        ret = DTMF_KP_1;
        break;
    case '2':
        ret = DTMF_KP_2;
        break;
    case '3':
        ret = DTMF_KP_3;
        break;
    case '4':
        ret = DTMF_KP_4;
        break;
    case '5':
        ret = DTMF_KP_5;
        break;
    case '6':
        ret = DTMF_KP_6;
        break;
    case '7':
        ret = DTMF_KP_7;
        break;
    case '8':
        ret = DTMF_KP_8;
        break;
    case '9':
        ret = DTMF_KP_9;
        break;
    case '*':
        ret = DTMF_KP_AST;
        break;
    case '#':
        ret = DTMF_KP_POUND;
        break;
    case 'A':
        ret = DTMF_KP_A;
        break;
    case 'B':
        ret = DTMF_KP_B;
        break;
    case 'C':
        ret = DTMF_KP_C;
        break;
    case 'D':
        ret = DTMF_KP_D;
        break;
    };

    return ret;
}

BOOL dtmf_is_keypad_value( const DTMF_KEYPAD _kp )
{
    return ( (    _kp >= DTMF_KP_1
               && _kp <= DTMF_KP_D ) ?
                true :
                false
           );
}
