/*
 * ConnectionArrowItem.cpp
 *
 *  Created on: 30/5/2017
 *      Author: juanpebm
 */
#define _USE_MATH_DEFINES
#include <cmath>

#include "ConnectionArrowItem.h"

namespace nslib
{
  namespace cortex
  {

    const auto M_PI_3 = float( M_PI ) * 0.33f;

	  ConnectionArrowItem::ConnectionArrowItem(
      const ConnectionArrowRep& connectionArrowRep )
      : QObject( )
      , QGraphicsPolygonItem( )
      , nslib::Item( )
      , nslib::InteractiveItem( )
      , _arrowOriItem( nullptr )
      , _arrowThickness( 2.0f )
	  {
      this->_parentRep =
        &( const_cast< ConnectionArrowRep& >( connectionArrowRep ));
    }

    void ConnectionArrowItem::setOrigin( const QPointF& origin_ )
    {
      _arrowOrigin = origin_;
      createArrow( _arrowOrigin, _arrowDest, _arrowThickness );
    }

    void ConnectionArrowItem::setDest( const QPointF& dest_ )
    {
      _arrowDest = dest_;
      createArrow( _arrowOrigin, _arrowDest, _arrowThickness );
    }

    void ConnectionArrowItem::createArrow( const QPointF& origin,
                                           const QPointF& dest,
                                           float thickness )
    {
      _arrowOrigin = origin;
      _arrowDest = dest;
      _arrowThickness = thickness;

      QPolygonF arrowShape;

      float arrowWidth = 8;
      float arrowLength = 8;

      QLineF auxLine( origin, dest );

      auto lengthInv = 1.0f / auxLine.length( );

      double angle = ::acos( auxLine.dx( ) * lengthInv );
      if ( auxLine.dy( ) >= 0 )
    	  angle = ( M_PI * 2.0 ) - angle;

      QPointF arrowInit = auxLine.pointAt(
        1.0f - (arrowLength * lengthInv ));
      QPointF arrowP1 = arrowInit -
        QPointF( sin( angle + M_PI_3 ) * arrowWidth,
                   cos( angle + M_PI_3 ) * arrowWidth );
      QPointF arrowP2 = arrowInit -
        QPointF( sin(angle + M_PI - M_PI_3 ) * arrowWidth,
                   cos( angle + M_PI - M_PI_3 ) * arrowWidth);

      float size = arrowLength;
      const Color baseColor( 0, 0, 0 );

      if ( _arrowOriItem != nullptr ) delete _arrowOriItem;
      _arrowOriItem = new QGraphicsEllipseItem( );
      _arrowOriItem->setRect( origin.x( ) - size * 0.5f,
                              origin.y( ) - size * 0.5f,
                              size,
                              size );

      _arrowOriItem->setPen( Qt::NoPen );
      _arrowOriItem->setBrush( QBrush( baseColor ));
      _arrowOriItem->setParentItem( this );

      arrowShape.clear( );
      arrowShape  << auxLine.p1( )
                  << arrowInit
                  << arrowP1
                  << auxLine.p2( )
                  << arrowP2
                  << arrowInit;

      this->setPolygon( arrowShape );
    }
  }
}
