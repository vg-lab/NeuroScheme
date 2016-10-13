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


  QGraphicsItem* MiniColumnRep::item( QGraphicsScene* scene, bool create )
    {
      if ( create && ( _items.find( scene ) == _items.end( )) &&
           !_items[ scene ] )
      {
        _items[ scene ] = new MiniColumnItem( *this );
      }
      return _items.at( scene );
    }

} // namespace neuroscheme
