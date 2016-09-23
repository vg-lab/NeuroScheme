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

#include "RepresentationCreator.h"

namespace neuroscheme
{


  class RepresentationCreatorManager
  {
  public:
    static void addCreator( RepresentationCreator* repCreator,
                            unsigned int repCreatorId = 0 );

    static void create( const shift::Objects& objects,
                        shift::Representations& representations,
                        // TObjectsToReps& objectsToReps,
                        // TRepsToObjects& repsToObjects,
                        bool linkObjectsToReps = false,
                        bool linkRepsToObjs = false,
                        unsigned int repCreatorId = 0 );

    static const TObjectsToReps& objectsToReps( unsigned int repCreatorId = 0 );
    static const TRepsToObjects& repsToObjects( unsigned int repCreatorId = 0 );

    // static const shift::Representation& objectToRep(
  protected:
    static std::map< unsigned int, RepresentationCreator* > _repCreators;
    static std::map< unsigned int, TObjectsToReps > _objectsToReps;
    static std::map< unsigned int, TRepsToObjects > _repsToObjects;

  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__REPRESENTATION_CREATOR__
