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
#include "AutoConnectionArrowItem.h"

namespace nslib
{
  namespace congen
  {

    const float AutoConnectionArrowRep::_centersDistFactor = 0.5f;
    const float AutoConnectionArrowRep::_arcSizeFactor = 0.3f;
    float  AutoConnectionArrowRep::glyphRadius;
    float  AutoConnectionArrowRep::arcRadius;
    float  AutoConnectionArrowRep::dist;
    float  AutoConnectionArrowRep::startAngle;
    float  AutoConnectionArrowRep::arcDegrees;
    float  AutoConnectionArrowRep::glyphBoundingRect = 0.0f;
    float  AutoConnectionArrowRep::glyphScale = 0.0f;

    AutoConnectionArrowRep::AutoConnectionArrowRep(
      shift::Representation* Rep_ )
      : ConnectionArrowRep( Rep_, Rep_ )
    {
    }

    void AutoConnectionArrowRep::preRender( shift::OpConfig* opConfig_ )
    {
      OpConfig* opConfig = dynamic_cast< OpConfig* >( opConfig_ );
      if( !opConfig )
      {
        return;
      }
      GraphicsScene* scene = opConfig->scene( );

      auto arrowItem = this->item( scene );

      auto originItem = dynamic_cast< QGraphicsItemRepresentation* >(
        _originRep )->item( scene );

      if( originItem->boundingRect( ).x( ) != glyphBoundingRect ||
        originItem->scale( ) != glyphScale )
      {
        AutoConnectionArrowRep::recalcArcData( originItem );
        glyphScale = float( originItem->scale( ) );
        glyphBoundingRect = float( originItem->boundingRect( ).x( ) );
      }

      QPointF glyphCenter = QPointF( originItem->x( ), originItem->y( ) );

      float relativeAngle;

      if( true )
      {
        relativeAngle = M_PI * 1.75f;
      }
      else
      {
        relativeAngle = atanf(
          float( originItem->y( ) / originItem->x( ) ) );
        if( originItem->x( ) < 0 )
        {
          relativeAngle += M_PI;
        }
      }

      QPointF arcCenter = QPointF(
        originItem->x( ) + dist * cosf( relativeAngle ),
        originItem->y( ) + dist * sinf( relativeAngle ) );

      dynamic_cast< AutoConnectionArrowItem* >( arrowItem )->
        createAutoArrow( arcCenter, arcDegrees, arcRadius,
        startAngle + float( M_PI ) - relativeAngle,
        glyphCenter );
    }

    QGraphicsItem*
    AutoConnectionArrowRep::item( QGraphicsScene* scene, bool create )
    {
      if( create && ( _items.find( scene ) == _items.end( ) ) &&
        !_items[ scene ] )
      {
        _items[ scene ] = new AutoConnectionArrowItem( *this );
      }
      return _items.at( scene );
    }

    void AutoConnectionArrowRep::recalcArcData( QGraphicsItem* originItem )
    {

      glyphRadius = float( originItem->boundingRect( ).width( ) ) * 0.5f *
        float( originItem->scale( ) );

      arcRadius = ( glyphRadius * _arcSizeFactor );

      dist = glyphRadius + arcRadius * ( _centersDistFactor );

      if( _centersDistFactor < 1 )
      {
        startAngle = acosf(
          ( arcRadius * arcRadius + dist * dist - glyphRadius * glyphRadius ) /
            ( 2.0f * arcRadius * dist ) );
      }
      else
      {
        startAngle = 0.0f;
      }
      arcDegrees = 2.0f * ( float( M_PI ) - startAngle );

    }
  } // namespace congen
} // namespace nslib
