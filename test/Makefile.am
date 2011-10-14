TESTS = kvqc2dTest

check_PROGRAMS = kvqc2dTest

kvqc2dTest_SOURCES= \
        test_main.cc \
        TestQc2D.cc \
	ConfigParserTest.cc \
	HelpersTest.cc \
	ReadProgramOptionsTest.cc \
	algorithms/AlgorithmTestBase.cc \
	algorithms/AkimaSplineTest.cc \
	algorithms/SingleLinearTest.cc

#	-DSETUPDB_SQL=\"$(srcdir)/test/database/setupdb.sql\"

SETUP_DB = \"$(srcdir)/test/database/setupdb.sql\"
#SETUP_DB = \"$(top_srcdir)/src/kvalobs_database/kvalobs_schema.sql\"

kvqc2dTest_CPPFLAGS = \
	$(TEST_DEPS_CFLAGS) \
	$(gtest_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)
	$(omniDynamic4_CFLAGS) \
	-DDBDRIVER=\"$(top_builddir)/src/lib/dbdrivers/.libs/sqlite3driver.so\" \
	-DSETUPDB_SQL=$(SETUP_DB) \
	-DCHECKRUNNERTEST_INIT_SQL=\"$(srcdir)/test/etc/checkrunnertest.init.sql\" \
	-DLOG_CHECK_SCRIPT 

kvqc2dTest_LDADD = ../src/libkvqc2d.a \
	$(TEST_DEPS_LIBS) \
	$(gtest_LIBS) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_REGEX_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(omniDynamic4_LIBS) \
        -lnetcdf_c++

EXTRA_DIST = \
	test/database/setupdb.sql \
	test/etc/checkrunnertest.init.sql
