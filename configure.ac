AC_INIT(murmurhash, 1, kloetzl@evolbio.mpg.de)
AC_CONFIG_HEADERS([config.h])

AC_PREREQ(2.57)

#	Directory that contains install-sh and other auxiliary files
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR(config)

AM_INIT_AUTOMAKE([1.6 foreign dist-zip tar-ustar filename-length-max=299])

LIB_VERSION=1:1

AC_SUBST([VERSION])
AC_SUBST([LIB_VERSION])

AC_SUBST([VERSION])

# Checks for programs.
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_LIBTOOL

################################################################################
# calling AC_PROG_CXX resets CXXFLAGS, we use our own flags set in the
#	the AX_CXXFLAGS macro below.
# So we cache the argument to configure
#	here in ARG_CXX_FLAGS (so that our macro does not override them)
################################################################################
ARG_CXX_FLAGS="$CXXFLAGS"
AC_PROG_CXX

#Ranlib handled by check for libtool
CXXFLAGS="$ARG_CXX_FLAGS"
AX_CXXFLAGS

AC_SUBST(CXXFLAGS)

CPPFLAGS="-I\$(top_srcdir) $CPPFLAGS"
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

AC_PROG_MAKE_SET

AC_CONFIG_FILES([
	Makefile
	libmurmurhash.pc
	])
AC_OUTPUT

