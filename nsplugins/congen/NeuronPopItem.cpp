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
#include "NeuronPopItem.h"
#include <nslib/reps/RingItem.h>
#include <QPen>

#define POSX0 1.0f                    // Precomputed values for cos(0)
#define POSY0 0.0f                    // Precomputed values for sin(0)

#define POSX1 0.5000007660251953f     // Precomputed values for cos(60)
#define POSY1 0.8660249615191342f     // Precomputed values for sin(60)

#define POSX2 -0.49999846794843594f   // Precomputed values for cos(120)
#define POSY2 0.8660262883130146f     // Precomputed values for sin(120)

#define POSX3 -0.9999999999964793f    // Precomputed values for cos(180)
#define POSY3 0.00000265358979335273f // Precomputed values for sin(180)

#define POSX4 -0.5000030640984338f    // Precomputed values for cos(240)
#define POSY4 -0.8660236347191557f    // Precomputed values for sin(240)

#define POSX5 0.49999616986815576f    // Precomputed values for cos(300)
#define POSY5 -0.8660276151007971f    // Precomputed values for sin(300)

namespace nslib
{
  namespace congen
  {
    NeuronPopItem::NeuronPopItem( const NeuronPopRep& neuronRep,
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

      this->_parentRep = &( const_cast< NeuronPopRep& >( neuronRep ));
    }

    void NeuronPopItem::hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
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
    void NeuronPopItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
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

    void NeuronPopItem::contextMenuEvent(
      QGraphicsSceneContextMenuEvent* event_ )
    {
      if ( _interactive )
      {
        InteractionManager::contextMenuEvent( this, event_ );
      }
    }

  } // namespace congen
} // namespace nslib
