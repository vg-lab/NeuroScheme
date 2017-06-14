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
#ifndef __NSLIB__CONNECTION_ARROW_REP__
#define __NSLIB__CONNECTION_ARROW_REP__

#include <nslib/reps/ConnectivityRep.h>
#include <nslib/reps/QGraphicsItemRepresentation.h>
#include <shift/shift.h>
#include <shift_ConnectionArrowRep.h>

namespace nslib
{
  namespace cortex
  {
    class ConnectionArrowRep
      : public shiftgen::ConnectionArrowRep
      , public QGraphicsItemRepresentation
      , public ConnectivityRep
    {
    public:

      ConnectionArrowRep( shift::Representation* originRep_,
                          shift::Representation* destRep_ );
      ConnectionArrowRep( const ConnectionArrowRep& );
      virtual ~ConnectionArrowRep( void ) {}
      QGraphicsItem* item( QGraphicsScene* scene = nullptr,
                           bool create = true );

      void preRender( shift::OpConfig* opConfig = nullptr );

      void hoverEnterEvent( QGraphicsSceneHoverEvent* event );
      void hoverLeaveEvent( QGraphicsSceneHoverEvent* event );

      virtual void highlight(
        const scoop::Color& color = scoop::Color( 255, 0, 0 ));
      virtual void unHighlight( void ) final { hoverLeaveEvent( nullptr ); }

    protected:
      shift::Representation* _originRep;
      shift::Representation* _destRep;
    };
  } // namespace cortex
} // namespace nslib

#endif
