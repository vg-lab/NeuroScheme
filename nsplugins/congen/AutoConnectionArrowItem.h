/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Iago Calvo Lista
 *          Fernando Lucas Pérez
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
       * Creates a new AutoConnectionArrowItem from a AutoConnectionArrowRep
       * @param autoConnectionArrowRep AutoConnectionArrowRep to which the new
       * AutoConnectionArrowItem correspond
       */
      AutoConnectionArrowItem(
        const AutoConnectionArrowRep& autoConnectionArrowRep );

      /**
       *
       * Override getter for the line used for transition animations
       *
       * @return line animated in transition animations
       */
      virtual const QLineF& line( void ) override;

      /**
       *
       * Override setter for the line used during transitions; calls the
       * auxiliar function createAutoArrow
       *
       * @param line_ new line to be drawn
       */
      virtual void setLine( const QLineF& line_ ) override;

      /**
       *
       * Creates the path corresponding to the autoconnection arrow
       *
       * @param glyphRadius_ value of the radius of the NeuronPop representation
       * @param isGrid_ if equal to 1.0f uses the angle for grid layouts
       * @param glyphCenter Qpoint of the center of the NeuronPop representation
       */
      void createAutoArrow(
        float glyphRadius_, float isGrid_, QPointF glyphCenter);

      /**
       *
       * Recalcs static common data common for all the representations
       *
       */
      static void recalcArcData( );

      /**
       *
       * Set the color to red when the mouse is over it
       *
       */
      virtual void hoverEnter( void ) override;

      /**
       *
       * Restore the normal color when the mouses leaves the neuron
       *
       * @param event_
       */
      virtual void hoverLeave( void ) override;
      virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ ) override;

      /**
      *
      * Set the color to green when the mouse is over the its NeuronPop
      *
      */
      virtual void highlight( scoop::Color color_ ) override;


      /**
       * Sets the _arcSizeFactor who the determines the arc size
       *
       * @param arcSizeFactor_ positive float value of the new _arcSizeFactor
       */
      void setArcSizeFactor( float arcSizeFactor_ );

      /**
       * sets the _acentersDistFactor who the determines the adetermines
       * distance between arc and glyph centres
       *
       * @param centersDistFactor_ float between -1.0f and 1.0f of the new
       * _centersDistFactor
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

      // determines distance between arc and glyph centres; -1.0f to 1.0f
      static float _centersDistFactor;

      // determines arc size; positive float
      static float _arcSizeFactor;

      //static const pi values used in different render calculations
      static const float M_PI_x2;
      static const float Rad_To_Deg;
      static const float M_PI_0825;
      static const float M_PI_1115;
      static const float M_PI_175;

    };
  } // namespace congen
} // namespace nslib


#endif //NEUROSCHEME_AUTOCONNECTIONAROWITEM_H


