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

#include    <stdio.h>

#include    "main.h"
#include    "encoder.h"
#include    "decoder.h"

int main( int argc, char** argv )
{
#ifdef _DEBUG /* ============================================================ */
    printf( "statrup\n" );

    {
        int i;
        for ( i = 0; i < argc; i++ )
        {
            printf( "argv[%d] = '%s'\n"
                    , i
                    , ( argv[i] ? argv[i] : "-null-" )
                  );
        }
    }
#endif /* =================================================================== */

    display_license();

    /*check if help required*/
    if (    argc == 2
         && (    strncmp( argv[1], "-h", 2 ) == 0
              || strncmp( argv[1], "--help", 6 ) == 0 )
       )
    {
        display_usage(   argv[0]
                       , NULL
                       );
        return ES_OK;
    }

    /*check if encoder/decoder option specified*/
    if (    argc < 2
         || strlen( argv[1] ) < 2 
         )
    {
        display_usage(   argv[0]
                       , "first option must be 2char length"
                       );

        return ES_BADARG;
    }
    else if ( strncmp( argv[1], "-d", 2 ) == 0 )
    {
        /*decoder requires input file to decode*/
        if ( argc < 3 )
        {
            display_usage(   argv[0]
                           , "decoder requires input file to decode"
                           );
            return ES_BADARG;
        }
#ifdef _DEBUG /* ============================================================ */
        printf( "run decoder\n" );
#endif /* =================================================================== */

        return bb_decode( argv[2] );
    }
    else if ( strncmp( argv[1], "-e", 2 ) == 0 )
    {
        /*encoder requires string to encode and output file*/
        if ( argc < 4 )
        {
            display_usage(   argv[0]
                           , "encoder requires string to encode and output file"
                           );
            return ES_BADARG;
        }
#ifdef _DEBUG /* ============================================================ */
        printf( "run encoder\n" );
#endif /* =================================================================== */

        return bb_encode( argv[2], argv[3] );
    }
    else
    {
        display_usage(   argv[0]
                       , "unsupported option"
                       );
    }

#ifdef _DEBUG /* ============================================================ */
    printf( "shutdown\n" );
#endif /* =================================================================== */
    return ES_OK;
}

void display_usage( const char* _argv, const char* _errmsg )
{
    if ( _errmsg )
    {
        fprintf( stderr, "%s\n", _errmsg );
    }

    printf( "\nUsage: "
            "run %s with following options:\n"
            "\t-d <infile> - decode DTMF in <infile>\n"
            "\t-e <number> <outfile> - encode <number> to <outfile>\n"
            , ( _argv ? _argv : "program" )
            );
}

void display_license()
{
    printf( "bluebox Copyright (C) 2012 Ilya Arefiev <arefiev.id@gmail.com>\n"
            "This program comes with ABSOLUTELY NO WARRANTY.\n"
            "This is free software, and you are welcome to redistribute it\n"
            "under certain conditions.\n"
            "See LICENSE file (or obtain your copy of license\n"
            "from <http://www.gnu.org/licenses/>) for details.\n"
            "\n"
            );
}
