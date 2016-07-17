#include "Color.h"

namespace neuroscheme
{

  Color::Color( const uint8_t r_,
                const uint8_t g_,
                const uint8_t b_,
                const uint8_t a_ )
  {
    _rgba[0] = r_;
    _rgba[1] = g_;
    _rgba[2] = b_;
    _rgba[3] = a_;
  }

  Color::Color( const uint8_t rgba[] )
  {
    _rgba[0] = rgba[0];
    _rgba[1] = rgba[1];
    _rgba[2] = rgba[2];
    _rgba[3] = rgba[3];

  }

  uint8_t& Color::r( void ) { return _rgba[0]; }
  uint8_t& Color::g( void ) { return _rgba[1]; }
  uint8_t& Color::b( void ) { return _rgba[2]; }
  uint8_t& Color::a( void ) { return _rgba[3]; }

  const uint8_t& Color::r( void ) const { return _rgba[0]; }
  const uint8_t& Color::g( void ) const { return _rgba[1]; }
  const uint8_t& Color::b( void ) const { return _rgba[2]; }
  const uint8_t& Color::a( void ) const { return _rgba[3]; }

  std::ostream& operator<< ( std::ostream& os, const Color& color )
  {
    os << "Color(" << int( color.r( )) << ',' << int( color.g( )) << ','
       << int( color.b( )) << ',' << int( color.a( )) << ')';
    return os;
  }

}
