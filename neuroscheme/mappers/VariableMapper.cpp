/**
 * @file    VariableMapper.cpp
 * @brief
 * @author  Pablo Toharia <pablo.toharia@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 Do not distribute without further notice.
*/

#include "VariableMapper.h"
#include <stdexcept>
#include <cmath>

namespace neuroscheme
{

  //
  // MapperToFloat
  //

  MapperToFloat::MapperToFloat( float outMin_,
                                float outMax_ )
    : _outMin(outMin_),
      _outMax(outMax_)
  {
  }

  float MapperToFloat:: map( void ) const
  {
    std::cerr << "Error: map method not implemented" << std::endl;
    return 0;
  }

  float & MapperToFloat::outMax( void )
  {
    return _outMax;
  }

  float & MapperToFloat::outMin( void )
  {
    return _outMin;
  }


  //
  // MapperFloatToFloat
  //


  MapperFloatToFloat::MapperFloatToFloat( float inMin_,
                                          float inMax_,
                                          float outMin_,
                                          float outMax_,
                                          TLinearity linearity_,
                                          float exponent_ )
    : MapperToFloat( outMin_, outMax_ )
    , _inMin( inMin_ )
    , _inMax( inMax_ )
    , _value( 0.0f )
    , _linearity( linearity_ )
    , _exponent( exponent_ )
  {

  }

  float & MapperFloatToFloat::inMax( void )
  {
    return _inMax;
  }

  float & MapperFloatToFloat::inMin( void )
  {
    return _inMin;
  }

  float & MapperFloatToFloat::value( void )
  {
    return _value;
  }

  MapperFloatToFloat::TLinearity& MapperFloatToFloat::linearity( void )
  {
    return _linearity;
  }

  float & MapperFloatToFloat::exponent( void )
  {
    return _exponent;
  }

  float MapperFloatToFloat::map( void ) const
  {

#ifdef DEBUG
    if (_value > _inMax || _value < _inMin) {
      std::cerr << "MapperFloatToFloat: warning, value "
                << _value << " out of range [" <<  _inMin << ","
                << _inMax << "]"
                << std::endl;
    }
#endif

    if (_value > _inMax)
      return _outMax;
    if (_value < _inMin)
      return _outMin;

    float normalizedInValue = ((_value - _inMin) / (_inMax - _inMin));
    float normalizedOutValue = 0.0f;
    switch ( _linearity )
    {
    case LINEAR:
      normalizedOutValue = normalizedInValue;
      break;

    case LOGARITHMIC:
      normalizedOutValue = std::pow( normalizedInValue, 1.0f / _exponent );
      break;

    case EXPONENTIAL:
      normalizedOutValue =
        std::pow( 2.0f,
                  std::pow( normalizedInValue, _exponent )) - 1.0f;
      break;

    default:
      throw std::runtime_error( "MapperFloatToFloat: not known "
                                "linearity relation" );
    }
    return normalizedOutValue * (_outMax - _outMin) + _outMin;

  }

  float MapperFloatToFloat::map( const float value_ )
  {
    this->value() = value_;
    return this->map();
  }



} // namespace neurosheme

// EOF
