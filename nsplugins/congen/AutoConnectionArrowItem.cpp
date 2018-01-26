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

      _arrowOrigin  = origin;
      _arrowDest    = dest;

      _arrowDest = QPointF(55555.0f, 55555.0f);

      QPolygonF arrowShape;

      float arrowWidth  = 6 * nslib::Config::scale( );
      float arrowLength = 3 * nslib::Config::scale( );

      QLineF auxLine( _arrowOrigin, _arrowDest );

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

      QPointF arrowI1 = _arrowOrigin -
                        QPointF( sin( angle + M_PI ) * arrowWidth,
                                 cos( angle + M_PI ) * arrowWidth );
      QPointF arrowI2 = _arrowOrigin +
                        QPointF( sin(angle - M_PI ) * arrowWidth,
                                 cos( angle - M_PI ) * arrowWidth );


      float size = arrowLength;

      /*
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
       */

      arrowShape.clear( );

      if ( this->_parentRep->getProperty( "head" ).
          value< shiftgen::ConnectionArrowRep::TArrowHead >( ) ==
           shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE )
      {
        _arrowCircleEnd = new QGraphicsEllipseItem( );
        _arrowCircleEnd->setRect( dest.x( ) - size * 0.5f,
                                  dest.y( ) - size * 0.5f,
                                  size,
                                  size );

        _arrowCircleEnd->setPen( Qt::NoPen );
        _arrowCircleEnd->setBrush( QBrush( color ));
        _arrowCircleEnd->setPen( QPen( QBrush( color ), _arrowThickness ));
        _arrowCircleEnd->setParentItem( this );

        arrowShape  << arrowI1
                    << arrowI2
                    << auxLine.p1( )
                    << auxLine.p2( )
                    << auxLine.p1( );
      }
      else
      {
        arrowShape  << arrowI1
                    << arrowI2
                    << auxLine.p1( )
                    << arrowInit
                    << arrowP1
                    << auxLine.p2( )
                    << arrowP2
                    << arrowInit
                    << auxLine.p1( );
      }

      this->setBrush( QBrush( color ));
      this->setPen( QPen( QBrush( color ), _arrowThickness ));
      this->setPolygon( arrowShape );
      this->setZValue( -100.0f );

    }

  } // namespace congen
} // namespace nslib
