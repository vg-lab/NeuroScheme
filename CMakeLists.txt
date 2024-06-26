# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   NeuroScheme
#   2015-2020 (c) VG-LAB / GMRV / URJC / UPM
#   gmrv@gmrv.es
#   www.vg-lab.es
#   www.gmrv.es
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
project( NeuroScheme VERSION 0.7.6 )
set( NeuroScheme_VERSION_ABI 12 )

# Disable in source building
if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
  message( FATAL "no in source building allowed." )
endif()

list( APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/CMake
  ${CMAKE_SOURCE_DIR}/CMake/common )

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif()

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/deps/silo/silo.in.hpp)
  message(FATAL_ERROR "deps/silo/silo.in.hpp missing, run: git submodule update --init")
endif()

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


# description
set( NEUROSCHEME_DESCRIPTION "NeuroScheme" )
set( NEUROSCHEME_MAINTAINER "Pablo Toharia <pablo.toharia@upm.es>")
set( NEUROSCHEME_LICENSE LGPL)
set( COMMON_PROJECT_DOMAIN vg-lab.es )

include( Common )

common_find_package( Boost REQUIRED COMPONENTS unit_test_framework )
common_find_package( ShiFT REQUIRED )
common_find_package( scoop REQUIRED )
common_find_package( Qt5Widgets SYSTEM REQUIRED )
common_find_package( Qt5Xml SYSTEM REQUIRED )
common_find_package( Eigen3 SYSTEM REQUIRED )
if(NEUROSCHEME_NSOL_ENABLED)
  common_find_package( nsol REQUIRED )
endif()
common_find_package( Lexis ${NEUROSCHEME_OPTS_FIND_ARGS} )
common_find_package( ZeroEQ ${NEUROSCHEME_OPTS_FIND_ARGS} )
common_find_package( Servus ${NEUROSCHEME_OPTS_FIND_ARGS} )
common_find_package( gmrvlex ${NEUROSCHEME_OPTS_FIND_ARGS} )
common_find_package( acuterecorder )
common_find_package_post( )

list( APPEND NEUROSCHEME_DEPENDENT_LIBRARIES ShiFT scoop Qt5Widgets acuterecorder )

add_subdirectory( nslib )
add_subdirectory( nsplugins )
add_subdirectory( neuroscheme )

include( CPackConfig )
include( DoxygenRule )
