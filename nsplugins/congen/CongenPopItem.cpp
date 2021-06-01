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

#include "CongenPopItem.h"
#include <nslib/reps/RingItem.h>
#include <QPen>
#include <nslib/Config.h>

namespace nslib
{
  namespace congen
  {
    CongenPopItem::CongenPopItem( const CongenPopRep& entityRep,
      const unsigned int size, const bool interactive_ )
      :_itemText( nullptr )
    {
      setInteractive( interactive_ );
      if ( interactive_ )
      {
        this->setAcceptHoverEvents( true );
      }

      const int itemSize = static_cast<int>( ceilf( static_cast<float>( size ) * 0.5f ));
      this->setRect ( -itemSize, -itemSize, itemSize * 2 , itemSize * 2 );
      this->setPen( QPen( Qt::NoPen ));

      const Color& bgColor = entityRep.getPropertyValue< Color >( "color" );
      auto circleItem = new QGraphicsEllipseItem( this );
      const float circleItemSize = size * 0.9f;
      const float halfcircleItemSize = - 0.5f * circleItemSize;
      circleItem->setRect( halfcircleItemSize, halfcircleItemSize,
        circleItemSize, circleItemSize );
      circleItem->setPen( Qt::NoPen );
      circleItem->setBrush( QBrush( bgColor ));

      auto circleItemInner = new QGraphicsEllipseItem( this );
      const float circleItemSizeInner = size * 0.7f;
      const float halfcircleItemSizeInner = - 0.5f * circleItemSizeInner;
      circleItemInner->setRect( halfcircleItemSizeInner, halfcircleItemSizeInner,
        circleItemSizeInner, circleItemSizeInner );
      circleItemInner->setPen( Qt::NoPen );
      circleItemInner->setBrush( QBrush( QColor( 255, 255, 255 )));

      const float barHeight = size * 0.1f;
      const float halfBarHeight = - 0.5f * barHeight;
      const float barWidth = circleItemSizeInner * 1.1f;
      const float halfBarWidth = - 0.5f * barWidth;

      auto bar = new QGraphicsRectItem(
        halfBarWidth, halfBarHeight,
        barWidth,  barHeight );
      bar->setPen( QColor( bgColor ));
      bar->setBrush( QColor( 255, 255, 255 ));
      bar->setParentItem( this );

      auto barFill = new QGraphicsRectItem(
        halfBarWidth, halfBarHeight,
        roundf(barWidth *
        entityRep.getPropertyValue< float >( "line perc", .0f )),
        barHeight );
      barFill->setPen( Qt::NoPen );
      barFill->setBrush( QColor( bgColor ));
      barFill->setParentItem( bar );

      if ( Config::showEntitiesName( ))
      {
        _itemText = new ItemText( QString::fromStdString( entityRep
          .getPropertyValue<std::string>( "Entity name", " " )), this );
      }

      _parentRep = &( const_cast< CongenPopRep& >( entityRep ));
    }

    void CongenPopItem::hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
    {
      if ( _interactive )
      {
        auto qGraphicsItemRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _parentRep );
        if ( qGraphicsItemRep )
        {
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
        }

        InteractionManager::highlightConnectivity( this );
      }
    }

    void CongenPopItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      if ( _interactive )
      {
        auto qGraphicsItemRep =
          dynamic_cast< QGraphicsItemRepresentation* >( _parentRep );
        if ( qGraphicsItemRep )
        {
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
        }

        InteractionManager::highlightConnectivity( this, false );
      }
    }

    void CongenPopItem::contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
    {
      if ( _interactive )
      {
        InteractionManager::contextMenuEvent( this, event_ );
      }
    }

    CongenPopItem::~CongenPopItem( void )
    {
      if(_itemText) delete _itemText;
    }

  } // namespace congen
} // namespace nslib
