INCLUDES = $(all_includes) -I$(top_srcdir)/include -I$(top_builddir)
AM_CFLAGS=-Wall $(LIBOSMOCORE_CFLAGS) $(LIBOSMOGSM_CFLAGS) $(LIBOSMOVTY_CFLAGS) $(LIBOSMOABIS_CFLAGS) $(COVERAGE_CFLAGS)
AM_LDFLAGS = $(COVERAGE_LDFLAGS)

if HAVE_LIBCDK
bin_PROGRAMS = bs11_config isdnsync meas_vis
else
bin_PROGRAMS = bs11_config isdnsync
endif

bs11_config_SOURCES = bs11_config.c
bs11_config_LDADD = $(top_builddir)/src/libcommon/libcommon.a \
		    $(top_builddir)/src/libbsc/libbsc.a \
		    $(top_builddir)/src/libtrau/libtrau.a \
		    $(LIBOSMOCORE_LIBS) $(LIBOSMOGSM_LIBS) $(LIBOSMOABIS_LIBS)

isdnsync_SOURCES = isdnsync.c

meas_vis_SOURCES = meas_vis.c
meas_vis_LDADD = $(LIBOSMOCORE_LIBS) $(LIBOSMOGSM_LIBS) -lcdk -lncurses
meas_vis_CFLAGS = $(LIBOSMOCORE_CFLAGS) $(LIBOSMOGSM_CFLAGS)
