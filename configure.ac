AC_PREREQ([2.61])
#AC_INIT([kvalobs], [2.0.6], [kvoss@met.no])
AC_INIT([kvqc2], [1.2.1], [kvoss@met.no])
AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])
#METNO_SET_VERSION_INFO


#AC_CONFIG_SRCDIR([src/Qc2Main.cc])
#AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS(config.h)

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_LIBTOOL
#AM_PROG_LEX
#AM_PROG_CC_C_O
#AC_PROG_YACC
AC_PROG_CXX
AC_PROG_INSTALL


AC_CONFIG_FILES([Makefile
                 src/Makefile
])


PKG_CHECK_MODULES(putools, puTools)
KV_FIND_OMNIORB4
KV_POSTGRESQL
PKG_CHECK_MODULES(kvcpp, libkvcpp)
PKG_CHECK_MODULES(sqlite3, sqlite3)
PKG_CHECK_MODULES(libxml2, libxml-2.0)
PKG_CHECK_MODULES(libxmlpp, libxml++-2.6)
PKG_CHECK_MODULES(cppunit, cppunit)
#KV_PERL
AX_BOOST_BASE([1.33.1])
AX_BOOST_THREAD
if test -z $BOOST_THREAD_LIB; then AC_MSG_ERROR([Unable to find boost thread library]); fi 
AX_BOOST_REGEX
if test -z $BOOST_REGEX_LIB; then AC_MSG_ERROR([Unable to find boost regex library]); fi 
AX_BOOST_FILESYSTEM
if test -z $BOOST_FILESYSTEM_LIB; then AC_MSG_ERROR([Unable to find boost filesystem library]); fi
AX_BOOST_PROGRAM_OPTIONS
if test -z $BOOST_PROGRAM_OPTIONS_LIB; then AC_MSG_ERROR([Unable to find boost program_options library]); fi
 
#KV_MODEL2KV

AM_INIT_AUTOMAKE(-W -Wall -Werror subdir-objects)

AC_OUTPUT
