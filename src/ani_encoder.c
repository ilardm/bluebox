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

#include    "ani_encoder.h"

EXIT_STATUS bb_ani_encode( const char* _number, const char* _outfname )
{
    if (    !_number
         || !_outfname
       )
    {
        fprintf(  stderr, "NULL number(%c) || outfname(%c)\n"
                , ( _number ? 'f' : 'T' )
                , ( _outfname ? 'f' : 'T' )
               );

        return ES_BADARG;
    }

#ifdef _DEBUG /* ============================================================ */
    printf( "'%s' -> '%s'\n"
            , _number
            , _outfname
          );
#endif /* =================================================================== */

    /*check for number validity*/
    int i = 0;
    for ( i = 0; i < strlen( _number ); i++ )
    {
        if ( !index( ANI_NUMBERS, _number[i] ) )
        {
            fprintf( stderr, "incorrect character '%c'\n"
                     "allowed characters are '%s'\n"
                     , _number[i]
                     , ANI_NUMBERS
                   );
            return ES_BADARG;
        }
    }

    size_t pnumbersz = strlen(_number)+1+2; /* +1 -- NULL; +2 -- S && category */
    char* pnumber = (char*)calloc( pnumbersz, sizeof(char) );
    if ( !pnumber )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for preppared number\n"
                , pnumbersz
               );

        return ES_BAD;
    }

    char c = 0x00;
    if ( ani_kp2c( ANI_KP_S, &c ) != ES_OK )
    {
        fprintf( stderr, "unable to convert ANI_KP_S to char\n" );

        free( pnumber );
        return ES_BAD;
    }
    memcpy( pnumber, _number, strlen(_number) );
    *( pnumber + strlen(pnumber) ) = ANI_ENCODER_NUMBER_CATEGORY;
    *( pnumber + strlen(pnumber) ) = c;

    if ( ES_OK != bb_ani_prepare_number( pnumber, pnumbersz ) )
    {
        fprintf( stderr, "unable to prepare number\n" );

        free( pnumber );
        return ES_BAD;
    }

    static const char* outext = ".wav";
    char* outfname = (char*)calloc( strlen( _outfname ) + strlen( outext ) + 1
                                    , sizeof(char)
                                    );
    if ( !outfname )
    {
        fprintf( stderr, "unable to allocate memory for extfilename\n" );

        free( pnumber );
        return ES_BAD;
    }

    strncpy( outfname
             , _outfname
             , strlen(_outfname)
           );

    /*check for file extension*/
    if ( !strstr( _outfname + strlen( _outfname ) - strlen( outext ), outext ) )
    {
        strncpy( outfname + strlen( outfname )
                 , outext
                 , strlen( outext )
               );
    }

    SF_INFO opt = { 0 };
    opt.samplerate  = ANI_SAMPLE_RATE;
    opt.channels    = 1;
    opt.format      =   SF_FORMAT_WAV
                      | SF_FORMAT_FLOAT
                      ;

    SNDFILE* ofd = sf_open( outfname
                            , SFM_WRITE
                            , &opt
                          );
    if ( !ofd )
    {
        fprintf( stderr, "unable to open output file: %s\n"
                , sf_strerror( ofd )
                );

        free( pnumber );
        free( outfname );
        return ES_BAD;
    }

    printf( "encoding '%s' to '%s'\n"
            , pnumber
            , outfname
          );

    EXIT_STATUS es = ES_OK;

    /*run encoding*/
    for ( i = 0; i < strlen( pnumber ); i++ )
    {
        char c = pnumber[ i ];
#ifdef _DEBUG /* ============================================================ */
        printf( "processing '%c' key\n"
                , c
              );
#endif /* =================================================================== */
        ANI_KEYPAD key = ani_c2kp( c );
        if ( !ani_is_keypad_value( key ) )
        {
            fprintf( stderr, "'%c'(position %d) -> %d - in not keypad value\n"
                    , c
                    , i
                    , key
                   );

            es = ES_BAD;
            break;
        }

        if ( !ANI_KEY_SIGNALS[ key ].filled )
        {
#ifdef _DEBUG /* ============================================================ */
        printf( "generating '%c' key signal data\n"
                , c
              );
#endif /* =================================================================== */

            if ( bb_ani_fill_key_signal( key ) != ES_OK )
            {
                fprintf( stderr, "unable to fill signal data for key '%c'\n"
                        , c
                       );

                es = ES_BAD;
                break;
            }
        }
#ifdef _DEBUG /* ============================================================ */
        else
        {
            printf( "'%c' signal already cached\n"
                    , c
                  );
        }
#endif /* =================================================================== */

        sf_write_float(   ofd
                        , (float*)(ANI_KEY_SIGNALS[ key ].data)
                        , ANI_KEY_SIGNALS[ key ].datasz
                      );
    }

    /* --------------------------------------------------------------------- */

    bb_ani_free_key_signals();
    free( outfname );
    free( pnumber );
    sf_close( ofd );

    printf( "encoding %s\n"
            , ( es == ES_OK ?
                "done" :
                "failed"
              )
          );
    return es;
}

/** create string ready for encoding
 *
 * searches for repeats and reverses input string
 *
 * @param _dst destination string
 * @param _dstsz size of dst string (including NULL-terminator)
 *
 * @return ES_BADARG in case of NULL dst<br>
 *         ES_OK if preparation successfull<br>
 *         ES_BAD if unable ot prepare
 */
EXIT_STATUS bb_ani_prepare_number( char* _dst, const size_t _dstsz )
{
    if ( !_dst )
    {
        fprintf( stderr, "NULL dst\n" );

        return ES_BADARG;
    }

#ifdef _DEBUG
    printf( "processing '%s' string\n"
            , _dst
          );
#endif

    int i = 0;
    int mid = (_dstsz-1)/2;
    char prev = _dst[0];
    char xchg = 0x00;
    int pos = 0;
    char r = 0x00;
    int rcnt = 0;
    if ( ani_kp2c( ANI_KP_R, &r ) != ES_OK )
    {
        fprintf( stderr, "unable to convert ANI_KP_R to char\n" );
        return ES_BAD;
    }

    /*insert repeats*/
    for ( i = 1; i < _dstsz-1; i++ )
    {
        xchg = _dst[i];
        if ( _dst[i] == prev
             && rcnt % 2 == 0
           )
        {
            xchg = r;
            rcnt++;
        }
        else
        {
            rcnt=0;
        }

        prev = _dst[i];
        _dst[i] = xchg;
    }

    /*reverse string*/
    for ( i = 0; i < mid; i++ )
    {
        pos = _dstsz - i - 2;   /* -2 -- NULL-terminated && zero-based */
        xchg = _dst[ pos ];
        _dst[pos] = _dst[i];
        _dst[i] = xchg;
    }

#ifdef _DEBUG
    printf( "prepared: '%s'\n"
            , _dst
          );
#endif

    return ES_OK;
}

EXIT_STATUS bb_ani_fill_key_signal( ANI_KEYPAD _key )
{
    if ( _key == ANI_KP_COUNT )
    {
        return ES_BADARG;
    }

    ANI_KEY_SIGNAL* signal = &( ANI_KEY_SIGNALS[ _key ] );

    if ( signal->filled )
    {
        return ES_OK;
    }

    ANI_KEY_FREQ key_freq = ANI_KEYPAD_FREQ[ _key ];
    size_t data_len = (ANI_SIGNAL_DUR / 1000.0) / (1.0 / ANI_SAMPLE_RATE);

    float* data = (float*)calloc( data_len, sizeof( float ) );
    if ( !data )
    {
        fprintf( stderr, "unable to allocate (%zu bytes) memory for signal data\n"
                , data_len * sizeof( float )
               );
        return ES_BAD;
    }

    signal->data = data;
    signal->datasz = data_len;

    size_t i = 0;
    for ( i = 0; i < data_len; i++ )
    {
        data[ i ] = ( sinf( 2 * M_PI * key_freq.lo * ( i * (1.0/ANI_SAMPLE_RATE) ) ) +
                      sinf( 2 * M_PI * key_freq.hi * ( i * (1.0/ANI_SAMPLE_RATE) ) )
                    );
    }

    signal->filled = true;


    return ES_OK;
}

void bb_ani_free_key_signals()
{
    ANI_KEYPAD k = 0;
    for ( k = ANI_KP_S; k < ANI_KP_COUNT; k++ )
    {
        ANI_KEY_SIGNAL* signal = &( ANI_KEY_SIGNALS[ k ] );

        if (    signal->filled
             && signal->data
           )
        {
            free( signal->data );
            signal->data = 0x00;
            signal->filled = false;
        }
    }
}
