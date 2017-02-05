include( shift_generate_cxx )

set( Name NeuroScheme )
set( NAME NEUROSCHEME )
set( namespace neuroscheme )
set( NAMESPACE NEUROSCHEME )
set( INCLUDE_NAME neuroscheme )
set( PROJECT_VERSION_ABI ${${PROJECT_NAME}_VERSION_ABI} )
configure_file(
  ${CMAKE_SOURCE_DIR}/CMake/common/cpp/api.h
  ${PROJECT_BINARY_DIR}/include/neuroscheme/api.h @ONLY )
configure_file(
  ${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.h
  ${PROJECT_BINARY_DIR}/include/neuroscheme/version.h @ONLY )
configure_file(
  ${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.cpp
  ${PROJECT_BINARY_DIR}/src/version.cpp @ONLY )

set( NEUROSCHEME_SOURCES
  ${PROJECT_BINARY_DIR}/src/version.cpp
  MainWindow.ui
  resources.qrc
  Canvas.cpp
  # Color.cpp
  Config.cpp
  DataManager.cpp
  DomainManager.cpp
  FilterWidget.cpp
  InteractionManager.cpp
  Log.cpp
  MainWindow.cpp
  # mappers/ColorMapper.cpp
  neuroScheme.cpp
  PaneManager.cpp
  RepresentationCreatorManager.cpp
  ScatterPlotWidget.cpp
  SelectionManager.cpp
  SortWidget.cpp
  ZeroEQManager.cpp
  layouts/CircularLayout.cpp
  layouts/GridLayout.cpp
  layouts/Layout.cpp
  layouts/CameraBasedLayout.cpp
  layouts/ScatterPlotLayout.cpp
  mappers/VariableMapper.cpp
  reps/CollapseButtonItem.cpp
  reps/RingItem.cpp
  reps/SelectableItem.cpp
  qxt/qxtspanslider.cpp
  )

set( NEUROSCHEME_HEADERS
  ${PROJECT_BINARY_DIR}/include/neuroscheme/version.h
  CameraBasedLayout.h
  Canvas.h
  Color.h
  Config.h
  DataManager.h
  DomainManager.cpp
  error.h
  FilterWidget.h
  InteractionManager.h
  Log.h
  MainWindow.h
  # mappers/ColorMapper.h
  PaneManager.h
  RepresentationCreatorManager.h
  ScatterPlotWidget.h
  SelectedState.h
  SelectionManager.h
  SortWidget.h
  ZeroEQManager.h
  layouts/CircularLayout.h
  layouts/GridLayout.h
  layouts/Layout.h
  layouts/LayoutManager.h
  layouts/ScatterPlotLayout.h
  mappers/VariableMapper.h
  qxt/qxtspanslider.h
  qxt/qxtspanslider_p.h
  reps/CollapsableItem.h
  reps/CollapseButtonItem.h
  reps/InteractiveItem.h
  reps/Item.h
  reps/QGraphicsItemRepresentation.h
  reps/RingItem.h
  reps/SelectableItem.h
  )

include ( domains/cortex/CMakeLists.txt )
include ( domains/spines/CMakeLists.txt )

set( NEUROSCHEME_LINK_LIBRARIES
  ShiFT
  scoop
  Qt5::Widgets
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_BINARY_DIR}/neuroscheme
  ${SHIFT_GENERATED_DIR} )


if ( TARGET ZeroEQ AND TARGET Lexis AND TARGET Servus )
  list( APPEND NEUROSCHEME_LINK_LIBRARIES Lexis ZeroEQ Servus )
endif( )

if ( TARGET gmrvlex )
  list( APPEND NEUROSCHEME_LINK_LIBRARIES gmrvlex )
endif( )

if ( TARGET nsol )
  list( APPEND NEUROSCHEME_LINK_LIBRARIES nsol )
endif( )


if (MSVC)
  add_definitions(-DNEUROSCHEME_STATIC)
  # if (WITH_NO_CONSOLE)
  #   message(STATUS "No console, WIN32 added to common app args")
  #   set(COMMON_APP_ARGS WIN32)
  # else()
  #   message(STATUS "neuroscheme with console")
  # endif()
endif( )


set( NEUROSCHEME_INCLUDE_NAME neuroscheme )
common_application( NeuroScheme GUI ${COMMON_APP_ARGS})
