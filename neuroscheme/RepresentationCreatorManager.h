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
#ifndef __NEUROSCHEME__REPRESENTATION_CREATOR_MANAGER__
#define __NEUROSCHEME__REPRESENTATION_CREATOR_MANAGER__

#include "domains/domains.h"

namespace neuroscheme
{


  class RepresentationCreatorManager
  {
  public:
    static void addCreator( shift::RepresentationCreator* repCreator,
                            unsigned int repCreatorId = 0 );

    static void create( const shift::Entities& entities,
                        shift::Representations& representations,
                        // TEntitiesToReps& entitiesToReps,
                        // TRepsToEntities& repsToEntities,
                        bool linkEntitiesToReps = false,
                        bool linkRepsToObjs = false,
                        unsigned int repCreatorId = 0 );

    static const shift::TEntitiesToReps& entitiesToReps(
      unsigned int repCreatorId = 0 );
    static const shift::TRepsToEntities& repsToEntities(
      unsigned int repCreatorId = 0 );

    static void deleteItemsOfCanvas( Canvas* canvas );

    // static const shift::Representation& objectToRep(
  protected:
    static std::map< unsigned int, shift::RepresentationCreator* > _repCreators;
    static std::map< unsigned int, shift::TEntitiesToReps > _entitiesToReps;
    static std::map< unsigned int, shift::TRepsToEntities > _repsToEntities;

  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__REPRESENTATION_CREATOR__
