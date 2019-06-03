/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Iago Calvo Lista <i.calvol@alumnos.urjc.es>
 *          Fernando Lucas Pérez <f.lucas@alumnos.urjc.es>
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
       * Returns the corresponding item from the scene
       *
       * @param scene QGraphicsScene from which the item is obtained
       * @param create creates a new item if true; else, returns existing item
       * @return corresponding AutoConnectionArrowItem
       */
      QGraphicsItem* item( QGraphicsScene* scene = nullptr,
        bool create = true ) override;

      /**
       *
       * Creates a new AutoConnectionArrowRep from a CongenPopRep
       *
       * @param Rep_ CongenPopRep to which an AutoConnectionArrowRep
       * will be attached
       */
      AutoConnectionArrowRep( shift::Representation* Rep_ );

      /**
       *
       * Creates and configures the corresponding AutoConnectionArrowItem and
       * configures transition animations
       *
       * @param opConfig OpConfig object that contains information about the
       * current layout type and whether the scene is being animated
       */
      void preRender( shift::OpConfig* opConfig = nullptr ) override;
    };
  } // namespace congen
} // namespace nslib


#endif //NEUROSCHEME_AUTOCONNECTIONARROWREP_H
