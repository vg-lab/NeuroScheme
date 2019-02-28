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
#include <nslib/Config.h>

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
    const float NeuronSuperPopItem::minCircle = 0.1f;
    const float NeuronSuperPopItem::maxCircle = 0.7f;
    const float NeuronSuperPopItem::rangeCircle = maxCircle - minCircle;

    NeuronSuperPopItem::NeuronSuperPopItem( const NeuronSuperPopRep& neuronRep,
      unsigned int size, bool interactive_ )
    {
      setInteractive( interactive_ );
      if ( interactive_ )
      {
        this->setAcceptHoverEvents( true );
      }

      int itemSize = static_cast< int >( ceilf( float( size ) * 0.5f ));
      this->setRect ( -itemSize, -itemSize, itemSize * 2 , itemSize * 2 );
      this->setPen( QPen( Qt::NoPen ));

      const Color& bgColor = neuronRep.getProperty( "color" ).value< Color >( );
      auto circleItem = new QGraphicsEllipseItem( this );
      float circleItemSize = size * 0.9f;
      float halfcircleItemSize = - 0.5f * circleItemSize;
      circleItem->setRect( halfcircleItemSize, halfcircleItemSize,
        circleItemSize, circleItemSize );
      circleItem->setPen( Qt::NoPen );
      circleItem->setBrush( QBrush( bgColor ));

      auto circleItemInner = new QGraphicsEllipseItem( this );
      float circleItemSizeInner = size * maxCircle;
      float halfcircleItemSizeInner = - 0.5f * circleItemSizeInner;
      circleItemInner->setRect( halfcircleItemSizeInner, halfcircleItemSizeInner,
        circleItemSizeInner, circleItemSizeInner );
      circleItemInner->setPen( Qt::NoPen );
      circleItemInner->setBrush( QBrush( QColor( 255, 255, 255 )));

      int numCircles = neuronRep.getProperty( "num circles" )
        .value< unsigned int >( );

      float circleSeparation =
        neuronRep.getProperty("circles separation").value< float >( );
      float colorSeparation =
          neuronRep.getProperty("circles color separation").value< float >( );
      scoop::SequentialColorMap colorMap = neuronRep
        .getProperty("circles color map").value< scoop::SequentialColorMap >( );

      float realCircleSeparation = circleSeparation * size;
      float circleSizeInner2 = circleItemSizeInner;
      float colorValue = 0.0f;

      for( int i = 0; i < numCircles; ++i ){
        auto circleItemInner2 = new QGraphicsEllipseItem( this );
        circleSizeInner2 -= realCircleSeparation;
        colorValue += colorSeparation;
        float halfcircleItemSizeInner2 = - 0.5f * circleSizeInner2;
        circleItemInner2->setRect( halfcircleItemSizeInner2, halfcircleItemSizeInner2,
          circleSizeInner2, circleSizeInner2 );
        circleItemInner2->setPen( colorMap.getColor( colorValue ));
      }

      float barHeight = size * minCircle;
      float halfBarHeight = - 0.5f * barHeight;
      float barWidth = circleItemSizeInner * 1.1f;
      float halfBarWidth = - 0.5f * barWidth;

      auto bar = new QGraphicsRectItem(
        halfBarWidth, halfBarHeight,
        barWidth,  barHeight );
      bar->setPen( QColor( bgColor ));
      bar->setBrush( QColor( 255, 255, 255 ));
      bar->setParentItem( this );

      auto barFill = new QGraphicsRectItem(
        halfBarWidth, halfBarHeight,
        roundf(barWidth *
        neuronRep.getProperty( "line perc" ).value< float >( )),
        barHeight);
      barFill->setPen( Qt::NoPen );
      barFill->setBrush( QColor( bgColor ));
      barFill->setParentItem( bar );

      if ( nslib::Config::showEntitiesName( ))
      {
        auto text = new QGraphicsTextItem( QString::fromStdString(
          neuronRep.getProperty( "Entity name" ).value<std::string>( )));
        text->setPos( -0.32f * text->boundingRect( ).width( ),
          -0.32f * text->boundingRect( ).height( ));
        text->setDefaultTextColor( QColor::fromRgb( 0, 0, 0, 255 ));
        text->setScale( 0.64f );
        text->setParentItem( this );
      }


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
