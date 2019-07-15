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
#include "NeuronItem.h"
#include <nslib/reps/RingItem.h>
#include <QPen>
#include <nslib/Config.h>

namespace nslib
{
  namespace cortex
  {
    NeuronItem::NeuronItem( const NeuronRep* neuronRep,
                            unsigned int size,
                            bool interactive_ )
    : _itemText( nullptr )
    {
      setInteractive( interactive_ );
      if ( interactive_ )
      {
        //std::cout << "Hover events" << std::endl;
        this->setAcceptHoverEvents( true );
      }
//else
      //     std::cout << "No events" << std::endl;

      int size_2 = ceil( float( size ) / 1.3f );
      this->setRect ( -size_2, -size_2,
                      size_2 * 2 , size_2 * 2 );
      this->setPen( QPen( Qt::NoPen ));

      const Color& bgColor = neuronRep->getPropertyValue< Color >( "bg" );
      const NeuronRep::TSymbol& symbol =
        neuronRep->getPropertyValue< NeuronRep::TSymbol >( "symbol" );
      const NeuronRep::Rings rings =
        neuronRep->getPropertyValue< NeuronRep::Rings >( "rings" );

      auto somaItem = new QGraphicsEllipseItem( );
      somaItem->setRect( - int( size ) / 2,
                         - int( size ) / 2,
                         size,
                         size );
      somaItem->setPen( Qt::NoPen );
      somaItem->setBrush( QBrush( bgColor ));

      QGraphicsItem* symbolItem = _createSymbolItem( symbol, size );

      const unsigned int ringItemsWidth = int( size / 10 );
      const unsigned int ringItemPadding = 2;

      somaItem->setParentItem( this );
      if ( symbolItem )
        symbolItem->setParentItem( this );

      unsigned int ringCount = 1;
      for ( const auto& ring : rings )
      {
        RingItem* ringItem =
          new RingItem(
            90,
            size / 2 + ( ringItemPadding + ringItemsWidth ) * ringCount,
            size / 2 + ( ringItemPadding + ringItemsWidth ) * ringCount,
            ringItemsWidth,
            ring.getPropertyValue< int >( "angle" ),
            ring.getPropertyValue< Color >( "color" ));

        ringItem->setParentItem( this );
        ringItem->setPen( Qt::NoPen );
        ringItem->setZValue( -1 );

        ++ringCount;
      }

      if ( Config::showEntitiesName( ))
      {
        _itemText = new ItemText( QString::fromStdString( neuronRep
          ->getPropertyValue< std::string >( "Entity name", "" )), this,
          0.25f, 0.65f, QColor::fromRgb( 245, 245, 245, 255 ),
         QColor::fromRgb( 5, 5, 5, 255 ) );
      }

      _parentRep = const_cast< NeuronRep* >( neuronRep );
    }

    void NeuronItem::hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
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

    void NeuronItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
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
              InteractionManager::hoverLeaveEvent( qAbstractGraphicItem,
                event_ );
            }
          }
        InteractionManager::highlightConnectivity( this, false );

      }
    }

    void NeuronItem::contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
    {
      if ( _interactive )
      {
        InteractionManager::contextMenuEvent( this, event_ );
      }
    }

    QGraphicsItem* NeuronItem::_createSymbolItem( NeuronRep::TSymbol symbol,
                                                  unsigned int size )
    {
      QGraphicsItem* symbolItem = nullptr;

      switch ( symbol )
      {
      case NeuronRep::TRIANGLE:
      {
        QPolygonF triangle;
        triangle.append( QPointF(                 0, - int( size ) / 3 ));
        triangle.append( QPointF(          size / 3, size/4 ));
        triangle.append( QPointF( - int( size ) / 3, size / 4 ));
        triangle.append( QPointF(                 0, - int( size ) / 3 ));

        auto triangleItem =
          new QGraphicsPolygonItem( triangle );

        triangleItem->setPen( Qt::NoPen );
        triangleItem->setBrush( QBrush( QColor( 255, 255, 255 )));
        triangleItem->setX( 0 );
        triangleItem->setY( - int( size ) /20 );

        symbolItem = triangleItem;

        break;
      }
      case NeuronRep::CIRCLE:
      {

        auto circleItem = new QGraphicsEllipseItem( );
        circleItem->setRect( - int( size ) / 4, - int( size ) / 4,
                             size / 2, size / 2);
        circleItem->setPen( Qt::NoPen );
        circleItem->setBrush( QBrush( QColor(255, 255, 255 )));

        symbolItem = circleItem;

        break;
      }
      default:
        break;
        // no symbol needed
      }

      return symbolItem;
    }

    NeuronItem::~NeuronItem( void )
    {
      delete _itemText;
    }

  } // namespace cortex
} // namespace nslib
