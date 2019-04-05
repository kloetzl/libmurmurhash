AC_INIT(libmurmurhash, 1.5, kloetzl@evolbio.mpg.de)
AC_CONFIG_SRCDIR([murmurhash.h])
AC_CONFIG_HEADERS([config.h])

AC_PREREQ(2.57)

#	Directory that contains install-sh and other auxiliary files
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR(config)

AM_INIT_AUTOMAKE([1.6 foreign dist-zip tar-ustar filename-length-max=299])

LIB_VERSION=2:0:0

AC_SUBST([VERSION])
AC_SUBST([LIB_VERSION])

AC_SUBST([VERSION])

# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_LIBTOOL

CPPFLAGS="-I\$(top_srcdir) $CPPFLAGS"
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MMAP
AC_CHECK_FUNCS([munmap])

AC_PROG_MAKE_SET

AC_CONFIG_FILES([
	Makefile
	libmurmurhash.pc
	])
AC_OUTPUT

