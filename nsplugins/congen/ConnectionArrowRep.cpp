/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
#include "ConnectionArrowRep.h"

#include <nslib/Color.h>

#include <QVector2D>
#include <nslib/RepresentationCreatorManager.h>

namespace nslib
{
  namespace congen
  {
    ConnectionArrowRep::ConnectionArrowRep( shift::Representation* originRep_,
      shift::Representation* destRep_, const bool isAggregated_ )
      : shiftgen::ConnectionArrowRep( )
      , _originRep( originRep_ )
      , _destRep( destRep_ )
      , _isAggregated( isAggregated_ )
      , _lineStyle( _isAggregated ?
        Qt::PenStyle::DotLine : Qt::PenStyle::SolidLine )
    {}

    ConnectionArrowRep::ConnectionArrowRep( const ConnectionArrowRep& other )
      : shiftgen::ConnectionArrowRep( other )
      , _originRep( other._originRep )
      , _destRep( other._destRep )
      , _isAggregated( other._isAggregated )
      , _lineStyle( _isAggregated ?
        Qt::PenStyle::DotLine : Qt::PenStyle::SolidLine )
    {}

    QGraphicsItem*
    ConnectionArrowRep::item( QGraphicsScene* scene, bool create )
    {
      QGraphicsItem* item;
      auto it = _items.find( scene );
      if ( create && it == _items.end())
      {
        item = new ConnectionArrowItem( *this );
        _items.insert( std::make_pair( scene, item ));
      }
      else
      {
        item = it->second;
      }

      return item;
    }

    void ConnectionArrowRep::preRender( shift::OpConfig* opConfig_ )
    {
      OpConfig* opConfig = dynamic_cast< OpConfig* >( opConfig_ );
      if( !opConfig )
      {
        return;
      }

      GraphicsScene* scene = opConfig->scene( );

      auto arrowItem = this->item( scene );

      if( opConfig->isAnimating( ))
      {
        auto originRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _originRep );
        auto destRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _destRep );

        auto originItem = dynamic_cast< Item* >( originRep->item( scene ));
        auto destItem = dynamic_cast< Item* >( destRep->item( scene ));

        if( originItem == nullptr )
        {
          Loggers::get( )->log( "No successfully dynamic cast on originItem",
            LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );

          return;
        }

        if( destItem == nullptr )
        {
          Loggers::get( )->log( "No successfully dynamic cast on destItem",
            LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );

          return;
        }

        auto originArrowItem =
          dynamic_cast< ConnectionArrowItem* >( arrowItem );
        auto& lineAnim = originArrowItem->lineAnim( );
        lineAnim.setPropertyName( "line" );
        lineAnim.setTargetObject( originArrowItem );
        lineAnim.setDuration( ANIM_DURATION );

        auto originPosAnimStart =
          originItem->posAnim( ).startValue( ).toPointF( );
        auto originPosAnimEnd = originItem->posAnim( ).endValue( ).toPointF( );
        auto destPosAnimStart = destItem->posAnim( ).startValue( ).toPointF( );
        auto destPosAnimEnd = destItem->posAnim( ).endValue( ).toPointF( );
        auto originWidth_2 =
          originRep->item( scene )->boundingRect( ).width( ) * 0.5f;
        auto destWidth_2 =
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
          QLineF( QPointF( destIniOri.x( ), destIniOri.y( )),
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
            _originRep )->item( scene ));

        auto destItem = dynamic_cast< QGraphicsItem* > (
          dynamic_cast< QGraphicsItemRepresentation* >(
            _destRep )->item( scene ));

        const auto destOri = QVector2D( originItem->pos( )) +
          ( ( originItem->boundingRect( ).width( ) * 0.5f *
            originItem->scale( )) * QVector2D( destItem->pos( ) -
            originItem->pos( )).normalized( ));

        const auto destDest = QVector2D( destItem->pos( )) -
          ( ( destItem->boundingRect( ).width( ) * 0.5f *
            originItem->scale( )) * QVector2D( destItem->pos( ) -
            originItem->pos( )).normalized( ));

        dynamic_cast< ConnectionArrowItem* >( arrowItem )->
          createArrow( QPointF( destOri.x( ), destOri.y( )),
          QPointF( destDest.x( ), destDest.y( )));
      }
    }

    void ConnectionArrowRep::hoverEnterEvent( QGraphicsSceneHoverEvent* )
    {
      for( auto item_ : _items )
      {
        auto arrowItem = dynamic_cast< ConnectionArrowItem* >( item_.second );
        if( arrowItem )
        {
          arrowItem->hoverEnter( );
        }
      }
    }

    void ConnectionArrowRep::hoverLeaveEvent( QGraphicsSceneHoverEvent* )
    {
      for( auto item_ : _items )
      {
        auto arrowItem = dynamic_cast< ConnectionArrowItem* >( item_.second );
        if( arrowItem )
        {
          arrowItem->hoverLeave( );
        }
      }
    }

    void ConnectionArrowRep::highlight( const scoop::Color& color )
    {
      for( auto item_ : _items )
      {
        auto arrowItem = dynamic_cast< ConnectionArrowItem* >( item_.second );
        if( arrowItem )
        {
          arrowItem->highlight( color );
        }
      }
    }

    void ConnectionArrowRep::unHighlight( void )
    {
      hoverLeaveEvent( nullptr );
    }

    Qt::PenStyle ConnectionArrowRep::lineStyle( void ) const
    {
      return _lineStyle;
    }

    void ConnectionArrowRep::editConnectionWidget( void )
    {
      const auto& repsToEntities =
        RepresentationCreatorManager::repsToEntities( );
      const auto& originIt = repsToEntities.find( _originRep );
      const auto& destIt = repsToEntities.find( _destRep );
      const auto& repsEntitiesEnd = repsToEntities.end( );
      if ( originIt != repsEntitiesEnd && destIt != repsEntitiesEnd )
      {
        const auto& origEntities = originIt->second;
        const auto& destEntities = destIt->second;
        const auto origEntity = *origEntities.begin( );
        const auto destEntity = *destEntities.begin( );
        InteractionManager::createConnectionRelationship(
          origEntity, destEntity );
      }
    }

  } // namespace congen
} // namespace nslib
