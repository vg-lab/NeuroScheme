#ifndef __NEUROSCHEME__COLOR__
#define __NEUROSCHEME__COLOR__

#include <stdint.h>
#include <iostream>

namespace neuroscheme
{

  class Color
  {

  public:

    Color( const uint8_t r = 0,
           const uint8_t g = 0,
           const uint8_t b = 0,
           const uint8_t a = 0 );


    Color( const uint8_t rgba[] );

    uint8_t& r( void );
    uint8_t& g( void );
    uint8_t& b( void );
    uint8_t& a( void );

    const uint8_t& r( void ) const;
    const uint8_t& g( void ) const;
    const uint8_t& b( void ) const;
    const uint8_t& a( void ) const;

  protected:

    //! RGBA values whithin [0..255] range
    uint8_t _rgba[4];


  }; // class Color

  std::ostream& operator<< ( std::ostream& os, const Color& color );

} // namespace neuroscheme

#endif // __NEUROSCHEME__COLOR__
