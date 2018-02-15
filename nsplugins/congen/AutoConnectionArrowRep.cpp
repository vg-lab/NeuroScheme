/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Iago Calvo Lista <i.calvol@alumnos.urjc.es>
 *          Fernando Lucas Pérez <f.lucas@alumnos.urjc.es>
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

      //Checks whether the current scene is being animated
      if( opConfig->isAnimating( ) )
      {
        auto originRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _originRep );

        auto originItem = dynamic_cast< Item* >( originRep->item( scene ) );

        if( originItem == nullptr )
        {
          Loggers::get( )->log( "No successfully dynamic cast on originItem",
            LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        }
        auto originItemB = dynamic_cast< QGraphicsItemRepresentation* >(
          _originRep )->item( scene );

        auto originArrowItem =
          dynamic_cast< AutoConnectionArrowItem* >( arrowItem );

        //Creates the line to be animated
        auto& lineAnim = originArrowItem->lineAnim( );
        lineAnim.setPropertyName( "line" );
        lineAnim.setTargetObject( originArrowItem );
        lineAnim.setDuration( ANIM_DURATION );

        //Change of the NeuronPop glyph's Scale during the animation
        float glyphScaleStart =
          originItem->scaleAnim( ).startValue( ).toFloat( );
        float glyphScaleEnd = originItem->scaleAnim( ).endValue( ).toFloat( );

        //Radius of the glyph to animate
        float glyphBoundingRect =
          0.5f * float( originItemB->boundingRect( ).width( ) );

        //Change of glyph center position during the animation
        auto originPosAnimStart =
          originItem->posAnim( ).startValue( ).toPointF( );
        auto originPosAnimEnd = originItem->posAnim( ).endValue( ).toPointF( );

        float glyphRadius = glyphScaleStart * glyphBoundingRect;
        float isGrid = ( opConfig->isGrid( ) ) ? 1.0f : 0.0f;

        //Start values of the animation
        lineAnim.setStartValue( QLineF( QPointF( glyphRadius, isGrid ),
          QPointF( originPosAnimStart.x( ), originPosAnimStart.y( ) ) ) );

        //End values of the animation
        glyphRadius = glyphScaleEnd * glyphBoundingRect;
        lineAnim.setEndValue( QLineF( QPointF( glyphRadius, isGrid ),
          QPointF( originPosAnimEnd.x( ), originPosAnimEnd.y( ) ) ) );

        //Starts the animation
        lineAnim.start( );

      }
      else
      {
        auto originItem = dynamic_cast< QGraphicsItemRepresentation* >(
          _originRep )->item( scene );

        //Calculates glyph Radius
        float glyphRadius = 0.5f * float( originItem->scale( ) )
          * float( originItem->boundingRect( ).width( ) );

        //Calculates center of the Glyph
        QPointF glyphCenter = QPointF( originItem->x( ), originItem->y( ) );

        //Draws the new arrow
        dynamic_cast< AutoConnectionArrowItem* >( arrowItem )->
          createAutoArrow( glyphRadius, opConfig->isGrid( ), glyphCenter );
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
