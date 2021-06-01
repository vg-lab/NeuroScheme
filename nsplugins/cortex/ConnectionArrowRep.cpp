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

      auto arrowItem = this->item( scene );
      auto originArrowItem = dynamic_cast< ConnectionArrowItem* >( arrowItem );

      if (opConfig->isAnimating())
      {
        auto originRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _originRep );
        auto destRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _destRep );

        auto originItem = dynamic_cast< Item* >( originRep->item( scene ));
        auto destItem = dynamic_cast< Item* >( destRep->item( scene ));

        if ( originItem == nullptr )
        {
          Loggers::get( )->log( "No successfully dynamic cast on originItem",
                                LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
          return;
        }

        if ( destItem == nullptr )
        {
          Loggers::get( )->log( "No successfully dynamic cast on destItem",
                                LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
          return;
        }

        auto& lineAnim = originArrowItem->lineAnim( );
        lineAnim.setPropertyName( "line" );
        lineAnim.setTargetObject( originArrowItem );
        lineAnim.setDuration( ANIM_DURATION );


        const auto originPosAnimStart = originItem->posAnim( ).startValue( ).toPointF( );
        const auto originPosAnimEnd = originItem->posAnim( ).endValue( ).toPointF( );
        const auto destPosAnimStart = destItem->posAnim( ).startValue( ).toPointF( );
        const auto destPosAnimEnd = destItem->posAnim( ).endValue( ).toPointF( );
        const auto originWidth_2 =
          originRep->item( scene )->boundingRect( ).width( ) * 0.5f;
        const auto destWidth_2 =
          destRep->item( scene )->boundingRect( ).width( ) * 0.5f;

        const auto& originScaleAnim = originItem->scaleAnim( );
        const auto& destScaleAnim = destItem->scaleAnim( );

        const auto normAnimStart =
          QVector2D( destPosAnimStart - originPosAnimStart ).normalized( );
        const auto normAnimEnd =
          QVector2D( destPosAnimEnd - originPosAnimEnd ).normalized( );


        const auto destIniOri =
          QVector2D( originPosAnimStart ) + originWidth_2 *
          originScaleAnim.startValue( ).toDouble( ) * normAnimStart;

        const auto destIniDest =
          QVector2D( destPosAnimStart ) - destWidth_2 *
          originScaleAnim.startValue( ).toDouble( ) * normAnimStart;

        const auto destEndOri =
          QVector2D( originPosAnimEnd ) + originWidth_2 *
          originScaleAnim.endValue( ).toDouble( ) * normAnimEnd;

        const auto destEndDest =
          QVector2D( destPosAnimEnd ) - destWidth_2 *
          destScaleAnim.endValue( ).toDouble( ) * normAnimEnd;


        lineAnim.setStartValue(
          QLineF( QPointF( destIniOri.x( ),destIniOri.y( )),
                  QPointF( destIniDest.x( ), destIniDest.y( ))));
        lineAnim.setEndValue(
          QLineF( QPointF( destEndOri.x( ), destEndOri.y( )),
                  QPointF( destEndDest.x( ), destEndDest.y( ))));

        lineAnim.start( );

      }
      else
      {
        auto originItem = dynamic_cast< QGraphicsItem* > (
          dynamic_cast< QGraphicsItemRepresentation* >(
            _originRep )->item( scene ) );

        auto destItem = dynamic_cast< QGraphicsItem* > (
          dynamic_cast< QGraphicsItemRepresentation* >(
            _destRep )->item( scene ));

        const auto destOri = QVector2D( originItem->pos( )) +
          (( originItem->boundingRect().width( ) * 0.5f * originItem->scale( )) *
           QVector2D(destItem->pos( ) - originItem->pos( )).normalized( ));

        const auto destDest =  QVector2D( destItem->pos( )) -
          ((destItem->boundingRect().width() * 0.5f * originItem->scale( )) *
           QVector2D(destItem->pos( ) - originItem->pos( )).normalized());

        originArrowItem->createArrow( QPointF(destOri.x( ), destOri.y( ) ),
                                      QPointF(destDest.x( ), destDest.y( ) ));
      }
    }

    void ConnectionArrowRep::hoverEnterEvent( QGraphicsSceneHoverEvent*  )
    {
      auto hoverEventEnter = [](std::pair<QGraphicsScene *, QGraphicsItem *> p)
      {
        auto arrowItem = dynamic_cast< ConnectionArrowItem* >( p.second );
        if ( arrowItem ) arrowItem->hoverEnter( );
      };
      std::for_each(_items.begin(), _items.end(), hoverEventEnter);
    }

    void ConnectionArrowRep::hoverLeaveEvent( QGraphicsSceneHoverEvent*  )
    {
      auto hoverEventLeave = [](std::pair<QGraphicsScene *, QGraphicsItem *> p)
      {
        auto arrowItem = dynamic_cast< ConnectionArrowItem* >( p.second );
        if ( arrowItem ) arrowItem->hoverLeave( );
      };
      std::for_each(_items.begin(), _items.end(), hoverEventLeave);
    }

    void ConnectionArrowRep::highlight( const scoop::Color& color )
    {
      auto highlightArrow = [&color](std::pair<QGraphicsScene *, QGraphicsItem *> p)
      {
        auto arrowItem = dynamic_cast< ConnectionArrowItem* >( p.second );
        if ( arrowItem ) arrowItem->highlight(color);
      };
      std::for_each(_items.begin(), _items.end(), highlightArrow);
    }

  } // namespace cortex
} // namespace nslib
