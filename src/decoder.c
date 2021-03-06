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

    char* results = (char*)calloc( CHARBUFSZ * opt.channels, sizeof(char) );
    if ( !results )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for results\n"
                , (CHARBUFSZ * opt.channels * sizeof(char))
               );

        return ES_BAD;
    }

    for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
    {
        int ch = 0;
        for ( ch = 0; ch < opt.channels; ch++ )
        {
            GOERTZEL_DATA* data = &(gdata[ i*opt.channels + ch ]);
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

            bbg_initialize_goertzel_data( data, opt.samplerate, tfreq );
        }
    }

    float tm = 0;
    size_t tm_sample = 0;
    size_t samples_for_analyze = (size_t)(floorf((2 / 1000.0) / (1.0/opt.samplerate)));
    sf_count_t readcount = 0;
    int samplen = 0;
    float samplev = 0;
    int ch = 0;
    DECODER_STATE dstate = DS_WAIT_FOR_SIGNAL;
    while ( (readcount = sf_readf_float( ifd, block, blockcount )) > 0 )
    {
/*#ifdef _DEBUG*/
/*        printf( "read %lld samples\n"*/
/*                , readcount*/
/*              );*/
/*#endif*/
        for ( samplen = 0; samplen < readcount; samplen++ )
        {
            samplev = block[ samplen ];
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

                /*int pause_cnt = 0;*/
                BOOL update_wf = ( tm_sample % samples_for_analyze == 0 );
                for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
                {
                    GOERTZEL_DATA* data = &(gdata[ i*opt.channels + ch ]);
                    if ( bbg_goertzel( data, samplev, false ) == ES_OK )
                    {
                        /*printf( "Goertzel xk(%f hz) = %0.6f\n"*/
                        /*        , data->freq*/
                        /*        , data->xk*/
                        /*      );*/

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
#ifdef _DEBUG
                    printf( "%0.6f [ch %d] (%0.6f)\t"
                           , tm
                           , ch
                           , samplev
                         );
#endif

                    wfd[ch].wf3 = wfd[ch].wf2;
                    wfd[ch].wf2 = wfd[ch].wf1;
                    wfd[ch].wf1 = wfd[ch].wf0;
                    wfd[ch].wf0 = samplev;

#ifdef _DEBUG
                    printf( "%0.6f %0.6f %0.6f %0.6f\n"
                           , wfd[ch].wf3
                           , wfd[ch].wf2
                           , wfd[ch].wf1
                           , wfd[ch].wf0
                         );
#endif

#ifdef _DEBUG
                    printf( "dstate: %d\n"
                            , dstate
                          );
#endif
                    switch ( dstate )
                    {
                    case DS_WAIT_FOR_SIGNAL:
                        {
                            if ( bbg_is_signal( &(wfd[ch]) ) == ES_OK )
                            {
                                for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
                                {
                                    GOERTZEL_DATA* data = &(gdata[ i*opt.channels + ch ]);
                                    bbg_save_start_stop( data, true );
                                }

                                dstate = DS_WAIT_FOR_SIGNAL_END;
                                wfd[ch].tm = tm;
                            }
                            break;
                        }
                    case DS_WAIT_FOR_SIGNAL_END:
                        {
                            if ( bbg_is_signal( &(wfd[ch]) ) == ES_OK )
                            {
                                if ( (tm - wfd[ch].tm)*1000 >= DTMF_SIGNAL_LENGTH_MIN )
                                {
                                    dstate = DS_WAIT_FOR_PAUSE;
                                    wfd[ch].tm = tm;
                                }
                            }
                            else
                            {
                                dstate = DS_WAIT_FOR_SIGNAL;
                            }
                            break;
                        }
                    case DS_WAIT_FOR_PAUSE:
                        {
                            if ( bbg_is_pause( &(wfd[ch]) ) == ES_OK )
                            {
                                for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
                                {
                                    GOERTZEL_DATA* data = &(gdata[ i*opt.channels + ch ]);
                                    bbg_save_start_stop( data, false );
                                }

                                dstate = DS_WAIT_FOR_PAUSE_END;
                                wfd[ch].tm = tm;
                            }
                            break;
                        }
                    case DS_WAIT_FOR_PAUSE_END:
                        {
                            if ( bbg_is_pause( &(wfd[ch]) ) == ES_OK )
                            {
                                if ( (tm - wfd[ch].tm)*1000 >= DTMF_PAUSE_LENGTH_MIN )
                                {
/*TODO: detect key*/
                                    char c = 0x00;
                                    if ( bbd_detect_key( gdata, opt.channels, ch, &c ) == ES_OK )
                                    {
                                        /*printf( "%c", c );*/
                                        char* dst = results + CHARBUFSZ * ch;
                                        sprintf( dst + strlen( dst ), "%c", c );
                                    }

                                    for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
                                    {
                                        GOERTZEL_DATA* data = &(gdata[ i*opt.channels + ch ]);
                                        bbg_goertzel_reset( data );
                                    }

                                    dstate = DS_WAIT_FOR_SIGNAL;
                                    wfd[ch].tm = tm;
                                }
                            }
                            else
                            {
                                dstate = DS_WAIT_FOR_PAUSE;
                            }
                            break;
                        }
                    default:
                        {
                            fprintf( stderr, "unknown decoder state: %d\n"
                                    , dstate
                                   );
                        }
                    }
                }
                /*write waveform data*/
                if (    ofd
                     && ofd[ch]
                   )
                {
                    fprintf( ofd[ch], "%0.6f\n"
                            , samplev
                           );
                }
            }

            tm += (1.0 / opt.samplerate);
            tm_sample++;
        }

        memset( block, 0, blockcount * sizeof( float ) );
    }

    if (    dstate == DS_WAIT_FOR_PAUSE
         || dstate == DS_WAIT_FOR_PAUSE_END
       )
    {
        /*emulate normal signal end*/
        for ( ch = 0; ch < opt.channels; ch ++ )
        {
            for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
            {
                GOERTZEL_DATA* data = &(gdata[ i*opt.channels + ch ]);
                bbg_save_start_stop( data, false );
            }

/*TODO: detect signal*/
            char c = 0x00;
            if ( bbd_detect_key( gdata, opt.channels, ch, &c ) == ES_OK )
            {
                /*printf( "%c", c );*/
                char* dst = results + CHARBUFSZ * ch;
                sprintf( dst + strlen( dst ), "%c", c );
            }
        }
    }

    printf( "decoding %s\n"
            , ( es == ES_OK ?
                "done" :
                "failed"
              )
          );

    for ( ch = 0; ch < opt.channels; ch++ )
    {
        char* dst = results + CHARBUFSZ * ch;
        printf( "result for ch[%d]: '%s'\n"
                , ch
                , dst
                );
    }

    free( block );
    free( gdata );
    free( wfd );
    free( results );
    if ( ofd )
    {
        for ( i = 0; i < opt.channels; i++ )
        {
            fclose( ofd[i] );
        }
        free( ofd );
    }
    sf_close( ifd );

    return es;
}

EXIT_STATUS bbd_detect_key( const GOERTZEL_DATA* _data, const int _chcount, const int _ch, char* _key )
{
    if ( !_data )
    {
        fprintf( stderr, "NULL Goertzel data\n" );

        return ES_BADARG;
    }

    if ( !_key )
    {
        fprintf( stderr, "NULL destination character\n" );

        return ES_BADARG;
    }

    if (    _ch < 0
         || _chcount < 1
       )
    {
        fprintf( stderr, "invalid channels count(%d) || channel number(%d)\n"
                , _chcount
                , _ch
               );

        return ES_BADARG;
    }

    int i = 0;
    short freq = 0;
    DTMF_KEY_FREQ kf = { 0 }; /* short */
    float xk_hi = 0;
    float xk_lo = 0;
    for ( i = 0; i < DTMF_FREQ_COUNT; i++ )
    {
        const GOERTZEL_DATA* data = &(_data[ i*_chcount + _ch ]);

        switch ( i )
        {
        case 0:     freq = DTMF_X1_FREQ; break;
        case 1:     freq = DTMF_X2_FREQ; break;
        case 2:     freq = DTMF_X3_FREQ; break;
        case 3:     freq = DTMF_X4_FREQ; break;
        case 4:     freq = DTMF_Y1_FREQ; break;
        case 5:     freq = DTMF_Y2_FREQ; break;
        case 6:     freq = DTMF_Y3_FREQ; break;
        case 7:     freq = DTMF_Y4_FREQ; break;
        }

#ifdef _DEBUG
        printf( "check data(%f) @ freq %d(%f) (kf %dx%d; xk %fx%f)\n"
                , data->xk_stop
                , freq
                , data->freq
                , kf.hi
                , kf.lo
                , xk_hi
                , xk_lo
              );
#endif
        if ( data->xk_stop >= xk_hi )
        {
            xk_lo = xk_hi;
            kf.lo = kf.hi;

            xk_hi = data->xk_stop;
            kf.hi = freq;
        }
        else if ( data->xk_stop >= xk_lo )
        {
            xk_lo = data->xk_stop;
            kf.lo = freq;
        }
    }

    short exchg = MAX( kf.hi, kf.lo );
    kf.lo = MIN( kf.hi, kf.lo );
    kf.hi = exchg;

#ifdef _DEBUG
    printf( "key: %dx%d\n"
            , kf.hi
            , kf.lo
          );
#endif

    DTMF_KEYPAD kp = DTMF_KP_COUNT;
    if ( dtmf_kf2kp( &kf, &kp ) == ES_OK )
    {
        if ( dtmf_kp2c( kp, _key ) == ES_OK )
        {
            return ES_OK;
        }
    }

    return ES_BAD;
}
