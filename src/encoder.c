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

#include    "encoder.h"
#include    "dtmf.h"

extern const char* DTMF_NUMBERS;
extern DTMF_KEY_SIGNAL* DTMF_KEY_SIGNALS;
extern const DTMF_KEY_FREQ* DTMF_KEYPAD_FREQ;

EXIT_STATUS bb_encode( const char* _number, const char* _outfname )
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
        if ( !index( DTMF_NUMBERS, _number[i] ) )
        {
            fprintf( stderr, "incorrect character '%c'\n"
                     "allowed characters are '%s'\n"
                     , _number[i]
                     , DTMF_NUMBERS
                   );
            return ES_BADARG;
        }
    }

    static const char* outext = ".wav";
    char* outfname = (char*)calloc( strlen( _outfname ) + strlen( outext ) + 1
                                    , sizeof(char)
                                    );
    if ( !outfname )
    {
        fprintf( stderr, "unable to allocate memory for extfilename\n" );
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
    opt.samplerate  = DTMF_SAMPLE_RATE;
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

        return ES_BAD;
    }

    printf( "encoding '%s' to '%s'\n"
            , _number
            , outfname
          );

    EXIT_STATUS es = ES_OK;
    /*run encoding*/
    for ( i = 0; i < strlen( _number ); i++ )
    {
        char c = _number[ i ];
#ifdef _DEBUG /* ============================================================ */
        printf( "processing '%c' key\n"
                , c
              );
#endif /* =================================================================== */
        DTMF_KEYPAD key = dtmf_c2kp( c );
        if ( !dtmf_is_keypad_value( key ) )
        {
            fprintf( stderr, "'%c'(position %d) -> %d - in not keypad value\n"
                    , c
                    , i
                    , key
                   );

            es = ES_BAD;
            break;
        }

        if ( !DTMF_KEY_SIGNALS[ key ].filled )
        {
#ifdef _DEBUG /* ============================================================ */
        printf( "generating '%c' key signal data\n"
                , c
              );
#endif /* =================================================================== */

            if ( bbe_fill_key_signal( key ) != ES_OK )
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

        if ( !DTMF_KEY_SIGNALS[ DTMF_KP_PAUSE ].filled )
        {
#ifdef _DEBUG /* ============================================================ */
        printf( "generating pause signal data\n" );
#endif /* =================================================================== */

            if ( bbe_fill_key_signal( DTMF_KP_PAUSE ) != ES_OK )
            {
                fprintf( stderr, "unable to fill signal data for pause\n" );

                es = ES_BAD;
                break;
            }
        }
#ifdef _DEBUG /* ============================================================ */
        else
        {
            printf( "pause signal already cached\n" );
        }
#endif /* =================================================================== */

        sf_write_float(   ofd
                        , (float*)(DTMF_KEY_SIGNALS[ DTMF_KP_PAUSE ].data)
                        , DTMF_KEY_SIGNALS[ DTMF_KP_PAUSE ].datasz
                      );
        sf_write_float(   ofd
                        , (float*)(DTMF_KEY_SIGNALS[ key ].data)
                        , DTMF_KEY_SIGNALS[ key ].datasz
                      );
    }

    bbe_free_key_signals();
    free( outfname );
    sf_close( ofd );

    printf( "encoding %s\n"
            , ( es == ES_OK ?
                "done" :
                "failed"
              )
          );
    return es;
}

EXIT_STATUS bbe_fill_key_signal( DTMF_KEYPAD _key )
{
    if ( _key == DTMF_KP_COUNT )
    {
        return ES_BADARG;
    }

    DTMF_KEY_SIGNAL* signal = &( DTMF_KEY_SIGNALS[ _key ] );

    if ( signal->filled )
    {
        return ES_OK;
    }

    DTMF_KEY_FREQ key_freq = DTMF_KEYPAD_FREQ[ _key ];
    float signal_len = ( _key == DTMF_KP_PAUSE ?
                            DTMF_PAUSE_LENGTH :
                            DTMF_SIGNAL_LENGTH
                       );
    size_t data_len = (signal_len / 1000.0) / (1.0 / DTMF_SAMPLE_RATE);

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

    if ( _key != DTMF_KP_PAUSE )
    {
        size_t i = 0;
        for ( i = 0; i < data_len; i++ )
        {
            data[ i ] = ( sinf( 2 * M_PI * key_freq.lo * ( i * (1.0/DTMF_SAMPLE_RATE) ) ) +
                          sinf( 2 * M_PI * key_freq.hi * ( i * (1.0/DTMF_SAMPLE_RATE) ) )
                        );
        }
    }

    signal->filled = true;

    return ES_OK;
}

void bbe_free_key_signals()
{
    DTMF_KEYPAD k = DTMF_KP_1;
    for ( k = DTMF_KP_1; k < DTMF_KP_COUNT; k++ )
    {
        DTMF_KEY_SIGNAL* signal = &( DTMF_KEY_SIGNALS[ k ] );

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
