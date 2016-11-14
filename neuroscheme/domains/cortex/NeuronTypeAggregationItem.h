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
#ifndef __NEUROSCHEME__NEURON_TYPE_AGGREGATION_ITEM__
#define __NEUROSCHEME__NEURON_TYPE_AGGREGATION_ITEM__

#include "../../Color.h"
#include "../../InteractionManager.h"
#include "../../reps/CollapsableItem.h"
#include "../../reps/Item.h"
#include "../../reps/InteractiveItem.h"
#include "ColumnRep.h"
#include "LayerRep.h"
#include "NeuronRep.h"
#include "NeuronTypeAggregationRep.h"
#include <shift_NeuronAggregationRep.h>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>

namespace neuroscheme
{


  class NeuronTypeAggregationItem
  // : public QObject
    : public QGraphicsPathItem
    , public Item
    , public SelectableItem
    , public InteractiveItem
  {
    //Q_OBJECT
    // Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

  public:

    NeuronTypeAggregationItem( const NeuronTypeAggregationRep& layerRep  );

    void create(
                 // QGraphicsItem *parent_,
                 unsigned int size = 50
               // const QPoint& pLayerUL,
               // const QPoint& pLayerUM,
               // const QPoint& pLayerUR,
               // unsigned int layerHeight,
               // unsigned int numNeuronsHeight,
               // float percPyr,
               // float percInter,
               // const QBrush& brush_
    );
    // unsigned int& layer( void );
    QAbstractGraphicsShapeItem* symbolItem( void ) { return _symbolItem; }
    virtual ~NeuronTypeAggregationItem( void ) {}

    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
    {
      std::cout << "NeuronTypeAggregationItem::hover" << std::endl;
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

  public slots:

    void disable( void )
    {
      this->setEnabled( false );
      this->setVisible( false );
    }

  protected:
    //unsigned int _layer;
    QAbstractGraphicsShapeItem* _symbolItem;

  };




} // namespace neuroscheme

#endif
