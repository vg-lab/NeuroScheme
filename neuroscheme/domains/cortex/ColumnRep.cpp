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
//    std::cout << "--------------- Calling columnRep::item( )" << std::endl;
    if ( create && ( _items.find( scene ) == _items.end( )) &&
         !_items[ scene ] )
    {
      // std::cout << "New ColumnItem" << std::endl;
      _items[ scene ] = new ColumnItem( *this, scene );
      std::cout << "New ColumnItem " << _items[ scene ] << std::endl;
    }
    std::cout << "Return already created ColumnItem "
              << _items.at( scene ) << std::endl;

    return _items.at( scene );
  }

} // namespace neuroscheme
