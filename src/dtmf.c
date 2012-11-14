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

/*constants*/
const char* DTMF_NUMBERS = "123A456B789C*0#D";

const DTMF_KEY_FREQ DTMF_KEYPAD_FREQ[ DTMF_KP_COUNT ] = {
      { DTMF_X1_FREQ, DTMF_Y1_FREQ }
    , { DTMF_X2_FREQ, DTMF_Y1_FREQ }
    , { DTMF_X3_FREQ, DTMF_Y1_FREQ }
    , { DTMF_X4_FREQ, DTMF_Y1_FREQ }

    , { DTMF_X1_FREQ, DTMF_Y2_FREQ }
    , { DTMF_X2_FREQ, DTMF_Y2_FREQ }
    , { DTMF_X3_FREQ, DTMF_Y2_FREQ }
    , { DTMF_X4_FREQ, DTMF_Y2_FREQ }

    , { DTMF_X1_FREQ, DTMF_Y3_FREQ }
    , { DTMF_X2_FREQ, DTMF_Y3_FREQ }
    , { DTMF_X3_FREQ, DTMF_Y3_FREQ }
    , { DTMF_X4_FREQ, DTMF_Y3_FREQ }

    , { DTMF_X1_FREQ, DTMF_Y4_FREQ }
    , { DTMF_X2_FREQ, DTMF_Y4_FREQ }
    , { DTMF_X3_FREQ, DTMF_Y4_FREQ }
    , { DTMF_X4_FREQ, DTMF_Y4_FREQ }
    , { 0, 0 }
};

DTMF_KEY_SIGNAL DTMF_KEY_SIGNALS[ DTMF_KP_COUNT ] = { { 0 } };

/*/constants*/

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

EXIT_STATUS dtmf_kp2c( const DTMF_KEYPAD _kp, char* _char )
{
    if ( !_char )
    {
        fprintf( stderr, "NULL destination character\n" );

        return ES_BADARG;
    }

    if ( dtmf_is_keypad_value( _kp ) == false )
    {
        fprintf( stderr, "invalid keypad value: %d\n"
                , _kp
               );

        return ES_BADARG;
    }

    *_char = DTMF_NUMBERS[ _kp ];

    return ES_OK;
}

BOOL dtmf_is_keypad_value( const DTMF_KEYPAD _kp )
{
    return ( (    _kp >= DTMF_KP_1
               && _kp <= DTMF_KP_D ) ?
                true :
                false
           );
}

EXIT_STATUS dtmf_kf2kp( const DTMF_KEY_FREQ* _kf, DTMF_KEYPAD* _kp )
{
    if (    !_kf
         || !_kp
       )
    {
        fprintf( stderr, "NULL kf(%c) || kp(%c)\n"
                , ( _kf? 'F': 't' )
                , ( _kp? 'F': 't' )
               );

        return ES_BADARG;
    }

    DTMF_KEYPAD kp;
    for ( kp = DTMF_KP_1; dtmf_is_keypad_value( kp ) == true; kp++ )
    {
        if (    DTMF_KEYPAD_FREQ[ kp ].hi == _kf->hi
             && DTMF_KEYPAD_FREQ[ kp ].lo == _kf->lo
           )
        {
            *_kp = kp;

            return ES_OK;
        }
    }

    return ES_BAD;
}
