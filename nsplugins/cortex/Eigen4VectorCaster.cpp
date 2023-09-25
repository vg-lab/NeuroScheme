/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
 *
 * Authors: Iago Calvo <iago.calvo@urjc.es>
 *
 * This file is part of NeuroScheme
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <nslib/Loggers.h>
#include "Eigen4VectorCaster.h"
#include <Eigen/Dense>

namespace nslib
{
  namespace cortex
  {
    int Eigen4VectorCaster::toInt( const fires::Property&, TIntRounding )
    {
      return 0;
    };

    std::string Eigen4VectorCaster::toString( const fires::Property& prop )
    {
      Eigen::Vector4f vector = prop.value<Eigen::Vector4f>( );

      const auto space = std::string(" ");
      std::stringstream ss;
      ss << vector[0] << space << vector[1] << space << vector[2] << space << vector[3];
      return ss.str();
    };

    void Eigen4VectorCaster::fromString(
        fires::Property& property, const std::string& value_ )
    {
      float x = 0.0f;
      float y = 0.0f;
      float z = 0.0f;
      float w = 0.0f;

      int nItemsRead = sscanf( value_.c_str( ), "%f %f %f %f\n", &x, &y, &z,
                               &w );
      if( 4 != nItemsRead )
      {
        Loggers::get( )->log( "ERROR: Cast to eigen::Vector4f failed",
          LOG_LEVEL_WARNING );
      }
      Eigen::Vector4f vector( x, y, z, w );
      property.set( vector );
    };

    std::vector <std::string> Eigen4VectorCaster::categories( void )
    {
      return std::vector<std::string>( );
    }
  }
} // namespace nslib
