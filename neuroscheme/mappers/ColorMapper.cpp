/**
 * @file    ColorMapper.cpp
 * @brief
 * @author  Pablo Toharia <pablo.toharia@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 Do not distribute without further notice.
 */

#include "ColorMapper.h"

namespace neuroscheme
{

  //
  // Color
  //

  ColorMapper::ColorMapper( float min_, float max_  )
  {
    _min = min_;
    _max = max_;
  }

  Color ColorMapper::map( void ) const
  {
    return valueToColor ( _value );
  }

  Color ColorMapper::valueToColor( float /* value */ ) const
  {
    assert( false );
    return Color( );
  }

  float& ColorMapper::min( void )
  {
    return _min;
  }

  float& ColorMapper::max( void )
  {
    return _max;
  }

  float& ColorMapper::value( void )
  {
    return _value;
  }


  //
  // ConstantColorMapper
  //

  ConstantColorMapper::ConstantColorMapper( Color color_ )
    : ColorMapper( 0, 0 )
  {
    _color = color_;
  }

  Color ConstantColorMapper::valueToColor( float /* value */ ) const
  {
    return _color;
  }

  Color& ConstantColorMapper::color()
  {
    return _color;
  }


  //
  // DiscreteColorMapper
  //


  DiscreteColorMapper::DiscreteColorMapper( float min_,
                                            float max_ )
    : ColorMapper( min_, max_ )
  {
  }

  Color DiscreteColorMapper::valueToColor( float value_ ) const
  {

    int numColors = ( int ) this->size( );

    int colorIdx = round (( numColors - 1 ) *
                          ( value_ - _min ) / (_max - _min));

    if ( colorIdx < 0 || colorIdx > ( int ) this->size( ) - 1 )
    {
      std::cerr << "DiscreteColorMapper: value " << value_
                << " out of range" << std::endl;
      return Color( 0, 0, 0 );
    }
    return ( *this )[colorIdx];
  }



} // namespace nore

// EOF
