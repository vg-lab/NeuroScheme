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
#ifndef NEUROSCHEME_AUTOCONNECTIONARROWREP_H
#define NEUROSCHEME_AUTOCONNECTIONARROWREP_H

#include "ConnectionArrowRep.h"

namespace nslib
{
  namespace congen
  {
    class AutoConnectionArrowRep : public nslib::congen::ConnectionArrowRep
    {
      public :
      QGraphicsItem* item( QGraphicsScene* scene = nullptr,
        bool create = true );
      AutoConnectionArrowRep( shift::Representation* Rep_ );
      void preRender( shift::OpConfig* opConfig = nullptr ) override;

      protected:
      // determines distance between arc and glyph centres; -1 to 1
      static const float _centersDistFactor;

      // determines arc size; positive float
      static const float _arcSizeFactor;

      //the following determine various values needed to paint auto-connections
      static float glyphRadius;
      static float arcRadius;
      static float dist;
      static float startAngle;
      static float arcDegrees;

      // determines whether the values above are up to date
      static float glyphBoundingRect;
      static float glyphScale;

    };
  } // namespace congen
} // namespace nslib


#endif //NEUROSCHEME_AUTOCONNECTIONARROWREP_H
