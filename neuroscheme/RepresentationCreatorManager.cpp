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

namespace neuroscheme
{

  std::map< unsigned int, shift::RepresentationCreator* >
  RepresentationCreatorManager::_repCreators =
    std::map< unsigned int, shift::RepresentationCreator* >( );

  std::map< unsigned int, shift::TEntitiesToReps >
  RepresentationCreatorManager::_entitiesToReps =
    std::map< unsigned int, shift::TEntitiesToReps >( );

  std::map< unsigned int, shift::TRepsToEntities>
  RepresentationCreatorManager::_repsToEntities =
    std::map< unsigned int, shift::TRepsToEntities>( );


  void RepresentationCreatorManager::addCreator(
    shift::RepresentationCreator* repCreator,
    unsigned int repCreatorId )
  {
    //TODO check if exists
    _repCreators[ repCreatorId ] = repCreator;
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
                                            linkEntitiesToReps,
                                            linkRepsToObjs );
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
