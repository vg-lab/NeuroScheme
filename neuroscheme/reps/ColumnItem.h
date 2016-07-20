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
#ifndef __NEUROSCHEME__COLUMN_ITEM__
#define __NEUROSCHEME__COLUMN_ITEM__

#include "../Color.h"
#include "ColumnRep.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>

namespace neuroscheme
{
  class CollapsableItem
  {
  public:

    CollapsableItem( bool collapsed_ = true )
      : _collapsed( collapsed_ ) { }
    virtual ~CollapsableItem( void ) { }
    virtual void toggleCollapse( void )
    {
      if ( _collapsed )
        this->uncollapse( );
      else
        this->collapse( );
    }
    virtual void collapse(bool anim = true) = 0;
    virtual void uncollapse(bool anim = true) = 0;
  protected:
    bool _collapsed;
  };

  class CollapseButton
    : public QGraphicsEllipseItem
  {

  public:
    CollapseButton( void );

    void mousePressEvent( QGraphicsSceneMouseEvent* event );
    void hoverEnterEvent( QGraphicsSceneHoverEvent* event );
    void hoverLeaveEvent( QGraphicsSceneHoverEvent* event );

  };

  class ColumnLayerItem
    : public QObject
    , public QGraphicsPathItem

  {
    Q_OBJECT
    Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

  public:

    ColumnLayerItem( unsigned int layer_,
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

    virtual ~ColumnLayerItem( void ) {}


  public slots:

    void disable(void)
    {
      this->setEnabled( false );
      this->setVisible( false );
    }

  protected:
    unsigned int _layer;
  };


  class ColumnItem
    : public QGraphicsPathItem
    , public CollapsableItem
  {

  public:

    ColumnItem( const ColumnRep& columnRep,
                unsigned int size = 300 );

    virtual ~ColumnItem( void ) {}

    void collapse( bool anim = true );
    void uncollapse( bool anim = true );

  protected:
    QGraphicsLineItem* _collapseItemVerticalLine;
    ColumnLayerItem* _layerItems[ 6 ];
    QPropertyAnimation* _layerAnimations[ 6 ];

  };


} // namespace neuroscheme

#endif
