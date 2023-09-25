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
#ifndef __NSLIB_VARIABLE_MAPPER__
#define __NSLIB_VARIABLE_MAPPER__

#include <nslib/api.h>
#include <iostream>

namespace nslib
{
  //
  // MapperToFloat
  //

  class NSLIB_API MapperToFloat
  {
  public:
    MapperToFloat( float outMin = 0.0f, float outMax = 1.0f );

    virtual ~MapperToFloat( void ) { }

    virtual float map( void ) const;

    float & outMax( void );

    float & outMin( void );

    protected:
      float _outMin;
      float _outMax;
  };

  //
  // MapperFloatToFloat
  //

  class NSLIB_API MapperFloatToFloat
    : public MapperToFloat
  {
  public:

    typedef enum
    {
      LINEAR,
      LOGARITHMIC,
      EXPONENTIAL
    } TLinearity;

    MapperFloatToFloat( float inMin = 0.0f, float inMax = 0.0f,
                        float outMin = 0.0f, float outMax = 0.0f,
                        TLinearity linearity = LINEAR,
                        float exponent = 2.0f );

    float& inMax( void );
    float& inMin( void );
    float& value( void );
    TLinearity& linearity( void );
    float& exponent( void );

    virtual float map( void ) const;
    virtual float map( const float value );

  protected:

    float _inMin;
    float _inMax;
    float _value;
    TLinearity _linearity;
    float _exponent;
  };
} // namespace nslib

#endif
