/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
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
#include "ConnectionArrowItem.h"
#include "NeuronItem.h"
#include <nslib/Color.h>
#include <stdint.h>


namespace nslib
{
  namespace cortex
  {

    ConnectionArrowRep::ConnectionArrowRep( shift::Representation* originRep_,
                                            shift::Representation* destRep_ )
      : shiftgen::ConnectionArrowRep( )
      , _originRep( originRep_ )
      , _destRep( destRep_ )
    {
    }

    ConnectionArrowRep::ConnectionArrowRep( const ConnectionArrowRep& other )
      : shiftgen::ConnectionArrowRep( other )
      , _originRep( other._originRep )
      , _destRep( other._destRep )
    {
    }

    // ConnectionArrowRep::ConnectionArrowRep( const shiftgen::ConnectionArrowRep& other )
    //   : shiftgen::ConnectionArrowRep( other )
    // {
    // }


    QGraphicsItem* ConnectionArrowRep::item( QGraphicsScene* scene, bool create )
    {
      if ( create && ( _items.find( scene ) == _items.end( )) &&
           !_items[ scene ] )
      {
        _items[ scene ] = new ConnectionArrowItem( *this );
      }
      return _items.at( scene );
    }

    void ConnectionArrowRep::preRender( shift::OpConfig* opConfig_ )
    {
      OpConfig* opConfig = dynamic_cast< OpConfig* >( opConfig_ );
      if ( !opConfig )
        return;

      GraphicsScene* scene = opConfig->scene( );

      auto  arrowItem   = this->item( scene );
      float lThickness  = 2.0;

      if (opConfig->isAnimating())
      {
        //TODO: It needs to be changed in the future!
        auto originItem = dynamic_cast< Item* > (
                                dynamic_cast<QGraphicsItemRepresentation* >(
                                _originRep )->item( scene ) );

        auto destItem = dynamic_cast< Item* > (
                          dynamic_cast< QGraphicsItemRepresentation* >(
                              _destRep )->item( scene ));

        if ( originItem == nullptr )
          Log::log( "No successfully dynamic cast on originItem",
                    LOG_LEVEL_ERROR );

        if ( destItem == nullptr )
          Log::log( "No successfully dynamic cast on destItem",
                    LOG_LEVEL_ERROR );

        auto originArrowItem = dynamic_cast< ConnectionArrowItem* >( arrowItem );
        auto& originAnim = originArrowItem->originAnim( );
        originAnim.setPropertyName( "origin" );
        originAnim.setTargetObject( originArrowItem );
        originAnim.setDuration( ANIM_DURATION );
        originAnim.setStartValue( originItem->posAnim( ).startValue( ).toPointF( ));
        originAnim.setEndValue( originItem->posAnim( ).endValue( ).toPointF( ));
        originAnim.start( );

        auto destArrowItem = dynamic_cast< ConnectionArrowItem* >( arrowItem );
        auto& destAnim = destArrowItem->destAnim( );
        destAnim.setPropertyName( "dest" );
        destAnim.setTargetObject( destArrowItem );
        destAnim.setDuration( ANIM_DURATION );
        destAnim.setStartValue( destItem->posAnim( ).startValue( ).toPointF( ));
        destAnim.setEndValue( destItem->posAnim( ).endValue( ).toPointF( ));
        destAnim.start( );
      }
      else
      {
        auto originItem = dynamic_cast< QGraphicsItem* > (
                                dynamic_cast<QGraphicsItemRepresentation* >(
                                _originRep )->item( scene ) );

        auto destItem = dynamic_cast< QGraphicsItem* > (
                          dynamic_cast< QGraphicsItemRepresentation* >(
                              _destRep )->item( scene ));

        dynamic_cast< ConnectionArrowItem* >( arrowItem )
                        ->createArrow( originItem->pos( ),
                                       destItem->pos( ),
                                       lThickness );

      }
    }
  } // namespace cortex
} // namespace nslib
