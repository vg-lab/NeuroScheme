include( shift_generate_cxx )

# Generate entities
set( SHIFT_GENERATED_OBJECTS
  Neuron Column MiniColumn Layer NeuronTypeAggregation )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
  shift_generate_cxx(
    "Entity"
    ${CMAKE_CURRENT_SOURCE_DIR}/entities.json
    ${SHIFT_GENERATED_DIR}
    ${SHIFT_GENERATED_DIR}/shift_cortex_entities
    "${SHIFT_GENERATED_OBJECTS}"
    )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )
list( APPEND SHIFT_GENERATED_HEADER_FILES
  ${SHIFT_GENERATED_DIR}/shift_cortex_entities.h )

# Generate relationships
set( SHIFT_GENERATED_OBJECTS ConnectsWith AggregatedConnectsWith ParentOf ChildOf )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
shift_generate_cxx(
  "Relationship"
  ${CMAKE_CURRENT_SOURCE_DIR}/entities.json
  ${SHIFT_GENERATED_DIR}
  ${SHIFT_GENERATED_DIR}/shift_cortex_relationshipProperties
  "${SHIFT_GENERATED_OBJECTS}"
  )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )
list( APPEND SHIFT_GENERATED_HEADER_FILES
  ${SHIFT_GENERATED_DIR}/shift_cortex_relationshipProperties.h )
list( APPEND SHIFT_GENERATED_IMPL_FILES
  ${SHIFT_GENERATED_DIR}/shift_cortex_relationshipProperties.cpp )

# Generate representations
set( SHIFT_GENERATED_OBJECTS
  NeuronRep Ring LayerRep NeuronTypeAggregationRep NeuronAggregationRep
  ConnectionArrowRep )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
shift_generate_cxx(
  "Representation"
  ${CMAKE_CURRENT_SOURCE_DIR}/reps.json
  ${SHIFT_GENERATED_DIR}
  ${SHIFT_GENERATED_DIR}/shift_cortex_reps
  "${SHIFT_GENERATED_OBJECTS}"
  )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )
list( APPEND SHIFT_GENERATED_HEADER_FILES
  ${SHIFT_GENERATED_DIR}/shift_cortex_reps.h )

set( NSLIBCORTEX_SOURCES
  ${SHIFT_GENERATED_IMPL_FILES}
  Circle.cpp
  ColumnItem.cpp
  ColumnRep.cpp
  ConnectionArrowItem.cpp
  ConnectionArrowRep.cpp
  DataLoader.cpp
  Domain.cpp
  Eigen4VectorCaster.cpp
  LayerItem.cpp
  LayerRep.cpp
  MiniColumnItem.cpp
  MiniColumnRep.cpp
  NeuronAggregationItem.cpp
  NeuronItem.cpp
  NeuronRep.cpp
  NeuronTypeAggregationItem.cpp
  NeuronTypeAggregationRep.cpp
  RepresentationCreator.cpp
  Triangle.cpp
  )

set( NSLIBCORTEX_HEADERS
  ${SHIFT_GENERATED_HEADERS_FILES}
  DataLoader.h
  Domain.h
  Neuron.h
  ColumnItem.h
  ColumnRep.h
  ConnectionArrowItem.h
  ConnectionArrowRep.h
  LayerRep.h
  MiniColumnItem.h
  MiniColumnRep.h
  NeuronAggregationItem.h
  NeuronTypeAggregationItem.h
  NeuronTypeAggregationRep.h
  NeuronRep.h
  NeuronItem.h
  RepresentationCreator.h
  Circle.h
  Triangle.h
  Eigen4VectorCaster.h
  LayerItem.h
  )

include_directories( ${SHIFT_GENERATED_DIR} )

set( NSLIBCORTEX_LINK_LIBRARIES
  nslib
  )

# To generate own api.h and version.h
set( NSLIBCORTEX_INCLUDE_NAME nslibcortex )
set( NSLIBCORTEX_NAMESPACE  nslibcortex )

common_library( nslibcortex )
