#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys, math

# === definitions ==============================================================
DPX = [   1209
        , 1336
        , 1477
        , 1633
        ]
DPY = [   697
        , 770
        , 852
        , 941
        ]
TONES = {   "0": ( DPX[1], DPY[3] )
          , "1": ( DPX[0], DPY[0] )
          , "2": ( DPX[1], DPY[0] )
          , "3": ( DPX[2], DPY[0] )
          , "4": ( DPX[0], DPY[1] )
          , "5": ( DPX[1], DPY[1] )
          , "6": ( DPX[2], DPY[1] )
          , "7": ( DPX[0], DPY[2] )
          , "8": ( DPX[1], DPY[2] )
          , "9": ( DPX[2], DPY[2] ) 
          , "A": ( DPX[3], DPY[0] )
          , "B": ( DPX[3], DPY[1] )
          , "C": ( DPX[3], DPY[2] )
          , "D": ( DPX[3], DPY[3] )
          , "*": ( DPX[0], DPY[3] )
          , "#": ( DPX[2], DPY[3] )
          , "p": ( 0, 0 )
          }

TONELEN = 70 * 10**-3   # ms
SAMPLERATE = 8 * 10**3  # hz

def signal( freq, time ):
    return (  math.sin( 2.0 * math.pi * freq[0] * time )
            + math.sin( 2.0 * math.pi * freq[1] * time )
            )

# === body =====================================================================

ARGV = sys.argv
ARGC = len( sys.argv )

if ( ARGC < 3 ):
    print "specify a number and output file"
    sys.exit( 1 )

NUM = ARGV[1]
OFNAME = ARGV[2]

if ( not TONES.has_key( NUM ) ):
    print "incorrect number specified"

    SUP = ""
    for k in TONES:
        SUP += k
    print "available numbers: '%s'" % SUP

    sys.exit( 1 )

OFD = open( OFNAME, "w" )
TIME = 0
while True:
    if ( TIME > TONELEN ):
        break

    OFD.write( "%.6f\t%.6f\n" % (   TIME
                                  , signal( TONES[ NUM ], TIME)
                                  )
               )

    TIME += 1.0/SAMPLERATE

OFD.close()
print "done"

