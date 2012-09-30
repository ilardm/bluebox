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

#ifdef _DEBUG /* ============================================================ */
    printf( "'%s' -> '%s'\n"
            , _number
            , outfname
          );
#endif /* =================================================================== */

    FILE* ofd = fopen( outfname, "w" );
    if ( !ofd )
    {
        fprintf( stderr, "unable to open output file\n" );

        return ES_BAD;
    }

    /*run encoding*/
    /*TODO*/

    free( outfname );
    fclose( ofd );
    return ES_OK;
}
