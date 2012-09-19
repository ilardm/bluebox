# Locate sndfile
# This module defines
# SNDFILE_LIBRARY
# SNDFILE_FOUND, if false, do not try to link to sndfile
# SNDFILE_INCLUDE_DIR, where to find the headers
#
# $SNDFILE_DIR is an environment variable that would
# correspond to the ./configure --prefix=$SNDFILE_DIR

# http://code.google.com/p/mgep/source/browse/trunk/CMakeModules/FindSndFile.cmake?r=466

FIND_PATH(SNDFILE_INCLUDE_DIR sndfile.h
    PATH_SUFFIXES sndfile
    PATHS
    ${SNDFILE_DIR}/include
    $ENV{SNDFILE_DIR}/include
    $ENV{SNDFILE_DIR}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /usr/include/sndfile
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)

find_library(SNDFILE_LIBRARY 
    NAMES "sndfile"
    PATHS
    ${SNDFILE_DIR}/lib
    $ENV{SNDFILE_DIR}/lib
    $ENV{SNDFILE_DIR}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/lib
    /usr/lib
    /usr/lib64
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/lib
    /usr/freeware/lib64
    PATH_SUFFIXES
    win32 Darwin Linux ""
)

SET(SNDFILE_FOUND "NO")
IF(SNDFILE_LIBRARY AND SNDFILE_INCLUDE_DIR)
    SET(SNDFILE_FOUND "YES")
ENDIF(SNDFILE_LIBRARY AND SNDFILE_INCLUDE_DIR)


