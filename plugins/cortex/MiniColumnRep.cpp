#include "MiniColumnRep.h"
#include "MiniColumnItem.h"
#include "NeuronRep.h"
#include <neuroscheme/Color.h>
#include <stdint.h>


namespace neuroscheme
{

  MiniColumnRep::MiniColumnRep( void )
    : neuroscheme::shiftgen::NeuronAggregationRep( )
  {
  }

  MiniColumnRep::MiniColumnRep( const MiniColumnRep& other )
    : neuroscheme::shiftgen::NeuronAggregationRep( other )
  {
  }

  MiniColumnRep::MiniColumnRep(
    const neuroscheme::shiftgen::NeuronAggregationRep& other )
    : neuroscheme::shiftgen::NeuronAggregationRep( other )
  {
  }


  QGraphicsItem* MiniColumnRep::item( QGraphicsScene* scene, bool create )
    {
      if ( create && ( _items.find( scene ) == _items.end( )) &&
           !_items[ scene ] )
      {
        _items[ scene ] = new MiniColumnItem( *this, scene );
      }
      return _items.at( scene );
    }

} // namespace neuroscheme
