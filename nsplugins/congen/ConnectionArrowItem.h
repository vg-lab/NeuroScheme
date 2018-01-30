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
#ifndef __NSLIB__CONGEN__CONNECTION_ARROW_ITEM__
#define __NSLIB__CONGEN__CONNECTION_ARROW_ITEM__

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
  namespace congen
  {

    class ConnectionArrowItem
        : public QObject
            , public QGraphicsPathItem
            , public nslib::Item
            , public nslib::InteractiveItem
    {
      Q_OBJECT
      Q_PROPERTY( QLineF line READ line WRITE setLine )

      public:

      ConnectionArrowItem( const ConnectionArrowRep& connectionArrowRep );

      virtual ~ConnectionArrowItem( void );

      const QLineF& line( void );

      void setLine( const QLineF& line_ );

      virtual void createArrow( const QPointF& origin, const QPointF& dest );

      QPropertyAnimation& lineAnim( void );

      virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ );

      virtual void hoverEnter( void );

      virtual void highlight( scoop::Color color_ );

      virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ );

      virtual void hoverLeave( void );

      static QColor color;
      static QColor hoverColor;

      protected:
      //QGraphicsEllipseItem* _arrowOriItem;

      const float M_PI_3 = float( M_PI ) * 0.33f;
      QGraphicsEllipseItem* _arrowCircleEnd;

      float _arrowThickness;
      QLineF _line;
      QPointF _arrowOrigin;
      QPointF _arrowDest;

      QPropertyAnimation _lineAnim;

    };
  } // namespace congen
} // namespace nslib

#endif
