#include "LayerRep.h"
#include "NeuronAggregationItem.h"
#include "NeuronRep.h"
#include "../../Color.h"
#include <stdint.h>


namespace neuroscheme
{

  LayerRep::LayerRep( void )
    : neuroscheme::shiftgen::LayerRep( )
  {
  }

  LayerRep::LayerRep( const LayerRep& other )
    : neuroscheme::shiftgen::LayerRep( other )
  {
  }


  QGraphicsItem* LayerRep::item( QGraphicsScene* scene, bool create )
  {
    if ( create && ( _items.find( scene ) == _items.end( )) &&
         !_items[ scene ] )
    {
      _items[ scene ] = new LayerItem( *this );
    }
    return _items.at( scene );
  }

} // namespace neuroscheme
