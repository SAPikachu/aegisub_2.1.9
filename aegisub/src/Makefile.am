AUTOMAKE_OPTIONS = foreign
SUFFIXES = .c .cpp .rc
noinst_LIBRARIES= 
AM_CXXFLAGS = 
DISTCLEANFILES =

if BUILD_DARWIN
libosxutil_subdir = libosxutil
libosxutil_lib = libosxutil/libosxutil.a
libosxutil_ldflags = -framework CoreFoundation
endif

if HAVE_AUTO3_LUA
libauto3 = libauto3
endif

if PRECOMPILED_HEADER
BUILT_SOURCES = stdwx.h.gch
precompiled_header = stdwx.h
AM_CXXFLAGS += -Winvalid-pch -fpch-deps -fpch-preprocess -include stdwx.h
nodist_aegisub_2_1_SOURCES = stdwx.h.gch
DISTCLEANFILES += stdwx.h.gch
endif

SUBDIRS = \
	bitmaps \
	libresrc \
	$(libauto3) \
	$(libosxutil_subdir)

AM_CXXFLAGS += -DAEGISUB -Iinclude @CXXFLAGS_OSX@ @WX_CXXFLAGS@ @OPENMP_CXXFLAGS@ @LIBASS_CFLAGS@

bin_PROGRAMS = aegisub-2.1
aegisub_2_1_LDADD = libresrc/libresrc.a $(libosxutil_lib)
aegisub_2_1_CXXFLAGS = @FREETYPE_CFLAGS@ $(AM_CXXFLAGS)
aegisub_2_1_LDFLAGS = @DEBUG_FLAGS@ @PROFILE_FLAGS@ @GL_LIBS@ @PTHREAD_LIBS@ @WX_LIBS@ @ICONV_LIBS@ $(libosxutil_ldflags) @CCMALLOC_LDFLAGS@ @EFENCE_LDFLAGS@
LIBS += @FREETYPE_LIBS@ @FONTCONFIG_LIBS@ @CCMALLOC_LIBS@

if PRECOMPILED_HEADER
# This doesn't depend on Makefile on purpose, you should already know what you're doing when using this.
stdwx.h.gch: stdwx.h
	@CXX@ -include ../acconf.h $(DEFAULT_INCLUDES) @CXXFLAGS@ $(AM_CXXFLAGS) @DEBUG_FLAGS@ @PROFILE_FLAGS@ stdwx.h
endif

if BUILD_DARWIN
aegisub_2_1_LDFLAGS += -L/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries
endif

noinst_LIBRARIES += libaudio_player.a
libaudio_player_a_SOURCES = audio_player.cpp
libaudio_player_a_CPPFLAGS = @ALSA_CFLAGS@ @PORTAUDIO_CFLAGS@ @LIBPULSE_CFLAGS@ @OPENAL_CFLAGS@
aegisub_2_1_LDADD += libaudio_player.a

if HAVE_ALSA
noinst_LIBRARIES += libaudio_alsa.a
libaudio_alsa_a_SOURCES = audio_player_alsa.cpp
aegisub_2_1_LDADD += libaudio_alsa.a @ALSA_LIBS@
endif

if HAVE_PORTAUDIO
noinst_LIBRARIES += libaudio_portaudio.a
libaudio_portaudio_a_SOURCES = audio_player_portaudio.cpp
libaudio_portaudio_a_CPPFLAGS = @PORTAUDIO_CFLAGS@
aegisub_2_1_LDADD += libaudio_portaudio.a @PORTAUDIO_LIBS@
endif

if HAVE_PULSEAUDIO
noinst_LIBRARIES += libaudio_pulseaudio.a
libaudio_pulseaudio_a_SOURCES = audio_player_pulse.cpp
libaudio_pulseaudio_a_CPPFLAGS = @LIBPULSE_CFLAGS@
aegisub_2_1_LDADD += libaudio_pulseaudio.a @LIBPULSE_LIBS@
endif

if HAVE_OPENAL
noinst_LIBRARIES += libaudio_openal.a
libaudio_openal_a_SOURCES = audio_player_openal.cpp
libaudio_openal_a_CPPFLAGS = @OPENAL_CFLAGS@
aegisub_2_1_LDADD += libaudio_openal.a @OPENAL_LIBS@
endif

if HAVE_OSS
noinst_LIBRARIES += libaudio_oss.a
libaudio_oss_a_SOURCES = audio_player_oss.cpp
aegisub_2_1_LDADD += libaudio_oss.a
endif

if HAVE_PROVIDER_FFMPEGSOURCE
noinst_LIBRARIES += libaudiovideo_ffmpegsource.a
libaudiovideo_ffmpegsource_a_SOURCES = audio_provider_ffmpegsource.cpp video_provider_ffmpegsource.cpp ffmpegsource_common.cpp
libaudiovideo_ffmpegsource_a_CPPFLAGS = @FFMS_CFLAGS@
aegisub_2_1_LDADD += libaudiovideo_ffmpegsource.a @FFMS_LIBS@
endif

noinst_LIBRARIES += libsubtitle_provider.a
libsubtitle_provider_a_SOURCES = subtitles_provider.cpp
aegisub_2_1_LDADD += libsubtitle_provider.a

if WITH_LIBASS
noinst_LIBRARIES += libsubtitle_ass.a
libsubtitle_ass_a_SOURCES = subtitles_provider_libass.cpp
LIBS += @LIBASS_LIBS@
aegisub_2_1_LDADD += libsubtitle_ass.a
endif

if HAVE_AUTO4_LUA
noinst_LIBRARIES += libauto4_lua.a
libauto4_lua_a_SOURCES = auto4_lua.cpp auto4_lua_assfile.cpp auto4_lua_dialog.cpp auto4_lua_scriptreader.cpp
libauto4_lua_a_CPPFLAGS = @LUA_CFLAGS@
LIBS += @LUA_LDFLAGS@
aegisub_2_1_LDADD += libauto4_lua.a
endif

if HAVE_AUTO4_PERL
noinst_LIBRARIES += libauto4_perl.a
libauto4_perl_a_SOURCES = auto4_perl.cpp auto4_perl_script.cpp auto4_perl_dialogs.cpp auto4_perl_ass.cpp auto4_perl_console.cpp auto4_perl.h auto4_perl_console.h
libauto4_perl_a_CPPFLAGS = @PERL_CFLAGS@
LIBS += @PERL_LDFLAGS@
aegisub_2_1_LDADD += libauto4_perl.a
endif

if HAVE_AUTO4_RUBY
noinst_LIBRARIES += libauto4_ruby.a
libauto4_ruby_a_SOURCES = auto4_ruby_assfile.cpp auto4_ruby.cpp auto4_ruby_dialog.cpp
libauto4_ruby_a_CPPFLAGS = @RUBY_CFLAGS@
aegisub_2_1_LDFLAGS += @RUBY_LDFLAGS@
aegisub_2_1_LDADD += libauto4_ruby.a
endif

if BUILD_DARWIN
AUTO3_LIB = $(srcdir)/libauto3/.libs/libaegisub-auto3-2.1.0.dylib
else
AUTO3_LIB = $(srcdir)/libauto3/.libs/libaegisub-auto3-2.1.so
endif

if HAVE_AUTO3_LUA
AUTO3_LUA = auto4_auto3.cpp
# This is a workaround top stop libtool from binging in dependency_libs which
# causes many link errors due to auto4 (lua 5.1) support.
#aegisub_2_1_LDADD += -L$(srcdir)/../auto3 -laegisub-auto3-2.1
aegisub_2_1_LDADD += ${AUTO3_LIB}
endif

if HAVE_HUNSPELL
noinst_LIBRARIES += libmisc_hunspell.a
libmisc_hunspell_a_SOURCES = spellchecker_hunspell.cpp
aegisub_2_1_LDADD += libmisc_hunspell.a @HUNSPELL_LIBS@
endif

if HAVE_UNIVCHARDET
noinst_LIBRARIES += libmisc_universalchardet.a
libmisc_universalchardet_a_SOURCES = charset_detect.cpp text_file_reader.cpp
libmisc_universalchardet_a_CPPFLAGS = -D_X86_
aegisub_2_1_LDADD += libmisc_universalchardet.a ../universalchardet/libuniversalchardet.a
else
UNIVCHARSET = charset_detect.cpp text_file_reader.cpp
endif

if FONTLISTER_FREETYPE
  FONT_LISTER = font_file_lister_freetype.cpp
endif

if FONTLISTER_FONTCONFIG
  FONT_LISTER = font_file_lister_fontconfig.cpp
endif


## These aren't built, but are listed here so 'make dist' can always find all the sources
## This should also list all Win32 specific files
EXTRA_aegisub_2_1_SOURCES = \
	$(precompiled_header) \
	font_file_lister.cpp \
	$(FONT_LISTER) \
	audio_player_dsound.cpp \
	audio_player_dsound2.cpp \
	audio_player_portaudio.cpp \
	audio_player_pulse.cpp \
	audio_player_oss.cpp \
	audio_provider_avs.cpp \
	auto4_auto3.cpp \
	auto4_lua.cpp \
	auto4_lua_assfile.cpp \
	auto4_lua_dialog.cpp \
	auto4_lua_scriptreader.cpp \
	auto4_ruby.cpp \
	auto4_ruby_assfile.cpp \
	auto4_ruby_dialog.cpp \
	avisynth_wrap.cpp \
	dialog_associations.cpp \
	setup.cpp \
	spellchecker_hunspell.cpp \
	stdwx.cpp \
	subtitle_format_prs.cpp \
	subtitles_provider_csri.cpp \
	subtitles_provider_libass.cpp \
	video_provider_avs.cpp \
	video_provider_dshow.cpp


aegisub_2_1_SOURCES = \
	$(AUTO3_LUA) \
	$(HUNSPELL) \
	$(LIBASS) \
	$(UNIVCHARSET) \
	font_file_lister.cpp \
	$(FONT_LISTER) \
	MatroskaParser.c \
	aegisublocale.cpp \
	ass_attachment.cpp \
	ass_dialogue.cpp \
	ass_entry.cpp \
	ass_export_filter.cpp \
	ass_exporter.cpp \
	ass_file.cpp \
	ass_karaoke.cpp \
	ass_override.cpp \
	ass_style.cpp \
	ass_style_storage.cpp \
	ass_time.cpp \
	audio_box.cpp \
	audio_display.cpp \
	audio_karaoke.cpp \
	audio_provider.cpp \
	audio_provider_convert.cpp \
	audio_provider_downmix.cpp \
	audio_provider_hd.cpp \
	audio_provider_pcm.cpp \
	audio_provider_ram.cpp \
	audio_provider_stream.cpp \
	audio_spectrum.cpp \
	auto4_base.cpp \
	avisynth_wrap.cpp \
	base_grid.cpp \
	browse_button.cpp \
	colorspace.cpp \
	colour_button.cpp \
	dialog_about.cpp \
	dialog_associations.cpp \
	dialog_attachments.cpp \
	dialog_automation.cpp \
	dialog_colorpicker.cpp \
	dialog_detached_video.cpp \
	dialog_dummy_video.cpp \
	dialog_export.cpp \
	dialog_fonts_collector.cpp \
	dialog_jumpto.cpp \
	dialog_kanji_timer.cpp \
	dialog_options.cpp \
	dialog_paste_over.cpp \
	dialog_progress.cpp \
	dialog_properties.cpp \
	dialog_resample.cpp \
	dialog_search_replace.cpp \
	dialog_selection.cpp \
	dialog_shift_times.cpp \
	dialog_spellchecker.cpp \
	dialog_style_editor.cpp \
	dialog_style_manager.cpp \
	dialog_styling_assistant.cpp \
	dialog_text_import.cpp \
	dialog_timing_processor.cpp \
	dialog_tip.cpp \
	dialog_translation.cpp \
	dialog_version_check.cpp \
	dialog_video_details.cpp \
	drop.cpp \
	audio_provider_dummy.cpp \
	export_clean_info.cpp \
	export_fixstyle.cpp \
	export_framerate.cpp \
	export_visible_lines.cpp \
	fft.cpp \
	frame_main.cpp \
	frame_main_events.cpp \
	gl_text.cpp \
	gl_wrap.cpp \
	help_button.cpp \
	hilimod_textctrl.cpp \
	hotkeys.cpp \
	idle_field_event.cpp \
	kana_table.cpp \
	keyframe.cpp \
	main.cpp \
	md5.c \
	mkv_wrap.cpp \
	mythes.cxx \
	options.cpp \
	plugin_manager.cpp \
	scintilla_text_ctrl.cpp \
	spellchecker.cpp \
	spline.cpp \
	spline_curve.cpp \
	standard_paths.cpp \
	static_bmp.cpp \
	string_codec.cpp \
	subs_edit_box.cpp \
	subs_edit_ctrl.cpp \
	subs_grid.cpp \
	subs_preview.cpp \
	subtitle_format.cpp \
	subtitle_format_ass.cpp \
	subtitle_format_dvd.cpp \
	subtitle_format_ebu3264.cpp \
	subtitle_format_encore.cpp \
	subtitle_format_microdvd.cpp \
	subtitle_format_mkv.cpp \
	subtitle_format_srt.cpp \
	subtitle_format_transtation.cpp \
	subtitle_format_ttxt.cpp \
	subtitle_format_txt.cpp \
	text_file_writer.cpp \
	thesaurus.cpp \
	thesaurus_myspell.cpp \
	timeedit_ctrl.cpp \
	toggle_bitmap.cpp \
	tooltip_manager.cpp \
	utils.cpp \
	validators.cpp \
	variable_data.cpp \
	vector2d.cpp \
	version.cpp \
	vfr.cpp \
	video_box.cpp \
	video_context.cpp \
	video_display.cpp \
	video_frame.cpp \
	video_out_gl.cpp \
	video_provider_cache.cpp \
	video_provider_dummy.cpp \
	video_provider_manager.cpp \
	video_slider.cpp \
	visual_feature.cpp \
	visual_tool.cpp \
	visual_tool_clip.cpp \
	visual_tool_cross.cpp \
	visual_tool_drag.cpp \
	visual_tool_rotatexy.cpp \
	visual_tool_rotatez.cpp \
	visual_tool_scale.cpp \
	visual_tool_vector_clip.cpp


aegisub_2_1_SOURCES += \
	$(srcdir)/*.h \
	$(srcdir)/include/aegisub/*.h \
	$(srcdir)/config/*.h

noinst_HEADERS = \
	$(srcdir)/boost/*.hpp \
	gl/glext.h \
	$(srcdir)/*.hxx

EXTRA_DIST = \
	auto4_perldata.inc \
	$(srcdir)/*.hxx

