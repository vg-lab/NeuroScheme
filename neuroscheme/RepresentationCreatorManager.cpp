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

namespace neuroscheme
{

  std::map< unsigned int, RepresentationCreator* >
  RepresentationCreatorManager::_repCreators =
    std::map< unsigned int, RepresentationCreator* >( );

  std::map< unsigned int, TObjectsToReps >
  RepresentationCreatorManager::_objectsToReps =
    std::map< unsigned int, TObjectsToReps >( );

  std::map< unsigned int, TRepsToObjects>
  RepresentationCreatorManager::_repsToObjects =
    std::map< unsigned int, TRepsToObjects>( );


  void RepresentationCreatorManager::addCreator(
    RepresentationCreator* repCreator,
    unsigned int repCreatorId )
  {
    //TODO check if exists
    _repCreators[ repCreatorId ] = repCreator;
  }

  void RepresentationCreatorManager::create(
    const shift::Objects& objects,
    shift::Representations& representations,
    // TObjectsToReps& objectsToReps,
    // TRepsToObjects& repsToObjects,
    bool linkObjectsToReps,
    bool linkRepsToObjs,
    unsigned int repCreatorId )
  {
    //TODO check if exists
    _repCreators[ repCreatorId ]->create( objects, representations,
                                          _objectsToReps[ repCreatorId ],
                                          _repsToObjects[ repCreatorId ],
                                          linkObjectsToReps,
                                          linkRepsToObjs );
  }

  const TObjectsToReps& RepresentationCreatorManager::objectsToReps(
    unsigned int repCreatorId )
  {
    return _objectsToReps[ repCreatorId ];
  }
  const TRepsToObjects& RepresentationCreatorManager::repsToObjects(
    unsigned int repCreatorId )
  {
    return _repsToObjects[ repCreatorId ];
  }

}
