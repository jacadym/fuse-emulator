dnl Process this file with autoconf to produce a configure script.
dnl $Id$

dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License along
dnl with this program; if not, write to the Free Software Foundation, Inc.,
dnl 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
dnl
dnl Author contact information:
dnl
dnl E-mail: philip-fuse@shadowmagic.org.uk

dnl Package version
m4_define([fuse_utils_version], [1.1.0])

dnl Product full version
m4_define([fuse_utils_major_version], [1])
m4_define([fuse_utils_minor_version], [1])
m4_define([fuse_utils_micro_version], [0])
m4_define([fuse_utils_nano_version],  [0])
m4_define([fuse_utils_full_version],
          [fuse_utils_major_version.fuse_utils_minor_version.fuse_utils_micro_version.fuse_utils_nano_version])
m4_define([fuse_utils_rc_version],
          [fuse_utils_major_version,fuse_utils_minor_version,fuse_utils_micro_version,fuse_utils_nano_version])

dnl Package info
m4_define([fuse_url], [http://fuse-emulator.sourceforge.net/])
m4_define([fuse_bugreport], [http://sourceforge.net/p/fuse-emulator/bugs/])

AC_INIT([fuse-utils],[fuse_utils_version],[fuse_bugreport],
        [fuse-utils],[fuse_url])
AC_CONFIG_SRCDIR([tzxlist.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST

dnl Use automake to produce `Makefile.in'
AM_INIT_AUTOMAKE([subdir-objects])

dnl Definitions for config.h
AC_DEFINE([FUSE_UTILS_RC_VERSION], [fuse_utils_rc_version],
          [Define version information for win32 executables])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
LT_INIT

dnl Check for host specific programs.
case "$host_os" in
  mingw32*)
    AC_CHECK_TOOL([WINDRES], [windres])
    AC_CHECK_PROGS([MAN2HTML], [man2html])
    AC_CHECK_PROGS([GROFF], [groff])
    AC_CHECK_PROGS([UNIX2DOS], [unix2dos])
    ;;
esac
AM_CONDITIONAL([HAVE_WINDRES], [test "$WINDRES" != ""])

dnl Check non C89 functions
AC_CHECK_FUNCS(fdopen signal)

dnl Check for big endianness
AC_C_BIGENDIAN

dnl Check for header files
AC_CHECK_HEADERS(strings.h)
AC_CHECK_HEADERS(termios.h)

dnl Check for zlib (the UNIX version is called z, Win32 zdll)
AC_MSG_CHECKING(whether to use zlib)
AC_ARG_WITH(zlib,
[  --without-zlib          don't use zlib],
if test "$withval" = no; then zlib=no; else zlib=yes; fi,
zlib=yes)
AC_MSG_RESULT($zlib)
if test "$zlib" = yes; then
  AC_CHECK_HEADERS(
    zlib.h,
    AC_SEARCH_LIBS(compress2, z zdll)
  )
fi

dnl Allow the user to say that various libraries are in one place
AC_ARG_WITH(local-prefix,
[  --with-local-prefix=PFX local libraries installed in PFX (optional)],
CPPFLAGS="$CPPFLAGS -I$withval/include";
CXXFLAGS="$CXXFLAGS -I$withval/include";
LDFLAGS="$LDFLAGS -L$withval/lib",
if test "$prefix" != "NONE"; then
  CPPFLAGS="$CPPFLAGS -I$prefix/include";
  CXXFLAGS="$CXXFLAGS -I$prefix/include";
  LDFLAGS="$LDFLAGS -L$prefix/lib"
fi)

dnl Check if libjpeg is available and supports jpeg_write_scanlines
AC_MSG_CHECKING(whether to use libjpeg)
AC_ARG_WITH(libjpeg,
  [  --without-libjpeg        don't use libjpeg],
  if test "$withval" = no; then libjpeg=no; else libjpeg=yes; fi,
  libjpeg=yes
)
AC_MSG_RESULT($libjpeg)

if test "$libjpeg" = yes; then
  AC_CHECK_HEADER(
    jpeglib.h,
    [AC_CHECK_LIB( jpeg, jpeg_write_scanlines,
      [AC_DEFINE([USE_LIBJPEG], 1,
        [Defined if we're going to be using the installed libjpeg])
        JPEG_LIBS='-ljpeg'],
      [AC_MSG_WARN(jpeg_write_scanlines not found - jpeg output not supported...)],
    )],
    [AC_MSG_WARN(jpeglib.h not found - jpeg output not supported...)]
  )
fi
AM_CONDITIONAL(HAVE_LIBJPEG, test x$JPEG_LIBS != x)
AC_SUBST(JPEG_LIBS)

dnl Check if a version of libpng which supplies png_write_png is available
AC_MSG_CHECKING(whether to use libpng)
AC_ARG_WITH(libpng,
  [  --without-libpng        don't use libpng],
  if test "$withval" = no; then libpng=no; else libpng=yes; fi,
  libpng=yes
)
AC_MSG_RESULT($libpng)
if test "$libpng" = yes; then
  AC_CHECK_HEADER(
    png.h,
    [AC_CHECK_LIB( png, png_write_png,
      [AC_DEFINE([USE_LIBPNG], 1,
        [Defined if we're going to be using the installed libpng])
        PNG_LIBS='-lpng'],
      [AC_MSG_WARN(png_write_png not found - PNG output not supported...)],
    )],
    [AC_MSG_WARN(png.h not found - PNG output not supported...)]
  )
fi
AM_CONDITIONAL(HAVE_LIBPNG, test x$PNG_LIBS != x)
AC_SUBST(PNG_LIBS)

dnl Check whether to use libgcrypt
AC_MSG_CHECKING(whether to use libgcrypt)
AC_ARG_WITH(libgcrypt,
[  --without-libgcrypt     don't use libgcrypt],
if test "$withval" = no; then libgcrypt=no; else libgcrypt=yes; fi,
libgcrypt=yes)
AC_MSG_RESULT($libgcrypt)
if test "$libgcrypt" = yes; then
  AC_CHECK_HEADERS(gcrypt.h,LIBS="$LIBS -lgcrypt",libgcrypt=no)
fi
AM_CONDITIONAL(BUILD_RZXCHECK, test "$libgcrypt" = yes)

dnl Do we want lots of warning messages?
AC_MSG_CHECKING(whether lots of warnings requested)
AC_ARG_ENABLE(warnings,
[  --enable-warnings       give lots of warnings if using gcc],
if test "$enableval" = yes; then
    warnings=yes;
else
    warnings=no;
fi,
warnings=no)
AC_MSG_RESULT($warnings)

dnl If it appears we're using gcc as our compiler, turn on warnings
if test "$ac_cv_c_compiler_gnu" = yes; then
    CFLAGS="$CFLAGS -Wall"
    CXXFLAGS="$CXXFLAGS -Wall"
    dnl And possibly lots of warnings
    if test "$warnings" = yes; then
        CFLAGS="$CFLAGS -Wstrict-prototypes -Wmissing-prototypes -Winline -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Waggregate-return -W -Wsign-compare"
    fi
fi

# Must either find glib or use the replacement (as supplied by libspectrum)
AC_MSG_CHECKING(whether to use glib)
AC_ARG_WITH(glib,
[  --without-glib          don't use glib],
if test "$withval" = no; then glib=no; else glib=yes; fi,
glib=yes)
AC_MSG_RESULT($glib)
if test "$glib" = yes; then
  glib2=no
  AM_PATH_GLIB_2_0(
    2.0.0,
    glib2=yes
    AC_DEFINE([HAVE_LIB_GLIB], 1, [Defined if we've got glib])
    GLIBS="`pkg-config --libs glib-2.0`",
  )
  if test "$glib2" = no; then
    AM_PATH_GLIB(
      1.2.0,
      AC_DEFINE([HAVE_LIB_GLIB], 1, [Defined if we've got glib])
      GLIBS="`glib-config --libs`",
      AC_MSG_WARN(glib not found - using libspectrum's replacement)
    )
  fi
fi

dnl Check that libspectrum is available
AC_ARG_WITH(libspectrum-prefix,
[  --with-libspectrum-prefix=PFX where libspectrum is installed (optional)],
LIBSPEC_CFLAGS=-I$withval/include; LIBSPEC_LIBS=-L$withval/lib,
LIBSPEC_CFLAGS=""; LIBSPEC_LIBS="")
ac_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $LIBSPEC_CFLAGS $GLIB_CFLAGS"
AC_CHECK_HEADERS(libspectrum.h,LIBSPEC_LIBS="$LIBSPEC_LIBS -lspectrum $GLIBS",
	         AC_MSG_ERROR(Sorry - I need libspectrum to be available))
CPPFLAGS="$ac_save_CPPFLAGS"
AC_SUBST(LIBSPEC_CFLAGS)
AC_SUBST(LIBSPEC_LIBS)

# Look for audiofile (default=yes)
AC_MSG_CHECKING(whether to use audiofile)
AC_ARG_WITH(audiofile,
[  --without-audiofile     don't use audiofile],
if test "$withval" = no; then audiofile=no; else audiofile=yes; fi,
audiofile=yes)
AC_MSG_RESULT($audiofile)
if test "$audiofile" = yes; then
  PKG_CHECK_MODULES(
    AUDIOFILE,
    audiofile,
    AC_DEFINE([HAVE_LIB_AUDIOFILE], 1, [Defined if we've got audiofile]),
    true
  )
  if test -z "$AUDIOFILE_LIBS"; then
    AM_PATH_AUDIOFILE(
      0.2.3,
      AC_DEFINE([HAVE_LIB_AUDIOFILE], 1, [Defined if we've got audiofile]),
      audiofile=no
    )
  fi
fi
AM_CONDITIONAL(BUILD_AUDIOTOOLS, test "$audiofile" = yes)

# Look for iconv (default=yes)
AC_MSG_CHECKING(whether to use iconv)
AC_ARG_WITH(iconv,
[  --without-iconv         don't use iconv],
if test "$withval" = no; then iconv=no; else iconv=yes; fi,
iconv=yes)
AC_MSG_RESULT($iconv)
if test "$iconv" = yes; then
  AC_CHECK_HEADERS(locale.h)
  AC_CHECK_FUNCS(setlocale)
  AM_ICONV()
fi
AC_SUBST(LTLIBICONV)

dnl Work out which compatibility routines to use
AC_MSG_CHECKING(which compatibility routines to use)
case "$host_os" in
  amigaos)
    COMPAT_OSNAME='amiga'
    ;;
  mingw32*)
    COMPAT_OSNAME='win32'
    ;;
  *)
    COMPAT_OSNAME='unix'
  ;;
esac
AC_MSG_RESULT($COMPAT_OSNAME)
AM_CONDITIONAL(COMPAT_AMIGA, test "$COMPAT_OSNAME" = 'amiga')
AM_CONDITIONAL(COMPAT_UNIX, test "$COMPAT_OSNAME" = 'unix')
AM_CONDITIONAL(COMPAT_WIN32, test "$COMPAT_OSNAME" = 'win32')

if test "$ac_cv_func_getopt_long" = no; then
  case "$host_os" in
    amigaos|morphos)
      dnl Our getopt_long replacement doesn't work, so don't use it
      ;;
    *)
      compat_getopt=yes
      missing_routines="$missing_routines"'getopt_long '
      ;;
  esac
fi
AM_CONDITIONAL(COMPAT_GETOPT, test "$compat_getopt" = yes)
if test "$ac_cv_func_mkstemp" = no; then
  missing_routines="$missing_routines"'mkstemp '
fi

AC_CONFIG_FILES([
  Makefile
  fuse-utils.qpg
])

AC_OUTPUT
