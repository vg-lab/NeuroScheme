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
#ifndef __NEUROSCHEME__NEURON_AGGREGATION_ITEM__
#define __NEUROSCHEME__NEURON_AGGREGATION_ITEM__

#include "../../Color.h"
#include "../../InteractionManager.h"
#include "../../reps/CollapsableItem.h"
#include "../../reps/Item.h"
#include "../../reps/InteractiveItem.h"
#include "ColumnRep.h"
#include "LayerRep.h"
#include "NeuronRep.h"
#include <shift_NeuronAggregationRep.h>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>

namespace neuroscheme
{

  using Layers = shiftgen::NeuronAggregationRep::Layers;

  class LayerItem
    : public QObject
    , public QGraphicsPathItem
    , public Item
    , public SelectableItem
    , public InteractiveItem
  {
    Q_OBJECT
    Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

  public:


  LayerItem( const LayerRep& layerRep );
      // unsigned int layer_,
      //   QGraphicsItem *parent_ //,
        // const QPoint& pLayerUL,
        // const QPoint& pLayerUM,
        // const QPoint& pLayerUR,
        // unsigned int layerHeight,
        // unsigned int numNeuronsHeight,
        // float percPyr,
        // float percInter,
        // const QBrush& brush_
        //);

  void create( unsigned int layer_,
               QGraphicsItem *parent_,
               const QPoint& pLayerUL,
               const QPoint& pLayerUM,
               const QPoint& pLayerUR,
               unsigned int layerHeight,
               unsigned int numNeuronsHeight,
               float percPyr,
               float percInter,
               const QBrush& brush_ );
    unsigned int& layer( void );
    virtual ~LayerItem( void ) {}

    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverEnterEvent( this, event_ );
    }
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverLeaveEvent( this, event_ );
    }
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
    {
      InteractionManager::contextMenuEvent( this, event_ );
    }

    virtual void mousePressEvent( QGraphicsSceneMouseEvent* event_ )
    {
      InteractionManager::mousePressEvent( this, event_ );
    }

//     virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* // event_
//       )
//     {
//       std::cout << "enter layer" << std::endl;
// //InteractionManager::hoverEnterEvent( this, event_ );
//     }
//     virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* // event_
//       )
//     {
//       std::cout << "leave layer" << std::endl;
//       //InteractionManager::hoverLeaveEvent( this, event_ );
//     }


  public slots:

    void disable( void )
    {
      this->setEnabled( false );
      this->setVisible( false );
    }

  protected:
    unsigned int _layer;
  };


  class NeuronAggregationItem
    : public QObject
    , public QGraphicsPathItem
    , public Item
    , public CollapsableItem
    , public SelectableItem
    , public InteractiveItem
  {
    Q_OBJECT
    Q_PROPERTY( QPointF pos READ pos WRITE setPos )
    Q_PROPERTY( qreal scale READ scale WRITE setScale )

  public:

    NeuronAggregationItem( void )
    {
      this->setAcceptHoverEvents( true );
    }


    virtual ~NeuronAggregationItem( void ) {}

    void collapse( bool anim = true );
    void uncollapse( bool anim = true );

    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverEnterEvent( this, event_ );
    }
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverLeaveEvent( this, event_ );
    }
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
    {
      InteractionManager::contextMenuEvent( this, event_ );
    }

    virtual void mousePressEvent( QGraphicsSceneMouseEvent* event_ )
    {
      InteractionManager::mousePressEvent( this, event_ );
    }


  protected:

    void _createNeuronAggregationItem(
      QGraphicsScene* scene,
      const NeuronRep& meanNeuron,
      const Layers& layers,
      const QPainterPath& path,
      const QPoint& layerLeftPoint,
      const QPoint& layerCenterPoint,
      const QPoint& layerRightPoint,
      const QColor& baseColor,
      unsigned int size = 300 );

    QGraphicsLineItem* _collapseItemVerticalLine;
    LayerItem* _layerItems[ 6 ];
    QPropertyAnimation* _layerAnimations[ 6 ];

  };


} // namespace neuroscheme

#endif
