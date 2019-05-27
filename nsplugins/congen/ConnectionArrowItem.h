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

#include "ConnectionArrowRep.h"

#include <nslib/Color.h>
#include <nslib/InteractionManager.h>
#include <nslib/reps/InteractiveItem.h>
#include <nslib/reps/Item.h>
#include <nslib/reps/SelectableItem.h>

#include <QGraphicsPolygonItem>
#include <QPropertyAnimation>

#include <ctime>

namespace nslib
{
  namespace congen
  {

    class ConnectionArrowItem
      : public QObject,
        public QGraphicsPathItem,
        public nslib::Item,
        public nslib::InteractiveItem
    {
      Q_OBJECT
      Q_PROPERTY( QLineF line
        READ
        line
        WRITE
        setLine )

      public:

      ConnectionArrowItem(
        const ConnectionArrowRep& connectionArrowRep,
        const bool interactive_ = true );

      virtual ~ConnectionArrowItem( void );

      virtual const QLineF& line( void );

      virtual void setLine( const QLineF& line_ );

      void createArrow( const QPointF& origin, const QPointF& dest );

      QPropertyAnimation& lineAnim( void );

      virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ ) override;

      virtual void hoverEnter( void );

      virtual void highlight( scoop::Color color_ );

      virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ ) override;

      virtual void hoverLeave( void );

      virtual bool connectionRep( void ) const override;

      virtual void contextMenuEvent(
        QGraphicsSceneContextMenuEvent* event_ ) override;

      static QColor color;
      static QColor hoverColor;

      protected:
      //QGraphicsEllipseItem* _arrowOriItem;

      const static float M_PI_Float;
      const static float M_PI_3;
      const static float M_PI_Inverse;
      static const float M_PI_x2;
      static const float M_PI_067;


      QGraphicsEllipseItem* _arrowCircleEnd;

      float _arrowThickness;
      QLineF _line;
      QPointF _arrowOrigin;
      QPointF _arrowDest;

      QPropertyAnimation _lineAnim;
      Qt::PenStyle _lineStyle;
      QPen _actualPen;

    };
  } // namespace congen
} // namespace nslib

#endif
