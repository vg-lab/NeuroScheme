#include "SelectableItem.h"

namespace  neuroscheme
{
  QPen SelectableItem::_selectedPen =
    QPen( Qt::red, 2, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_hoverSelectedPen =
    QPen( Qt::red, 2, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_partiallySelectedPen =
    QPen( QColor( 255, 170, 80, 255 ), 2, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_hoverPartiallySelectedPen =
    QPen( QColor( 255, 170, 80, 255 ), 2, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_unselectedPen = QPen( Qt::NoPen );

  QPen SelectableItem::_hoverUnselectedPen =
    QPen( QColor( 200, 200, 200, 255 ), 2, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  void SelectableItem::init( void )
  {
    _selectedPen.setCosmetic( true );
    _hoverSelectedPen.setCosmetic( true );
    _partiallySelectedPen.setCosmetic( true );
    _hoverPartiallySelectedPen.setCosmetic( true );
    _unselectedPen.setCosmetic( true );
    _hoverUnselectedPen.setCosmetic( true );
  }

}
