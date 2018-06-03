# Automake file for Bean Counters Classic

gamedatadir = $(pkgdatadir)/data

bin_PROGRAMS = bean-counters-classic
bean_counters_classic_SOURCES = beans.c \
	gfx_blit_func.c gfx_blit_func.h \
	path.c path.h \
	gettext.h

if MACOSX
bean_counters_classic_SOURCES += SDLMain.m SDLMain.h
endif

if MINGW32
bean_counters_classic_SOURCES += 
endif

EXTRA_DIST = coffee_bag.rc SDLMain.m SDLMain.h

if MINGW32
coffee_bag_ico.o: coffee_bag.rc ../data/coffee_bag.ico
	$(WINDRES) $(srcdir)/coffee_bag.rc -O coff -o coffee_bag_ico.o

mingw_ldadd = coffee_bag_ico.o

else # NOT MINGW
mingw_ldadd =
endif

bean_counters_classic_CPPFLAGS = -DGAMEDATA_DIR=\"$(gamedatadir)/\" -DLOCALEDIR=\"$(localedir)\" $(AM_CPPFLAGS)
bean_counters_classic_CFLAGS = $(SDL_CFLAGS) $(SDL_image_CFLAGS) $(SDL_mixer_CFLAGS) $(SDL_ttf_CFLAGS) $(AM_CFLAGS)
if MACOSX
# En MAC OS X, hay que ligar/compilar contra los frameworks
bean_counters_classic_LDFLAGS = -Wl,-rpath,@loader_path/../Frameworks $(AM_LDFLAGS)
else
bean_counters_classic_LDADD = $(SDL_LIBS) $(SDL_image_LIBS) $(SDL_mixer_LIBS) $(SDL_ttf_LIBS) -lm $(mingw_ldadd)
endif
LDADD = $(LIBINTL)

#------------------ Packaging rules for Mac OSX ------------------------

bundle_root = $(top_builddir)/etc/macfiles
bundle_name = $(bundle_root)/BeanCountersClassic.app
# Special target to copy all needed game files to PaddlePuffle.app/Contents/Resources.
# For Mac OS X bundle.
.PHONY: bundle
bundle:
	-rm -rf $(bundle_root)
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources/data/
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	cp $(top_builddir)/etc/Info.plist $(bundle_name)/Contents/
	cp $(top_builddir)/etc/coffee_bag.icns $(bundle_name)/Contents/Resources/
	mkdir -p $(bundle_name)/Contents/Resources/data/images
	cp -R $(top_builddir)/data/images/* $(bundle_name)/Contents/Resources/data/images
	mkdir -p $(bundle_name)/Contents/Resources/data/music
	cp -R $(top_builddir)/data/music/* $(bundle_name)/Contents/Resources/data/music
	mkdir -p $(bundle_name)/Contents/Resources/data/sounds
	cp -R $(top_builddir)/data/sounds/* $(bundle_name)/Contents/Resources/data/sounds
	cp $(top_builddir)/data/*.ttf $(bundle_name)/Contents/Resources/data
	for cat in $(ALL_LINGUAS); do \
	  dir=$(bundle_name)/Contents/Resources/locale/$$cat/LC_MESSAGES; \
	  mkdir -p $$dir; \
	  echo "installing $$cat as $$dir/bean-counters-classic.mo"; \
	  cp $(top_builddir)/po/$$cat.gmo $$dir/bean-counters-classic.mo; \
	done

# Special target to copy the binary to PaddlePuffle.app/Contents/MacOS.
.PHONY: bundle-fw
bundle-fw: bundle
	cp bean-counters-classic $(bundle_name)/Contents/MacOS/bean-counters-classic
	mkdir  $(bundle_name)/Contents/Frameworks
	for i in SDL SDL_mixer SDL_image SDL_ttf ; do \
	  cp -a /Library/Frameworks/$${i}.framework $(bundle_name)/Contents/Frameworks/; \
	done
	strip $(bundle_name)/Contents/MacOS/bean-counters-classic

# Special target to make a disk image of the bundled app
.PHONY: bundle-dmg
bundle-dmg: bundle-fw
	ln -s /Applications $(bundle_root)/Applications
	hdiutil create -ov -srcfolder $(bundle_root) -volname BeanCountersClassic -imagekey zlib-level=6 $(top_builddir)/etc/bean-counters-classic.dmg

