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

    AutoConnectionArrowRep::AutoConnectionArrowRep(
      shift::Representation* Rep_ )
      : ConnectionArrowRep( Rep_, Rep_ )
    {
    }

    void AutoConnectionArrowRep::preRender( shift::OpConfig* opConfig_ )
    {
      auto opConfig = dynamic_cast< OpConfig* >( opConfig_ );
      if( !opConfig )
      {
        return;
      }
      GraphicsScene* scene = opConfig->scene( );

      auto arrowItem = this->item( scene );

      if( opConfig->isAnimating( ) )
      {
        auto originRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _originRep );
        auto destRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _destRep );

        auto originItem = dynamic_cast< Item* >( originRep->item( scene ) );
        auto destItem = dynamic_cast< Item* >( destRep->item( scene ) );

        if( originItem == nullptr )
        {
          Loggers::get( )->log( "No successfully dynamic cast on originItem",
            LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        }

        if( destItem == nullptr )
        {
          Loggers::get( )->log( "No successfully dynamic cast on destItem",
            LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        }

        auto originArrowItem =
          dynamic_cast< AutoConnectionArrowItem* >( arrowItem );
        auto& lineAnim = originArrowItem->lineAnim( );
        lineAnim.setPropertyName( "line" );
        lineAnim.setTargetObject( originArrowItem );
        lineAnim.setDuration( ANIM_DURATION );

        auto originItemB = dynamic_cast< QGraphicsItemRepresentation* >(
          _originRep )->item( scene );

        float glyphScaleStart =
          originItem->scaleAnim( ).startValue( ).toFloat( );

        float glyphScaleEnd = originItem->scaleAnim( ).endValue( ).toFloat( );

        float glyphBoundingRect =
          0.5f * float( originItemB->boundingRect( ).width( ) );

        auto originPosAnimStart =
          originItem->posAnim( ).startValue( ).toPointF( );
        auto originPosAnimEnd = originItem->posAnim( ).endValue( ).toPointF( );

        float glyphRadius = glyphScaleStart * glyphBoundingRect;
        float isGrid = ( opConfig->isGrid( ) ) ? 1.0f : 0.0f;

        lineAnim.setStartValue(
          QLineF( QPointF( glyphRadius, isGrid ),
          QPointF( originPosAnimStart.x( ), originPosAnimStart.y( ) ) ) );

        glyphRadius = glyphScaleEnd * glyphBoundingRect;

        lineAnim.setEndValue(
          QLineF( QPointF( glyphRadius, isGrid ),
          QPointF( originPosAnimEnd.x( ), originPosAnimEnd.y( ) ) ) );

        lineAnim.start( );

      }
      else
      {

        auto originItem = dynamic_cast< QGraphicsItemRepresentation* >(
          _originRep )->item( scene );

        float glyphRadius = 0.5f * float( originItem->scale( ) )
          * float( originItem->boundingRect( ).width( ) );
        float isGrid = ( opConfig->isGrid( ) ) ? 1.0f : 0.0f;

        QPointF glyphCenter = QPointF( originItem->x( ), originItem->y( ) );

        dynamic_cast< AutoConnectionArrowItem* >( arrowItem )->
          createAutoArrow( glyphRadius, isGrid, glyphCenter );
      }
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

  } // namespace congen
} // namespace nslib
