AC_PREREQ([2.61])
AC_INIT([kvqc2d], [1.3.1], [kvalobs-dev@met.no])
AM_INIT_AUTOMAKE([silent-rules color-tests])
AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])

#AC_PROG_LIBTOOL presently we have no shared libraries
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB

AC_SEARCH_LIBS([pj_init_plus], [proj])
AC_SEARCH_LIBS([nc_create], [netcdf_c++])
KV_FIND_OMNIORB4
KV_POSTGRESQL
PKG_CHECK_MODULES([PROG_DEPS], [puTools >= 3.3.0 libkvcpp >= 2.2.0 gsl >= 1.13])
PKG_CHECK_MODULES([TEST_DEPS], [puTools >= 3.3.0 libkvcpp >= 2.2.0 gsl >= 1.13 sqlite3 >= 3.6.0])
GTEST_CHECK

AX_BOOST_BASE([1.33.1])
AX_BOOST_THREAD
if test -z $BOOST_THREAD_LIB; then AC_MSG_ERROR([Unable to find boost thread library]); fi 
AX_BOOST_REGEX
if test -z $BOOST_REGEX_LIB; then AC_MSG_ERROR([Unable to find boost regex library]); fi 
AX_BOOST_FILESYSTEM
if test -z $BOOST_FILESYSTEM_LIB; then AC_MSG_ERROR([Unable to find boost filesystem library]); fi
AX_BOOST_PROGRAM_OPTIONS
if test -z $BOOST_PROGRAM_OPTIONS_LIB; then AC_MSG_ERROR([Unable to find boost program_options library]); fi

AC_CONFIG_HEADERS(src/config.h)
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT
