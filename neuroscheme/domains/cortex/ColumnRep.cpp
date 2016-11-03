#include "ColumnRep.h"
#include "ColumnItem.h"
#include "NeuronRep.h"
#include "../../Color.h"
#include <stdint.h>


namespace neuroscheme
{

  ColumnRep::ColumnRep( void )
    : neuroscheme::shiftgen::NeuronAggregationRep( )
  {
  }

  ColumnRep::ColumnRep( const ColumnRep& other )
    : neuroscheme::shiftgen::NeuronAggregationRep( other )
  {
  }

  ColumnRep::ColumnRep(
    const neuroscheme::shiftgen::NeuronAggregationRep& other )
    : neuroscheme::shiftgen::NeuronAggregationRep( other )
  {
  }


  QGraphicsItem* ColumnRep::item( QGraphicsScene* scene, bool create )
  {
    if ( create && ( _items.find( scene ) == _items.end( )) &&
         !_items[ scene ] )
    {
      _items[ scene ] = new ColumnItem( *this, scene );
    }
    return _items.at( scene );
  }

} // namespace neuroscheme
