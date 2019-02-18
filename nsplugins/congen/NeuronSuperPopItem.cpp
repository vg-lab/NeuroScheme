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

      int itemSize = static_cast< int >( ceil( float( size ) * 0.5f ));
      this->setRect ( -itemSize, -itemSize, itemSize * 2 , itemSize * 2 );
      this->setPen( QPen( Qt::NoPen ));

      const Color& bgColor = neuronRep.getProperty( "color" ).value< Color >( );
      (void)( bgColor );
      auto circleItem = new QGraphicsEllipseItem( this );
      auto circleItemSize = roundf( size * 0.9f );
      int halfcircleItemSize = - static_cast< int >(roundf( size * 0.45f ));
      circleItem->setRect( halfcircleItemSize, halfcircleItemSize,
        circleItemSize, circleItemSize );
      circleItem->setPen( Qt::NoPen );
      circleItem->setBrush( QBrush( bgColor ));

      auto circleItemInner = new QGraphicsEllipseItem( this );
      auto circleItemSizeInner = roundf( size * 0.7f );
      int halfcircleItemSizeInner = - static_cast< int >( roundf( size * 0.35f ));
      circleItemInner->setRect( halfcircleItemSizeInner, halfcircleItemSizeInner,
                                circleItemSizeInner, circleItemSizeInner );
      circleItemInner->setPen( Qt::NoPen );
      circleItemInner->setBrush( QBrush( QColor( 255, 255, 255 )));

      auto circleItemInner2 = new QGraphicsEllipseItem( this );
      auto circleItemSizeInner2 = roundf( size * 0.6f );
      int halfcircleItemSizeInner2 = - static_cast< int >( roundf( size * 0.3f ));
      circleItemInner2->setRect( halfcircleItemSizeInner2, halfcircleItemSizeInner2,
                                circleItemSizeInner2, circleItemSizeInner2 );
      circleItemInner2->setPen( bgColor );

      int barWidth = roundf( size * 0.05f );

      auto bar = new QGraphicsRectItem(
        -barWidth, halfcircleItemSizeInner-2,
        2 * barWidth, circleItemSizeInner+4
        );
      bar->setPen( QColor( bgColor ));
      bar->setBrush( QColor( 255, 255, 255 ));
      bar->setParentItem( this );

      auto barFill = new QGraphicsRectItem(
        -barWidth, halfcircleItemSizeInner-2,
        2 * barWidth,
        roundf(( circleItemSizeInner + 4 ) *
                 neuronRep.getProperty( "line perc" ).value< float >( )));
      barFill->setPen( Qt::NoPen );
      barFill->setBrush( QColor( bgColor ));
      barFill->setParentItem( bar );

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
