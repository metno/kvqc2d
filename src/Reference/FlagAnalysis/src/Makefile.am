bin_PROGRAMS = main

main_SOURCES = \
        main.cc epsx.cc

main_LDADD = \
        /metno/kvalobs/kvalobs-svn/src/lib/milog/libmilog.la \
        /metno/kvalobs/kvalobs-svn/src/lib/kvalobs/libkvalobs.la \
        /metno/kvalobs/kvalobs-svn/src/lib/kvdb/libkvdb.la \
        /metno/kvalobs/kvalobs-svn/src/lib/fileutil/libfileutil.la \
        /metno/kvalobs/kvalobs-svn/src/lib/miutil/libmiutil.la \
        /metno/kvalobs/kvalobs-svn/src/lib/miconfparser/libmiconfparser.a \
        /metno/kvalobs/kvalobs-svn/src/lib/corbahelper/libcorbahelper.la \
        /metno/kvalobs/kvalobs-svn/src/lib/kvskel/libkvadmininterface.la \
        /metno/kvalobs/kvalobs-svn/src/lib/kvskel/libkvskel.la \
        /metno/kvalobs/kvalobs-svn/src/lib/dnmithread/libdnmithread.la \
        $(putools_LIBS) 


main_CPPFLAGS = \
        -I/metno/kvalobs/kvalobs-svn/src/lib \
        -I/metno/kvalobs/kvalobs-svn/src/lib/kvskel \
        -I/metno/kvalobs/kvalobs-svn/src/service-libs \
        $(putools_CFLAGS)

	
main_LDFLAGS = -pthread -export-dynamic	-lkvalobs -lmiutil -lmilog \
	 $(putools_LIBS) 



