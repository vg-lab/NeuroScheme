include( shift_generate_cxx )

# Generate objects
set( SHIFT_GENERATED_OBJECTS
  Neuron Column MiniColumn Layer NeuronTypeAggregation )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  shift_generate_cxx(
    "Entity"
    ${GENERATED_OBJECT}
    ${CMAKE_CURRENT_SOURCE_DIR}/entities.json
    ${SHIFT_GENERATED_DIR}
    )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )

# Generate representations
set( SHIFT_GENERATED_OBJECTS
  NeuronRep Ring LayerRep NeuronTypeAggregationRep NeuronAggregationRep )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  shift_generate_cxx(
    "Representation"
    ${GENERATED_OBJECT}
    ${CMAKE_CURRENT_SOURCE_DIR}/reps.json
    ${SHIFT_GENERATED_DIR}
    )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )

set( NEUROSCHEMELIBCORTEX_SOURCES
  ${SHIFT_GENERATED_IMPL_FILES}
  ColumnItem.cpp
  ColumnRep.cpp
  DataLoader.cpp
  Domain.cpp
  LayerRep.cpp
  MiniColumnItem.cpp
  MiniColumnRep.cpp
  NeuronAggregationItem.cpp
  NeuronTypeAggregationItem.cpp
  NeuronTypeAggregationRep.cpp
  NeuronRep.cpp
  NeuronItem.cpp
  RepresentationCreator.cpp
  )

set( NEUROSCHEMELIBCORTEX_HEADERS
  ${SHIFT_GENERATED_HEADERS_FILES}
  DataLoader.h
  Domain.h
  Neuron.h
  ColumnItem.h
  ColumnRep.h
  LayerRep.h
  MiniColumnItem.h
  MiniColumnRep.h
  NeuronAggregationItem.h
  NeuronTypeAggregationItem.h
  NeuronTypeAggregationRep.h
  NeuronRep.h
  NeuronItem.h
  RepresentationCreator.h
  )

include_directories( ${SHIFT_GENERATED_DIR} )

set( NEUROSCHEMELIBCORTEX_LINK_LIBRARIES
  NeuroSchemeLib
  )

common_library( NeuroSchemeLibCortex )
