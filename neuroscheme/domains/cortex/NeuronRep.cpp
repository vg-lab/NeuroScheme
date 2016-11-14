#include "NeuronRep.h"
#include "NeuronItem.h"
#include "../../Color.h"
#include <stdint.h>


namespace neuroscheme
{

  NeuronRep::NeuronRep( void )
    : neuroscheme::shiftgen::NeuronRep( )
  {
  }

  NeuronRep::NeuronRep( const NeuronRep& other )
    : neuroscheme::shiftgen::NeuronRep( other )
  {
  }

  NeuronRep::NeuronRep( const neuroscheme::shiftgen::NeuronRep& other )
    : neuroscheme::shiftgen::NeuronRep( other )
  {
  }


  QGraphicsItem* NeuronRep::item( QGraphicsScene* scene, bool create )
  {
    if ( create && ( _items.find( scene ) == _items.end( )) &&
      !_items[ scene ] )
    {
      _items[ scene ] = new NeuronItem( *this );
    }
    return _items.at( scene );
  }

} // namespace neuroscheme
