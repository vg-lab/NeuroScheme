# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   NeuroScheme
#   2016 (c) GMRV / URJC / UPM
#   gmrv@gmrv.es
#   www.gmrv.es
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
project( NeuroScheme VERSION 0.1.6 )
set( NeuroScheme_VERSION_ABI 6 )

# Disable in source building
if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
  message( FATAL "no in source building allowed." )
endif()

list( APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/CMake
  ${CMAKE_SOURCE_DIR}/CMake/common )

# Default build type is Debug
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif( NOT CMAKE_BUILD_TYPE )

if ( CMAKE_BUILD_TYPE MATCHES "Debug" )
  add_definitions( -DDEBUG )
endif( )

option( NEUROSCHEME_WITH_LOGGING "NEUROSCHEME_WITH_LOGGING" ON )
if ( NEUROSCHEME_WITH_LOGGING )
  add_definitions( -DNEUROSCHEME_WITH_LOGGING )
endif( )

if ( NEUROSCHEME_OPTIONALS_AS_REQUIRED )
  set( NEUROSCHEME_OPTS_FIND_ARGS "REQUIRED" )
else()
  set( NEUROSCHEME_OPTS_FIND_ARGS "" )
endif()


include( GitExternal )

# description
set( NEUROSCHEME_DESCRIPTION "NeuroScheme" )
set( NEUROSCHEME_MAINTAINER "Pablo Toharia <pablo.toharia@upm.es>")
set( NEUROSCHEME_LICENSE LGPL)
set( COMMON_PROJECT_DOMAIN gmrv.es )

include( Common )

common_find_package( Boost REQUIRED COMPONENTS unit_test_framework )
common_find_package( ShiFT REQUIRED )
common_find_package( scoop REQUIRED )
common_find_package( Qt5Widgets SYSTEM REQUIRED )
common_find_package( Eigen3 SYSTEM REQUIRED )
common_find_package( nsol ${NEUROSCHEME_OPTS_FIND_ARGS} )
common_find_package( Lexis ${NEUROSCHEME_OPTS_FIND_ARGS} )
common_find_package( ZeroEQ ${NEUROSCHEME_OPTS_FIND_ARGS} )
common_find_package( Servus ${NEUROSCHEME_OPTS_FIND_ARGS} )
common_find_package( gmrvlex ${NEUROSCHEME_OPTS_FIND_ARGS} )
common_find_package_post( )

list( APPEND NEUROSCHEME_DEPENDENT_LIBRARIES ShiFT scoop Qt5Widgets )

add_subdirectory( nslib )
add_subdirectory( nsplugins )
add_subdirectory( neuroscheme )

include( CPackConfig )
include( DoxygenRule )
