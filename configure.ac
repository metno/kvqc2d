AC_PREREQ([2.61])
AC_INIT([kvqc2d], [1.4.1~rc8], [kvalobs-dev@met.no])
AM_INIT_AUTOMAKE([1.10 foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB

KV_POSTGRESQL
PKG_CHECK_MODULES([PROG_DEPS], [omniORB4 >= 4.0 libkvcpp2 >= 2.5.2a gsl >= 1.8])
PKG_CHECK_MODULES([TEST_DEPS], [omniORB4 >= 4.0 libkvcpp2 >= 2.5.2a gsl >= 1.8 sqlite3 >= 3.6])
GTEST_CHECK

AX_BOOST_BASE([1.40.0])
AX_BOOST_THREAD
if test -z $BOOST_THREAD_LIB; then AC_MSG_ERROR([Unable to find boost thread library]); fi
AX_BOOST_FILESYSTEM
if test -z $BOOST_FILESYSTEM_LIB; then AC_MSG_ERROR([Unable to find boost filesystem library]); fi

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        src/interpolation/Makefile
        src/statisticalmean/Makefile
        src/helpers/Makefile
        test/Makefile
])
AC_OUTPUT
