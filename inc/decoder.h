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

#ifndef DECODER_H
#define DECODER_H

#include    "common.h"
#include    "dtmf.h"

typedef struct GOERTZEL_DATA_STRUCT
{
    float freq;
    float cfactor;

    float sn1;
    float sn2;
    float sn;

    float xk0;
    float xk;

    float xk_start;
    float xk_stop;
} GOERTZEL_DATA;

typedef struct VAWEFORM_DATA_STRUCT
{
    float wf0;
    float wf1;
    float wf2;
    float wf3;

    float tm;
} WAVEFORM_DATA;

typedef enum DECODER_STATE_ENUM
{
      DS_WAIT_FOR_SIGNAL
    , DS_WAIT_FOR_SIGNAL_END
    , DS_WAIT_FOR_PAUSE
    , DS_WAIT_FOR_PAUSE_END
} DECODER_STATE;

EXIT_STATUS bb_decode( const char* );
EXIT_STATUS bbd_initialize_goertzel_data( GOERTZEL_DATA*, const float, const float);
EXIT_STATUS bbd_goertzel( GOERTZEL_DATA*, const float, const BOOL );
EXIT_STATUS bbd_save_start_stop( GOERTZEL_DATA*, const BOOL );

EXIT_STATUS bbd_is_signal( WAVEFORM_DATA* );
EXIT_STATUS bbd_is_pause( WAVEFORM_DATA* );
EXIT_STATUS bbd_goertzel_reset( GOERTZEL_DATA* );

#endif
