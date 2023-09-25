/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef __NSLIBCORTEX__EIGEN_4_VECTOR_CASTER__
#define __NSLIBCORTEX__EIGEN_4_VECTOR_CASTER__

#include <fires/Property/PropertyCaster.h>

namespace nslib
{
  namespace cortex
  {
    class Eigen4VectorCaster : public fires::PropertyCaster
    {
      public:

      virtual ~Eigen4VectorCaster( void )
      {}

      int toInt( const fires::Property&, TIntRounding ) override;

      std::string toString( const fires::Property& prop ) override;

      void fromString( fires::Property& property,
        const std::string& value_ ) override;

      std::vector <std::string> categories( void ) override;
    };
  }
}

#endif //__NSLIBCORTEX__EIGEN_4_VECTOR_CASTER__
