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

#include    "ani.h"

ANI_KEYPAD ani_c2kp( const char _c )
{
    ANI_KEYPAD ret = ANI_KP_COUNT;

    switch ( _c )
    {
    case '0':
        ret = ANI_KP_0;
        break;
    case '1':
        ret = ANI_KP_1;
        break;
    case '2':
        ret = ANI_KP_2;
        break;
    case '3':
        ret = ANI_KP_3;
        break;
    case '4':
        ret = ANI_KP_4;
        break;
    case '5':
        ret = ANI_KP_5;
        break;
    case '6':
        ret = ANI_KP_6;
        break;
    case '7':
        ret = ANI_KP_7;
        break;
    case '8':
        ret = ANI_KP_8;
        break;
    case '9':
        ret = ANI_KP_9;
        break;
    case 'R':
        ret = ANI_KP_R;
        break;
    case 'S':
        ret = ANI_KP_S;
        break;
    };

    return ret;
}

EXIT_STATUS ani_kp2c( const ANI_KEYPAD _kp, char* _char )
{
    if ( !_char )
    {
        fprintf( stderr, "NULL destination character\n" );

        return ES_BADARG;
    }

    if ( ani_is_keypad_value( _kp ) == false )
    {
        fprintf( stderr, "invalid keypad value: %d\n"
                , _kp
               );

        return ES_BADARG;
    }

    *_char = ANI_NUMBERS[ _kp ];

    return ES_OK;
}

BOOL ani_is_keypad_value( const ANI_KEYPAD _kp )
{
    return ( (    _kp >= ANI_KP_S
               && _kp <= ANI_KP_R ) ?
                true :
                false
           );
}

EXIT_STATUS ani_kf2kp( const ANI_KEY_FREQ* _kf, ANI_KEYPAD* _kp )
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

    for ( ANI_KEYPAD kp = ANI_KP_S; ani_is_keypad_value( kp ) == true; kp++ )
    {
        if (    ANI_KEYPAD_FREQ[ kp ].hi == _kf->hi
             && ANI_KEYPAD_FREQ[ kp ].lo == _kf->lo
           )
        {
            *_kp = kp;

            return ES_OK;
        }
    }

    return ES_BAD;
}
