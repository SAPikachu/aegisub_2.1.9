AUTOMAKE_OPTIONS = foreign


if HAVE_UNIVCHARDET
univchardet = universalchardet
endif

SUBDIRS = \
	$(univchardet) \
	src \
	automation \
	po \
	desktop

osx-bundle:
	@SHELL@ scripts/osx-bundle.sh "@BUNDLE_STRING@" @AEGISUB_VERSION_DATA@ DICT_DIR=$(DICT_DIR)


osx-dmg:
	@SHELL@ scripts/osx-dmg.sh "@BUNDLE_STRING@" "@DMG_STRING@"

osx-tinderbox-bundle:
	@SHELL@ scripts/osx-bundle.sh "$(T_BUNDLE)" @AEGISUB_VERSION_DATA@ DICT_DIR="$(DICT_DIR)"

osx-tinderbox-dmg:
	@SHELL@ scripts/osx-dmg.sh "$(T_DMG)" "$(T_BUNDLE)"
	mv  "$(T_DMG).dmg" bundle.dmg

EXTRA_DIST = \
	intltool-extract.in \
	intltool-merge.in   \
	intltool-update.in  \
	config.guess \
	config.sub \
	depcomp \
	install-sh \
	LICENCE \
	missing \
	INSTALL \
	README \
	svn_revision

# m4macros/
EXTRA_DIST += \
	m4macros/check_gnu_make.m4 \
	m4macros/boost.m4 \
	m4macros/acx_pthread.m4 \
	m4macros/ac_agi.m4 \
	m4macros/ax_lang_compiler_ms.m4 \
	m4macros/ac_agi_mdcpucfg.m4 \
	m4macros/ax_openmp.m4 \
	m4macros/ax_check_gl.m4 \
	m4macros/ac_flag.m4

# scripts/
EXTRA_DIST += \
	scripts/osx-bundle-restart-helper.c \
	scripts/osx-bundle.sh \
	scripts/osx-dmg.sh \
	scripts/osx-fix-libs.py \
	scripts/unix-gen-res.awk \
	scripts/unix-gen-xpm.awk

# packages/osx_bundle/
EXTRA_DIST += \
	packages/osx_bundle/Contents/Info.plist \
	packages/osx_bundle/Contents/Resources/*.icns \
	packages/osx_bundle/Contents/Resources/etc/fonts/fonts.conf \
	packages/osx_bundle/Contents/Resources/etc/fonts/fonts.dtd \
	packages/osx_bundle/Contents/Resources/etc/fonts/conf.d/*.conf

# packages/osx_dmg/
EXTRA_DIST += \
	packages/osx_dmg/dmg_background.png \
	packages/osx_dmg/dmg_set_style.applescript


CLEANFILES= \
	intltool-extract \
	intltool-merge \
	intltool-update
