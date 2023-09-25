/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
#include <nslib/layouts/FreeLayout.h>
#include "Canvas.h"
#include "PaneManager.h"
#include "Config.h"
// #include "domains/domains.h"

namespace nslib
{


  class NSLIB_API RepresentationCreatorManager
  {
  public:
    typedef std::unordered_map< unsigned int, shift::RepresentationCreator* >
      TCreatorsMap;

    static void addCreator( shift::RepresentationCreator* repCreator,
      unsigned int repCreatorId = 0 );

    static void create( const shift::Entities& entities,
      shift::Representations& representations,
      bool linkEntitiesToReps = false,
      bool linkRepsToObjs = false,
      unsigned int repCreatorId = 0 );

    static void generateRelations( const shift::Entities& entities,
      shift::Representations& representations,
      const std::string& name, bool aggregated = false,
      unsigned int repCreatorId = 0 );

    static const shift::TEntitiesToReps& entitiesToReps(
      unsigned int repCreatorId = 0 );

    static const shift::TRepsToEntities& repsToEntities(
      unsigned int repCreatorId = 0 );

    static void clearEntitiesToReps( unsigned int repCreatorId = 0 );

    static void clearEntitiesCache( unsigned int repCreatorId = 0,
      const bool freeLayoutInUse = false );

    static void clearRelationshipsCache( unsigned int repCreatorId = 0 );

    static void clearCaches( void );
    static void clearMaximums( void );

    static void updateEntitiyRepresentations(
      const shift::Entity* entity_,
      std::set< shift::Representation* > entityReps_,
      unsigned int repCreatorId = 0,
      const bool freeLayoutInUse_ = false );

    static void updateEntities(
      const shift::Entities& updatedEntities_,
      const unsigned int repCreatorId = 0,
      const bool freeLayoutInUse_ = false );

    static void removeEntity(
      shift::Entity* entity, unsigned int repCreatorId = 0 );

    static const shift::TRelatedEntitiesReps& relatedEntities(
      unsigned int repCreatorID = 0 );

    static const shift::TGidToEntitiesReps& gidsToEntitiesReps(
      unsigned int repCreatorID = 0 );

    static void deleteItemsOfCanvas( Canvas* canvas );

    static shift::RepresentationCreator* getCreator( unsigned int idx = 0 );

    static TCreatorsMap& creators( void );

    // static const shift::Representation& objectToRep(

  protected:

    static TCreatorsMap _repCreators;
    static std::unordered_map< unsigned int, shift::TEntitiesToReps > _entitiesToReps;
    static std::unordered_map< unsigned int, shift::TRepsToEntities > _repsToEntities;

    static std::unordered_map< unsigned int, shift::TRelatedEntitiesReps > _relatedEntitiesReps;
    static std::unordered_map< unsigned int, shift::TGidToEntitiesReps > _gidsToEntitiesReps;
  };

} // namespace nslib

#endif // __NSLIB__REPRESENTATION_CREATOR__
