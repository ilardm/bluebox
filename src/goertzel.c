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

#include    "goertzel.h"

EXIT_STATUS bbg_initialize_goertzel_data( GOERTZEL_DATA* _data, const float _sr, const float _target_freq)
{
    if ( !_data )
    {
        fprintf( stderr, "NULL Goertzel data\n" );

        return ES_BADARG;
    }

    _data->sn = 0;
    _data->sn1 = 0;
    _data->sn2 = 0;

    _data->xk = 0;

    _data->xk_start = 0;
    _data->xk_stop = 0;

    _data->freq = _target_freq;
    _data->cfactor = 2 * cosf( 2 * M_PI * _target_freq / _sr );

    return ES_OK;
}

EXIT_STATUS bbg_goertzel( GOERTZEL_DATA* _data, const float _sample, const BOOL _update_xk )
{
    if ( !_data )
    {
        fprintf( stderr, "NULL Goertzel data\n" );

        return ES_BADARG;
    }

    _data->sn2 = _data->sn1;
    _data->sn1 = _data->sn;

    _data->sn = _data->cfactor * _data->sn1 -
                _data->sn2 +
                _sample;

    _data->xk = SQR(_data->sn1) -
                _data->cfactor * _data->sn1 * _data->sn2 +
                SQR(_data->sn2);

    if ( _update_xk )
    {
    }

    return ES_OK;
}

EXIT_STATUS bbg_is_signal( WAVEFORM_DATA* _data )
{
    if ( !_data )
    {
        fprintf( stderr, "NULL waveform data\n" );

        return ES_BADARG;
    }

/*FIXME: magic number*/
    float trs = 0.01;

    if (    ABS(_data->wf3) > trs
         || ABS(_data->wf2) > trs
         || ABS(_data->wf1) > trs
         || ABS(_data->wf0) > trs
       )
    {
        return ES_OK;
    }

    return ES_BAD;
}

EXIT_STATUS bbg_is_pause( WAVEFORM_DATA* _data )
{
    if ( !_data )
    {
        fprintf( stderr, "NULL waveform data\n" );

        return ES_BADARG;
    }

    float max = MAX4(   ABS(_data->wf3)
                      , ABS(_data->wf2)
                      , ABS(_data->wf1)
                      , ABS(_data->wf0)
                    );

/*FIXME: magic number*/
    if ( max < 0.01 )
    {
#ifdef _DEBUG
        printf( "possible pause: max = %0.6f\n"
                , max
              );
#endif
        return ES_OK;
    }

    return ES_BAD;
}

EXIT_STATUS bbg_save_start_stop( GOERTZEL_DATA* _data, const BOOL _start )
{
    if ( !_data )
    {
        fprintf( stderr, "NULL Goertzel data\n" );

        return ES_BADARG;
    }

    if ( _start )
    {
        _data->xk_start = _data->xk;
    }
    else
    {
        _data->xk_stop = _data->xk;
    }

#ifdef _DEBUG
    printf( "save %s for freq %f: %0.6f\n"
            , ( _start == true ? "start" : "stop" )
            , _data->freq
            , ( _start == true ? _data->xk_start : _data->xk_stop )
          );
#endif

    return ES_OK;
}

EXIT_STATUS bbg_goertzel_reset( GOERTZEL_DATA* _data )
{
    if ( !_data )
    {
        fprintf( stderr, "NULL Goertzel data\n" );

        return ES_BADARG;
    }

    _data->sn = 0;
    _data->sn1 = 0;
    _data->sn2 = 0;

    _data->xk = 0;

#ifdef _DEBUG
    printf( "reset Goertzel data @ freq %0.6f\n"
            , _data->freq
          );
#endif

    return ES_BAD;
}
