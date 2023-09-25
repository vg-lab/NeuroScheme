/*
 * Copyright (c) 2017 VG-Lab/URJC/UPM.
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
  namespace cortex
  {
    constexpr auto M_PI_3 = static_cast<float>( M_PI ) * 0.33f;

    const QColor ConnectionArrowItem::color = QColor( 100, 100, 100, 255 );
    const QColor ConnectionArrowItem::hoverColor = QColor( 255, 100, 100, 255 );

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
        connectionArrowRep.getPropertyValue< unsigned int >( "width" );
    }

    void ConnectionArrowItem::createArrow( const QPointF& origin,
      const QPointF& dest )
    {
      QPolygonF arrowShape;
      arrowShape.clear( );
      if ( origin != dest ) //Arrows with same origin and destiny are not drawn.
      {
        const float itemInvScale = 1.0f / static_cast< float >( this->scale( ));

        _arrowOrigin = itemInvScale * origin;
        _arrowDest = itemInvScale * dest;

        const float arrowWidth = 3 * itemInvScale * nslib::Config::scale( );
        const float arrowLength = arrowWidth;

        QLineF auxLine( _arrowOrigin, _arrowDest );

        const auto lengthInv = 1.0f / auxLine.length( );

        double angle = ::acos( auxLine.dx( ) * lengthInv );
        if ( auxLine.dy( ) >= 0 )
          angle = ( M_PI * 2.0 ) - angle;

        QPointF arrowInit = auxLine.pointAt(
          1.0f - ( arrowLength * lengthInv ));
        QPointF arrowP1 = arrowInit -
          QPointF( sin( angle + M_PI_3 ) * arrowWidth,
          cos( angle + M_PI_3 ) * arrowWidth );
        QPointF arrowP2 = arrowInit -
          QPointF( sin( angle + M_PI - M_PI_3 ) * arrowWidth,
          cos( angle + M_PI - M_PI_3 ) * arrowWidth );

        const float size = arrowLength;

        if ( _arrowOriItem != nullptr ) delete _arrowOriItem;
        _arrowOriItem = new QGraphicsEllipseItem( );
        _arrowOriItem->setRect( origin.x( ) - size * 0.5f,
          origin.y( ) - size * 0.5f, size, size );

        _arrowOriItem->setPen( Qt::NoPen );
        _arrowOriItem->setBrush( QBrush( color ));
        _arrowOriItem->setPen( QPen( QBrush( color ), _arrowThickness ));
        _arrowOriItem->setParentItem( this );

        arrowShape << auxLine.p1( )
                   << arrowInit
                   << arrowP1
                   << auxLine.p2( )
                   << arrowP2
                   << arrowInit;

        this->setBrush( QBrush( color ));
        this->setPen( QPen( QBrush( color ), _arrowThickness ));
        this->setZValue( -100.0f );
      }
      this->setPolygon( arrowShape );
    }

    const QLineF& ConnectionArrowItem::line( void ) const
    {
      return _line;
    }

    void ConnectionArrowItem::setLine( const QLineF& line_ )
    {
      _line = line_;
      createArrow( _line.p1( ), _line.p2( ) );
    }

    QPropertyAnimation& ConnectionArrowItem::lineAnim( void )
    {
      return _lineAnim;
    }

    void ConnectionArrowItem::hoverEnterEvent(
      QGraphicsSceneHoverEvent* event_ )
    {
      auto rep = dynamic_cast< ConnectionArrowRep* >( _parentRep );
      if( rep )
      {
        rep->hoverEnterEvent( event_ );
      }
    }

    void ConnectionArrowItem::hoverEnter( void )
    {
      this->setZValue( 100 );
      this->setBrush( QBrush( hoverColor ) );
      this->setPen( QPen( QBrush( hoverColor ), _arrowThickness ) );
      _arrowOriItem->setPen( QPen( QBrush( hoverColor ), _arrowThickness ) );
      _arrowOriItem->setBrush( QBrush( hoverColor ) );
    }

    void ConnectionArrowItem::highlight( const scoop::Color color_ )
    {
      this->setZValue( 100 );
      this->setBrush( QBrush( color_ ) );
      this->setPen( QPen( QBrush( color_ ), _arrowThickness ) );
      _arrowOriItem->setPen( QPen( QBrush( color_ ), _arrowThickness ) );
      _arrowOriItem->setBrush( QBrush( color_ ) );
    }

    void ConnectionArrowItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      auto rep = dynamic_cast< ConnectionArrowRep* >( _parentRep );
      if( rep )
      {
        rep->hoverLeaveEvent( event_ );
      }
    }

    void ConnectionArrowItem::hoverLeave( void )
    {
      this->setZValue( -100 );
      this->setBrush( QBrush( color ) );
      this->setPen( QPen( QBrush( color ), _arrowThickness ) );
      _arrowOriItem->setPen( QPen( QBrush( color ), _arrowThickness ) );
      _arrowOriItem->setBrush( QBrush( color ) );
    }

    bool ConnectionArrowItem::connectionRep( void ) const
    {
      return true;
    }
  }
}
