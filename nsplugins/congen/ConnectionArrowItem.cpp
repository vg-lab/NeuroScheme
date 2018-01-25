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

	  /*ConnectionArrowItem::ConnectionArrowItem(
      const ConnectionArrowRep& connectionArrowRep )
      : QObject( )
      , QGraphicsPolygonItem( )
      , nslib::Item( )
      , nslib::InteractiveItem( )
      , _arrowCircleEnd( nullptr )
	  {
      this->_parentRep =
        &( const_cast< ConnectionArrowRep& >( connectionArrowRep ));


      this->setAcceptHoverEvents( true );

      _arrowThickness = nslib::Config::scale( ) *
        connectionArrowRep.getProperty( "width" ).value< unsigned int >( );
    }*/



    QPropertyAnimation& ConnectionArrowItem::lineAnim( void ) { return _lineAnim; }

    void ConnectionArrowItem::hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
    {
      auto rep = dynamic_cast< ConnectionArrowRep* >( _parentRep );
      if ( rep )
        rep->hoverEnterEvent( event_ );
    }

    void ConnectionArrowItem::hoverEnter( void )
    {
      this->setZValue( 100 );
      this->setBrush( QBrush( hoverColor ));
      this->setPen( QPen( QBrush( hoverColor ), _arrowThickness ));

      if ( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( QPen( QBrush( hoverColor ), _arrowThickness ));
        _arrowCircleEnd->setBrush( QBrush( hoverColor ));
      }
    }

    void ConnectionArrowItem::highlight( scoop::Color color_ )
    {
      this->setZValue( 100 );
      this->setBrush( QBrush( color_ ));
      this->setPen( QPen( QBrush( color_ ), _arrowThickness ));
      if ( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( QPen( QBrush( color_ ), _arrowThickness ));
        _arrowCircleEnd->setBrush( QBrush( color_ ));
      }
    }

    void ConnectionArrowItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      auto rep = dynamic_cast< ConnectionArrowRep* >( _parentRep );
      if ( rep )
        rep->hoverLeaveEvent( event_ );
    }

    void ConnectionArrowItem::hoverLeave( void )
    {
      this->setZValue( -100 );
      this->setBrush( QBrush( color ));
      this->setPen( QPen( QBrush( color ), _arrowThickness ));
      if ( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( QPen( QBrush( color ), _arrowThickness ));
        _arrowCircleEnd->setBrush( QBrush( color ));
      }
    }

  } // namespace congen
} // namespace nslib