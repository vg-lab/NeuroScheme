#include "InteractionManager.h"

namespace neuroscheme
{

  QPen InteractionManager::_selectedPen =
    QPen( Qt::red, 3, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen InteractionManager::_unselectedPen = QPen( Qt::NoPen );

  QPen InteractionManager::_hoverSelectedPen =
    QPen( Qt::red, 3, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen InteractionManager::_hoverUnselectedPen =
    QPen( QColor( 200, 200, 200, 255 ), 3, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

}
