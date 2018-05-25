gamedatadir = $(pkgdatadir)/data

nobase_dist_gamedata_DATA = images/background.png \
	images/penguin_1_back.png \
	images/penguin_1_color.png \
	images/penguin_1_front.png \
	images/penguin_2_back.png \
	images/penguin_2_color.png \
	images/penguin_2_front.png \
	images/penguin_3_back.png \
	images/penguin_3_color.png \
	images/penguin_3_front.png \
	images/penguin_4_back.png \
	images/penguin_4_color.png \
	images/penguin_4_front.png \
	images/penguin_5_1_front.png \
	images/penguin_5_2_front.png \
	images/penguin_5_3_front.png \
	images/penguin_5_back.png \
	images/penguin_5_color.png \
	images/penguin_6_1_back.png \
	images/penguin_6_1_color.png \
	images/penguin_6_1_front.png \
	images/penguin_6_2_back.png \
	images/penguin_6_2_color.png \
	images/penguin_6_2_front.png \
	images/penguin_6_3_front.png \
	images/penguin_6_4_front.png \
	images/penguin_6_5_front.png \
	images/penguin_6_6_front.png \
	images/penguin_7_back.png \
	images/penguin_7_color.png \
	images/penguin_7_front.png \
	images/penguin_8_1_front.png \
	images/penguin_8_2_front.png \
	images/penguin_8_3_front.png \
	images/penguin_8_back.png \
	images/penguin_8_color.png \
	images/plataform.png

# Instalar los archivos .desktop e iconos
applicationsdir = $(datadir)/applications
applications_DATA = desktop/bean_counters_classic.desktop

icons16dir = $(datadir)/icons/hicolor/16x16/apps
icons16_DATA = desktop/16x16/coffee_bag.png

icons32dir = $(datadir)/icons/hicolor/32x32/apps
icons32_DATA = desktop/32x32/coffee_bag.png

icons48dir = $(datadir)/icons/hicolor/48x48/apps
icons48_DATA = desktop/48x48/coffee_bag.png

icons64dir = $(datadir)/icons/hicolor/64x64/apps
icons64_DATA = desktop/64x64/coffee_bag.png

icons128dir = $(datadir)/icons/hicolor/128x128/apps
icons128_DATA = desktop/128x128/coffee_bag.png

icons256dir = $(datadir)/icons/hicolor/256x256/apps
icons256_DATA = desktop/256x256/coffee_bag.png

# touch the toplevel icon directory as required by the icon theme spec
install-data-hook:
	touch $(DESTDIR)$(datadir)/icons/hicolor

EXTRA_DIST = \
	desktop/bean_counters_classic.desktop	\
	desktop/16x16/coffee_bag.png		\
	desktop/32x32/coffee_bag.png		\
	desktop/48x48/coffee_bag.png		\
	desktop/64x64/coffee_bag.png		\
	desktop/128x128/coffee_bag.png		\
	desktop/256x256/coffee_bag.png		\
	coffee_bag.ico
