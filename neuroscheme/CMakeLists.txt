include( shift_generate_cxx )

# Generate objects
set( SHIFT_GENERATED_OBJECTS Neuron Column )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  shift_generate_cxx(
    "Object"
    ${GENERATED_OBJECT}
    ${CMAKE_CURRENT_SOURCE_DIR}/objs/objs.json
    ${SHIFT_GENERATED_DIR}
    )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )

# Generate representations
set( SHIFT_GENERATED_OBJECTS NeuronRep Ring LayerRep ColumnRep )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  shift_generate_cxx(
    "Representation"
    ${GENERATED_OBJECT}
    ${CMAKE_CURRENT_SOURCE_DIR}/reps/reps.json
    ${SHIFT_GENERATED_DIR}
    )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )

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
  ${SHIFT_GENERATED_IMPL_FILES}
  ${PROJECT_BINARY_DIR}/src/version.cpp
  Canvas.cpp
  Color.cpp
  MainWindow.cpp
  MainWindow.ui
  mappers/ColorMapper.cpp
  mappers/VariableMapper.cpp
  neuroScheme.cpp
  RepresentationCreator.cpp
  reps/ColumnItem.cpp
  reps/ColumnRep.cpp
  reps/NeuronRep.cpp
  reps/NeuronItem.cpp
  reps/RingItem.cpp
  )

set( NEUROSCHEME_HEADERS
  ${SHIFT_GENERATED_HEADERS_FILES}
  ${PROJECT_BINARY_DIR}/include/neuroscheme/version.h
  Canvas.h
  Color.h
  MainWindow.h
  mappers/ColorMapper.h
  mappers/VariableMapper.h
  RepresentationCreator.h
  objs/Neuron.h
  reps/ColumnItem.h
  reps/ColumnRep.h
  reps/NeuronRep.h
  reps/NeuronItem.h
  reps/RingItem.h
  )

set( NEUROSCHEME_LINK_LIBRARIES
  ShiFT
  Qt5::Widgets
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_BINARY_DIR}/neuroscheme
  ${SHIFT_GENERATED_DIR} )

set( NEUROSCHEME_INCLUDE_NAME neuroscheme )
common_application( NeuroScheme )
