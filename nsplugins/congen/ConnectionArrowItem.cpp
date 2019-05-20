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

#include "ConnectionArrowItem.h"

#include <nslib/Config.h>
#include <cmath>

namespace nslib
{
  namespace congen
  {
    const float ConnectionArrowItem::M_PI_Float = float( M_PI );
    const float ConnectionArrowItem::M_PI_Inverse = 1.0f/M_PI_Float;
    const float ConnectionArrowItem::M_PI_x2 = M_PI_Float + M_PI_Float;
    const float ConnectionArrowItem::M_PI_3 = M_PI_Float * 0.33f;
    const float ConnectionArrowItem::M_PI_067 = M_PI_Float - M_PI_3;
    QColor ConnectionArrowItem::color = QColor( 100, 100, 100, 255 );
    QColor ConnectionArrowItem::hoverColor = QColor( 255, 100, 100, 255 );

    ConnectionArrowItem::ConnectionArrowItem(
      const ConnectionArrowRep& connectionArrowRep )
      : QObject( )
      , QGraphicsPathItem( )
      , nslib::Item( )
      , nslib::InteractiveItem( )
      , _arrowCircleEnd( nullptr )
      ,_arrowThickness( nslib::Config::scale( ) *
         connectionArrowRep.getProperty( "width" ).value< unsigned int >( ))
      , _lineStyle( connectionArrowRep.lineStyle( ))
      , _actualPen( QPen( QBrush( color ),
          _arrowThickness, _lineStyle, Qt::RoundCap, Qt::RoundJoin ))
    {
      this->_parentRep =
        &( const_cast< ConnectionArrowRep& >( connectionArrowRep ));
      _actualPen.setCosmetic( true );

      this->setAcceptHoverEvents( true );
    }

    ConnectionArrowItem::~ConnectionArrowItem( void )
    {
    }

    const QLineF& ConnectionArrowItem::line( void )
    {
      return _line;
    }

    void ConnectionArrowItem::setLine( const QLineF& line_ )
    {
      _line = line_;
      createArrow( _line.p1( ), _line.p2( ));
    }

    void ConnectionArrowItem::createArrow( const QPointF& origin,
      const QPointF& dest )
    {
      float itemInvScale =
        1.0f / static_cast< float >( this->scale( ));

      _arrowOrigin = itemInvScale * origin;
      _arrowDest = itemInvScale * dest;

      QPolygonF arrowShape;

      float arrowWidth = 6.0f * itemInvScale * nslib::Config::scale( );
      float arrowLength = 3.0f * itemInvScale * nslib::Config::scale( );

      QLineF auxLine( _arrowOrigin, _arrowDest );

      auto lengthInv = 1.0f / auxLine.length( );

      double angle = ::acos( auxLine.dx( ) * lengthInv );
      if( auxLine.dy( ) >= 0 )
      {
        angle = M_PI_x2 - angle;
      }

      QPointF arrowInit = auxLine.pointAt(
        1.0f - ( arrowLength * lengthInv ));
      QPointF arrowP1 = arrowInit -
        QPointF( sin( angle + M_PI_3 ) * arrowWidth,
          cos( angle + M_PI_3 ) * arrowWidth );
      QPointF arrowP2 = arrowInit -
        QPointF( sin( angle + M_PI_067 ) * arrowWidth,
          cos( angle + M_PI_067 ) * arrowWidth );

      QPointF arrowI1 = _arrowOrigin -
        QPointF( sin( angle + M_PI_Float ) * arrowWidth,
          cos( angle + M_PI_Float ) * arrowWidth );
      QPointF arrowI2 = _arrowOrigin +
        QPointF( sin( angle - M_PI_Float ) * arrowWidth,
          cos( angle - M_PI_Float ) * arrowWidth );

      float size = arrowLength;

      /*
      if ( _arrowOriItem != nullptr ) delete _arrowOriItem;
      _arrowOriItem = new QGraphicsEllipseItem( );
      _arrowOriItem->setRect( origin.x( ) - size * 0.5f,
                              origin.y( ) - size * 0.5f,
                              size,
                              size );

      _arrowOriItem->setPen( Qt::NoPen );
      _arrowOriItem->setBrush( QBrush( brushColor ));
      _arrowOriItem->setPen( QPen( QBrush( color ), _arrowThickness ));
      _arrowOriItem->setParentItem( this );
       */

      arrowShape.clear( );

      if( this->_parentRep->getProperty( "head" ).
        value< shiftgen::ConnectionArrowRep::TArrowHead >( ) ==
        shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE )
      {
        _arrowCircleEnd = new QGraphicsEllipseItem( );
        _arrowCircleEnd->setRect( dest.x( ) - size * 0.5f,
          dest.y( ) - size * 0.5f, size, size );

        _arrowCircleEnd->setPen( _actualPen );
        _arrowCircleEnd->setParentItem( this );

        arrowShape << arrowI1
                   << arrowI2
                   << auxLine.p1( )
                   << auxLine.p2( )
                   << auxLine.p1( );
      }
      else
      {
        arrowShape << arrowI1
                   << arrowI2
                   << auxLine.p1( )
                   << arrowInit
                   << arrowP1
                   << auxLine.p2( )
                   << arrowP2
                   << arrowInit
                   << auxLine.p1( );
      }

      auto painterPath = QPainterPath( );
      painterPath.moveTo( _arrowDest );
      painterPath.addPolygon( arrowShape );

      _actualPen.setColor( color );
      _actualPen.setCosmetic( true );
      this->setPen( _actualPen );
      this->setBrush( color );
      this->setPath( painterPath );
      this->setZValue( -100.0f );
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
      _actualPen.setColor( hoverColor );
      _actualPen.setCosmetic( true );
      this->setPen( _actualPen );
      this->setBrush( hoverColor );
      if( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( _actualPen );
      }
    }

    void ConnectionArrowItem::highlight( scoop::Color color_ )
    {
      this->setZValue( 100 );
      _actualPen.setColor( color_ );
      _actualPen.setCosmetic( true );
      this->setPen( _actualPen );
      this->setBrush( color_ );
      if( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( _actualPen );
      }
    }

    void ConnectionArrowItem::hoverLeaveEvent(
      QGraphicsSceneHoverEvent* event_ )
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
      _actualPen.setColor( color );
      _actualPen.setCosmetic( true );
      this->setPen( _actualPen );
      this->setBrush( color );
      if( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( _actualPen );
      }
    }

    bool ConnectionArrowItem::connectionRep( void ) const
    {
      return true;
    }

  } // namespace congen
} // namespace nslib
