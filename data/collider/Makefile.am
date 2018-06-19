collidergamedatadir = $(pkgdatadir)/data/collider

nobase_dist_collidergamedata_DATA = \
	penguin_1.col \
	penguin_2.col \
	penguin_3.col \
	penguin_4.col \
	penguin_5.col \
	penguin_6.col \
	penguin_7.col \
	penguin_8.col \
	penguin_9.col \
	penguin_10.col \
	bag_3.col \
	oneup.col

noinst_PROGRAMS = penguin-generator collider-generator
penguin_generator_SOURCES = generate-penguins.c \
	savepng.c savepng.h \
	gfx_blit_func.c gfx_blit_func.h

penguin_generator_CPPFLAGS = -DDATA_DIR=\"$(top_srcdir)/data\" -DBUILD_DIR=\"$(builddir)\" $(AM_CPPFLAGS)
penguin_generator_CFLAGS = $(SDL_CFLAGS) $(SDL_image_CFLAGS) $(AM_CFLAGS)
if MACOSX
# En MAC OS X, hay que ligar/compilar contra los frameworks
penguin_generator_LDFLAGS = -Wl,-rpath,@loader_path/../Frameworks $(AM_LDFLAGS)
else
penguin_generator_LDADD = $(SDL_LIBS) $(SDL_image_LIBS) -lm -lpng
endif

collider_generator_SOURCES = generate-collider.c
collider_generator_CFLAGS = $(SDL_CFLAGS) $(SDL_image_CFLAGS) $(AM_CFLAGS)
if MACOSX
# En MAC OS X, hay que ligar/compilar contra los frameworks
collider_generator_LDFLAGS = -Wl,-rpath,@loader_path/../Frameworks $(AM_LDFLAGS)
else
collider_generator_LDADD = $(SDL_LIBS) $(SDL_image_LIBS)
endif

#SUFFIXES = .png .col

COLLIDER_GENERATOR=$(builddir)/collider-generator
PENGUIN_GENERATOR=$(builddir)/penguin-generator
PENGUINS=1 2 3 4 5 6 7 8 9 10
colliders: collider-generator penguin-generator
	$(PENGUIN_GENERATOR)
	$(COLLIDER_GENERATOR) penguin_1.png penguin_1.col
	$(COLLIDER_GENERATOR) penguin_2.png penguin_2.col
	$(COLLIDER_GENERATOR) penguin_3.png penguin_3.col
	$(COLLIDER_GENERATOR) penguin_4.png penguin_4.col
	$(COLLIDER_GENERATOR) penguin_5.png penguin_5.col
	$(COLLIDER_GENERATOR) penguin_6.png penguin_6.col
	$(COLLIDER_GENERATOR) penguin_7.png penguin_7.col
	$(COLLIDER_GENERATOR) penguin_8.png penguin_8.col
	$(COLLIDER_GENERATOR) penguin_9.png penguin_9.col
	$(COLLIDER_GENERATOR) penguin_10.png penguin_10.col
	
	rm penguin_1.png penguin_2.png penguin_3.png penguin_4.png penguin_5.png penguin_6.png penguin_7.png penguin_8.png penguin_9.png penguin_10.png
	$(COLLIDER_GENERATOR) $(top_srcdir)/data/images/bag_3.png bag_3.col
	$(COLLIDER_GENERATOR) $(top_srcdir)/data/images/oneup.png oneup.col
