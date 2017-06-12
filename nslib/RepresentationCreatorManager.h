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
#ifndef __NSLIB__REPRESENTATION_CREATOR_MANAGER__
#define __NSLIB__REPRESENTATION_CREATOR_MANAGER__

#include <nslib/api.h>
#include <shift/shift.h>
#include "Canvas.h"
// #include "domains/domains.h"

namespace nslib
{


  class NSLIB_API RepresentationCreatorManager
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

    static void generateRelations( const shift::Entities& entities,
                                   shift::Representations& representations,
                                   const std::string& relationName,
                                   unsigned int repCreatorId = 0 );

    static const shift::TEntitiesToReps& entitiesToReps(
        unsigned int repCreatorId = 0 );

    static const shift::TRepsToEntities& repsToEntities(
        unsigned int repCreatorId = 0 );

    static const shift::TRelatedEntitiesReps& relatedEntities(
        unsigned int repCreatorID = 0 );

    static const shift::TGidToEntitiesReps& gidsToEntitiesReps(
        unsigned int repCreatorID = 0 );

    static void deleteItemsOfCanvas( Canvas* canvas );

    // static const shift::Representation& objectToRep(
  protected:
    static std::unordered_map< unsigned int, shift::RepresentationCreator* > _repCreators;
    static std::unordered_map< unsigned int, shift::TEntitiesToReps > _entitiesToReps;
    static std::unordered_map< unsigned int, shift::TRepsToEntities > _repsToEntities;

    static std::unordered_map< unsigned int, shift::TRelatedEntitiesReps > _relatedEntitiesReps;
    static std::unordered_map< unsigned int, shift::TGidToEntitiesReps > _gidsToEntitiesReps;
  };

} // namespace nslib

#endif // __NSLIB__REPRESENTATION_CREATOR__
