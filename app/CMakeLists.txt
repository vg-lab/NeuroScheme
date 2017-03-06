set( NEUROSCHEME_SOURCES
  MainWindow.ui
  resources.qrc
  MainWindow.cpp
  neuroScheme.cpp
  )

set( NEUROSCHEME_HEADERS
  MainWindow.h
  )


set( NEUROSCHEME_LINK_LIBRARIES
  NeuroSchemeLib
  NeuroSchemeLibCortex
  )

include_directories(
  ${PROJECT_BINARY_DIR}/app
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/plugins
  ${PROJECT_SOURCE_DIR}/neuroscheme
  ${NEUROSCHEME_SHIFT_GENERATED_DIR} # This line is a temporary WAR
  )

if (MSVC)
  add_definitions(-DNEUROSCHEME_STATIC)
  if (WITH_NO_CONSOLE)
    message(STATUS "No console, WIN32 added to common app args")
    set(COMMON_APP_ARGS WIN32)
  else()
    message(STATUS "neuroscheme with console")
  endif()
endif( )


set( NEUROSCHEME_INCLUDE_NAME neuroscheme )
common_application( NeuroScheme GUI ${COMMON_APP_ARGS})
