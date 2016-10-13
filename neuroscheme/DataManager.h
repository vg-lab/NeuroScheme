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
#ifndef __NEUROSCHEME__DATA_MANAGER__
#define __NEUROSCHEME__DATA_MANAGER__

#include <shift/shift.h>
#include "domains/domains.h"

namespace neuroscheme
{
  class DataManager
  {
  public:

    static shift::EntitiesWithRelationships& entities( void );
    static shift::Representations& representations( void );
    static void loadData( void );

  protected:
    static shift::EntitiesWithRelationships _entities;
    static shift::Representations _representations;

  };
}

#endif
