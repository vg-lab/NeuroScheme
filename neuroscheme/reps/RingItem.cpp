#include "RingItem.h"
#include <QBrush>

namespace neuroscheme
{
  RingItem::RingItem( unsigned int initAngle,
                      unsigned int xRadius,
                      unsigned int yRadius,
                      unsigned int width,
                      int angle,
                      Color color )
  {
    // Define outer circle
    int x_ = int( xRadius );
    int y_ = int( yRadius );
    QPainterPath outerArc( QPoint( 0, 0 ));
    outerArc.arcTo( -x_, -y_, x_ * 2, y_ * 2, initAngle, angle );
    QPainterPath outerFill( QPoint( 0, 0 ));
    outerFill.arcTo( - x_, - y_, x_ * 2, y_ * 2, initAngle, angle );
    outerFill.closeSubpath( );
    ( void ) outerFill.united( outerArc );

    // Define inner circle
    x_ = int( xRadius ) - int( width );
    y_ = int( yRadius ) - int( width );
    QPainterPath innerArc( QPoint( 0, 0 ));
    innerArc.arcTo( -x_, -y_, x_ * 2, y_ * 2, initAngle, angle );
    QPainterPath innerFill( QPoint( 0, 0 ));
    innerFill.arcTo( - x_, - y_, x_ * 2, y_ * 2, initAngle, angle );
    innerFill.closeSubpath( );
    ( void ) innerFill.united( innerArc );

    // Substract inner from outer
    QPainterPath ringFill = outerFill.subtracted( innerFill );
    this->setPath( ringFill );

    setBrush( QBrush( color ));

  }

} // namespace neuroscheme
