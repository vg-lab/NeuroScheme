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
  bool Config::_showConnectivity = false;
  float Config::_scale = 1.f;

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

  bool Config::showConnectivity( void )
  {
    return _showConnectivity;
  }

  void Config::showConnectivity( bool showConnectivity_ )
  {
    _showConnectivity = showConnectivity_;
  }

  float Config::scale( void )
  {
    return _scale;
  }

  void Config::scale( float scale_ )
  {
    _scale = scale_;
  }

}
