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
    int x = int( xRadius );
    int y = int( yRadius );
    QPainterPath outerArc( QPoint( 0, 0 ));
    outerArc.arcTo( -x, -y, x * 2, y * 2, initAngle, angle );
    QPainterPath outerFill( QPoint( 0, 0 ));
    outerFill.arcTo( - x, - y, x * 2, y * 2, initAngle, angle );
    outerFill.closeSubpath( );
    outerFill.united( outerArc );

    // Define inner circle
    x = int( xRadius ) - int( width );
    y = int( yRadius ) - int( width );
    QPainterPath innerArc( QPoint( 0, 0 ));
    innerArc.arcTo( -x, -y, x * 2, y * 2, initAngle, angle );
    QPainterPath innerFill( QPoint( 0, 0 ));
    innerFill.arcTo( - x, - y, x * 2, y * 2, initAngle, angle );
    innerFill.closeSubpath( );
    innerFill.united( innerArc );

    // Substract inner from outer
    QPainterPath ringFill = outerFill.subtracted( innerFill );
    this->setPath( ringFill );

    setBrush( QBrush(
                QColor( color.r( ),
                        color.g( ),
                        color.b( ))));

  }

} // namespace neuroscheme
