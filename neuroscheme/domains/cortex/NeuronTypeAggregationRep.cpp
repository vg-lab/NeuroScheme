#include "NeuronTypeAggregationRep.h"
#include "NeuronTypeAggregationItem.h"
#include <stdint.h>


namespace neuroscheme
{

  NeuronTypeAggregationRep::NeuronTypeAggregationRep( void )
    : neuroscheme::shiftgen::NeuronTypeAggregationRep( )
  {
  }

  NeuronTypeAggregationRep::NeuronTypeAggregationRep(
    const NeuronTypeAggregationRep& other )
    : neuroscheme::shiftgen::NeuronTypeAggregationRep( other )
  {
  }


  QGraphicsItem* NeuronTypeAggregationRep::item(
    QGraphicsScene* scene, bool create )
  {
    if ( create && ( _items.find( scene ) == _items.end( )) &&
         !_items[ scene ] )
    {
      _items[ scene ] = new NeuronTypeAggregationItem( *this );
    }
    return _items.at( scene );
  }

} // namespace neuroscheme
