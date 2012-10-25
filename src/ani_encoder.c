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

    size_t pnumbersz = strlen(_number)+1;
    char* pnumber = (char*)calloc( pnumbersz, sizeof(char) );
    if ( !pnumber )
    {
        fprintf( stderr, "unable to allocate memory(%zu bytes) for preppared number\n"
                , pnumbersz
               );

/*TODO: close and free all allocated resources*/

        return ES_BAD;
    }

    if ( ES_OK != bb_ani_prepare_number( _number, pnumber, pnumbersz ) )
    {
        fprintf( stderr, "unable to prepare number\n" );

/*TODO: close and free all allocated resources*/

        return ES_BAD;
    }

/*FIXME: delete*/
    return ES_OK;

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

/** create string ready for encoding
 *
 * searches for repeats and reverses input string
 *
 * @param _src source string
 * @param _dst destination string
 * @paaram _dstsz size of destination string (with trailing NULL)
 *
 * @return ES_BADARG in case of NULL src/dst or if dstsz is not enought<br>
 *         ES_OK if preparation successfull<br>
 *         ES_BAD if unable ot prepare
 */
EXIT_STATUS bb_ani_prepare_number( const char* _src, char* _dst, const size_t _dstsz )
{
    if (    !_src
         || !_dst
         || _dstsz < strlen(_src)+1
       )
    {
        fprintf( stderr, "NULL  src(%c) || dst(%c) || not enought dst size(%c)\n"
                 , ( _src?'f':'t' )
                 , ( _dst?'f':'t' )
                 , ( (_dstsz < strlen(_src)+1)?'f':'t' )
               );

        return ES_BADARG;
    }

#ifdef _DEBUG
    printf( "processing '%s' string\n"
            , _src
          );
#endif

    int i = 0;
    char prev = _src[0];
    _dst[_dstsz-2] = _src[0];   /* -2 -- zero-based && NULL-terminated*/
    for ( i = 1; i < strlen(_src); i++ )
    {
        char c = _src[i];

        if (    _src[i] == prev
             && (i+1) % 2 == 0
           )
        {
            if ( ani_kp2c( ANI_KP_R, &c ) != ES_OK )
            {
                fprintf( stderr, "unable to convert ANI_KP_R to char\n" );
                return ES_BAD;
            }
        }

        _dst[ _dstsz-2 - i ] = c;
        prev = _src[i];
    }

#ifdef _DEBUG
    printf( "prepared: '%s' -> '%s'\n"
            , _src
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
