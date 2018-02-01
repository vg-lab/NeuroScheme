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
      _arrowThickness = 2;
    }

    void AutoConnectionArrowItem::createAutoArrow( const QPointF& arcCenter, float arcDegrees,
                          float arcRadius, float startAngle)
    {
      _arrowOrigin = arcCenter;

      auto painterPath = QPainterPath( );
      painterPath.moveTo( arcCenter.x() + arcRadius*cos(startAngle),
                          arcCenter.y() - arcRadius*sin(startAngle) );
      painterPath.arcTo( arcCenter.x() - arcRadius,
                         arcCenter.y() - arcRadius,
                         arcRadius*2.0f, arcRadius*2.0f, startAngle*180/M_PI, arcDegrees*180/M_PI );

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
