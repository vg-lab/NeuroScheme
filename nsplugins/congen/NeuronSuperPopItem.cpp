/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 *          Iago Calvo <i.calvol@alumnos.urjc.es>
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
#include "NeuronSuperPopItem.h"
#include <nslib/reps/RingItem.h>
#include <QPen>

#define POSX0 0.0f              // Precomputed value for cos(270)
#define POSY0 -1.0f             // Precomputed value for sin(270)

#define POSX1 -0.86602540378f   // Precomputed value for cos(150)
#define POSY1 0.5f              // Precomputed value for sin(150)

#define POSX2 0.86602540378f    // Precomputed value for cos(30)
#define POSY2 0.5f              // Precomputed value for sin(30)

namespace nslib
{
  namespace congen
  {
    NeuronSuperPopItem::NeuronSuperPopItem( const NeuronSuperPopRep& neuronRep,
      unsigned int size, bool interactive_ )
    {
      setInteractive( interactive_ );
      if ( interactive_ )
      {
        this->setAcceptHoverEvents( true );
      }

      int itemSize = static_cast<int>( ceil( float( size ) * 0.5f ));
      this->setRect ( -itemSize, -itemSize, itemSize * 2 , itemSize * 2 );
      this->setPen( QPen( Qt::NoPen ));

      const Color& bgColor = neuronRep.getProperty( "color" ).value< Color >( );

      auto circleItem = new QGraphicsEllipseItem( /* this */ );
      auto circleItemSize = roundf( size * 0.75f );
      int halfcircleItemSize = - static_cast<int>(roundf( size * 0.375f ));
      circleItem->setRect( halfcircleItemSize, halfcircleItemSize,
        circleItemSize, circleItemSize );
      circleItem->setPen( Qt::NoPen );
      circleItem->setBrush( QBrush( bgColor ));

      QPainterPath path_;
      QPolygon poly;

      float size_2 = roundf( size * 0.5f );

      poly << QPoint(
        ( size_2 * POSX0 ),
        ( size_2 * POSY0 )
      );

      poly << QPoint(
        ( size_2 * POSX1 ),
        ( size_2 * POSY1 )
      );

      poly << QPoint(
        ( size_2 * POSX2 ),
        ( size_2 * POSY2 )
      );

      path_.addPolygon( poly );
      path_.closeSubpath(  );

      auto icon = new QGraphicsPathItem( this );
      icon->setPath( path_ );
      icon->setPen( Qt::NoPen );
      icon->setBrush( bgColor ); //QBrush( QColor( 114, 188, 196 )));
      //icon->setBrush( QBrush( baseColor ));

      // auto lineContainerWidth = roundf( circleItemSize * .9f );
      // auto lineContainerHeight = roundf( circleItemSize * .1f );
      // auto lineContainer = new QGraphicsRectItem(
      //   roundf( - int( lineContainerWidth ) * .5f ),
      //   roundf( - int( lineContainerHeight ) * .5f ),
      //   lineContainerWidth,
      //   lineContainerHeight );

      // lineContainer->setPen( Qt::NoPen );
      // lineContainer->setBrush( QBrush( QColor( 255, 255, 255 )));
      // // lineContainer->setParentItem( this );

      //auto linePadding = roundf( lineContainerWidth * 0.01f );
      // auto lineWidth = lineContainerWidth; //roundf(  circleItemSize * .85f );
      // auto lineHeight = lineContainerHeight; //roundf( circleItemSize * .09f );
      auto line = new QGraphicsRectItem(
        size_2 * POSX1,
        size_2 * POSY1 - size_2 * 0.03,
        roundf( size_2 * ( POSX2 - POSX1) *
                neuronRep.getProperty( "line perc" ).value< float >( )),
        size_2 * 0.06 );

        // roundf( - int( lineWidth ) * .5f ) + linePadding,
        // roundf( - int( lineHeight ) * .5f ) + linePadding,
        // roundf( lineWidth *
        //         neuronRep.getProperty( "line perc" ).value< float >( )) -
        // 2 * linePadding,
        // lineHeight - 2 * linePadding);

      line->setPen( Qt::NoPen );
      line->setBrush( QBrush( QColor( 180, 70, 70 )));
      line->setParentItem( this );

      this->_parentRep = &( const_cast< NeuronSuperPopRep& >( neuronRep ));
    }

    void NeuronSuperPopItem::hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
    {
      if ( _interactive )
      {
        auto qGraphicsItemRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _parentRep );
        if ( qGraphicsItemRep )
          for ( auto& item : qGraphicsItemRep->items( ))
          {
            auto qAbstractGraphicItem =
              dynamic_cast< QAbstractGraphicsShapeItem* >( item.second );
            if ( qAbstractGraphicItem )
            {
              InteractionManager::hoverEnterEvent(
                qAbstractGraphicItem, event_ );
            }
          }
        InteractionManager::highlightConnectivity( this );
      }
    }
    void NeuronSuperPopItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      if ( _interactive )
      {
        auto qGraphicsItemRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _parentRep );
        if ( qGraphicsItemRep )
          for ( auto& item : qGraphicsItemRep->items( ))
          {
            auto qAbstractGraphicItem =
              dynamic_cast< QAbstractGraphicsShapeItem* >( item.second );
            if ( qAbstractGraphicItem )
            {
              InteractionManager::hoverLeaveEvent(
                qAbstractGraphicItem, event_ );
            }

          }
        InteractionManager::highlightConnectivity( this, false );
      }
    }

    void NeuronSuperPopItem::contextMenuEvent(
      QGraphicsSceneContextMenuEvent* event_ )
    {
      if ( _interactive )
      {
        InteractionManager::contextMenuEvent( this, event_ );
      }
    }

  } // namespace congen
} // namespace nslib
