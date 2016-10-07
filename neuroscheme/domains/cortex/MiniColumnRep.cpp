#include "MiniColumnRep.h"
#include "MiniColumnItem.h"
#include "NeuronRep.h"
#include "../../Color.h"
#include <stdint.h>


namespace neuroscheme
{

  MiniColumnRep::MiniColumnRep( void )
    : neuroscheme::shiftgen::MiniColumnRep( )
  {
  }

  MiniColumnRep::MiniColumnRep( const MiniColumnRep& other )
    : neuroscheme::shiftgen::MiniColumnRep( other )
  {
  }


  QGraphicsItem* MiniColumnRep::item( bool create )
  {
    if ( create && !_item )
    {
      _item = new MiniColumnItem( *this );
    }
    return _item;
    }

} // namespace neuroscheme
