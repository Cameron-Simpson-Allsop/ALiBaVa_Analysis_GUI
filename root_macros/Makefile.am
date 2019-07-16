EXTRA_DIST=alibava_linkdef.h AsciiRoot.h AsciiRoot.cc DataFileRoot.cc DataFileRoot.h \
               HDFRoot.cc HDFRoot.h compile.C rootlogon.C \
  Tracer.h Tracer.cc Data.h Hit.cc Hit.h utils.h utils.cc tstLib.cc dbg_lib.cc \
  Tracer.cc ChanList.h ChanList.cc UserMakefile \
  analysis.cc analysis.h sin_preguntas.C sin_preguntas.h rootlogon.C utils.cc utils.h \
  Hit.cc Hit.h fix_header.C run_analysis.cc crrc.cc crrc.h \
  compile_macro 


if USE_ROOT
INCLUDES      = @ROOT_CFLAGS@ @HDF5_INC@ -I$(top_srcdir)/root_macros 
lib_LTLIBRARIES = libAlibavaRoot.la
libAlibavaRoot_la_SOURCES = alibava_linkdef.h \
               analysis.cc analysis.h \
               DataFileRoot.cc DataFileRoot.h \
               HDFRoot.cc HDFRoot.h \
               AsciiRoot.cc AsciiRoot.h \
               ChanList.cc ChanList.h \
               Tracer.cc Tracer.h \
               utils.cc utils.h \
               Hit.cc Hit.h Data.h \
               sin_preguntas.h crrc.cc crrc.h

rootHeaders = analysis.h DataFileRoot.h AsciiRoot.h HDFRoot.h Tracer.h utils.h Hit.h Data.h ChanList.h sin_preguntas.h crrc.h
libAlibavaRootincludedir = $(includedir)/alibava/root
libAlibavaRootinclude_HEADERS = $(rootHeaders)
libAlibavaRoot_la_LDFLAGS = @HDF5_LIB@ @ROOT_LIBS@ -lSpectrum
nodist_libAlibavaRoot_la_SOURCES = alibavaCint.cc

alibavaCint.cc alibavaCint.h: $(rootHeaders)  alibava_linkdef.h
	@echo "Generating dictionary ..."
	@ROOT_CINT@ -v4 -f $@ -c -p $(INCLUDES)  $^ 

bin_PROGRAMS=run_analysis
noinst_PROGRAMS=tstLib dump_data dbg_lib
#
run_analysis_SOURCES = run_analysis.cc
run_analysis_LDFLAGS = @HDF5_LIB@ @ROOT_LIBS@
run_analysis_LDADD = libAlibavaRoot.la
#
tstLib_SOURCES = tstLib.cc
tstLib_LDFLAGS = @HDF5_LIB@ @ROOT_LIBS@
tstLib_LDADD = libAlibavaRoot.la
#
dump_data_SOURCES=dump_data.C 
dump_data_LDFLAGS=@HDF5_LIB@ @ROOT_LIBS@
dump_data_LDADD=libAlibavaRoot.la
#
dbg_lib_SOURCES=dbg_lib.cc
dbg_lib_LDFLAGS=@HDF5_LIB@ @ROOT_LIBS@
dbg_lib_LDADD=libAlibavaRoot.la

clean-generic:
	rm -f alibavaCint.cc alibavaCint.h

endif

