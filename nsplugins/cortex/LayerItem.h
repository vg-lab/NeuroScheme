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

#ifndef __NSLIBCORTEX__LAYER_ITEM__
#define __NSLIBCORTEX__LAYER_ITEM__

#include <nslib/reps/Item.h>
#include <nslib/reps/SelectableItem.h>
#include <nslib/reps/InteractiveItem.h>
#include <nslib/InteractionManager.h>
#include "LayerRep.h"

namespace nslib
{
  namespace cortex
  {
    class LayerItem
      : public QObject
      , public QGraphicsPathItem
      , public ::nslib::Item
      , public ::nslib::SelectableItem
      , public ::nslib::InteractiveItem
    {
      Q_OBJECT
      Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

      public:
        virtual ~LayerItem( void ) {}
        LayerItem( const LayerRep& layerRep );

        void create( const unsigned int layer_, QGraphicsItem *parent_,
          const QPoint& pLayerUL, const QPoint& pLayerUM, const QPoint& pLayerUR,
          unsigned int layerHeight, unsigned int numNeuronsHeight, float percPyr,
          float percInter, const QBrush& brush_ );

        unsigned int& layer( void );

        virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ ) override;

        virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ ) override;

        virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ ) override;

      public slots:
        void disable( void );

      protected:
        unsigned int _layer;
    };
  }
}


#endif //__NSLIBCORTEX__LAYER_ITEM__
