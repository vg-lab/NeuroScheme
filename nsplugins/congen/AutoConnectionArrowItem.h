/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Iago Calvo Lista <i.calvol@alumnos.urjc.es>
 *          Fernando Lucas Pérez <f.lucas@alumnos.urjc.es>
 *
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
      /**
       *
       * Creates a new AutoConnectionArrowItem from an AutoConnectionArrowRep
       *
       * @param autoConnectionArrowRep AutoConnectionArrowRep that contains the
       * necessary info for the new AutoConnectionArrowItem
       */
      AutoConnectionArrowItem(
        const AutoConnectionArrowRep& autoConnectionArrowRep );

      /**
       *
       * Override getter for the QLineF used to animate transitions
       *
       * @return line QLineF with the necessary info to be interpolated in
       * animations
       */
      virtual const QLineF& line( void ) override;

      /**
       *
       * Override setter for the QLineF used for animations;
       * invokes createAutoArrow
       *
       * @param line_ new line to be stored
       */
      virtual void setLine( const QLineF& line_ ) override;

      /**
       *
       * Creates the graphic elements of the arrow
       *
       * @param glyphRadius_ value of the radius of the NeuronPop representation
       * @param isGrid_ true if the current layout is a grid
       * @param glyphCenter_ QPointF pointing to the center of the NeuronPop
       * representation
       */
      void createAutoArrow(
        float glyphRadius_, bool isGrid_, QPointF glyphCenter_ );

      /**
       *
       * Recalcs static data common to all current autoconnection arrows
       *
       */
      static void recalcArcData( );

      /**
       *
       * Sets item color to red on mouseover
       *
       */
      virtual void hoverEnter( void ) override;

      /**
       *
       * Restores original color after mouseover
       *
       */
      virtual void hoverLeave( void ) override;
      virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ ) override;

      /**
      *
      * Sets the color to green when pointer is over its associated NeuronPop
      *
      */
      virtual void highlight( scoop::Color color_ ) override;

      /**
       * Sets _arcSizeFactor, which determines the size of the autoconnection
       * arc
       *
       * @param arcSizeFactor_ positive float that calculates a size for the arc
       * based on the size of the NeuronPop glyph it's associated with
       */
      void setArcSizeFactor( float arcSizeFactor_ );

      /**
       * Sets the _centersDistFactor, which determines the distance between
       * arrow and glyph centres
       *
       * @param centersDistFactor_ float to which _centersDistFactor will be set
       * to. -1.0f hides the arrow inside the glyph, while 1.0f makes its arc
       * tangent to the glyph
       */
      void setCentersDistFactor( float centersDistFactor_ );

    protected:

      //the following determine various values needed to paint auto-connections
      static float arcRadius;
      static float dist;
      static float startAngle;
      static float arcDegrees;

      // determines whether the values above are up to date
      static float glyphRadius;

      // determines distance between arc and glyph centres
      static float _centersDistFactor;

      // determines arc size; positive float
      static float _arcSizeFactor;

      //static const pi values used in different render calculations
      static const float Rad_To_Deg;
      static const float M_PI_0825;
      static const float M_PI_1115;
      static const float M_PI_175;

    };
  } // namespace congen
} // namespace nslib


#endif //NEUROSCHEME_AUTOCONNECTIONAROWITEM_H


