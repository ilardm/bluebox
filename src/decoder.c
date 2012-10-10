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

#include    "decoder.h"

EXIT_STATUS bb_decode( const char* _infname )
{
    if ( !_infname )
    {
        fprintf( stderr, "NULL infname\n" );

        return ES_BADARG;
    }
    int i = 0;

    printf( "decode '%s' file\n"
            , _infname
          );

    SF_INFO opt = { 0 };
    SNDFILE* ifd = sf_open( _infname
                            , SFM_READ
                            , &opt
                          );

    if ( !ifd )
    {
        fprintf( stderr, "unable to open input file: %s\n"
                 , sf_strerror( ifd )
               );

        return ES_BAD;
    }

#ifdef _DEBUG
    FILE** ofd = (FILE**)calloc( opt.channels, sizeof(FILE*) );
    if ( !ofd )
    {
        fprintf( stderr, "unable to allocate memory for debug goertzel fd array\n" );
    }
    else
    {
        for ( i = 0; i < opt.channels; i++ )
        {
/*FIXME: magic number*/
            char fname[256];
            sprintf( fname, "goertz_ch%d", i );

            ofd[i] = fopen( fname, "w" );
            if ( !(ofd[i]) )
            {
                fprintf( stderr, "unable to open Goertzel debug file for ch %d\n"
                        , i
                        );
            }
        }
    }
#endif

#ifdef _DEBUG
    printf( "'%s' -> %d Hz, %d ch\n"
            , _infname
            , opt.samplerate
            , opt.channels
          );
#endif

    EXIT_STATUS es = ES_OK;

    size_t blockcount = DTMF_READ_BLOCK_SIZE * opt.channels;
    float* block = (float*)calloc( blockcount, sizeof(float) );

    if ( !block )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for readblock\n"
                , blockcount * opt.channels
               );

        sf_close( ifd );
        return ES_BAD;
    }

    /*allocate memory for each DTMF freq on each channel*/
    size_t gdatasz = DTMF_FREQ_COUNT * opt.channels;
    GOERTZEL_DATA* gdata = (GOERTZEL_DATA*)calloc( gdatasz, sizeof(GOERTZEL_DATA) );
    if ( !gdata )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for goertzel data\n"
                , gdatasz
               );

        free( block );
        sf_close( ifd );
        return ES_BAD;
    }

    /*allocate memory for wavefrom data on each channel*/
    WAVEFORM_DATA* wfd = (WAVEFORM_DATA*)calloc( opt.channels, sizeof(WAVEFORM_DATA) );
    if ( !wfd )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for waveform data\n"
                , (opt.channels * sizeof(WAVEFORM_DATA))
               );

        free( block );
        free( gdata );
        sf_close( ifd );
        return ES_BAD;
    }

    for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
    {
        int ch = 0;
        for ( ch = 0; ch < opt.channels; ch++ )
        {
            GOERTZEL_DATA* data = &(gdata[ i + ch ]);
            float tfreq = 0;
            switch ( i )
            {
            case 0:     tfreq = DTMF_X1_FREQ; break;
            case 1:     tfreq = DTMF_X2_FREQ; break;
            case 2:     tfreq = DTMF_X3_FREQ; break;
            case 3:     tfreq = DTMF_X4_FREQ; break;
            case 4:     tfreq = DTMF_Y1_FREQ; break;
            case 5:     tfreq = DTMF_Y2_FREQ; break;
            case 6:     tfreq = DTMF_Y3_FREQ; break;
            case 7:     tfreq = DTMF_Y4_FREQ; break;
            }

            bbd_initialize_goertzel_data( data, opt.samplerate, tfreq );
        }
    }

    float tm = 0;
    size_t tm_sample = 0;
    size_t samples_in_10ms = (size_t)(floorf((10 / 1000.0) / (1.0/opt.samplerate)));
    sf_count_t readcount = 0;
    int samplen = 0;
    float samplev = 0;
    int ch = 0;
    while ( (readcount = sf_readf_float( ifd, block, blockcount )) > 0 )
    {
/*#ifdef _DEBUG*/
/*        printf( "read %lld samples\n"*/
/*                , readcount*/
/*              );*/
/*#endif*/
        for ( samplen = 0; samplen < readcount; samplen++ )
        {
            samplev = block[ samplen * opt.channels + ch ];
/*#ifdef _DEBUG*/
/*            printf( "%0.6f: ", tm );*/
/*#endif*/
            for ( ch = 0; ch < opt.channels; ch++ )
            {
/*#ifdef _DEBUG*/
/*                printf( "ch[%d] = %2.6f%c"*/
/*                        , ch*/
/*                        , block[ sample * opt.channels + ch ]*/
/*                        , (ch == opt.channels-1 ? '\n' : '\t')*/
/*                      );*/
/*#endif*/
/*TODO: run processing*/

                /*
                 *firstly, run goetzel alg. for whole file and write
                 *result for each freq into file. plot the result and
                 *choose how signal detection should be performed
                 *since pauses between signals R not necessary NULLS
                 */

                int pause_cnt = 0;
                BOOL update_wf = ( tm_sample % samples_in_10ms == 0 );
                for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
                {
                    GOERTZEL_DATA* data = &(gdata[ i + ch ]);
                    if ( bbd_goertzel( data, samplev, false ) == ES_OK )
                    {
                        /*printf( "Goertzel xk(%f hz) = %0.6f\n"*/
                        /*        , data->freq*/
                        /*        , data->xk*/
                        /*      );*/

#ifdef _DEBUG
                        if (    ofd
                             && ofd[ch]
                           )
                        {
                            if ( i == 0 )
                            {
                                fprintf( ofd[ch], "%0.6f ", tm );
                            }

                            fprintf( ofd[ch], "%0.6f%c"
                                    , data->xk
                                    /*, ( i != DTMF_FREQ_COUNT-1 ?*/
                                        /*' ' :*/
                                        /*'\n'*/
                                      /*)*/
                                    , ' '
                                   );
                        }
#endif
                    }
#ifdef _DEBUG
                    else
                    {
                        fprintf( stderr, "unable to run Goertzel for current sample\n" );
                    }
#endif
                }

                /*if ( (int)(floorf( tm*1000 )) % 10 == 0 )*/
                if ( update_wf )
                {
                    printf( "%0.6f [ch %d] (%0.6f)\t"
                            , tm
                            , ch
                            , samplev
                          );

                    wfd[ch].wf3 = wfd[ch].wf2;
                    wfd[ch].wf2 = wfd[ch].wf1;
                    wfd[ch].wf1 = wfd[ch].wf0;
                    wfd[ch].wf0 = samplev;

                    printf( "%0.6f %0.6f %0.6f %0.6f\n"
                            , wfd[ch].wf3
                            , wfd[ch].wf2
                            , wfd[ch].wf1
                            , wfd[ch].wf0
                          );

                    if ( bbd_is_pause( &(wfd[ch]) ) == ES_OK )
                    {
                        pause_cnt++;
                    }
                }
#ifdef _DEBUG
                /*write waveform data*/
                if (    ofd
                     && ofd[ch]
                   )
                {
                    fprintf( ofd[ch], "%0.6f\n"
                            , samplev
                           );
                }
#endif

                if ( pause_cnt+1 == DTMF_FREQ_COUNT )
                {
                    for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
                    {
                        GOERTZEL_DATA* data = &(gdata[ i + ch ]);
                        bbd_goertzel_reset( data );
                    }
                }
            }

            tm += (1.0 / opt.samplerate);
            tm_sample++;
        }

        memset( block, 0, blockcount * sizeof( float ) );
    }

    free( block );
    free( gdata );
 #ifdef _DEBUG
    if ( ofd )
    {
        for ( i = 0; i < opt.channels; i++ )
        {
            fclose( ofd[i] );
        }
    }
#endif
   sf_close( ifd );

    printf( "decoding %s\n"
            , ( es == ES_OK ?
                "done" :
                "failed"
              )
          );
    return ES_OK;
}

EXIT_STATUS bbd_initialize_goertzel_data( GOERTZEL_DATA* _data, const float _sr, const float _target_freq)
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

    _data->freq = _target_freq;
    _data->cfactor = 2 * cosf( 2 * M_PI * _target_freq / _sr );

    return ES_OK;
}

EXIT_STATUS bbd_goertzel( GOERTZEL_DATA* _data, const float _sample, const BOOL _update_xk )
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

EXIT_STATUS bbd_is_pause( WAVEFORM_DATA* _data )
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
        printf( "possible pause: max = %0.6f\n"
                , max
              );
        return ES_OK;
    }

    return ES_BAD;
}

EXIT_STATUS bbd_goertzel_reset( GOERTZEL_DATA* _data )
{
    if ( !_data )
    {
        fprintf( stderr, "NULL Goertzel data\n" );

        return ES_BADARG;
    }

    return ES_BAD;
}
