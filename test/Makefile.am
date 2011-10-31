TESTS = kvqc2dTest

check_PROGRAMS = kvqc2dTest

kvqc2dTest_SOURCES= \
        test_main.cc \
	ConfigParserTest.cc \
	FlagMatcherTest.cc \
	HelpersTest.cc \
	ReadProgramOptionsTest.cc \
	algorithms/AlgorithmHelpersTest.cc \
	algorithms/AlgorithmTestBase.cc \
	algorithms/AkimaSplineTest.cc \
	algorithms/DipTestTest.cc \
	algorithms/RedistributionTest.cc \
	algorithms/SingleLinearTest.cc

#	-DSETUPDB_SQL=\"$(srcdir)/test/database/setupdb.sql\"

SETUP_DB = \"$(srcdir)/test/database/setupdb.sql\"
#SETUP_DB = \"$(top_srcdir)/src/kvalobs_database/kvalobs_schema.sql\"

kvqc2dTest_CPPFLAGS = -Wall -W \
	$(TEST_DEPS_CFLAGS) \
	$(gtest_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)
	$(omniDynamic4_CFLAGS) \
	-DDBDRIVER=\"$(top_builddir)/src/lib/dbdrivers/.libs/sqlite3driver.so\" \
	-DSETUPDB_SQL=$(SETUP_DB) \
	-DCHECKRUNNERTEST_INIT_SQL=\"$(srcdir)/test/etc/checkrunnertest.init.sql\" \
	-DLOG_CHECK_SCRIPT 

#kvqc2dTest_CPPFLAGS += -ftest-coverage -fprofile-arcs
#kvqc2dTest_LDFLAGS  += -fprofile-arcs

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
