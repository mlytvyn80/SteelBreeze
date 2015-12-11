# Process this file with autoconf to produce a configure script.
#
#################################################################################
#                                                                               #
# This configure.ac file is part of SteelBreeze.                                #
#                                                                               #
# SteelBreeze, the geodetic VLBI data analysing software.                       #
# Copyright (C) 2001-2003 Sergei Bolotin, MAO NASU, Kiev, Ukraine.              #
#                                                                               #
# SteelBreeze is free software; you can redistribute it and/or modify           #
# it under the terms of the GNU General Public License as published by          #
# the Free Software Foundation; either version 2 of the License, or             #
# (at your option) any later version.                                           #
#                                                                               #
# SteelBreeze is distributed in the hope that it will be useful,                #
# but WITHOUT ANY WARRANTY; without even the implied warranty of                #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 #
# GNU General Public License for more details.                                  #
#                                                                               #
# You should have received a copy of the GNU General Public License             #
# along with SteelBreeze; if not, write to the Free Software Foundation,        #
# Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                #
#                                                                               #
#                                                                               #
#################################################################################
#
#



########################################################################
# Constants:

AC_PREREQ(2.61)
AC_COPYRIGHT([This file is part of SteelBreeze.

SteelBreeze, the geodetic VLBI data analysing software.
Copyright (C) 2001-2003 Sergei Bolotin, MAO NASU, Kiev, Ukraine.

SteelBreeze is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

SteelBreeze is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SteelBreeze; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.])



########################################################################
# Project name and version:
AC_INIT([SteelBreeze],[2.0.2],[bolotin@mao.kiev.ua])
#AM_INIT_AUTOMAKE(${PACKAGE_NAME}, ${PACKAGE_VERSION})
AM_INIT_AUTOMAKE(SteelBreeze, 2.0.2)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CONFIG_SRCDIR([config.h.in])
#CONFIG_HEADER(config.h)
#AC_CONFIG_HEADER(config.h)
AM_CONFIG_HEADER(config.h)



########################################################################
# Compiler's flags:
GCC2_WARN="-Wall"
GCC2_OPT="-O3 -fomit-frame-pointer -fstrength-reduce -funroll-loops"

EGCS_WARN="$GCC2_WARN -W -Wpointer-arith -Wcast-align -Wconversion -Wredundant-decls"
#-Woverloaded-virtual -Wwrite-strings 
EGCS_OPT="$GCC2_OPT -ffast-math -funsafe-math-optimizations \
-mfpmath=sse -msse -msse2 -mmmx"

# aux flags:
CFLAGS="$CFLAGS -pipe"
CXXFLAGS="$CXXFLAGS -D_GNU_SOURCE -pipe"



########################################################################
# SteelBreeze configure (local) flags:
debug="no"
optimization="no"
qt_libraries=""
qt_includes=""
QT_INCLUDE=""
QT_LIB=""

# user arguments:
AC_ARG_WITH(qt-dir, 
    AS_HELP_STRING([--with-qt-dir=DIR],[where the root of Qt library is installed]),
    [qt_includes="$withval"/include; qt_libraries="$withval"/lib])
AC_ARG_WITH(qt-includes, 
    AS_HELP_STRING([--with-qt-includes=DIR],[where the Qt includes are]),
    [qt_includes="$withval"])
AC_ARG_WITH(qt-libraries,
    AS_HELP_STRING([--with-qt-libraries=DIR],[where the Qt libraries are]),
    [qt_libraries="$withval"])

AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug],[enable debug/developing mode (default)]),
    [if test "x$enableval" == "xyes"; then debug="yes"; fi], 
    [debug="yes"]
)
AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--disable-debug],[disable debug/developing mode (default=enable)]),
    [if test "x$enableval" == "xno"; then debug="no"; fi], 
    [debug="yes"]
)

AC_ARG_ENABLE(optimization,
    AS_HELP_STRING([--enable-optimization],[enable optimization (default=disable)]),
    [if test "x$enableval" == "xyes"; then optimization="yes"; fi], 
    [optimization="no"]
)
AC_ARG_ENABLE(optimization,
    AS_HELP_STRING([--disable-optimization],[disable optimization (default)]),
    [if test "x$enableval" == "xno"; then optimization="no"; fi], 
    [optimization="no"]
)



########################################################################
# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET



########################################################################
# Check for QT's path
AC_MSG_CHECKING(for path to Qt)
if test -n "$QTDIR"; then
 if test -z "$qt_libraries"; then qt_libraries="$QTDIR/lib"; fi
 if test -z "$qt_includes";  then qt_includes="$QTDIR/include"; fi
fi
if test -z "$qt_libraries" || test -z "$qt_includes"; then
   AC_MSG_ERROR(not found)
 else
   QT_INCLUDE="-I$qt_includes"
   QT_LIB="-L$qt_libraries"
   AC_MSG_RESULT(found)
fi
AC_SUBST(QT_INCLUDE)
AC_SUBST(QT_LIB)
      

########################################################################
# Check for moc's path
AC_PATH_PROG(
    MOC, 
    moc, 
    /usr/bin/moc, 
    $QTDIR/bin:$PATH:/usr/bin:/usr/X11R6/bin:/usr/qt/bin:/usr/local/qt/bin
)


########################################################################
# Checks for libraries.
AC_CHECK_LIB([c],  [printf])
AC_CHECK_LIB([m],  [sin])

LDFLAGS="$LDFLAGS $QT_LIB"
AC_CHECK_LIB([qt], [main])


########################################################################
# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h stdlib.h string.h unistd.h fcntl.h])



########################################################################
# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_UID_T
AC_TYPE_SIZE_T



########################################################################
# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([dup2 floor gethostname memset memcpy mkdir sqrt strdup \
strerror strstr uname sincos modf rmdir])



########################################################################
# 
AC_MSG_CHECKING(for debug flags)
# do not strip executable if the debug `-g' is given:
LDFLAGS=`echo $LDFLAGS | sed 's/-s//g'`
# remove duplicates (-Wall will added anyway <grin>, `-g' adds if debug enable, 
# otherwice, it's removed because of the strip `-s' flag for linking):
CFLAGS=`echo $CFLAGS | sed 's/-g//g' | sed 's/-Wall//g'`
CXXFLAGS=`echo $CXXFLAGS | sed 's/-g//g' | sed 's/-Wall//g'`
if test "$debug" == "yes"; then
    CFLAGS="$CFLAGS -g $GCC2_WARN -Wunreachable-code -DDEBUG"
    CXXFLAGS="$CXXFLAGS -g $EGCS_WARN -Wunreachable-code -DDEBUG"
    LDFLAGS="$LDFLAGS"
else
    CFLAGS="$CFLAGS $GCC2_WARN"
    CXXFLAGS="$CXXFLAGS $GCC2_WARN"
    LDFLAGS="$LDFLAGS -s"
fi
AC_MSG_RESULT($debug)
		    

########################################################################
# 
AC_MSG_CHECKING(for optimization flags for ${host})
if test "$optimization" == "yes"; then
    CFLAGS="$CFLAGS $EGCS_OPT"
    CXXFLAGS="$CXXFLAGS $EGCS_OPT"
    case "${host}" in
        i686-pc-linux-gnu)
	    CFLAGS="$CFLAGS -march=pentium4 -fschedule-insns2"
	    CXXFLAGS="$CXXFLAGS -march=pentium4 -fschedule-insns2"
	;;
        i586-pc-linux-gnu)
	    CFLAGS="$CFLAGS -march=pentium -fschedule-insns2"
	    CXXFLAGS="$CXXFLAGS -march=pentium -fschedule-insns2"
	;;
	i486-pc-linux-*)
	    CFLAGS="$CFLAGS -m486"
	    CXXFLAGS="$CXXFLAGS -m486"
	;;
    esac
else
# Comment out this two string to disable -O* flags overwriting:
    CFLAGS=`echo $CFLAGS | sed 's/-O[[0-9]]*//g'`
    CXXFLAGS=`echo $CXXFLAGS | sed 's/-O[[0-9]]*//g'`
fi
AC_MSG_RESULT($optimization)

echo ${PACKAGE_VERSION} > VERSION
SB_VERSION_MAJOR=`cat VERSION | awk -F. '{ print $1 }'`
SB_VERSION_MINOR=`cat VERSION | awk -F. '{ print $2 }'`
SB_VERSION_PATCHLEVEL=`cat VERSION | awk -F. '{ print $3 }'`
SB_VERSION="$SB_VERSION_MAJOR.$SB_VERSION_MINOR.$SB_VERSION_PATCHLEVEL"
AC_SUBST(SB_VERSION_MAJOR)
AC_SUBST(SB_VERSION_MINOR)
AC_SUBST(SB_VERSION_PATCHLEVEL)
AC_SUBST(SB_VERSION)



########################################################################
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 info/Makefile
                 src/Makefile])

AC_OUTPUT



########################################################################
# reporting:
echo "
Configuration:

  Package:                    ${PACKAGE_NAME}-${PACKAGE_VERSION}
  Host System Type:           ${host}
  Source code location:       ${srcdir}
  C   Compiler:               ${CC} ${CFLAGS}
  C++ Compiler:               ${CXX} ${CXXFLAGS}
  QT includes:                ${QT_INCLUDE}
  QT library:                 ${QT_LIB}
  debug:                      ${debug}
  optimization:               ${optimization}
  Install path:               ${prefix}
"
