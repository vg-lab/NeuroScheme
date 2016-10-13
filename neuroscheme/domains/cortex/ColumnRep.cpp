#include "ColumnRep.h"
#include "ColumnItem.h"
#include "NeuronRep.h"
#include "../../Color.h"
#include <stdint.h>


namespace neuroscheme
{

  ColumnRep::ColumnRep( void )
    : neuroscheme::shiftgen::ColumnRep( )
  {
  }

  ColumnRep::ColumnRep( const ColumnRep& other )
    : neuroscheme::shiftgen::ColumnRep( other )
  {
  }


  QGraphicsItem* ColumnRep::item( QGraphicsScene* scene, bool create )
  {
    std::cout << "--------------- Calling columnRep::item( )" << std::endl;
    if ( create && ( _items.find( scene ) == _items.end( )) &&
         !_items[ scene ] )
    {
      _items[ scene ] = new ColumnItem( *this );
    }
    return _items.at( scene );
  }

} // namespace neuroscheme
