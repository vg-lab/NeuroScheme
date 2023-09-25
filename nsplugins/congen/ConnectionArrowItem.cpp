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
      const ConnectionArrowRep& connectionArrowRep,
      const bool interactive_ )
      : QObject( )
      , QGraphicsPathItem( )
      , nslib::Item( )
      , nslib::InteractiveItem( )
      , _arrowCircleEnd( nullptr )
      ,_arrowThickness( nslib::Config::scale( ) *
         connectionArrowRep.getPropertyValue< unsigned int >( "width" , 1u ))
      , _lineStyle( connectionArrowRep.lineStyle( ))
      , _actualPen( QPen( QBrush( color ),
          _arrowThickness, _lineStyle, Qt::RoundCap, Qt::RoundJoin ))
    {
      setInteractive( interactive_ );
      if ( interactive_ )
      {
        this->setAcceptHoverEvents( true );
      }

      this->_parentRep =
        &( const_cast< ConnectionArrowRep& >( connectionArrowRep ));
      _actualPen.setCosmetic( true );

      this->setAcceptHoverEvents( true );
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
      auto painterPath = QPainterPath( );
      if ( origin != dest ) //Arrows with same origin and destiny are not drawn.
      {
        const float itemInvScale = 1.0f / static_cast<float>( this->scale( ));

        _arrowOrigin = itemInvScale * origin;
        _arrowDest = itemInvScale * dest;

        QPolygonF arrowShape;

        const float arrowWidth = 6.0f * itemInvScale * nslib::Config::scale( );
        const float arrowLength = 3.0f * itemInvScale * nslib::Config::scale( );

        const QLineF auxLine( _arrowOrigin, _arrowDest );

        const auto lengthInv = 1.0f / auxLine.length( );

        double angle = acos( auxLine.dx( ) * lengthInv );
        if( auxLine.dy( ) >= 0 )
        {
          angle = M_PI_x2 - angle;
        }

        const QPointF arrowInit = auxLine.pointAt(
          1.0f - ( arrowLength * lengthInv ));
        const QPointF arrowP1 = arrowInit -
          QPointF( sin( angle + M_PI_3 ) * arrowWidth,
            cos( angle + M_PI_3 ) * arrowWidth );
        const QPointF arrowP2 = arrowInit -
          QPointF( sin( angle + M_PI_067 ) * arrowWidth,
            cos( angle + M_PI_067 ) * arrowWidth );

        const QPointF arrowI1 = _arrowOrigin -
          QPointF( sin( angle + M_PI_Float ) * arrowWidth,
            cos( angle + M_PI_Float ) * arrowWidth );
        const QPointF arrowI2 = _arrowOrigin +
          QPointF( sin( angle - M_PI_Float ) * arrowWidth,
            cos( angle - M_PI_Float ) * arrowWidth );

        const float size = arrowLength;

        arrowShape.clear( );

        if( this->_parentRep->getPropertyValue
          < shiftgen::ConnectionArrowRep::TArrowHead >( "head",
          shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE ) ==
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

        painterPath.moveTo( _arrowDest );
        painterPath.addPolygon( arrowShape );
        _actualPen.setColor( color );
        _actualPen.setCosmetic( true );
        this->setPen( _actualPen );
        this->setBrush( color );
        this->setZValue( -100.0f );
      }
      this->setPath( painterPath );

    }

    QPropertyAnimation& ConnectionArrowItem::lineAnim( void )
    {
      return _lineAnim;
    }

    void ConnectionArrowItem::hoverEnterEvent(
      QGraphicsSceneHoverEvent* event_ )
    {
      if ( _interactive )
      {
        auto rep = dynamic_cast< ConnectionArrowRep* >( _parentRep );
        if( rep )
        {
          rep->hoverEnterEvent( event_ );
          if ( event_ && event_->modifiers( ).testFlag( Qt::ControlModifier ))
          {
            rep->editConnectionWidget( );
          }
        }
      }
    }

    void ConnectionArrowItem::contextMenuEvent(
        QGraphicsSceneContextMenuEvent* event_ )
    {
      if ( _interactive )
      {
        auto contextMenu = InteractionManager::contextMenu( );
        contextMenu->clear( );
        auto rep = dynamic_cast< ConnectionArrowRep* >( _parentRep );
        if( rep )
        {
          QAction* editRel = contextMenu->addAction( "Edit Relationship" );
          QAction* selectedAction = contextMenu->exec( event_->screenPos( ));
          if( editRel == selectedAction )
          {
            rep->editConnectionWidget( );
          }
        }
      }
      InteractionManager::tmpConnectionLineRemove( );
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

    void ConnectionArrowItem::highlight( const scoop::Color color_ )
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
