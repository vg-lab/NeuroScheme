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
#include "AutoConnectionArrowRep.h"
#include "ConnectionArrowItem.h"
#include "../../ShiFT/shift/Representation.h"
#include "AutoConnectionArrowItem.h"
#include <iostream>


namespace nslib
{
  namespace congen
  {

    AutoConnectionArrowRep::AutoConnectionArrowRep(
          shift::Representation *Rep_)
        : ConnectionArrowRep(Rep_, Rep_) {
    }

    void AutoConnectionArrowRep::preRender( shift::OpConfig* opConfig_ ){
      OpConfig* opConfig = dynamic_cast< OpConfig* >( opConfig_ );
      if ( !opConfig )
        return;

      GraphicsScene* scene = opConfig->scene( );

      auto  arrowItem   = this->item( scene );

      auto originItem = dynamic_cast< QGraphicsItemRepresentation* >(
              _originRep )->item( scene );

      float glyphRadius = originItem->boundingRect().width( ) * 0.5f * originItem->scale( );
      float arcRadius = (glyphRadius * 0.3f);

      float relativeAngle = atan(originItem->y()/originItem->x());
      float dist = glyphRadius + arcRadius*0.7;

      QPointF arcCenter = QPointF(originItem->x()+dist*cos(relativeAngle),
                                  originItem->y()+dist*sin(relativeAngle));

      float startAngle = acos((arcRadius*arcRadius + dist*dist
                                - glyphRadius*glyphRadius)
                               /(2*arcRadius*dist));
      float arcDegrees = 2*(M_PI - startAngle);

      dynamic_cast< AutoConnectionArrowItem* >( arrowItem )->
          createAutoArrow( arcCenter, arcDegrees, arcRadius, startAngle + M_PI - relativeAngle );
    }

    QGraphicsItem* AutoConnectionArrowRep::item( QGraphicsScene* scene, bool create )
    {
      if ( create && ( _items.find( scene ) == _items.end( )) &&
           !_items[ scene ] )
      {
        _items[ scene ] = new AutoConnectionArrowItem( *this );
      }
      return _items.at( scene );
    }

  } // namespace congen
} // namespace nslib
