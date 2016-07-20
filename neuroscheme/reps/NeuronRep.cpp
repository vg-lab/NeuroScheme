#include "NeuronRep.h"
#include "NeuronItem.h"
#include "../Color.h"
#include <stdint.h>


namespace neuroscheme
{


  // Ring::Ring( void )
  //   : neuroscheme::shiftgen::Ring( )
  // {
  // }

  // Ring::Ring( const Ring& other )
  //   : neuroscheme::shiftgen::Ring( other )
  // {
  // }

  NeuronRep::NeuronRep( void )
    : neuroscheme::shiftgen::NeuronRep( )
  {
  }

  NeuronRep::NeuronRep( const NeuronRep& other )
    : neuroscheme::shiftgen::NeuronRep( other )
  {
  }


  QGraphicsItem* NeuronRep::item( bool create )
  {
    if ( create && !_item )
    {
      _item = new NeuronItem( *this );
    }
    return _item;
    }

} // namespace neuroscheme
