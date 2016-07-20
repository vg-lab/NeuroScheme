#include "ColumnRep.h"
#include "ColumnItem.h"
#include "NeuronRep.h"
#include "../Color.h"
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


  QGraphicsItem* ColumnRep::item( bool create )
  {
    if ( create && !_item )
    {
      _item = new ColumnItem( *this );
    }
    return _item;
    }

} // namespace neuroscheme
