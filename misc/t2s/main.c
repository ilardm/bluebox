#include    <stdio.h>
#include    <memory.h>
#include    <string.h>

#include    <sndfile.h>

/*#define BUFSZ   256*/
#define BUFSZ   24

int main( int argc, char** argv )
{
    if ( argc < 3 )
    {
        printf( "specify input and output files\n" );
        return 1;
    }

    const char* infname     = argv[1];
    const char* ofname      = argv[2];

    printf( "'%s' -> '%s'\n"
            , infname
            , ofname
          );

    FILE* ifd = fopen( infname, "r" );
    if ( !ifd )
    {
        printf( "unable to open in file\n" );

        return 1;
    }

    SF_INFO opt;
    memset( &opt, 0, sizeof(opt) );

    opt.samplerate  = 8000;
    opt.channels    = 1;
    opt.format      =   SF_FORMAT_WAV
                      | SF_FORMAT_FLOAT
                      ;

    SNDFILE* ofd = sf_open(   ofname
                            , SFM_WRITE
                            , &opt
                          );

    if ( !ofd )
    {
        printf( "unable to open out file: '%s'\n"
                , sf_strerror( ofd )
                );

        fclose( ifd );
        return 1;
    }

    char line[BUFSZ];
    memset( line, 0, sizeof(line) );
    char c = 0x00;
    int p = 0x00;
    do
    {
        if ( '\n' == c )
        {
            line[p] = 0x00;

            if ( strlen( line ) )
            {
                float t = 0x00;
                float v = 0x00;

                int r = sscanf(   line
                                , "%f\t%f"
                                , &t
                                , &v
                              );

                if ( r != 2 )
                {
                    printf( "unable to parse line\n" );
                }

                /*write file*/
                sf_write_float(   ofd
                                , &v
                                , 1
                              );
            }

            p = 0x00;
        }

        c = fgetc( ifd );
        line[ p++ ] = c;
    } while ( c != EOF );

    fclose( ifd );
    sf_close( ofd );

    return 0;
}
