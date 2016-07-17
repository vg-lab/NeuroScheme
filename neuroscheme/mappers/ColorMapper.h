/**
 * @file    ColorMapper.h
 * @brief
 * @author  Pablo Toharia <pablo.toharia@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 Do not distribute without further notice.
 */
#ifndef __NEUROSCHEME__COLOR_MAPPER__
#define __NEUROSCHEME__COLOR_MAPPER__

#include <vector>
#include <iostream>
#include <math.h>
#include <assert.h>

#include "../Color.h"

namespace neuroscheme
{


  class ColorMapper
  {

  public:

    ColorMapper( float min = 0.0, float max = 0.0 );

    virtual ~ColorMapper( void ) { };

    virtual Color map ( void ) const;
    virtual Color valueToColor( float value = 0.0f ) const;

    float& min ( void );
    float& max ( void );
    float& value ( void );

  protected:
    float _min, _max;
    float _value;

  };

  class ConstantColorMapper
    : public ColorMapper
  {

    ConstantColorMapper( Color color = Color( ));
    virtual ~ConstantColorMapper( void ) { };
    virtual Color valueToColor( float value = 0.0f ) const;
    Color& color( void );

  protected:

    Color _color;

  };

  class DiscreteColorMapper
    : public ColorMapper
    , public std::vector<Color>
  {

  public:

    DiscreteColorMapper( float min = 0.0f, float max = 0.0f );
    virtual ~DiscreteColorMapper( void ) { };
    virtual Color valueToColor( float value ) const;

  protected:

  };

  class HotColdColorMapper
    : public ColorMapper
    , public std::vector< Color >
  {

  public:

    HotColdColorMapper(float min, float max);
    virtual ~HotColdColorMapper( void ) { };

    virtual Color valueToColor(float value) const;


  };



} // namespace nore

// EOF


#endif
