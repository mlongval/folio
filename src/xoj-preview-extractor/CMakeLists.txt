## folio-thumbnailer executable ##

if (WIN32)
  # Windows does not support the thumbnailer like this
  return ()
endif ()

add_definitions (-DBUILD_THUMBNAILER)

if (DEV_ENABLE_GCOV)
  add_compile_options("--coverage")
  add_link_options("--coverage")
endif (DEV_ENABLE_GCOV)

add_executable (folio-thumbnailer
        xournalpp-thumbnailer.cpp
        "${PROJECT_SOURCE_DIR}/src/util/GzUtil.cpp"
        "${PROJECT_SOURCE_DIR}/src/util/PlaceholderString.cpp"
        "${PROJECT_SOURCE_DIR}/src/util/StringUtils.cpp"
        "${PROJECT_SOURCE_DIR}/src/util/PathUtil.cpp"
        "${PROJECT_SOURCE_DIR}/src/util/Stacktrace.cpp"
        "${PROJECT_SOURCE_DIR}/src/util/XojPreviewExtractor.cpp"
        )

target_compile_features (folio-thumbnailer PRIVATE cxx_std_20)
target_link_libraries (folio-thumbnailer
  util
  ${ZLIB_LIBRARIES}
  ${librsvg_LDFLAGS}
  ${Glib_LDFLAGS}
  ${ZIP_LDFLAGS}
  )

target_include_directories (folio-thumbnailer PRIVATE ${librsvg_INCLUDE_DIRS})

## Install ##

install (TARGETS folio-thumbnailer
  RUNTIME DESTINATION bin
  COMPONENT folio-thumbnailer
)

## Desktop install targets ##

configure_file (xournalpp-thumbnailer-xoj.schemas xournalpp-thumbnailer-xoj.schemas COPYONLY)

add_custom_target (desktop-install-thumbnailer
  COMMAND "${PROJECT_SOURCE_DIR}/src/xoj-preview-extractor/desktop-install.sh" install
  COMMENT "Install folio-thumbnailer as xoj file thumbnail generator"
)

add_custom_target (desktop-uninstall-thumbnailer
  COMMAND "${PROJECT_SOURCE_DIR}/src/xoj-preview-extractor/desktop-install.sh" uninstall
  COMMENT "Uninstall folio-thumbnailer as xoj file thumbnail generator"
)
