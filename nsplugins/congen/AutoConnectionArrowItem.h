/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Fernando Lucas Pérez
 * 			    Iago Calvo Lista
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
#ifndef NEUROSCHEME_AUTOCONNECTIONAROWITEM_H
#define NEUROSCHEME_AUTOCONNECTIONAROWITEM_H

#include "AutoConnectionArrowRep.h"
#include "ConnectionArrowItem.h"

namespace nslib
{
  namespace congen
  {
    class AutoConnectionArrowItem : public nslib::congen::ConnectionArrowItem
    {
      public:
      AutoConnectionArrowItem(
          const AutoConnectionArrowRep& connectionArrowRep );

      void createAutoArrow( const QPointF& arcCenter, float arcDegrees,
         float arcRadius, float startAngle, QPointF glyphCenter );

      virtual void hoverEnter( void ) override;

      virtual void highlight( scoop::Color color_ ) override;

      virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ ) override;

      virtual void hoverLeave( void ) override;

    };
  } // namespace congen
} // namespace nslib


#endif //NEUROSCHEME_AUTOCONNECTIONAROWITEM_H


