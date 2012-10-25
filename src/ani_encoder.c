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
    opt.samplerate  = ANI_SAMPLE_RATE;
    opt.channels    = 1;
    opt.format      =   SF_FORMAT_WAV
                      | SF_FORMAT_PCM_16
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

    /*TODO: prepare number*/
    /*TODO: run encoding*/

    /* --------------------------------------------------------------------- */

    bb_ani_free_key_signals();
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

EXIT_STATUS bb_ani_fill_key_signal( ANI_KEYPAD _key )
{
    if ( _key == ANI_KP_COUNT )
    {
        return ES_BADARG;
    }

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
