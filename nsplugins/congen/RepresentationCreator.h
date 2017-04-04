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
#ifndef __NSLIB__REPRESENTATION_CREATOR__
#define __NSLIB__REPRESENTATION_CREATOR__
#include <shift/shift.h>
#include <nslib/mappers/VariableMapper.h>
#include <scoop/scoop.h>

#include <unordered_map>
#include <set>

namespace nslib
{
  namespace congen
  {

    class RepresentationCreator : public shift::RepresentationCreator
    {
    public:
      virtual ~RepresentationCreator( void ) {};

      void create(
        const shift::Entities& entities,
        shift::Representations& representations,
        shift::TEntitiesToReps& entitiesToReps,
        shift::TRepsToEntities& repsToEntities,
        bool linkEntitiesToReps = false,
        bool linkRepsToObjs = false ) final;

    protected:
    };

  } // namespace congen
} // namespace nslib

#endif // __NSLIB__REPRESENTATION_CREATOR__
