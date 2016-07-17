#include "NeuronRep.h"
#include "NeuronItem.h"
#include "../Color.h"
#include <stdint.h>


namespace neuroscheme
{


  Ring::Ring( void )
    : neuroscheme::shiftgen::Ring( )
  {
  }

  Ring::Ring( const Ring& other )
    : neuroscheme::shiftgen::Ring( other )
  {
  }

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
      _item = new NeuronItem(
        this->getProperty( "bg" ).value< Color >( ),
        this->getProperty( "symbol" ).value< TSymbol >( ),
        this->getProperty( "rings" ).value< Rings >( ));
    }
    return _item;
    }

} // namespace neuroscheme
