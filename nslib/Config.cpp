/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 *
 * This file is part of NeuroScheme
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include "Config.h"

namespace nslib
{

  NeuroSchemeInputArguments& Config::inputArgs( void )
  {
    return _inputArgs;
  }

  NeuroSchemeInputArguments Config::_inputArgs = NeuroSchemeInputArguments( );

#define NSLIB_CONFIG_INIT( TYPE, NAME, VALUE )  \
  TYPE Config::_##NAME = VALUE

  NSLIB_CONFIG_INIT( bool, showConnectivity, false );
  NSLIB_CONFIG_INIT( bool, showNoHierarchyEntities, false );
  NSLIB_CONFIG_INIT( bool, autoPublishSelection, true );
  NSLIB_CONFIG_INIT( bool, autoPublishFocusOnSelection, false );
  NSLIB_CONFIG_INIT( bool, autoPublishFocusOnDisplayed, false );
  NSLIB_CONFIG_INIT( float, scale, 1.0f );
  NSLIB_CONFIG_INIT( std::string, zeroEQSession, "" );

  std::string Config::isArgumentDefined(
      const std::vector< std::string >& argNames )
  {
    for ( const auto& arg : argNames )
    {
      if ( _inputArgs.count( arg ) > 0 )
        return arg;
    }
    return std::string( );
  }

}
