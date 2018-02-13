/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Iago Calvo Lista
 *          Fernando Lucas Pérez
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
      /**
       *
       * Returns the corresponding item on the scene
       *
       * @param scene scene from where obtain the item
       * @param create if true creates the item if necessary
       * @return corresponding AutoConnectionArrowItem
       */
      QGraphicsItem* item( QGraphicsScene* scene = nullptr,
        bool create = true ) override;

      /**
       *
       * Creates a new AutoConnectionArrowRep from a NeuronPopRep representation
       *
       * @param Rep_ NeuronPopRep Representation corresponding to the new
       * AutoConnectionArrowItem
       */
      AutoConnectionArrowRep( shift::Representation* Rep_ );

      /**
       *
       * Creates and configures the corresponding AutoConnectionArrowItem and
       * configures the transition animations
       *
       * @param opConfig Storage configuration data that allows detect
       * animations when necessary and a layout type
       */
      void preRender( shift::OpConfig* opConfig = nullptr ) override;
    };
  } // namespace congen
} // namespace nslib


#endif //NEUROSCHEME_AUTOCONNECTIONARROWREP_H
