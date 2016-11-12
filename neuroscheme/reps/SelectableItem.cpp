#include "SelectableItem.h"

namespace  neuroscheme
{
  QPen SelectableItem::_selectedPen =
    QPen( Qt::red, 3, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_hoverSelectedPen =
    QPen( Qt::red, 3, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_partiallySelectedPen =
    QPen( Qt::yellow, 3, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_hoverPartiallySelectedPen =
    QPen( Qt::yellow, 3, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_unselectedPen = QPen( Qt::NoPen );

  QPen SelectableItem::_hoverUnselectedPen =
    QPen( QColor( 200, 200, 200, 255 ), 3, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );
}
