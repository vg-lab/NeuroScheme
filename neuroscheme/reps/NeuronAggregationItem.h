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

#include "../Color.h"
#include "ColumnRep.h"
#include "NeuronRep.h"
#include "CollapsableItem.h"
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

  {
    Q_OBJECT
    Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

  public:

    LayerItem( unsigned int layer_,
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


  public slots:

    void disable(void)
    {
      this->setEnabled( false );
      this->setVisible( false );
    }

  protected:
    unsigned int _layer;
  };


  class NeuronAggregationItem
    : public QGraphicsPathItem
    , public CollapsableItem
  {

  public:

    NeuronAggregationItem( void ) {}


    virtual ~NeuronAggregationItem( void ) {}

    void collapse( bool anim = true );
    void uncollapse( bool anim = true );

  protected:

    void _createNeuronAggregationItem(
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
