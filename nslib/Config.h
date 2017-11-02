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
#ifndef __NSLIB_CONFIG__
#define __NSLIB_CONFIG__
#include <nslib/api.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>

namespace nslib
{

  class NeuroSchemeInputArguments
    : public std::unordered_map< std::string, std::vector< std::string >>
  {
  public:

    bool has( const std::string& arg )
    {
      return this->count( arg ) == 1;
    }

    bool hasOne( const std::vector< std::string >& args )
    {
      for ( const auto& arg : args )
      {
        if (( this->count( arg ) == 1 ))
          return true;
      }
      return false;
    }

    std::string checkIfOnlyOne( const std::vector< std::string >& args,
                                unsigned int& numArgsFound )
    {
      std::string argFound( "" );
      numArgsFound = 0;
      for ( const auto& arg : args )
      {
        if ( this->count( arg ) == 1 )
        {
          ++numArgsFound;
          argFound = arg;
        }
      }
      if ( numArgsFound > 1 )
        return std::string( );
      return argFound;
    }

  };


  class Config
  {

  public:
    NSLIB_API
    static NeuroSchemeInputArguments& inputArgs( void );

    NSLIB_API
    static std::string isArgumentDefined(
      const std::vector< std::string >& argNames );

    NSLIB_API
    static bool showConnectivity( void );

    NSLIB_API
    static void showConnectivity( bool showConnectivity_ );

    NSLIB_API
    static float scale( void );

    NSLIB_API
    static void scale( float scale );

  protected:
    static NeuroSchemeInputArguments _inputArgs;
    static bool _showConnectivity;
    static float _scale;
  };

}

#endif // __NSLIB_CONFIG__
