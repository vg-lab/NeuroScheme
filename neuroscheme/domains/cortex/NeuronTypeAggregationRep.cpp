#include "NeuronTypeAggregationRep.h"
#include "NeuronTypeAggregationItem.h"
#include <stdint.h>


namespace neuroscheme
{

  NeuronTypeAggregationRep::NeuronTypeAggregationRep( void )
    : neuroscheme::shiftgen::NeuronTypeAggregationRep( )
  {
    std::cout << "+++ New NeuronTypeAggregationRep " << this << std::endl;
  }

  NeuronTypeAggregationRep::NeuronTypeAggregationRep(
    const NeuronTypeAggregationRep& other )
    : neuroscheme::shiftgen::NeuronTypeAggregationRep( other )
  {
  }


  QGraphicsItem* NeuronTypeAggregationRep::item(
    QGraphicsScene* scene, bool create )
  {
    static int i = 0;
    if ( create && ( _items.find( scene ) == _items.end( )) &&
         !_items[ scene ] )
    {
      _items[ scene ] = new NeuronTypeAggregationItem( *this );
      std::cout << "++++ Creating NeuronTypeAggregationItem " << _items[ scene ] << std::endl;
      ++i;
      // if ( i == 15 ) assert( false );
    }
    return _items.at( scene );
  }

} // namespace neuroscheme
