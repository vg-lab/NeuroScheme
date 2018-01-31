/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Fernando Lucas Pérez
 * 			    Iago Calvo Lista
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
#include "AutoConnectionArrowItem.h"
#include "ConnectionArrowItem.h"
#include <nslib/Color.h>
#include <nslib/InteractionManager.h>
#include <nslib/reps/InteractiveItem.h>
#include <nslib/reps/Item.h>
#include <nslib/reps/SelectableItem.h>
#include "ConnectionArrowRep.h"

#include <QGraphicsPolygonItem>
#include <QPropertyAnimation>

#include <ctime>
#include <nslib/Config.h>

namespace nslib
{
  namespace congen
  {

    AutoConnectionArrowItem::AutoConnectionArrowItem(
        const AutoConnectionArrowRep& connectionArrowRep )
        : ConnectionArrowItem( connectionArrowRep )
    {
    }

    void AutoConnectionArrowItem::createArrow( const QPointF& origin,
                                               const QPointF& dest )
    {
      //SLDEBUG
      std::cout << "AutoConnectionArrowItem createArrow" << std::endl;
      //END

      _arrowOrigin = origin;
      _arrowDest = dest;

      //SLDEBUG
      float dist = 2.0f;//statify
      float radius = 40.0f;//statify
      //float radiusGlifo = 40.0f;//statify
      //END

      auto painterPath = QPainterPath( );
      painterPath.moveTo( _arrowOrigin.x()*dist + radius, _arrowOrigin.y()*dist );
      painterPath.arcTo( _arrowOrigin.x()*dist - radius, _arrowOrigin.y()*dist - radius, radius*2.0f, radius*2.0f, 20.0f, 200.0f );

      this->setBrush( QBrush(QColor(0,0,0,0)) );
      this->setPen( QPen( QBrush( color ), _arrowThickness ) );
      this->setPath( painterPath );
      this->setZValue( -100.0f );

    }

    void AutoConnectionArrowItem::hoverEnter( void )
    {
      this->setZValue( 100 );
      this->setBrush( QBrush( Qt::NoBrush ));
      this->setPen( QPen( QBrush( hoverColor ), _arrowThickness ));

      if ( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( QPen( QBrush( hoverColor ), _arrowThickness ));
        _arrowCircleEnd->setBrush( QBrush( hoverColor ));
      }
    }

    void AutoConnectionArrowItem::highlight( scoop::Color color_ )
    {
      this->setZValue( 100 );
      this->setBrush( QBrush( Qt::NoBrush  ));
      this->setPen( QPen( QBrush( QColor(50, 206, 22)/*Green*/ ), _arrowThickness ));
      if ( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( QPen( QBrush( color_ ), _arrowThickness ));
        _arrowCircleEnd->setBrush( QColor(50, 206, 22)/*Green*/ );
      }
    }

    void AutoConnectionArrowItem::hoverLeaveEvent(
        QGraphicsSceneHoverEvent* event_ )
    {
      auto rep = dynamic_cast< ConnectionArrowRep* >( _parentRep );
      if ( rep )
        rep->hoverLeaveEvent( event_ );
    }

    void AutoConnectionArrowItem::hoverLeave( void )
    {
      this->setZValue( -100 );
      this->setBrush( QBrush( Qt::NoBrush  ));
      this->setPen( QPen( QBrush( color ), _arrowThickness ));
      if ( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( QPen( QBrush( color ), _arrowThickness ));
        _arrowCircleEnd->setBrush( QBrush( Qt::NoBrush  ));
      }
    }

  } // namespace congen
} // namespace nslib
