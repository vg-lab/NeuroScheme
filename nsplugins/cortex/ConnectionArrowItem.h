/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 * 			Juan Pedro Brito <juanpedro.brito@upm.es>
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
#ifndef __NSLIB__CONNECTION_ARROW_ITEM__
#define __NSLIB__CONNECTION_ARROW_ITEM__

#include <nslib/Color.h>
#include <nslib/InteractionManager.h>
#include <nslib/reps/InteractiveItem.h>
#include <nslib/reps/Item.h>
#include <nslib/reps/SelectableItem.h>
#include "ConnectionArrowRep.h"

//#include <QGraphicsEllipseItem>
//#include <QPainterPath>
#include <QGraphicsPolygonItem>
#include <QPropertyAnimation>

#include <ctime>
#include <math.h>

namespace nslib
{
  namespace cortex
  {

    class ConnectionArrowItem
      : public QObject
      //TODO
      //, public QGraphicsLineItem
      , public QGraphicsPolygonItem
      //, public QPainterPath
      , public nslib::Item
      //, public nslib::SelectableItem // (No render polygons!!!)
      , public nslib::InteractiveItem
    {
      Q_OBJECT
      Q_PROPERTY( QPointF origin READ origin WRITE setOrigin )
      Q_PROPERTY( QPointF dest READ dest WRITE setDest )

    public:

      ConnectionArrowItem( const ConnectionArrowRep& connectionArrowRep );

      virtual ~ConnectionArrowItem( void ) {}

      const QPointF& origin( void ) const { return _arrowOrigin; }
      const QPointF& dest( void ) const { return _arrowDest; }

      void setOrigin( const QPointF& origin_ );

      void setDest( const QPointF& dest_ );

      void createArrow( const QPointF& origin, const QPointF& dest, float thickness );

      QPropertyAnimation& originAnim( void ) { return _originAnim; }
      QPropertyAnimation& destAnim( void ) { return _destAnim; }


    protected:
      float _arrowThickness;
      QPointF _arrowOrigin;
      QPointF _arrowDest;

      QPropertyAnimation _originAnim;
      QPropertyAnimation _destAnim;

    };
  } // namespace cortex
} // namespace nslib

#endif
