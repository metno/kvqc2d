AM_CPPFLAGS = -D_REENTRANT \
	$(kvcpp_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniORB4_CFLAGS)  

bin_PROGRAMS = kvqc2d

kvqc2d_SOURCES = \
	CheckedDataCommandBase.h \
	CheckedDataHelper.cc \
	CheckedDataHelper.h \
	kvMetadataTable.cc \
	kvMetadataTable.h \
	InitLogger.cc \
	InitLogger.h \
	ProcessControl.cc \
	ProcessControl.h \
	ProcessImpl.cc \
	ProcessImpl.h \
	Qc2App.cc \
	Qc2App.h \
	Qc2Connection.h \
	Qc2Main.cc \
	Qc2Thread.cc \
	Qc2Thread.h \
	ReadProgramOptions.cc \
	ReadProgramOptions.h \
	StopWatch.cc \
	StopWatch.h \
	algorithms/BasicStatistics.h \
	algorithms/Distribute.cc \
	algorithms/Distribute.h \
	algorithms/kvQABaseTypes.h \
	algorithms/Process4D.cc \
	algorithms/ProcessRedistribution.cc \
	algorithms/ProcessUnitT.cc \
	algorithms/SingleMinMaxAverage.cc \
	algorithms/SingleLinear.cc \
	algorithms/SingleLinear_v32.cc \
	algorithms/DipTest.cc \
	algorithms/Interpolate.cc \
	algorithms/ProcessSpaceCheck.cc \
	algorithms/ProcessVariability.cc \
	algorithms/FlagTester.cc \
	algorithms/StressTester.cc \
	algorithms/proj++.cc \
	algorithms/proj++.h \
	algorithms/Qc2D.cc \
	algorithms/Qc2D.h \
	algorithms/Qc3D.cc \
	algorithms/Qc3D.h \
	algorithms/scone.h \
	algorithms/ParseParValFile.cc \
	algorithms/ParseParValFile.h \
	algorithms/StationSelection.cc \
	algorithms/StationSelection.h \
	algorithms/table_delaunay.cc \
	algorithms/table_delaunay.h \
	algorithms/AkimaSpline.cc \
	algorithms/AkimaSpline.h \
	algorithms/AkimaTest.cc\
	algorithms/tround.h 



kvqc2d_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(omniDynamic4_CFLAGS) \
	$(putools_CFLAGS) \
	$(perl_CFLAGS)

kvqc2d_LDFLAGS = -pthread -export-dynamic $(BOOST_LDFLAGS)

kvqc2d_LDADD = \
	$(kvcpp_LIBS) \
	$(putools_LIBS) \
	$(BOOST_THREAD_LIB) \
	$(BOOST_REGEX_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(omniDynamic4_LIBS) \
	$(perl_LIBS) \
        -lproj \
        -lnetcdf \
        -lnetcdf_c++ \
        -lgsl \
        -lgslcblas \
     	-ldl \
        -lm         


clean-local:
	rm -rf test/var

TESTS = kvqc2dTest

check_PROGRAMS = kvqc2dTest

kvqc2dTest_SOURCES= \
        test/Test.cc \
        test/TestQc2D.cc \
	$(kvqc2d_SOURCES:kvqc2d.cc=)

#	-DSETUPDB_SQL=\"$(srcdir)/test/database/setupdb.sql\"

SETUP_DB = \"$(srcdir)/test/database/setupdb.sql\"
#SETUP_DB = \"$(top_srcdir)/src/kvalobs_database/kvalobs_schema.sql\"
	
kvqc2dTest_CPPFLAGS = \
	-DDBDRIVER=\"$(top_builddir)/src/lib/dbdrivers/.libs/sqlite3driver.so\" \
	-DSETUPDB_SQL=$(SETUP_DB) \
	-DCHECKRUNNERTEST_INIT_SQL=\"$(srcdir)/test/etc/checkrunnertest.init.sql\" \
	$(kvqc2d_CPPFLAGS) \
	$(cppunit_CFLAGS) \
	-DLOG_CHECK_SCRIPT 
	
kvqc2dTest_LDADD = \
	$(kvqc2d_LDADD) \
	$(cppunit_LIBS)
	
EXTRA_DIST = \
	test/database/setupdb.sql \
	test/etc/checkrunnertest.init.sql

                        
#sqldir=$(datadir)/kvsynopd
#sql_DATA=kvsynopd_schema.sql
#bin_SCRIPTS = kvsynopd_initdb

include test/make.mk

