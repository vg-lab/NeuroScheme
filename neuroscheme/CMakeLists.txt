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
  Canvas.cpp
  Color.cpp
  DataManager.cpp
  InteractionManager.cpp
  LayoutManager.cpp
  Log.cpp
  MainWindow.cpp
  mappers/ColorMapper.cpp
  mappers/VariableMapper.cpp
  neuroScheme.cpp
  RepresentationCreatorManager.cpp
  reps/CollapseButtonItem.cpp
  reps/RingItem.cpp
  SelectionManager.cpp
  )

set( NEUROSCHEME_HEADERS
  ${PROJECT_BINARY_DIR}/include/neuroscheme/version.h
  Canvas.h
  Color.h
  DataManager.h
  error.h
  InteractionManager.h
  LayoutManager.h
  Log.h
  MainWindow.h
  mappers/ColorMapper.h
  mappers/VariableMapper.h
  RepresentationCreatorManager.h
  reps/CollapsableItem.h
  reps/CollapseButtonItem.h
  reps/InteractiveItem.h
  reps/Item.h
  reps/QGraphicsItemRepresentation.h
  reps/RingItem.h
  reps/SelectableItem.h
  SelectedState.h
  SelectionManager.h
  )

include ( domains/cortex/CMakeLists.txt )

set( NEUROSCHEME_LINK_LIBRARIES
  ShiFT
  Qt5::Widgets
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_BINARY_DIR}/neuroscheme
  ${SHIFT_GENERATED_DIR} )

if (MSVC)
  add_definitions(-DNEUROSCHEME_STATIC)
  # if (WITH_NO_CONSOLE)
  #   message(STATUS "No console, WIN32 added to common app args")
  #   set(COMMON_APP_ARGS WIN32)
  # else()
  #   message(STATUS "neuroscheme with console")
  # endif()
endif()


set( NEUROSCHEME_INCLUDE_NAME neuroscheme )
common_application( NeuroScheme GUI ${COMMON_APP_ARGS})
