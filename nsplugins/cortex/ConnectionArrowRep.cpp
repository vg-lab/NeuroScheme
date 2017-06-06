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

#include <QVector2D>

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
      //float lAdjustedParam = 5.0f;

      if (opConfig->isAnimating())
      {
        //TODO: It needs to be changed in the future!
        auto originRep = dynamic_cast<QGraphicsItemRepresentation* >( _originRep );
        auto destRep = dynamic_cast<QGraphicsItemRepresentation* >( _destRep );

        auto originItem = dynamic_cast< Item* >( originRep->item( scene ));
        auto destItem = dynamic_cast< Item* >( destRep->item( scene ));

        if ( originItem == nullptr )
          Log::log( "No successfully dynamic cast on originItem",
                    LOG_LEVEL_ERROR );

        if ( destItem == nullptr )
          Log::log( "No successfully dynamic cast on destItem",
                    LOG_LEVEL_ERROR );

        auto originArrowItem = dynamic_cast< ConnectionArrowItem* >( arrowItem );
        auto& lineAnim = originArrowItem->lineAnim( );
        lineAnim.setPropertyName( "line" );
        lineAnim.setTargetObject( originArrowItem );
        lineAnim.setDuration( ANIM_DURATION );

        auto destIniOri =
            QVector2D( originItem->posAnim( ).startValue( ).toPointF( ) )
                       +
                       ( (originRep->item( scene )->boundingRect().width() * 0.5f * originItem->scaleAnim().startValue().toDouble())
                       * QVector2D(destItem->posAnim( ).startValue( ).toPointF( )
                                   - originItem->posAnim( ).startValue( ).toPointF( ) )
                                   .normalized());

        auto destIniDest =
            QVector2D( destItem->posAnim( ).startValue( ).toPointF( ) )
                       -
                       ( (destRep->item( scene )->boundingRect().width() * 0.5f * destItem->scaleAnim().startValue().toDouble())
                       * QVector2D(destItem->posAnim( ).startValue( ).toPointF( )
                                   - originItem->posAnim( ).startValue( ).toPointF( ) )
                                   .normalized());

        auto destEndOri =
            QVector2D( originItem->posAnim( ).endValue( ).toPointF( ) )
                       +
                       ( (originRep->item( scene )->boundingRect().width() * 0.5f * originItem->scaleAnim().endValue().toDouble())
                       * QVector2D(destItem->posAnim( ).endValue( ).toPointF( )
                                   - originItem->posAnim( ).endValue( ).toPointF( ) )
                                   .normalized());

        auto destEndDest =
            QVector2D( destItem->posAnim( ).endValue( ).toPointF( ) )
                       -
                       ( (destRep->item( scene )->boundingRect().width() * 0.5f * destItem->scaleAnim().endValue().toDouble())
                       * QVector2D(destItem->posAnim( ).endValue( ).toPointF( )
                                   - originItem->posAnim( ).endValue( ).toPointF( ) )
                                   .normalized());

        lineAnim.setStartValue(
          QLineF( QPointF(destIniOri.x(),destIniOri.y()) ,
              QPointF(destIniDest.x(), destIniDest.y()) ) );
        lineAnim.setEndValue(
          QLineF( QPointF(destEndOri.x(), destEndOri.y()),
                  QPointF(destEndDest.x(), destEndDest.y() ) ) );

//        lineAnim.setStartValue(
//          QLineF( originItem->posAnim( ).startValue( ).toPointF( ),
//                  destItem->posAnim( ).startValue( ).toPointF( )));
//        lineAnim.setEndValue(
//          QLineF( originItem->posAnim( ).endValue( ).toPointF( ),
//                  destItem->posAnim( ).endValue( ).toPointF( )));

        lineAnim.start( );

      }
      else
      {
        auto originItem = dynamic_cast< QGraphicsItem* > (
                                dynamic_cast<QGraphicsItemRepresentation* >(
                                _originRep )->item( scene ) );

        auto destItem = dynamic_cast< QGraphicsItem* > (
                          dynamic_cast< QGraphicsItemRepresentation* >(
                              _destRep )->item( scene ));

        //std::cout<<"Scala"<<originItem->scale()<<std::endl;
        auto destOri =  QVector2D( originItem->pos( ) )
                        + ( (originItem->boundingRect().width() * 0.5f * originItem->scale() )
                        * QVector2D(destItem->pos( ) - originItem->pos( ) ).normalized());

        auto destDest =  QVector2D( destItem->pos( ) )
                        - ( (destItem->boundingRect().width() * 0.5f * originItem->scale()  )
                        * QVector2D(destItem->pos( ) - originItem->pos( ) ).normalized())
                        ;

        dynamic_cast< ConnectionArrowItem* >( arrowItem )
                        ->createArrow( QPointF(destOri.x(), destOri.y( ) ),
                                       QPointF(destDest.x(), destDest.y( ) ),
                                       lThickness );

      }
    }
  } // namespace cortex
} // namespace nslib
