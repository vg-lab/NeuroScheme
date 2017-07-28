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
#include "RepresentationCreatorManager.h"
#include "reps/QGraphicsItemRepresentation.h"

namespace nslib
{

  std::unordered_map< unsigned int, shift::RepresentationCreator* >
  RepresentationCreatorManager::_repCreators =
    std::unordered_map< unsigned int, shift::RepresentationCreator* >( );

  std::unordered_map< unsigned int, shift::TEntitiesToReps >
  RepresentationCreatorManager::_entitiesToReps =
    std::unordered_map< unsigned int, shift::TEntitiesToReps >( );

  std::unordered_map< unsigned int, shift::TRepsToEntities>
  RepresentationCreatorManager::_repsToEntities =
    std::unordered_map< unsigned int, shift::TRepsToEntities>( );

  std::unordered_map< unsigned int, shift::TRelatedEntitiesReps >
  RepresentationCreatorManager::_relatedEntitiesReps =
    std::unordered_map< unsigned int, shift::TRelatedEntitiesReps >( );

  std::unordered_map< unsigned int, shift::TGidToEntitiesReps >
  RepresentationCreatorManager::_gidsToEntitiesReps =
    std::unordered_map< unsigned int, shift::TGidToEntitiesReps >( );


  void RepresentationCreatorManager::addCreator(
    shift::RepresentationCreator* repCreator,
    unsigned int repCreatorId )
  {
    //TODO check if exists
    _repCreators[ repCreatorId ] = repCreator;
  }

  shift::RepresentationCreator* RepresentationCreatorManager::getCreator(
    unsigned int idx )
  {
    if ( _repCreators.count( idx ) == 1 )
      return _repCreators[ idx ];
    else
      return nullptr;
  }

  RepresentationCreatorManager::TCreatorsMap&
  RepresentationCreatorManager::creators( void )
  {
    return _repCreators;
  }

  void RepresentationCreatorManager::create(
    const shift::Entities& entities,
    shift::Representations& representations,
    // TEntitiesToReps& entitiesToReps,
    // TRepsToEntities& repsToEntities,
    bool linkEntitiesToReps,
    bool linkRepsToObjs,
    unsigned int repCreatorId )
  {
    //TODO check if exists
    if ( _repCreators.count( repCreatorId ) == 1 )
      _repCreators[ repCreatorId ]->create( entities, representations,
                                            _entitiesToReps[ repCreatorId ],
                                            _repsToEntities[ repCreatorId ],
                                            _gidsToEntitiesReps[ repCreatorId ],
                                            linkEntitiesToReps,
                                            linkRepsToObjs );
  }


  void RepresentationCreatorManager::generateRelations(
      const shift::Entities& entities,
      shift::Representations& representations,
      const std::string& relationName,
      unsigned int repCreatorId )
  {
    if( _repCreators.count( repCreatorId ) == 1 )
      _repCreators[ repCreatorId ]->generateRelations( entities,
                                                       _gidsToEntitiesReps[ repCreatorId],
                                                       _relatedEntitiesReps[ repCreatorId ],
                                                       representations,
                                                       relationName );
  }

  const shift::TEntitiesToReps& RepresentationCreatorManager::entitiesToReps(
    unsigned int repCreatorId )
  {
    return _entitiesToReps[ repCreatorId ];
  }
  const shift::TRepsToEntities& RepresentationCreatorManager::repsToEntities(
    unsigned int repCreatorId )
  {
    return _repsToEntities[ repCreatorId ];
  }

  const shift::TRelatedEntitiesReps& RepresentationCreatorManager::relatedEntities(
    unsigned int repCreatorId )
  {
    return _relatedEntitiesReps[ repCreatorId ];
  }

  const shift::TGidToEntitiesReps& RepresentationCreatorManager::gidsToEntitiesReps(
    unsigned int repCreatorId )
  {
    return _gidsToEntitiesReps[ repCreatorId ];
  }

  void RepresentationCreatorManager::deleteItemsOfCanvas( Canvas* canvas )
  {
    for ( auto& entitiesToReps : _entitiesToReps )
      for ( auto& entityToReps : entitiesToReps.second )
        for ( auto& rep : entityToReps.second )
        {
          auto qGraphicsRep =
            dynamic_cast< QGraphicsItemRepresentation* >( rep );
          if ( qGraphicsRep )
            qGraphicsRep->deleteItem( &canvas->scene( ));
        }
  }
}
