#include "SpineRep.h"
#include "SpineItem.h"
#include "../../Color.h"
#include <stdint.h>


namespace neuroscheme
{

  SpineRep::SpineRep( void )
    : neuroscheme::shiftgen::spines::SpineRep( )
  {
  }

  SpineRep::SpineRep( const SpineRep& other )
    : neuroscheme::shiftgen::spines::SpineRep( other )
  {
  }


  QGraphicsItem* SpineRep::item( QGraphicsScene* scene, bool create )
  {
    if ( create && ( _items.find( scene ) == _items.end( )) &&
      !_items[ scene ] )
    {
      _items[ scene ] = new SpineItem( *this );
    }
    return _items.at( scene );
  }

} // namespace neuroscheme
