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
#include <nslib/Config.h>

namespace nslib
{
  namespace congen
  {

    const auto M_PI_3 = float( M_PI ) * 0.33f;

    QColor ConnectionArrowItem::color = QColor( 100,100,100, 255 );
    QColor ConnectionArrowItem::hoverColor = QColor( 255,100,100, 255 );

	  ConnectionArrowItem::ConnectionArrowItem(
      const ConnectionArrowRep& connectionArrowRep )
      : QObject( )
      , QGraphicsPolygonItem( )
      , nslib::Item( )
      , nslib::InteractiveItem( )
      , _arrowOriItem( nullptr )
	  {
      this->_parentRep =
        &( const_cast< ConnectionArrowRep& >( connectionArrowRep ));


      this->setAcceptHoverEvents( true );

      _arrowThickness = nslib::Config::scale( ) *
        connectionArrowRep.getProperty( "width" ).value< unsigned int >( );
    }

    void ConnectionArrowItem::createArrow( const QPointF& origin,
                                           const QPointF& dest )
    {
      _arrowOrigin = origin;
      _arrowDest = dest;

      QPolygonF arrowShape;

      float arrowWidth = 3 * nslib::Config::scale( );
      float arrowLength = 3 * nslib::Config::scale( );

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


      if ( _arrowOriItem != nullptr ) delete _arrowOriItem;
      _arrowOriItem = new QGraphicsEllipseItem( );
      _arrowOriItem->setRect( origin.x( ) - size * 0.5f,
                              origin.y( ) - size * 0.5f,
                              size,
                              size );

      _arrowOriItem->setPen( Qt::NoPen );
      _arrowOriItem->setBrush( QBrush( color ));
      _arrowOriItem->setPen( QPen( QBrush( color ), _arrowThickness ));
      _arrowOriItem->setParentItem( this );

      arrowShape.clear( );

      arrowShape  << auxLine.p1( )
                  << arrowInit
                  << arrowP1
                  << auxLine.p2( )
                  << arrowP2
                  << arrowInit;

      this->setBrush( QBrush( color ));
      // this->setPen( QPen( color ));
      this->setPen( QPen( QBrush( color ), _arrowThickness ));
      this->setPolygon( arrowShape );
      this->setZValue( -100.0f );

    }
  }
}
