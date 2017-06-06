/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 * 			    Juan Pedro Brito <juanpedro.brito@upm.es>
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

#include <QGraphicsPolygonItem>
#include <QPropertyAnimation>

#include <ctime>

namespace nslib
{
  namespace cortex
  {

    class ConnectionArrowItem
      : public QObject
      , public QGraphicsPolygonItem
      , public nslib::Item
      , public nslib::InteractiveItem
    {
      Q_OBJECT
      Q_PROPERTY( QLineF line READ line WRITE setLine )

    public:

      ConnectionArrowItem( const ConnectionArrowRep& connectionArrowRep );

      virtual ~ConnectionArrowItem( void ) {}

      const QLineF& line( void ) const { return _line; }
      void setLine( const QLineF& line_ )
      {
        _line = line_;
        createArrow( _line.p1( ), _line.p2( ), _arrowThickness );
      }

      void createArrow( const QPointF& origin, const QPointF& dest, float thickness );

      QPropertyAnimation& lineAnim( void ) { return _lineAnim; }

    protected:
      QGraphicsEllipseItem* _arrowOriItem;

      float _arrowThickness;
      QLineF _line;
      QPointF _arrowOrigin;
      QPointF _arrowDest;

      QPropertyAnimation _lineAnim;

    };
  } // namespace cortex
} // namespace nslib

#endif
