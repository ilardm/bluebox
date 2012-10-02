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

    float tm = 0;
    sf_count_t readcount = 0;
    int sample = 0;
    int ch = 0;
    while ( (readcount = sf_readf_float( ifd, block, blockcount )) > 0 )
    {
#ifdef _DEBUG
        printf( "read %ld samples\n"
                , readcount
              );
#endif
        for ( sample = 0; sample < readcount; sample++ )
        {
#ifdef _DEBUG
            printf( "%0.6f: ", tm );
#endif
            for ( ch = 0; ch < opt.channels; ch++ )
            {
#ifdef _DEBUG
                printf( "ch[%d] = %2.6f%c"
                        , ch
                        , block[ sample * opt.channels + ch ]
                        , (ch == opt.channels-1 ? '\n' : '\t')
                      );
#endif
/*TODO: run processing*/
            }

            tm += (1.0 / opt.samplerate);
        }

        memset( block, 0, blockcount * sizeof( float ) );
    }

    free( block );
    sf_close( ifd );

    printf( "decoding %s\n"
            , ( es == ES_OK ?
                "done" :
                "failed"
              )
          );
    return ES_OK;
}
