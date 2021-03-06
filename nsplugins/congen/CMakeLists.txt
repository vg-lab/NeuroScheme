include( shift_generate_cxx )

# Generate objects
set( SHIFT_GENERATED_OBJECTS
  NeuronPop NeuronSuperPop Input Output )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
shift_generate_cxx(
  "Entity"
  ${CMAKE_CURRENT_SOURCE_DIR}/entities.json
  ${SHIFT_GENERATED_DIR}
  ${SHIFT_GENERATED_DIR}/shift_congen_entities
  "${SHIFT_GENERATED_OBJECTS}"
  )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )
list( APPEND SHIFT_GENERATED_HEADER_FILES
  ${SHIFT_GENERATED_DIR}/shift_congen_entities.h )

# Generate relationships
set( SHIFT_GENERATED_OBJECTS
  ConnectsWith AggregatedConnectsWith ParentOf ChildOf )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
shift_generate_cxx(
  "Relationship"
  ${CMAKE_CURRENT_SOURCE_DIR}/entities.json
  ${SHIFT_GENERATED_DIR}
  ${SHIFT_GENERATED_DIR}/shift_congen_relationshipProperties
  "${SHIFT_GENERATED_OBJECTS}"
  )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )
list( APPEND SHIFT_GENERATED_HEADER_FILES
  ${SHIFT_GENERATED_DIR}/shift_congen_relationshipProperties.h )
list( APPEND SHIFT_GENERATED_IMPL_FILES
  ${SHIFT_GENERATED_DIR}/shift_congen_relationshipProperties.cpp )

# Generate representations
set( SHIFT_GENERATED_OBJECTS CongenPopRep NeuronSuperPopRep ConnectionArrowRep )
set( SHIFT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/ShiFT )
file( MAKE_DIRECTORY ${SHIFT_GENERATED_DIR} )
shift_generate_cxx(
  "Representation"
  ${CMAKE_CURRENT_SOURCE_DIR}/reps.json
  ${SHIFT_GENERATED_DIR}
  ${SHIFT_GENERATED_DIR}/shift_congen_reps
  "${SHIFT_GENERATED_OBJECTS}"
  )
foreach( GENERATED_OBJECT ${SHIFT_GENERATED_OBJECTS} )
  list( APPEND SHIFT_GENERATED_IMPL_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.cpp )
  list( APPEND SHIFT_GENERATED_HEADER_FILES
    ${SHIFT_GENERATED_DIR}/shift_${GENERATED_OBJECT}.h )
endforeach( )

set( NSLIBCONGEN_SOURCES
  ${SHIFT_GENERATED_IMPL_FILES}
  ConnectionArrowItem.cpp
  ConnectionArrowRep.cpp
  AutoConnectionArrowItem.cpp
  AutoConnectionArrowRep.cpp
  DataLoader.cpp
  Domain.cpp
  CongenPopRep.cpp
  CongenPopItem.cpp
  NeuronSuperPopRep.cpp
  NeuronSuperPopItem.cpp
  RepresentationCreator.cpp
  XMLExporter.cpp
  DataSaver.cpp  
  )

set( NSLIBCONGEN_HEADERS
  ${SHIFT_GENERATED_HEADERS_FILES}
  ConnectionArrowItem.h
  ConnectionArrowRep.h
  AutoConnectionArrowItem.h
  AutoConnectionArrowRep.h
  DataLoader.h
  Domain.h
  CongenPopRep.h
  CongenPopItem.h
  NeuronSuperPopRep.h
  NeuronSuperPopItem.h
  RepresentationCreator.h
  XMLExporter.h
  DataSaver.h  
  )

include_directories( ${SHIFT_GENERATED_DIR} )

set( NSLIBCONGEN_LINK_LIBRARIES
  nslib
  )

# To generate own api.h and version.h
set( NSLIBCONGEN_INCLUDE_NAME nslibcongen )
set( NSLIBCONGEN_NAMESPACE  nslibcongen )

common_library( nslibcongen )
