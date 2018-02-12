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
    class AutoConnectionArrowItem
      : public nslib::congen::ConnectionArrowItem
    {
      Q_OBJECT
      Q_PROPERTY( QLineF line READ line WRITE setLine )

      public:
      AutoConnectionArrowItem(
        const AutoConnectionArrowRep& connectionArrowRep );

      virtual const QLineF& line( void ) override;
      virtual void setLine( const QLineF& line_ ) override;

      void createAutoArrow( float glyphScale_,
        float glyphBoundingRect_, QPointF glyphCenter );

      static void recalcArcData( );
      virtual void hoverEnter( void ) override;

      virtual void highlight( scoop::Color color_ ) override;

      virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ ) override;

      virtual void hoverLeave( void ) override;
      void setArcSizeFactor( float arcSizeFactor_ );
      void setCentersDistFactor( float centersDistFactor_ );

      protected:

      //the following determine various values needed to paint auto-connections
      static float arcRadius;
      static float dist;
      static float startAngle;
      static float arcDegrees;

      // determines whether the values above are up to date
      static float glyphRadius;

      // determines distance between arc and glyph centres; -1 to 1
      static float _centersDistFactor;

      // determines arc size; positive float
      static float _arcSizeFactor;

      static const float M_PI_x2;
      static const float Rad_To_Deg;
      static const float M_PI_0825;
      static const float M_PI_1115;
      static const float M_PI_175;

    };
  } // namespace congen
} // namespace nslib


#endif //NEUROSCHEME_AUTOCONNECTIONAROWITEM_H


