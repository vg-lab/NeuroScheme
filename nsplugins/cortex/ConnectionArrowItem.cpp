/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 * 			    Juan Pedro Brito <juanpedro.brito@upm.es>
 *
 * This file is part of NeuroScheme
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
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
                 cos( angle + M_PI - M_PI_3 ) * arrowWidth );

      float size = arrowLength;
      const Color baseColor( 0, 0, 0 );
      auto color = QColor( 100,100,100, 255 );

      if ( _arrowOriItem != nullptr ) delete _arrowOriItem;
      _arrowOriItem = new QGraphicsEllipseItem( );
      _arrowOriItem->setRect( origin.x( ) - size * 0.5f,
                              origin.y( ) - size * 0.5f,
                              size,
                              size );

      _arrowOriItem->setPen( Qt::NoPen );
      _arrowOriItem->setBrush( QBrush( color ));
      _arrowOriItem->setParentItem( this );

      arrowShape.clear( );

      arrowShape  << auxLine.p1( )
                  << arrowInit
                  << arrowP1
                  << auxLine.p2( )
                  << arrowP2
                  << arrowInit;

      this->setBrush( QBrush( color ));
      this->setPen( QPen( color ));
      this->setPolygon( arrowShape );
      this->setZValue( -100.0f );

    }
  }
}
