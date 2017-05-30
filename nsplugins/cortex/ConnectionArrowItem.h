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
      Q_PROPERTY( QPointF pos READ pos WRITE setPos )
      Q_PROPERTY( qreal scale READ scale WRITE setScale )

    public:

      ConnectionArrowItem( const ConnectionArrowRep& connectionArrowRep )
      {
        this->_parentRep =
          &( const_cast< ConnectionArrowRep& >( connectionArrowRep ));

        //DONE
        arrowWidht= //30;
        arrowLengh=30;

        QPoint mLineInit(rand()%800, rand()%600);
        QPoint mLineEnd(rand()%800, rand()%600);
        QLineF lAuxLine(mLineInit,mLineEnd);

        double angle = ::acos(lAuxLine.dx() / lAuxLine. length());
        if (lAuxLine.dy() >= 0) angle = (M_PI * 2) - angle;

        QPointF arrowInit 	= lAuxLine.pointAt(1.0f - (arrowLengh/lAuxLine.length()));
        QPointF arrowP1 	= arrowInit - QPointF(sin(angle + M_PI / 3) 		* arrowWidht
        										  ,cos(angle + M_PI / 3) 		* arrowWidht);
        QPointF arrowP2 	= arrowInit - QPointF(sin(angle + M_PI - M_PI / 3) 	* arrowWidht
        										  ,cos(angle + M_PI - M_PI / 3) * arrowWidht);
        arrowShape.clear();
        arrowShape		<< lAuxLine.p1()
        				<< arrowInit
        				<< arrowP1
        				<< lAuxLine.p2()
        				<< arrowP2
        				<< arrowInit
        				;

        this->setPolygon(arrowShape);

        //this->addPolygon(arrowShape);
      }

      virtual ~ConnectionArrowItem( void ) {}

    protected:

      QPolygonF arrowShape;

      float arrowWidht;
      float arrowLengh;

    };
  } // namespace cortex
} // namespace nslib

#endif
