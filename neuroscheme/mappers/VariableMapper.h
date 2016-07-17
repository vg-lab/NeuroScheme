#ifndef __NEUROSCHEME_VARIABLE_MAPPER__
#define __NEUROSCHEME_VARIABLE_MAPPER__

#include <iostream>

namespace neuroscheme
{

  //
  // MapperToFloat
  //

  class MapperToFloat
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

  class MapperFloatToFloat
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


} // namespace neuroscheme

// EOF


#endif
